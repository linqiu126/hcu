/*
 * hsmmp.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "hsmmp.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l2frame/cloudvela.h"
#include <curl/curl.h>

/*
** FSM of the HSMMP
*/
HcuFsmStateItem_t HcuFsmHsmmp[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_hsmmp_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_hsmmp_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_HSMMP_INITED,            		fsm_hsmmp_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_HSMMP_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          	fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,           fsm_hsmmp_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          	fsm_hsmmp_time_out},

    //Normal working status, 等待上层来的控制消息
	{MSG_ID_CLOUDVELA_HSMMP_DATA_REQ,		FSM_STATE_HSMMP_ACTIVED,      	  	fsm_hsmmp_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM,	FSM_STATE_HSMMP_ACTIVED,      	  	fsm_hsmmp_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ,		FSM_STATE_HSMMP_ACTIVED,      	  	fsm_hsmmp_cloudvela_ctrl_req},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,			FSM_STATE_HSMMP_ACTIVED,      	  	fsm_hsmmp_l3aqyc_exg_ctrl_req},
	{MSG_ID_SPSVIRGO_HSMMP_DATA_RX,     	FSM_STATE_HSMMP_ACTIVED,          	fsm_hsmmp_audio_data_rx},
	{MSG_ID_AVORION_HSMMP_DATA_RX,      	FSM_STATE_HSMMP_ACTIVED,         	fsm_hsmmp_avorion_data_rx},
	{MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM, FSM_STATE_HSMMP_ACTIVED,         	fsm_hsmmp_picture_data_confirm},

	//来自CLOUD的控制协议，可以在不同的激活状态下起作用，但起作用必须等待下一轮硬件空闲的时候
    //Waiting for feedback from
	{MSG_ID_AVORION_HSMMP_DATA_READ_FB,  	FSM_STATE_HSMMP_ACTIVED_WFFB,    	fsm_hsmmp_avorion_data_read_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global Variables


//Task Global variables
//gTaskHsmmpContext_t gTaskHsmmpContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_hsmmp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_HSMMP, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_hsmmp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		//hcu_usleep(rand()%DURATION_OF_INIT_FB_WAIT_MAX);
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_HSMMP, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_INITED);
	HCU_DEBUG_PRINT_FAT("HSMMP: Enter FSM_STATE_HSMMP_INITED status, Keeping refresh here!\n");

	//Task global variables init.
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP] = 0;
	memset(&gTaskHsmmpContext, 0, sizeof(gTaskHsmmpContext_t));
	gTaskHsmmpContext.Hsmmp_flag = FALSE;
	gTaskHsmmpContext.Hsmmp_alarm = HSMMP_ALARM_FLAG_NULL;

	/*
		//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
		i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
		hcu_sleep(i);

		//启动周期性定时器
		hcu_timer_start(TASK_ID_HSMMP, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

		//启动周期性定时器
		i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
		hcu_sleep(i);
		hcu_timer_start(TASK_ID_HSMMP, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_HSMMP_PERIOD_CURL_PICTURE), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	*/
		//设置状态机到目标状态
		//State Transfer to FSM_STATE_HSMMP_ACTIVED
		FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED);
		HCU_DEBUG_PRINT_FAT("GPIO: Enter FSM_STATE_HSMMP_ACTIVED status, Keeping refresh here!\n");
