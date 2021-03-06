/*
 * sensorpm25.c
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#include "sensorpm25.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"
#include "../l3app/hsmmp.h"


/*
** FSM of the PM25
*/
HcuFsmStateItem_t HcuFsmPm25[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_pm25_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_pm25_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_PM25_INITED,            		fsm_pm25_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_PM25_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_pm25_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_pm25_time_out},

    //Normal working status
	{MSG_ID_CLOUDVELA_PM25_DATA_REQ,       	FSM_STATE_PM25_ACTIVED,          	fsm_pm25_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM,   	FSM_STATE_PM25_ACTIVED,      		fsm_pm25_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_PM25_CTRL_REQ,     	FSM_STATE_PM25_ACTIVED,         	fsm_pm25_cloudvela_ctrl_req},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,			FSM_STATE_PM25_ACTIVED,      	  	fsm_pm25_l3aqyc_exg_ctrl_req},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,			FSM_STATE_PM25_ACTIVED,      	  	fsm_pm25_zhbl3mod_exg_ctrl_req},


    //Wait for Modbus Feedback
	{MSG_ID_MODBUS_PM25_DATA_REPORT,    FSM_STATE_PM25_OPT_WFFB,         fsm_pm25_data_report_from_modbus},
	{MSG_ID_MODBUS_PM25_CONTROL_FB,     FSM_STATE_PM25_OPT_WFFB,         fsm_pm25_modbus_control_fb},

	//added by Shanchun to ensure not blocking the next command in cast no response from the last command
	{MSG_ID_CLOUDVELA_PM25_CTRL_REQ, FSM_STATE_PM25_OPT_WFFB,         fsm_pm25_cloudvela_ctrl_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskPm25Context_t gTaskPm25Context;
gTaskHsmmpContext_t gTaskHsmmpContext;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_pm25_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_PM25, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_pm25_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_PM25, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_INITED);
	HCU_DEBUG_PRINT_FAT("PM25: Enter FSM_STATE_PM25_INITED status, everything goes well!\n");

	//Task global variables init.
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] = 0;
	memset(&gTaskPm25Context, 0, sizeof(gTaskPm25Context_t));
	gTaskPm25Context.AlarmFlag = FALSE;
	//目前暂时只有一个PM25传感器，但程序的框架可以支持无数个传感器
	//未来还需要支持传感器的地址可以被配置，随时被修改，通过后台命令
	for (i=0;i<MAX_NUM_OF_SENSOR_PM25_INSTALLED;i++){
		gTaskPm25Context.pm25[i].sensorId = i;


		gTaskPm25Context.pm25[i].equId = MODBUS_PM25_RTU_EQUIPMENT_ID_PMS;

		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)//for PMS
		{
			gTaskPm25Context.pm25[i].equId = MODBUS_PM25_RTU_EQUIPMENT_ID;
		}
		/*else
		{
			gTaskPm25Context.pm25[i].equId = MODBUS_PM25_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
		}*/

		gTaskPm25Context.pm25[i].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
		gTaskPm25Context.pm25[i].hwStatus = SENSOR_PM25_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskPm25Context.pm25[i].busyCount = 0;
	}

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	hcu_timer_start(TASK_ID_PM25, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_PM25_PERIOD_READ), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//State Transfer to FSM_STATE_PM25_ACTIVED
	FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
	return SUCCESS;
}

OPSTAT fsm_pm25_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("PM25: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_pm25_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_pm25_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("PM25: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_PM25]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_PM25, TASK_ID_PM25, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);
			return FAILURE;
		}
	}

	//Period time out received, send command to MODBUS for read
	if ((rcv.timeId == TIMER_ID_1S_PM25_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_PM25) != FSM_STATE_PM25_ACTIVED){
			FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
		}

#ifdef TARGET_RASPBERRY_PI3B
		//if ((SENSOR_PM25_RPI_BMPD300_PRESENT == SENSOR_PM25_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_BMPD300 == HCU_SENSOR_PRESENT_YES)) func_pm25_time_out_read_data_from_bmpd300();
		//if ((SENSOR_PM25_RPI_SHARP_PRESENT == SENSOR_PM25_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_SHARP == HCU_SENSOR_PRESENT_YES)) func_pm25_time_out_read_data_from_sharp();
