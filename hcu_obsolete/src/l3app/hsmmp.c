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
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"



/*
** FSM of the HSMMP
*/
HcuFsmStateItem_t FsmHsmmp[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_hsmmp_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_hsmmp_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_hsmmp_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_HSMMP_INITED,            		fsm_hsmmp_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_HSMMP_INITED,            		fsm_hsmmp_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_HSMMP_INITED,            		fsm_com_do_nothing},

    //Normal working status, 等待上层来的控制消息
    {MSG_ID_COM_RESTART,        			FSM_STATE_HSMMP_ACTIVED,          	fsm_hsmmp_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_HSMMP_ACTIVED,          	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_HSMMP_ACTIVED,          	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_HSMMP_ACTIVED,          	fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_HSMMP_ACTIVED,          	fsm_hsmmp_time_out},
	{MSG_ID_SPSVIRGO_HSMMP_DATA_RX,     	FSM_STATE_HSMMP_ACTIVED,          	fsm_hsmmp_audio_data_rx},
	{MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD,	FSM_STATE_HSMMP_ACTIVED,      	  	fsm_hsmmp_cloudvela_cmd},
	{MSG_ID_AVORION_HSMMP_DATA_RX,      	FSM_STATE_HSMMP_ACTIVED,         	fsm_hsmmp_avorion_data_rx},

	//来自CLOUD的控制协议，可以在不同的激活状态下起作用，但起作用必须等待下一轮硬件空闲的时候
    //Waiting for feedback from
    {MSG_ID_COM_RESTART,        			FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_hsmmp_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_hsmmp_time_out},
	{MSG_ID_SPSVIRGO_HSMMP_DATA_RX,       	FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_hsmmp_audio_data_rx},
	{MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD,    FSM_STATE_HSMMP_ACTIVED_WFFB,     	fsm_hsmmp_cloudvela_cmd},
	{MSG_ID_AVORION_HSMMP_DATA_RX,       	FSM_STATE_HSMMP_ACTIVED_WFFB,    	fsm_hsmmp_avorion_data_rx},
	{MSG_ID_AVORION_HSMMP_DATA_READ_FB,  	FSM_STATE_HSMMP_ACTIVED_WFFB,    	fsm_hsmmp_avorion_data_read_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global Variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表
extern HcuTimeDateTable_t zHcuVmCtrTab.clock;   //时间更新表

//用于描述发送到后台，多少次才发送一次
UINT32 zHcuHsmmpSendSaeCnt = 0;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_hsmmp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("HSMMP: Error Set FSM State at fsm_hsmmp_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_hsmmp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		//hcu_usleep(rand()%DURATION_OF_INIT_FB_WAIT_MAX);
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_HSMMP, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_INITED) == FAILURE){
		HcuErrorPrint("HSMMP: Error Set FSM State!\n");
		return FAILURE;
	}

	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("HSMMP: Enter FSM_STATE_HSMMP_INITED status, Keeping refresh here!\n");
	}

	//Task global variables init.
	zHcuHsmmpSendSaeCnt = 0;
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP] = 0;

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_HSMMP, TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ, zHcuSysEngPar.timer.hsmmpReqTimer,
			TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error start timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	//State Transfer to FSM_STATE_HSMMP_ACTIVED
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
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
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP], zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP]);
			return FAILURE;
		}
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_period();
	}

	//定时短时钟进行离线数据回送
	else if ((rcv.timeId == TIMER_ID_1S_HSMMP_AVORION_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_wait_for_cammera_fb();
	}
/*
	//For disk size checking by shanchun
	if ((rcv.timeId == TIMER_ID_1S_HSMMP_FILESIZE_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_hsmmp_time_out_disksize();
	}
*/

	//这里的ret=FAILURE並不算严重，只不过造成状态机返回差错而已，并不会造成程序崩溃和数据混乱，所以只是程序的自我保护而已
	return ret;

}