/*
		//For HKvision option setting
		HKVisionOption_t HKVisionOption;
		memset( (void *)&HKVisionOption, 0, sizeof(HKVisionOption_t));

		strcat(HKVisionOption.user_key, "admin");
		strcat(HKVisionOption.user_key, ":");
		strcat(HKVisionOption.user_key, "Bxxh!123");

		strcat(HKVisionOption.url_photo, "http://192.168.1.64/Streaming/channels/1/picture");
		strcat(HKVisionOption.url_video_start, "http://192.168.1.64/ISAPI/ContentMgmt/record/control/manual/start/tracks/1");
		strcat(HKVisionOption.url_video_stop, "http://192.168.1.64/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1");


		strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curPhotoDir);
		strcat(HKVisionOption.file_photo, "/");
		strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curHikvisionFname);
		strcpy(HKVisionOption.file_photo_pure, zHcuVmCtrTab.clock.curHikvisionPureFname);

		strcat(HKVisionOption.file_video, zHcuVmCtrTab.clock.curPhotoDir);
		strcat(HKVisionOption.file_video, "/");
		strcat(HKVisionOption.file_video, "hkvideo.txt");
*/
		//UINT32 timeStampStart,timeStampEnd;


		//进入循环工作模式
		while(1){

	    	sleep(1);

			HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_HSMMP, msg_struct_hsmmp_cloudvela_data_report_t);

			//发送数据给后台
			if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE)){//如果只发给Home,应该是Curl的连接状态
			//if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE)){//debug by shanchun

				msg_struct_hsmmp_cloudvela_data_report_t snd;
				memset(&snd, 0, sizeof(msg_struct_hsmmp_cloudvela_data_report_t));

				//L2信息
				strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
					sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
				strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
						sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
				snd.comHead.timeStamp = time(0);
				//snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
				snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
				strcpy(snd.comHead.funcFlag, "0");

				//CONTENT
				snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;


		    	if((TRUE == gTaskHsmmpContext.Hsmmp_flag) && (HSMMP_ALARM_FLAG_NULL == gTaskHsmmpContext.Hsmmp_alarm))
		    	{
		    		gTaskHsmmpContext.timeStamp = time(0);
		    		gTaskHsmmpContext.Hsmmp_alarm = HSMMP_ALARM_FLAG_ON;

					snd.link.alarmFlag = gTaskHsmmpContext.Hsmmp_alarm;
					snd.link.timeStamp = gTaskHsmmpContext.timeStamp;
					snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_report_t);
					HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_HSMMP);

		    	}

		    	if((TRUE == gTaskHsmmpContext.Hsmmp_flag) && (HSMMP_ALARM_FLAG_ON == gTaskHsmmpContext.Hsmmp_alarm))
		    	{
					snd.link.alarmFlag = gTaskHsmmpContext.Hsmmp_alarm;
					snd.link.timeStamp = gTaskHsmmpContext.timeStamp;
					snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_report_t);
					HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_HSMMP);
		    	}

		    	if((FALSE == gTaskHsmmpContext.Hsmmp_flag) && (HSMMP_ALARM_FLAG_ON == gTaskHsmmpContext.Hsmmp_alarm))
		    	{
		    		gTaskHsmmpContext.timeStamp = time(0);
		    		gTaskHsmmpContext.Hsmmp_alarm = HSMMP_ALARM_FLAG_OFF;

					snd.link.alarmFlag = gTaskHsmmpContext.Hsmmp_alarm;
					snd.link.timeStamp = gTaskHsmmpContext.timeStamp;
					snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_report_t);
					HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_HSMMP);
		    	}

		    	if((FALSE == gTaskHsmmpContext.Hsmmp_flag) && (HSMMP_ALARM_FLAG_OFF == gTaskHsmmpContext.Hsmmp_alarm))
		    	{
		    		gTaskHsmmpContext.Hsmmp_alarm = HSMMP_ALARM_FLAG_NULL;

					snd.link.alarmFlag = gTaskHsmmpContext.Hsmmp_alarm;
					snd.link.timeStamp = gTaskHsmmpContext.timeStamp;
					snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_report_t);
					HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_HSMMP);
		    	}
			}
	    	sleep(HSMMP_HKVISION_CAPTURE_DURATION_DEFAULT);
		}

		return ret;
	}