#endif
		func_pm25_time_out_read_data_from_modbus();
	}

	//Modbus received FB time out message received
	else if ((rcv.timeId == TIMER_ID_1S_PM25_MODBUS_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_pm25_time_out_processing_no_rsponse();
	}

	return SUCCESS;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_pm25_time_out_read_data_from_modbus(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess == SENSOR_PM25_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount < 0) || (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount >= SENSOR_PM25_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess =SENSOR_PM25_HW_ACCESS_IDLE;
		}else{
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess == SENSOR_PM25_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to MODBUS
		msg_struct_pm25_modbus_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_pm25_modbus_data_read_t));
		snd.length = sizeof(msg_struct_pm25_modbus_data_read_t);
		snd.equId = gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].equId;//Shanchun:to check if it is inline with Sensor equId
		snd.cmdId = L3CI_pm25;
		snd.optId = L3PO_pm25_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_PM25_MODBUS_DATA_READ, TASK_ID_MODBUS, TASK_ID_PM25, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName);
			return;
		}

		//启动一次性定时器
		hcu_timer_start(TASK_ID_PM25, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_PM25_MODBUS_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_BUSY;
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_PM25_OPT_WFFB
		FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_OPT_WFFB);
	}//SENSOR_PM25_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwStatus = SENSOR_PM25_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;
	}

	return;
}

//超时失败，走向下一个传感器
void func_pm25_time_out_processing_no_rsponse(void)
{
	//恢复当前传感器的空闲状态
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwStatus = SENSOR_PM25_HW_STATUS_DEACTIVE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//CurrentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskPm25Context.currentSensorId < 0) || (gTaskPm25Context.currentSensorId >= (MAX_NUM_OF_SENSOR_PM25_INSTALLED-1))){
		gTaskPm25Context.currentSensorId = 0;
	}else{
		gTaskPm25Context.currentSensorId++;
	}

	//暂时啥也不干，未来在瞬时模式下也许需要回一个失败的消息，当然缺省情况下没有反应就是表示失败

	//State Transfer to FSM_STATE_PM25_ACTIVE
	HcuDebugPrint("PM25: GET TIME OUT, FSM STATE SET TO %d !\n", FSM_STATE_PM25_ACTIVED);

	FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
	return;
}