OPSTAT fsm_hsmmp_audio_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_hsmmp_cloudvela_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_hsmmp_avorion_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_hsmmp_avorion_data_read_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret = 0;
	char newpath[HCU_FILE_NAME_LENGTH_MAX]="";
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	//检查状态
	if (FsmGetState(TASK_ID_HSMMP) != FSM_STATE_HSMMP_ACTIVED_WFFB){
		if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Error Set FSM State!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("AVORION: Recover to ACTIVED state during period timer out!\n");
		}
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
	if ((rcv.hsmmp.hsmmpFname == NULL) || ((strlen(rcv.hsmmp.hsmmpFname) > HCU_FILE_NAME_LENGTH_MAX))){
		HcuErrorPrint("HSMMP: Error hsmmpFname parameter message received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		return FAILURE;
	}
	if ((rcv.hsmmp.hsmmpFdir == NULL) || ((strlen(rcv.hsmmp.hsmmpFdir) > HCU_FILE_NAME_LENGTH_MAX))){
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
			record.gpsx = zHcuGpsPosInfo.gpsX;
			record.gpsy = zHcuGpsPosInfo.gpsY;
			record.gpsz = zHcuGpsPosInfo.gpsZ;
			record.ew = zHcuGpsPosInfo.EW;
			record.ns = zHcuGpsPosInfo.NS;
			//RECORD存入内存盘
			if (HCU_SENSOR_DATA_SAVE_TO_MEMDISK_SET == HCU_MEM_SENSOR_SAVE_FLAG_YES)
			{
				ret = hcu_save_to_storage_mem(&record);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
					HcuErrorPrint("HSMMP: Can not save data into memory buffer, might par error!\n");
				}
			}
			//RECORD存入硬盘
			if (HCU_SENSOR_DATA_SAVE_TO_FLASH_DISK_SET == HCU_DISC_SENSOR_SAVE_FLAG_YES)
			{
				ret = hcu_save_to_storage_disc(FILE_OPERATION_TYPE_SENSOR, &record, sizeof(HcuDiscDataSampleStorageArray_t));
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
					HcuErrorPrint("HSMMP: Can not save data into hard disk!\n");
				}
			}
			//RECORD还要存入数据库
			if (HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_DB_SENSOR_SAVE_FLAG_YES)
			{
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
				//printf("HSMMP: hsmmpData address = 0x%08x\n", (UINT32)&hsmmpData);
				//printf("HSMMP: hsmmpFdir = %s, hsmmpFname = %s, hsmmpLink = %s\n", hsmmpData.hsmmpFdir, hsmmpData.hsmmpFname, hsmmpData.hsmmpLink);
				ret = dbi_HcuHsmmpDataInfo_save(&hsmmpData);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
					HcuErrorPrint("HSMMP: Can not save data into database!\n");
				}
			}
			//瞬时模式，待完善
		}
		//在线模式
		else if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
			msg_struct_hsmmp_cloudvela_data_link_resp_t snd;
			memset(&snd, 0, sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t));
			snd.length = sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t);
			strcpy(snd.link.linkName, newpath);
			snd.link.linkLen = strlen(newpath);

			//考虑到AVORION将放到外部单独进程中，GPS时钟无法共享，故而GPS数据的采集直接放到HSMMP模块中，而不放到传感器中
			snd.link.gps.gpsx = zHcuGpsPosInfo.gpsX;
			snd.link.gps.gpsy = zHcuGpsPosInfo.gpsY;
			snd.link.gps.gpsz = zHcuGpsPosInfo.gpsZ;
			snd.link.gps.ew = zHcuGpsPosInfo.EW;
			snd.link.gps.ns = zHcuGpsPosInfo.NS;
			snd.link.timeStampStart = rcv.hsmmp.timeStamp;
			snd.link.timeStampEnd = time(0);

			//强行注入参数，以方便CLOUT中的编码，未来待完善
			snd.usercmdid = L3CI_hsmmp;
			snd.useroptid = L3PO_hsmmp_data_report;
			snd.link.equipid = rcv.hsmmp.equipid; //如果是0，可能会被L2编码拒绝，未来再优化完善
			snd.cmdIdBackType = L3CI_cmdid_back_type_period;
			//ZHB格式的参数应该在这里一并填入
			//snd.cn =
			ret = hcu_message_send(MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP, TASK_ID_CLOUDVELA, TASK_ID_HSMMP, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
				HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return FAILURE;
			}

			//Save to disk as request：在线是为了备份，离线是为了重发给后台
			//完整的处理情形，有待完成，存入磁盘还为实现
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.hsmmp.equipid;
			record.sensortype = L3CI_hsmmp;
			record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
			record.timestamp = rcv.hsmmp.timeStamp;
			strcpy(record.hsmmpFdir, rcv.hsmmp.hsmmpFdir);
			strcpy(record.hsmmpFname, rcv.hsmmp.hsmmpFname);
			strcpy(record.hsmmpLink, newpath); //都是以HCU_FILE_NAME_LENGTH_MAX长度为限
			record.gpsx = zHcuGpsPosInfo.gpsX;
			record.gpsy = zHcuGpsPosInfo.gpsY;
			record.gpsz = zHcuGpsPosInfo.gpsZ;
			record.ew = zHcuGpsPosInfo.EW;
			record.ns = zHcuGpsPosInfo.NS;
			//RECORD存入内存盘
			if (HCU_SENSOR_DATA_SAVE_TO_MEMDISK_SET == HCU_MEM_SENSOR_SAVE_FLAG_YES)
			{
				ret = hcu_save_to_storage_mem(&record);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
					HcuErrorPrint("HSMMP: Can not save data into memory buffer, might par error!\n");
				}
			}
			//RECORD存入硬盘
			if (HCU_SENSOR_DATA_SAVE_TO_FLASH_DISK_SET == HCU_DISC_SENSOR_SAVE_FLAG_YES)
			{
				ret = hcu_save_to_storage_disc(FILE_OPERATION_TYPE_SENSOR, &record, sizeof(HcuDiscDataSampleStorageArray_t));
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
					HcuErrorPrint("HSMMP: Can not save data into hard disk!\n");
				}
			}
			//RECORD还要存入数据库
			if (HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_DB_SENSOR_SAVE_FLAG_YES)
			{
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
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//长周期，定时工作模式
OPSTAT func_hsmmp_time_out_period(void)
{
	int ret=0;

	//判定状态机是否可以发送，如果不在ACTIVE状态，意味着一定有啥差错，强行恢复工作状态，等待下一次机会
	if (FsmGetState(TASK_ID_HSMMP) != FSM_STATE_HSMMP_ACTIVED){
		if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Error Set FSM State!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("AVORION: Recover to ACTIVED state during period timer out!\n");
		}


		//发送INIT消息给AVORION任务
		/*msg_struct_com_init_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_init_t));
		snd.length = sizeof(msg_struct_com_init_t);
		ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_AVORION, TASK_ID_HSMMP, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_HSMMP]++;
			HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_HSMMP], zHcuTaskNameList[TASK_ID_AVORION]);
			return FAILURE;
		}
		return SUCCESS;*/
	}

	//发送命令给摄像头
	msg_struct_hsmmp_avorion_data_read_t snd;
	memset(&snd, 0, sizeof(msg_struct_hsmmp_avorion_data_read_t));
	snd.equipid = HSMMP_EQUIPMENT_ID_DEFAULT; //虽然用处不大，对为了方便设备的存储和数据库操作，还是留着该域
	snd.captureDur = zHcuSysEngPar.timer.hsmmpCapDuration;
	snd.fileType = FILE_OPERATION_TYPE_AVORION_H264;
	snd.refreshRate = zHcuSysEngPar.timer.hsmmpRefRate;
	snd.timeStampStart = time(0);
	strcpy(snd.tmpFname, "output.yuv");

    if ((snd.fileType == FILE_OPERATION_TYPE_AVORION_H264) && (strlen(zHcuVmCtrTab.clock.curAvorionFdH264) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdH264);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameH264);
    }else if((snd.fileType == FILE_OPERATION_TYPE_AVORION_AVI) && (strlen(zHcuVmCtrTab.clock.curAvorionFdAvi) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdAvi);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameAvi);
    }else if((snd.fileType == FILE_OPERATION_TYPE_AVORION_MKV) && (strlen(zHcuVmCtrTab.clock.curAvorionFdMkv) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(snd.fDirName, zHcuVmCtrTab.clock.curAvorionFdMkv);
    	strcpy(snd.fName, zHcuVmCtrTab.clock.curAvorionFnameMkv);
    }else{
        HcuErrorPrint("AVORION: Not set save file type correctly.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
        return FAILURE;
    }
	//发送后台的控制器，目前暂时是以10次回送一次的方式，未来可以由服务器后台控制
    zHcuHsmmpSendSaeCnt++;
    zHcuHsmmpSendSaeCnt = (zHcuHsmmpSendSaeCnt % HSMMP_SEND_BACK_MOD_BASE);
	if (zHcuHsmmpSendSaeCnt == 0){
		snd.boolBackCloud = TRUE;
	}else{
		snd.boolBackCloud = FALSE;
	}
	snd.length = sizeof(msg_struct_hsmmp_avorion_data_read_t);
	ret = hcu_message_send(MSG_ID_HSMMP_AVORION_DATA_READ, TASK_ID_AVORION, TASK_ID_HSMMP, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP], zHcuSysCrlTab.taskRun.taskName[TASK_ID_AVORION]);
		return FAILURE;
	}

	//设置新状态
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED_WFFB) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error Set FSM State!\n");
		return FAILURE;
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
		HcuErrorPrint("HSMMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HSMMP], zHcuSysCrlTab.taskRun.taskName[TASK_ID_AVORION]);
		return FAILURE;
	}

	//恢复状态机
	if (FsmSetState(TASK_ID_HSMMP, FSM_STATE_HSMMP_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HSMMP]++;
		HcuErrorPrint("HSMMP: Error Set FSM State!\n");
		return FAILURE;
	}
	return SUCCESS;
}