OPSTAT fsm_hsmmp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("HSMMP: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_hsmmp_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_hsmmp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("HSMMP: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_HSMMP]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_HSMMP, TASK_ID_HSMMP, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName);
			return FAILURE;
		}
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_period_avorion_read();
	}

	//定时短时钟进行离线数据回送
	else if ((rcv.timeId == TIMER_ID_1S_HSMMP_AVORION_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_wait_for_cammera_fb();
	}

	//定时超长时钟进行照片回送
	else if ((rcv.timeId == TIMER_ID_1S_HSMMP_PERIOD_CURL_PICTURE) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_period_curl_picture();
	}

	//这里的ret=FAILURE並不算严重，只不过造成状态机返回差错而已，并不会造成程序崩溃和数据混乱，所以只是程序的自我保护而已
	return ret;

}

OPSTAT fsm_hsmmp_audio_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_hsmmp_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_hsmmp_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_hsmmp_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_hsmmp_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


OPSTAT fsm_hsmmp_avorion_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_hsmmp_picture_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_hsmmp_avorion_data_read_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret = 0;
	char newpath[HCU_SYSDIM_FILE_NAME_LEN_MAX]="";
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	//检查状态
	if (FsmGetState(TASK_ID_HSMMP) != FSM_STATE_HSMMP_ACTIVED_WFFB){
		FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED);
		HCU_DEBUG_PRINT_FAT("AVORION: Recover to ACTIVED state during period timer out!\n");
		return SUCCESS;
	}
	//收消息
	msg_struct_avorion_hsmmp_data_read_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_avorion_hsmmp_data_read_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_avorion_hsmmp_data_read_fb_t))){
		HcuErrorPrint("HSMMP: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查参数
	if ((rcv.boolBackCloud != TRUE) && (rcv.boolBackCloud != FALSE)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error boolBackCloud parameter message received!\n");
		return FAILURE;
	}
	if ((rcv.hsmmp.hsmmpFname == NULL) || ((strlen(rcv.hsmmp.hsmmpFname) > HCU_SYSDIM_FILE_NAME_LEN_MAX))){
		HcuErrorPrint("HSMMP: Error hsmmpFname parameter message received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		return FAILURE;
	}
	if ((rcv.hsmmp.hsmmpFdir == NULL) || ((strlen(rcv.hsmmp.hsmmpFdir) > HCU_SYSDIM_FILE_NAME_LEN_MAX))){
		HcuErrorPrint("HSMMP: Error hsmmpFdir parameter message received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		return FAILURE;
	}
	//停止定时器
	ret = hcu_timer_stop(TASK_ID_HSMMP, TIMER_ID_1S_HSMMP_AVORION_FB, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error stop timer!\n");
		return FAILURE;
	}
	//如果满足条件，移动文件到视频服务器目录中
	newpath[0]='\0';
	if(rcv.boolBackCloud == TRUE){
		strcpy(newpath, zHcuSysEngPar.videoSev.hcuVideoServerDir);
		strcat(newpath, rcv.hsmmp.hsmmpFname);
		//if (symlink(rcv.hsmmp.hsmmpFname, newpath) < 0){
		if (symlink(rcv.hsmmp.hsmmpFdir, newpath) < 0){
			HcuErrorPrint("HSMMP: Error create soft link for log avorion file!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			return FAILURE;
		}
		//重置为HTTP地址
		newpath[0]='\0';
		//strcpy(newpath, zHcuSysEngPar.videoSev.hcuVideoServerHttp);
		//strcat(newpath, rcv.hsmmp.hsmmpFname);
		strcpy(newpath, rcv.hsmmp.hsmmpFname);
	}
	//发送后台
	if (strlen(newpath) > 0){
		//离线模式
		//没有判定是否为周期模式，还是瞬时模式，均立即存储了，未来需要再完善
		if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
			//Save to disk as request：在线是为了备份，离线是为了重发给后台
			//完整的处理情形，有待完成，存入磁盘还为实现
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.hsmmp.equipid;
			record.sensortype = L3CI_hsmmp;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.hsmmp.timeStamp;
			strcpy(record.hsmmpFdir, rcv.hsmmp.hsmmpFdir);
			strcpy(record.hsmmpFname, rcv.hsmmp.hsmmpFname);
			strcpy(record.hsmmpLink, newpath); //都是以HCU_FILE_NAME_LENGTH_MAX长度为限
			record.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
			record.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
			record.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
			record.ew = zHcuVmCtrTab.hwinv.gps.EW;
			record.ns = zHcuVmCtrTab.hwinv.gps.NS;
			//RECORD还要存入数据库
			sensor_hsmmp_data_element_t hsmmpData;
			memset(&hsmmpData, 0, sizeof(sensor_hsmmp_data_element_t));
			hsmmpData.timeStamp = record.timestamp;
			strcpy(hsmmpData.hsmmpFdir, record.hsmmpFdir);
			strcpy(hsmmpData.hsmmpFname, record.hsmmpFname);
			strcpy(hsmmpData.hsmmpLink, record.hsmmpLink);
			hsmmpData.equipid = record.equipid;
			hsmmpData.gps.gpsx = record.gpsx;
			hsmmpData.gps.gpsy = record.gpsy;
			hsmmpData.gps.gpsz = record.gpsz;
			hsmmpData.gps.ew = record.ew;
			hsmmpData.gps.ns = record.ns;
			hsmmpData.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuHsmmpDataInfo_save(&hsmmpData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
				HcuErrorPrint("HSMMP: Can not save data into database!\n");
			}
		}
		//在线模式
		else if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
			msg_struct_hsmmp_cloudvela_data_resp_t snd;
			memset(&snd, 0, sizeof(msg_struct_hsmmp_cloudvela_data_resp_t));
			snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_resp_t);

			/*strcpy(snd.link.linkName, newpath);
			snd.link.linkLen = strlen(newpath);

			//考虑到AVORION将放到外部单独进程中，GPS时钟无法共享，故而GPS数据的采集直接放到HSMMP模块中，而不放到传感器中
			snd.link.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
			snd.link.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
			snd.link.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
			snd.link.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
			snd.link.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;
			snd.link.timeStampStart = rcv.hsmmp.timeStamp;
			snd.link.timeStampEnd = time(0);
*/
			//强行注入参数，以方便CLOUT中的编码，未来待完善
			snd.usercmdid = L3CI_hsmmp;
			snd.useroptid = L3PO_hsmmp_data_report;
			snd.link.equipid = rcv.hsmmp.equipid; //如果是0，可能会被L2编码拒绝，未来再优化完善
			snd.cmdIdBackType = L3CI_cmdid_back_type_period;

			ret = hcu_message_send(MSG_ID_HSMMP_CLOUDVELA_DATA_RESP, TASK_ID_CLOUDVELA, TASK_ID_HSMMP, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
				HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
				return FAILURE;
			}

			//RECORD还要存入数据库
			sensor_hsmmp_data_element_t hsmmpData;
			memset(&hsmmpData, 0, sizeof(sensor_hsmmp_data_element_t));
			hsmmpData.equipid = record.equipid;
			hsmmpData.timeStamp = record.timestamp;
			strcpy(hsmmpData.hsmmpFdir, record.hsmmpFdir);
			strcpy(hsmmpData.hsmmpFname, record.hsmmpFname);
			strcpy(hsmmpData.hsmmpLink, record.hsmmpLink);
			hsmmpData.gps.gpsx = record.gpsx;
			hsmmpData.gps.gpsy = record.gpsy;
			hsmmpData.gps.gpsz = record.gpsz;
			hsmmpData.gps.ew = record.ew;
			hsmmpData.gps.ns = record.ns;
			hsmmpData.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuHsmmpDataInfo_save(&hsmmpData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
				HcuErrorPrint("HSMMP: Can not save data into database!\n");
			}
			//瞬时模式，待完善
		}
		//差错情形
		else{
			HcuErrorPrint("HSMMP: Wrong state of CLOUDVELA when data need send out!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			return FAILURE;
		}
	}

	//设置新状态
	FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED);
	return SUCCESS;
}

//长周期
OPSTAT func_hsmmp_time_out_period_avorion_read(void)
{
	int ret=0;

	//判定状态机是否可以发送，如果不在ACTIVE状态，意味着一定有啥差错，强行恢复工作状态，等待下一次机会
	if (FsmGetState(TASK_ID_HSMMP) != FSM_STATE_HSMMP_ACTIVED){
		FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED);
		HCU_DEBUG_PRINT_FAT("AVORION: Recover to ACTIVED state during period timer out!\n");

		//发送INIT消息给AVORION任务
		/*msg_struct_com_init_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_t));
		snd.length = sizeof(msg_struct_com_init_t);
		ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_AVORION, TASK_ID_HSMMP, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_HSMMP].taskName, zHcuTaskInfo[TASK_ID_AVORION].taskName);
			return FAILURE;
		}
		return SUCCESS;*/
	}

	//发送命令给摄像头
	msg_struct_hsmmp_avorion_data_read_t snd;
	memset(&snd, 0, sizeof(msg_struct_hsmmp_avorion_data_read_t));
	snd.equipid = HSMMP_EQUIPMENT_ID_DEFAULT; //虽然用处不大，对为了方便设备的存储和数据库操作，还是留着该域
	snd.captureDur = HSMMP_AVORION_CAPTURE_DURATION_DEFAULT;
	snd.fileType = FILE_OPERATION_TYPE_AVORION_H264;
	snd.refreshRate = HSMMP_AVORION_REFRESH_RATE_DEFAULT;
	snd.timeStampStart = time(0);
	strcpy(snd.tmpFname, "output.yuv");

    if ((snd.fileType == FILE_OPERATION_TYPE_AVORION_H264) && (strlen(zHcuVmCtrTab.clock.curAvorionFdH264) < HCU_SYSDIM_FILE_NAME_LEN_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdH264);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameH264);
    }else if((snd.fileType == FILE_OPERATION_TYPE_AVORION_AVI) && (strlen(zHcuVmCtrTab.clock.curAvorionFdAvi) < HCU_SYSDIM_FILE_NAME_LEN_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdAvi);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameAvi);
    }else if((snd.fileType == FILE_OPERATION_TYPE_AVORION_MKV) && (strlen(zHcuVmCtrTab.clock.curAvorionFdMkv) < HCU_SYSDIM_FILE_NAME_LEN_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdMkv);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameMkv);
    }else{
        HcuErrorPrint("AVORION: Not set save file type correctly.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
        return FAILURE;
    }
	//发送后台的控制器，目前暂时是以10次回送一次的方式，未来可以由服务器后台控制
    gTaskHsmmpContext.sendCloudCnt++;
    gTaskHsmmpContext.sendCloudCnt = (gTaskHsmmpContext.sendCloudCnt % HSMMP_SEND_BACK_MOD_BASE);
	if (gTaskHsmmpContext.sendCloudCnt == 0){
		snd.boolBackCloud = TRUE;
	}else{
		snd.boolBackCloud = FALSE;
	}
	snd.length = sizeof(msg_struct_hsmmp_avorion_data_read_t);
	ret = hcu_message_send(MSG_ID_HSMMP_AVORION_DATA_READ, TASK_ID_AVORION, TASK_ID_HSMMP, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[TASK_ID_AVORION].taskName);
		return FAILURE;
	}

	//设置新状态
	FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED_WFFB) ;
	return SUCCESS;
}