//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_pm25_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
//    char file[200];
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("PM25: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	hcu_timer_stop(TASK_ID_PM25, TIMER_ID_1S_PM25_MODBUS_FB, TIMER_RESOLUTION_1S);
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
	//判断如果PM2.5超过阀值，若超过，则需要设alarm flag = ON, 启动拍照和录像，并触发告警，告警报告中需要包括告警类型，告警内容，及需要上传照片的文件名（包含设备名字日期时间）和录像的开始日期、时间和停止的日期、时间。
	//rcv.pm25.pmTSPValue = 30001;

	gTaskPm25Context.TSP = (float)rcv.pm25.pmTSPValue;
	HCU_DEBUG_PRINT_INF("PM25: TSP = %.1f\n\n", gTaskPm25Context.TSP);

	gTaskPm25Context.PM01Value = (float)rcv.pm25.pm1d0Value;
	HCU_DEBUG_PRINT_INF("PM25: PM1.0 = %.1f\n\n", gTaskPm25Context.PM01Value);

	gTaskPm25Context.PM25Value = (float)rcv.pm25.pm2d5Value;
	HCU_DEBUG_PRINT_INF("PM25: PM2.5 = %.1f\n\n", gTaskPm25Context.PM25Value);

	gTaskPm25Context.PM10Value = (float)rcv.pm25.pm10Value;
	HCU_DEBUG_PRINT_INF("PM25: PM10 = %.1f\n\n", gTaskPm25Context.PM10Value);


	if(rcv.pm25.pmTSPValue >= (HCU_SENSOR_PM25_VALUE_ALARM_THRESHOLD*10000))
	//if(rcv.pm25.pmTSPValue >= zHcuSysEngPar.serialport.SeriesPortForGPS) //for debug
	{
		if(FALSE == gTaskPm25Context.AlarmFlag)
		{
			gTaskHsmmpContext.Hsmmp_flag = TRUE;
		}

		gTaskPm25Context.AlarmFlag = TRUE;
/*
		//send alarm report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.pm25.equipid;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_TSP_VALUE;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_TSP_VALUE_EXCEED_THRESHLOD;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_PM25, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_PM25, "PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
*/
	}

	//若没超过阀值，而且alarm flag = TRUE, 则将alarm flag = FALSE，停止拍照和录像，然后需要发告警清除报告
	if((rcv.pm25.pmTSPValue <= HCU_SENSOR_PM25_VALUE_ALARM_THRESHOLD*10000) && (gTaskPm25Context.AlarmFlag == TRUE))
	//if((rcv.pm25.pmTSPValue <= zHcuSysEngPar.serialport.SeriesPortForGPS) && (gTaskPm25Context.AlarmFlag == TRUE)) //for debug

	{
		gTaskPm25Context.AlarmFlag = FALSE;
		gTaskHsmmpContext.Hsmmp_flag = FALSE;
/*
		//send alarm clear report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.pm25.equipid;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_TSP_VALUE;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_TSP_VALUE_EXCEED_THRESHLOD;
		//strcpy(snd.alarmDesc, HKVisionOption.file_photo_pure);

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_PM25, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_PM25, "PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
*/
	}
/*
	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.pm25.equipid;
			record.sensortype = L3CI_pm25;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.pm25.timeStamp;
			record.dataFormat = rcv.pm25.dataFormat;
			record.pm1d0Value = rcv.pm25.pm1d0Value;
			record.pm2d5Value = rcv.pm25.pm2d5Value;
			record.pm10Value = rcv.pm25.pm10Value;
			record.gpsx = rcv.pm25.gps.gpsx;
			record.gpsy = rcv.pm25.gps.gpsy;
			record.gpsz = rcv.pm25.gps.gpsz;
			record.ew = rcv.pm25.gps.ew;
			record.ns = rcv.pm25.gps.ns;
			//RECORD还要存入数据库
			sensor_pm25_data_element_t pm25Data;
			memset(&pm25Data, 0, sizeof(sensor_pm25_data_element_t));
			pm25Data.equipid = record.equipid;
			pm25Data.timeStamp = record.timestamp;
			pm25Data.dataFormat = record.dataFormat;
			pm25Data.pm1d0Value = record.pm1d0Value;
			pm25Data.pm2d5Value = record.pm2d5Value;
			pm25Data.pm10Value = record.pm10Value;
			pm25Data.gps.gpsx = record.gpsx;
			pm25Data.gps.gpsy = record.gpsy;
			pm25Data.gps.gpsz = record.gpsz;
			pm25Data.gps.ew = record.ew;
			pm25Data.gps.ns = record.ns;
			pm25Data.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuPm25DataInfo_save(&pm25Data);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
				HcuErrorPrint("PM25: Can not save data into database!\n");
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
				HcuErrorPrint("PM25: Offline but instance or other control message received!\n");
			}
	}
*/

	if(zHcuSysEngPar.hwBurnId.hwType != HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)
	{
		//在线模式
		if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE))
		{
			///////////
			//L2信息
			msg_struct_pm25_cloudvela_data_resp_t snd;
			memset(&snd, 0, sizeof(msg_struct_pm25_cloudvela_data_resp_t));
			strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
				sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
			strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
					sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
			snd.comHead.timeStamp = time(0);
			snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
			//snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID;
			strcpy(snd.comHead.funcFlag, "0");

			//CONTENT
			snd.baseResp = HUITP_IEID_UNI_COM_REPORT_YES;
			snd.usercmdid = rcv.usercmdid;
			snd.cmdIdBackType = rcv.cmdIdBackType;
			snd.useroptid = rcv.useroptid;

			snd.pm25.equipid = rcv.pm25.equipid;
			snd.pm25.timeStamp = rcv.pm25.timeStamp;
			snd.pm25.dataFormat = rcv.pm25.dataFormat;
			snd.pm25.pmTSPValue = rcv.pm25.pmTSPValue;
			snd.pm25.pm2d5Value = rcv.pm25.pm2d5Value;
			snd.pm25.pm10Value = rcv.pm25.pm10Value;
			snd.pm25.gps.gpsx = rcv.pm25.gps.gpsx;
			snd.pm25.gps.gpsy = rcv.pm25.gps.gpsy;
			snd.pm25.gps.gpsz = rcv.pm25.gps.gpsz;
			snd.pm25.gps.ew = rcv.pm25.gps.ew;
			snd.pm25.gps.ns = rcv.pm25.gps.ns;
			snd.pm25.nTimes = rcv.pm25.nTimes;
			snd.pm25.onOffLineFlag = rcv.pm25.onOffLineFlag;
			snd.length = sizeof(msg_struct_pm25_cloudvela_data_resp_t);

			//发送后台
			//if ((HCU_SYSCFG_SENSOR_REPORT_MODE_SET & HCU_SYSCFG_SENSOR_REPORT_MODE_GROUP) == TRUE){
			if (HCU_SYSCFG_SENSOR_REPORT_MODE_SET == HCU_SYSCFG_SENSOR_REPORT_MODE_INDIVIDUAL){
				ret = hcu_message_send(MSG_ID_PM25_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_PM25, &snd, snd.length);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
					HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
					return FAILURE;
				}
			}

			//Save to disk as request：在线是为了备份，离线是为了重发给后台
			//该函数，有待完成
			if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
				//存入内存缓冲磁盘，做为本地缓存，未来需要实现磁盘模式
				memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
				record.equipid = rcv.pm25.equipid;
				record.sensortype = L3CI_pm25;
				record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
				record.timestamp = rcv.pm25.timeStamp;
				record.dataFormat = rcv.pm25.dataFormat;
				record.pmTSPValue = rcv.pm25.pmTSPValue;
				record.pm2d5Value = rcv.pm25.pm2d5Value;
				record.pm10Value = rcv.pm25.pm10Value;
				record.gpsx = rcv.pm25.gps.gpsx;
				record.gpsy = rcv.pm25.gps.gpsy;
				record.gpsz = rcv.pm25.gps.gpsz;
				record.ew = rcv.pm25.gps.ew;
				record.ns = rcv.pm25.gps.ns;
				//RECORD还要存入数据库
				sensor_pm25_data_element_t pm25Data;
				memset(&pm25Data, 0, sizeof(sensor_pm25_data_element_t));
				pm25Data.equipid = record.equipid;
				pm25Data.timeStamp = record.timestamp;
				pm25Data.dataFormat = record.dataFormat;
				pm25Data.pmTSPValue = record.pmTSPValue;
				pm25Data.pm2d5Value = record.pm2d5Value;
				pm25Data.pm10Value = record.pm10Value;
				pm25Data.gps.gpsx = record.gpsx;
				pm25Data.gps.gpsy = record.gpsy;
				pm25Data.gps.gpsz = record.gpsz;
				pm25Data.gps.ew = record.ew;
				pm25Data.gps.ns = record.ns;
				pm25Data.onOffLineFlag = record.onOffLine;
				ret = dbi_HcuPm25DataInfo_save(&pm25Data);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
					HcuErrorPrint("PM25: Can not save data into database!\n");
				}
			}// Period mode OK!
			// Instance mode, no need store!
		}

		//差错情形
		else{
			HcuErrorPrint("PM25: Wrong state of CLOUDVELA when data need send out!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
			//If this shall work normally, it is too much for each sensor STM process!
			return FAILURE;
		}
	}

	//恢复当前传感器的空闲状态
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwStatus = SENSOR_PM25_HW_STATUS_ACTIVE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskPm25Context.currentSensorId < 0) || (gTaskPm25Context.currentSensorId >= (MAX_NUM_OF_SENSOR_PM25_INSTALLED-1))){
		gTaskPm25Context.currentSensorId = 0;
	}else{
		gTaskPm25Context.currentSensorId++;
	}

	//State Transfer to FSM_STATE_PM25_ACTIVE
	FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
	return SUCCESS;
}

//读取源泉，这个可能是控制命令，也可能是数据瞬时读取命令
//状态机方面，会有点交叉混淆的
//瞬时模式，有待完成，初期的必要性不大
OPSTAT fsm_pm25_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_pm25_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_pm25_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_pm25_data_req_t))){
		HcuErrorPrint("PM25: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//找到EquId对应的信息，判断其是否在被处理，如果空闲，可以下发命令，否则丢弃该命令

	//瞬时读取模式，待处理，可能需要在控制命令字中增加一个属性来控制

	//考虑跟周期读取模式的冲突问题：有可能发送命令到下面，下面还未返回之前，状态被周期命令抢走了，
	//这就是一次瞬时命令丢失而已，需要上层来解决，因为本层的状态一旦改回到ACTIVE，下面的结果也送不上来，这正是我们所想要的结果

	//还要区分属于控制命令，还是读取数据命令

	//启动定时器

	//以上这些都处理全了，这个模块基本上就做全了

	return SUCCESS;
}

OPSTAT fsm_pm25_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_pm25_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Added by Shanchun for control cmd
	int ret;

	msg_struct_cloudvela_pm25_ctrl_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_pm25_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_pm25_ctrl_req_t))){
		HcuErrorPrint("PM25: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
		HcuDebugPrint("PM25: receive data from cloudvela, data param_len=%d \n", param_len);
	}

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_PM25]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_PM25, TASK_ID_PM25, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);
			return FAILURE;
		}
	}

	if (FsmGetState(TASK_ID_PM25) != FSM_STATE_PM25_ACTIVED){
		FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
	}

	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess == SENSOR_PM25_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount < 0) || (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount >= SENSOR_PM25_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess =SENSOR_PM25_HW_ACCESS_IDLE;
		}else{
			gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess == SENSOR_PM25_HW_ACCESS_IDLE){

		//Send out PM25 control cmd message to MODBUS
		msg_struct_pm25_modbus_control_cmd_t snd;
		memset(&snd, 0, sizeof(msg_struct_pm25_modbus_control_cmd_t));
		snd.length = sizeof(msg_struct_pm25_modbus_control_cmd_t);
		snd.cmdId = L3CI_pm25;
		snd.optId = rcv.optId;
		snd.backType = rcv.backType;//L3CI_cmdid_back_type_control
		snd.opt.equId = rcv.opt.equId;

		switch(rcv.optId)
		{
		    case L3PO_pm25_set_switch:
		    	snd.opt.powerOnOff = rcv.opt.powerOnOff;
		    	break;
		    case L3PO_pm25_set_modbus_address:
		    	snd.opt.newEquId = rcv.opt.newEquId;
		    	break;
		    case L3PO_pm25_set_work_cycle:
		    	snd.opt.workCycle = rcv.opt.workCycle;
		    	break;
		    case L3PO_pm25_set_sample_cycle:
		    	snd.opt.interSample = rcv.opt.interSample;
		    	break;
		    case L3PO_pm25_set_sample_number:
			    snd.opt.meausTimes = rcv.opt.meausTimes;
			    break;
		    case L3PO_pm25_read_switch:
		    	break;
		    case L3PO_pm25_read_modbus_address:
		    	break;
		    case L3PO_pm25_read_work_cycle:
		    	break;
		    case L3PO_pm25_read_sample_cycle:
		    	break;
		    case L3PO_pm25_read_sample_number:
		    	break;
		    default:
				break;

		}

		ret = hcu_message_send(MSG_ID_PM25_MODBUS_CONTROL_CMD, TASK_ID_MODBUS, TASK_ID_PM25, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName);
			return FAILURE;
		}

		//启动一次性定时器
		/*
		hcu_timer_start(TASK_ID_PM25, TIMER_ID_1S_PM25_MODBUS_FB, PM25_TIMER_DURATION_MODBUS_FB, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		*/

		//设置当前传感器到忙，没反应之前，不置状态
		//zSensorPm25Info[currentSensorPm25Id].hwAccess = SENSOR_PM25_HW_ACCESS_BUSY;
		//zSensorPm25Info[currentSensorPm25Id].busyCount = 0;

		//State Transfer to FSM_STATE_PM25_OPT_WFFB
		FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_OPT_WFFB);
	}//SENSOR_PM25_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwStatus = SENSOR_PM25_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;
	}


	return SUCCESS;
}