OPSTAT func_hsmmp_time_out_period_curl_picture(void)
{
	int ret=0;

	//在线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		msg_struct_picture_cloudvela_data_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_picture_cloudvela_data_report_t));
		snd.length = sizeof(msg_struct_picture_cloudvela_data_report_t);
		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");
/*
		//LINK层参数
		snd.link.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
		snd.link.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
		snd.link.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
		snd.link.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
		snd.link.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;
		snd.link.timeStampStart = time(0);
		snd.link.timeStampEnd = time(0);
*/
		//强行注入参数
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.flag = HUITP_IEID_UNI_PICTURE_IND_FLAG_REMOTE_CURL_TRIGGER;

		ret = hcu_message_send(MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_HSMMP, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
			return FAILURE;
		}
	}

	return SUCCESS;
}


//不支持多个传感器同时工作，这里只架设有一个摄像头
//如果超时，说明摄像头工作不太正常，记录在案，并停止摄像头工作即可，其它的也不用干啥
OPSTAT func_hsmmp_time_out_wait_for_cammera_fb(void)
{
	int ret=0;

	//先不判断状态，因为超时通常都是不正常才能到达这儿
	HcuDebugPrint("AVORION: Avorion doesn't work properly, time out reached before return formally!\n");

	//发送停止命令给摄像头，防止其继续工作
	msg_struct_hsmmp_avorion_stop_t snd;
	memset(&snd, 0, sizeof(msg_struct_hsmmp_avorion_stop_t));
	snd.length = sizeof(msg_struct_hsmmp_avorion_stop_t);
	ret = hcu_message_send(MSG_ID_HSMMP_AVORION_STOP, TASK_ID_AVORION, TASK_ID_HSMMP, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName, zHcuVmCtrTab.task[TASK_ID_AVORION].taskName);
		return FAILURE;
	}

	//恢复状态机
	FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED);
	return SUCCESS;
}


/****************HKvision photo operation API******************/

OPSTAT hcu_hsmmp_video_capture_start(const HKVisionOption_t HKVisionOption)
{

	OPSTAT state;

	FILE *fp_write = fopen(HKVisionOption.file_video, "w");
	if(NULL == fp_write)
	{
		HcuErrorPrint("HSMMP: Open file failed at %s\n\n", HKVisionOption.file_video);
		return FAILURE;
	}
	else
	{
		HcuDebugPrint("HSMMP: Open file succeed at %s\n\n", HKVisionOption.file_video);
		fclose(fp_write);

	}

    //获取要上传的文件指针
    FILE* fp_read = fopen(HKVisionOption.file_video, "rb");
	if(NULL == fp_read)
	{
		HcuErrorPrint("HSMMP: Open file failed at %s\n\n", HKVisionOption.file_video);
		return FAILURE;
	}
	else
	{
		HcuDebugPrint("HSMMP: Open file succeed at %s\n\n", HKVisionOption.file_video);
	}

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	if(NULL == curl)
	{
		HcuErrorPrint("HSMMP: Init curl failed!\n");
	    fclose(fp_read);
	    curl_global_cleanup();
	    curl_easy_cleanup(curl);
		return FAILURE;
	}

    // 获取文件大小
    fseek(fp_read, 0, 2);
    int file_size = ftell(fp_read);
    rewind(fp_read);

    curl_easy_setopt(curl, CURLOPT_URL, HKVisionOption.url_video_start); //获取URL地址
    curl_easy_setopt(curl, CURLOPT_USERPWD, HKVisionOption.user_key);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1); //告诉easy_handle是做上传操作
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &hcu_hsmmp_write_data_callback); //调用重写的读文件流函数
    curl_easy_setopt(curl, CURLOPT_READDATA, fp_read); //往read_file()函数中传入用户自定义的数据类型
    curl_easy_setopt(curl, CURLOPT_INFILE, fp_read); //定位作为上传的输入文件
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印出具体http协议字段
    //curl_easy_setopt(easy_handle, CURLOPT_HEADER, 1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size); //上传的字节数

	CURLcode curlRes = curl_easy_perform(curl);

	if(CURLE_OK == curlRes){
		state = SUCCESS;
		HcuDebugPrint("HSMMP: curl perform success [%d]\n\n", curlRes);
	}
	else{
		state = FAILURE;
		HcuDebugPrint("HSMMP: curl perform failure [%d]\n\n", curlRes);
	}

	int HTTP_flag = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, & HTTP_flag);
	HcuDebugPrint("HSMMP: HTTP_flag is [%d]\n\n", HTTP_flag);

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	fclose(fp_read);
	return state;

}