OPSTAT fsm_pm25_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Adding by Shanchun for control cmd
	int ret=0;

	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	//HcuDiscDataSampleStorageArray_t record; //Shanchun: to discuss if include sensor_modbus_opertion_general_t

	msg_struct_modbus_pm25_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_control_fb_t))){
		HcuErrorPrint("PM25: Receive cmd feedback error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码
	//memset(&zPM25ConfigData,0,sizeof(sensor_modbus_opertion_general_t));
	gTaskPm25Context.cfgData.equId  = rcv.opt.equId;

	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.backType == L3CI_cmdid_back_type_control){

			/*
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.opt.equId;
			record.sensortype = L3CI_pm25;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.powerOnOff = rcv.opt.powerOnOff;//Shanchun: need switch-case save all operation status
			*/

			//RECORD还要存入数据库
			//to discuss if save to local DB
			/*
			sensor_modbus_opertion_general_t pm25Cmd; //zsc: need to define new struct for sensor config status or reuse sensor_modbus_opertion_general_t??
			memset(&pm25Cmd, 0, sizeof(sensor_modbus_opertion_general_t));

			pm25Cmd.equId = record.equipid;
			pm25Cmd.powerOnOff = record.powerOnOff;

			ret = dbi_HcuPm25DataInfo_save(&pm25Cmd);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_PM25]++;
				HcuErrorPrint("PM25: Can not save data into database!\n");
			}
			*/

		}
		else
		{
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
			HcuErrorPrint("PM25: Offline but instance or other control message received!\n");
		}

	}

	//在线模式
	else if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE)){
		//Online processing
		//赋值给发送消息
		msg_struct_pm25_cloudvela_ctrl_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_pm25_cloudvela_ctrl_resp_t));
		snd.length = sizeof(msg_struct_pm25_cloudvela_ctrl_resp_t);
		snd.opt.equId = rcv.opt.equId;
		snd.backType = rcv.backType;
		snd.cmdId = rcv.cmdId;
		//snd.zhbUl //Shanchun: handle zhb??

		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("PM25: switch control command optId = %d\n", rcv.optId);
		}

		switch(rcv.optId)
		{
			case L3PO_pm25_set_switch:
				snd.optId = L3PO_pm25_set_switch;
				snd.opt.powerOnOff = rcv.opt.powerOnOff;
				gTaskPm25Context.cfgData.powerOnOff = rcv.opt.powerOnOff;
				break;

			case L3PO_pm25_set_modbus_address:
				snd.optId = L3PO_pm25_set_modbus_address;
				snd.opt.newEquId = rcv.opt.newEquId;
				gTaskPm25Context.cfgData.equId = rcv.opt.newEquId;
				break;

			case L3PO_pm25_set_work_cycle:
				snd.optId = L3PO_pm25_set_work_cycle;
				snd.opt.workCycle = rcv.opt.workCycle;
				gTaskPm25Context.cfgData.workCycle = rcv.opt.workCycle;
				break;

			case L3PO_pm25_set_sample_cycle:
				snd.optId = L3PO_pm25_set_sample_cycle;
				snd.opt.interSample = rcv.opt.interSample;
				gTaskPm25Context.cfgData.interSample = rcv.opt.interSample;
				break;

			case L3PO_pm25_set_sample_number:
				snd.optId = L3PO_pm25_set_sample_number;
				snd.opt.meausTimes = rcv.opt.meausTimes;
				gTaskPm25Context.cfgData.meausTimes = rcv.opt.meausTimes;
				break;

			case L3PO_pm25_read_switch:
				snd.optId = L3PO_pm25_read_switch;
				snd.opt.powerOnOff = rcv.opt.powerOnOff;
				gTaskPm25Context.cfgData.powerOnOff = rcv.opt.powerOnOff;
				break;

			case L3PO_pm25_read_modbus_address:
				snd.optId = L3PO_pm25_read_modbus_address;
				snd.opt.newEquId = rcv.opt.newEquId;
				gTaskPm25Context.cfgData.equId = rcv.opt.newEquId;
				break;

			case L3PO_pm25_read_work_cycle:
				snd.optId = L3PO_pm25_read_work_cycle;
				snd.opt.workCycle = rcv.opt.workCycle;
				gTaskPm25Context.cfgData.workCycle = rcv.opt.workCycle;
				break;

			case L3PO_pm25_read_sample_cycle:
				snd.optId = L3PO_pm25_read_sample_cycle;
				snd.opt.interSample = rcv.opt.interSample;
				gTaskPm25Context.cfgData.interSample = rcv.opt.interSample;
				break;

			case L3PO_pm25_read_sample_number:
				snd.optId = L3PO_pm25_read_sample_number;
				snd.opt.meausTimes = rcv.opt.meausTimes;
				gTaskPm25Context.cfgData.meausTimes = rcv.opt.meausTimes;
				break;

			default:
				HcuErrorPrint("MODBUS: Error operation code received!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
				break;
		}

		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("PM25: control command cmdId= %d\n", snd.cmdId);
			HcuDebugPrint("PM25: control command optId= %d\n", snd.optId);
			HcuDebugPrint("PM25: control command backType = %d\n", snd.backType);
			HcuDebugPrint("PM25: control command equId= %d\n", snd.opt.equId);
			HcuDebugPrint("PM25: control command powerOnOff = %d\n", snd.opt.powerOnOff);
			HcuDebugPrint("PM25: control command newEquId = %d\n", snd.opt.newEquId);
			HcuDebugPrint("PM25: control command workCycle = %d\n", snd.opt.workCycle);
			HcuDebugPrint("PM25: control command interSample = %d\n", snd.opt.interSample);
			HcuDebugPrint("PM25: control command meausTimes = %d\n", snd.opt.meausTimes);

		}

	ret = hcu_message_send(MSG_ID_PM25_CLOUDVELA_CTRL_RESP, TASK_ID_CLOUDVELA, TASK_ID_PM25, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		HcuErrorPrint("PM25: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
		return FAILURE;
	}

	//Save to disk as request：在线是为了备份，离线是为了重发给后台
	if (rcv.backType == L3CI_cmdid_back_type_control){
			//存入内存缓冲磁盘，做为本地缓存，未来需要实现磁盘模式

			/*
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.opt.equId;
			record.sensortype = L3CI_pm25;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.powerOnOff = rcv.opt.powerOnOff; //Shanchun: need switch-case save all operation status
			*/
			//RECORD还要存入数据库
			//ret = dbi_HcuPm25ConfigData_save(&zPM25ConfigData);
			ret = dbi_HcuPm25ConfigData_update(rcv.optId, &gTaskPm25Context.cfgData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
				HcuErrorPrint("PM25: Can not save data into database!\n");
			}
		}
	}

	//差错情形
	else{
		HcuErrorPrint("PM25: Wrong state of CLOUDVELA when data need send out!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
		//If this shall work normally, it is too much for each sensor STM process!
		return FAILURE;
	}

	//恢复当前传感器的空闲状态
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwAccess = SENSOR_PM25_HW_ACCESS_IDLE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].hwStatus = SENSOR_PM25_HW_STATUS_ACTIVE;
	gTaskPm25Context.pm25[gTaskPm25Context.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskPm25Context.currentSensorId < 0) || (gTaskPm25Context.currentSensorId >= (MAX_NUM_OF_SENSOR_PM25_INSTALLED-1))){
		gTaskPm25Context.currentSensorId = 0;
	}else{
		gTaskPm25Context.currentSensorId++;
	}

	//State Transfer to FSM_STATE_PM25_ACTIVE
	FsmSetState(TASK_ID_PM25, FSM_STATE_PM25_ACTIVED);
	return SUCCESS;
}