OPSTAT hcu_hsmmp_video_capture_stop(const HKVisionOption_t HKVisionOption)
{
	OPSTAT state;

    //获取要上传的文件指针
    FILE* fp = fopen(HKVisionOption.file_video, "rb");
	if(NULL == fp)
	{
		HcuErrorPrint("HSMMP: Open file failed at %s\n\n", HKVisionOption.file_video);
		return FAILURE;
	}
	else
	{
		HcuDebugPrint("HSMMP: Open file succeed at %s\n\n", HKVisionOption.file_video);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if(NULL == curl)
	{
		HcuErrorPrint("HSMMP: Init curl failed!\n");
	    fclose(fp);
	    curl_global_cleanup();
	    curl_easy_cleanup(curl);
		return FAILURE;
	}

    // 获取文件大小
    fseek(fp, 0, 2);
    int file_size = ftell(fp);
    rewind(fp);

    curl_easy_setopt(curl, CURLOPT_URL, HKVisionOption.url_video_stop); //获取URL地址
    curl_easy_setopt(curl, CURLOPT_USERPWD, HKVisionOption.user_key);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1); //告诉easy_handle是做上传操作
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &hcu_hsmmp_write_data_callback); //调用重写的读文件流函数
    curl_easy_setopt(curl, CURLOPT_READDATA, fp); //往read_file()函数中传入用户自定义的数据类型
    curl_easy_setopt(curl, CURLOPT_INFILE, fp); //定位作为上传的输入文件
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印出具体http协议字段
    //curl_easy_setopt(easy_handle, CURLOPT_HEADER, 1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size); //上传的字节数

	CURLcode curlRes = curl_easy_perform(curl);

	if(CURLE_OK == curlRes){
		state = SUCCESS;
		HcuDebugPrint("HSMMP: curl perform success [%d]\n\n", curlRes);
	}
	else{
		state = FAILURE;
		HcuDebugPrint("HSMMP: curl perform failure [%d]\n\n", curlRes);
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	fclose(fp);
	return state;

}