OPSTAT func_pm25_time_out_read_data_from_bmpd300(void)
{
	int ret=0;

	//存入数据库
	sensor_pm25_bmpd300_data_element_t pm25Data;
	memset(&pm25Data, 0, sizeof(sensor_pm25_bmpd300_data_element_t));
	pm25Data.equipid = 0;
	pm25Data.timeStamp = time(0);
	pm25Data.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	pm25Data.pm2d5Value = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_I2CPM25BMPD300].fVal);

	ret = dbi_HcuPm25Bmpd300DataInfo_save(&pm25Data);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		HcuErrorPrint("PM25: Can not save Pm25Bmpd300 data into database!\n");
	}

	return SUCCESS;
}

OPSTAT func_pm25_time_out_read_data_from_sharp(void)
{
	int ret=0;

	//存入数据库
	sensor_pm25_sharp_data_element_t pm25Data;
	memset(&pm25Data, 0, sizeof(sensor_pm25_sharp_data_element_t));
	pm25Data.equipid = 0;
	pm25Data.timeStamp = time(0);
	pm25Data.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
	pm25Data.pm2d5Value = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].fVal*100);

	ret = dbi_HcuPm25SharpDataInfo_save(&pm25Data);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25]++;
		HcuErrorPrint("PM25: Can not save Pm25Sharp data into database!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_pm25_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_pm25_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}