size_t hcu_hsmmp_write_data_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t writen = fwrite(ptr,size,nmemb,stream);
	return writen;
}

OPSTAT hcu_hsmmp_photo_capture_start(const HKVisionOption_t HKVisionOption)
{
	OPSTAT state;
	int ret = 0;

	HcuDebugPrint("HSMMP: HKVisionOption.user_key: %s \n", HKVisionOption.user_key);
	HcuDebugPrint("HSMMP: HKVisionOption.url_photo: %s \n", HKVisionOption.url_photo);
	HcuDebugPrint("HSMMP: HKVisionOption.url_video_start: %s \n", HKVisionOption.url_video_start);
	HcuDebugPrint("HSMMP: HKVisionOption.url_video_stop: %s \n", HKVisionOption.url_video_stop);
	HcuDebugPrint("HSMMP: HKVisionOption.file_photo: %s \n", HKVisionOption.file_photo);
	HcuDebugPrint("HSMMP: HKVisionOption.file_photo_pure: %s \n", HKVisionOption.file_photo_pure);
	HcuDebugPrint("HSMMP: HKVisionOption.file_video: %s \n\n", HKVisionOption.file_video);

	FILE *fp = fopen(HKVisionOption.file_photo, "w");
	if(NULL == fp)
	{
		HcuErrorPrint("HSMMP: Open file failed at %s\n\n", HKVisionOption.file_photo);
		return FAILURE;
	}

	else
	{
		HcuDebugPrint("HSMMP: Open file succeed at %s\n\n", HKVisionOption.file_photo);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if(NULL == curl)
	{
		HcuErrorPrint("HSMMP: Init curl failed!\n");
		return FAILURE;
	}

	curl_easy_setopt(curl, CURLOPT_URL, HKVisionOption.url_photo);
	curl_easy_setopt(curl, CURLOPT_USERPWD, HKVisionOption.user_key);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, hcu_hsmmp_write_data_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	CURLcode curlRes = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	fclose(fp);

	if(CURLE_OK == curlRes){
		state = SUCCESS;
		HcuDebugPrint("HSMMP: curl perform success [%d]\n\n", curlRes);
	}
	else{
		state = FAILURE;
		HcuDebugPrint("HSMMP: curl perform failure [%d]\n\n", curlRes);
		ret = remove(HKVisionOption.file_photo);
		if(SUCCESS != ret){
			HcuErrorPrint("HSMMP: Remove photo file failure when photo capture not success !\n");
		}

	}


	return state;
}


void hcu_hsmmp_set_hkvision_option(void)
{

/*
	char usrtmp[3] = ":";
	char usrtmp1[3] ="/";
	strcat(HsmmpOption.user_key, "admin");
	strcat(HsmmpOption.user_key, usrtmp);
	strcat(HsmmpOption.user_key, "Bxxh!123");

	//GET方法，开始拍照
	// curl -u admin:'Bxxh!123' -X GET -o test.jpg http://192.168.111.64/Streaming/channels/1/picture
	// curl -u admin:'Bxxh!123' -X GET -o test.jpg http://ngrok.hkrob.com:30101/Streaming/channels/1/picture

	//PUT方法，开始录像
	//curl -u admin:'Bxxh!123' -X PUT http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/start/tracks/1
	//curl -u admin:'Bxxh!123' -X PUT http://ngrok.hkrob.com:30101/ISAPI/ContentMgmt/record/control/manual/start/tracks/1

	//PUT方法，停止录像
	//curl -u admin:'Bxxh!123' -X PUT http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1
	//curl -u admin:'Bxxh!123' -X PUT http://ngrok.hkrob.com:30101/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1
	strcat(HsmmpOption.url_photo, "http://ngrok.hkrob.com:30101/Streaming/channels/1/picture");
	strcat(HsmmpOption.url_video_start, "http://ngrok.hkrob.com:30101/ISAPI/ContentMgmt/record/control/manual/start/tracks/1");
	strcat(HsmmpOption.url_video_stop, "http://ngrok.hkrob.com:30101/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1");

	strcat(HsmmpOption.file_photo, zCurTimeDate.curPhotoDir);
	strcat(HsmmpOption.file_photo, usrtmp1);
	strcat(HsmmpOption.file_photo, zCurTimeDate.curHikvisionFname);

	strcat(HsmmpOption.file_video, zCurTimeDate.curPhotoDir);
	strcat(HsmmpOption.file_video, usrtmp1);
	strcat(HsmmpOption.file_video, "temp.txt");

	HcuDebugPrint("PM25: HsmmpOption.user_key: %s \n", HsmmpOption.user_key);
	HcuDebugPrint("PM25: HsmmpOption.url_photo: %s \n", HsmmpOption.url_photo);
	HcuDebugPrint("PM25: HsmmpOption.url_video_start: %s \n", HsmmpOption.url_video_start);
	HcuDebugPrint("PM25: HsmmpOption.url_video_stop: %s \n", HsmmpOption.url_video_stop);
	HcuDebugPrint("PM25: HsmmpOption.file_photo: %s \n", HsmmpOption.file_photo);
	HcuDebugPrint("PM25: HsmmpOption.file_video: %s \n\n", HsmmpOption.file_video);
*/

}

