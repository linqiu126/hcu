/*
 * sensorwindspd.c
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#include "sensorwindspd.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"
/*
** FSM of the WINDSPD
*/
HcuFsmStateItem_t HcuFsmWindspd[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_windspd_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_windspd_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_WINDSPD_INITED,            		fsm_windspd_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_WINDSPD_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_windspd_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_windspd_time_out},

    //Normal working status
	{MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,     	FSM_STATE_WINDSPD_ACTIVED,      	fsm_windspd_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM,     FSM_STATE_WINDSPD_ACTIVED,      	fsm_windspd_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ,  		FSM_STATE_WINDSPD_ACTIVED,          fsm_windspd_cloudvela_ctrl_req},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,				FSM_STATE_WINDSPD_ACTIVED,      	fsm_windspd_l3aqyc_exg_ctrl_req},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,				FSM_STATE_WINDSPD_ACTIVED,      	fsm_windspd_zhbl3mod_exg_ctrl_req},


    //Wait for Modbus Feedback
	{MSG_ID_MODBUS_WINDSPD_DATA_REPORT, FSM_STATE_WINDSPD_OPT_WFFB,        	fsm_windspd_data_report_from_modbus},
	{MSG_ID_MODBUS_WINDSPD_CONTROL_FB,  FSM_STATE_WINDSPD_OPT_WFFB,         fsm_windspd_modbus_control_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskWindspdContext_t gTaskWindspdContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_windspd_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_WINDSPD, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_windspd_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_WINDSPD, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("WINDSPD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_INITED);
	HCU_DEBUG_PRINT_FAT("WINDSPD: Enter FSM_STATE_WINDSPD_INITED status, everything goes well!\n");

	//Task global variables init.
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD] = 0;
	memset(&gTaskWindspdContext, 0, sizeof(gTaskWindspdContext_t));

	//目前暂时只有一个WINDSPD传感器，但程序的框架可以支持无数个传感器
	//未来还需要支持传感器的地址可以被配置，随时被修改，通过后台命令
	for (i=0;i<MAX_NUM_OF_SENSOR_WINDSPD_INSTALLED;i++){
		gTaskWindspdContext.windspd[i].sensorId = i;
		gTaskWindspdContext.windspd[i].equId = MODBUS_WINDSPD_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
		gTaskWindspdContext.windspd[i].hwAccess = SENSOR_WINDSPD_HW_ACCESS_IDLE;
		gTaskWindspdContext.windspd[i].hwStatus = SENSOR_WINDSPD_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskWindspdContext.windspd[i].busyCount = 0;
	}

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	hcu_timer_start(TASK_ID_WINDSPD, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_WINDSPD_PERIOD_READ), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//State Transfer to FSM_STATE_WINDSPD_ACTIVED
	FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_ACTIVED);
	return SUCCESS;
}

OPSTAT fsm_windspd_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("WINDSPD: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_windspd_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_windspd_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("WINDSPD: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_WINDSPD]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_WINDSPD, TASK_ID_WINDSPD, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
			HcuErrorPrint("WINDSPD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName, zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName);
			return FAILURE;
		}
	}

	//Period time out received, send command to MODBUS for read
	if ((rcv.timeId == TIMER_ID_1S_WINDSPD_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_WINDSPD) != FSM_STATE_WINDSPD_ACTIVED){
			FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_ACTIVED);
		}
		func_windspd_time_out_read_data_from_modbus();
	}

	//Modbus received FB time out message received
	else if ((rcv.timeId == TIMER_ID_1S_WINDSPD_MODBUS_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_windspd_time_out_processing_no_rsponse();
	}

	return SUCCESS;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_windspd_time_out_read_data_from_modbus(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess == SENSOR_WINDSPD_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount < 0) || (gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount >= SENSOR_WINDSPD_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount = 0;
			gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess =SENSOR_WINDSPD_HW_ACCESS_IDLE;
		}else{
			gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess == SENSOR_WINDSPD_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to MODBUS
		msg_struct_windspd_modbus_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_windspd_modbus_data_read_t));
		snd.length = sizeof(msg_struct_windspd_modbus_data_read_t);
		snd.equId = gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].equId;
		snd.cmdId = L3CI_windspd;
		snd.optId = L3PO_windspd_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_WINDSPD_MODBUS_DATA_READ, TASK_ID_MODBUS, TASK_ID_WINDSPD, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
			HcuErrorPrint("WINDSPD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName, zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName);
			return;
		}

		//启动一次性定时器
		hcu_timer_start(TASK_ID_WINDSPD, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_WINDSPD_MODBUS_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess = SENSOR_WINDSPD_HW_ACCESS_BUSY;
		gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_WINDSPD_OPT_WFFB
		FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_OPT_WFFB);
	}//SENSOR_WINDSPD_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess = SENSOR_WINDSPD_HW_ACCESS_IDLE;
		gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwStatus = SENSOR_WINDSPD_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount = 0;
	}

	return;
}

//超时失败，走向下一个传感器
void func_windspd_time_out_processing_no_rsponse(void)
{
	int ret=0;

	//恢复当前传感器的空闲状态
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess = SENSOR_WINDSPD_HW_ACCESS_IDLE;
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwStatus = SENSOR_WINDSPD_HW_STATUS_DEACTIVE;
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//CurrentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskWindspdContext.currentSensorId < 0) || (gTaskWindspdContext.currentSensorId >= (MAX_NUM_OF_SENSOR_WINDSPD_INSTALLED-1))){
		gTaskWindspdContext.currentSensorId = 0;
	}else{
		gTaskWindspdContext.currentSensorId++;
	}

	//暂时啥也不干，未来在瞬时模式下也许需要回一个失败的消息，当然缺省情况下没有反应就是表示失败

	//State Transfer to FSM_STATE_WINDSPD_ACTIVE
	FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_ACTIVED);
	return;
}

//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_windspd_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_windspd_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_windspd_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_windspd_data_report_t))){
		HcuErrorPrint("WINDSPD: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	hcu_timer_stop(TASK_ID_WINDSPD, TIMER_ID_1S_WINDSPD_MODBUS_FB, TIMER_RESOLUTION_1S);
	gTaskWindspdContext.windspdValue = (float)rcv.windspd.windspdValue;
	HCU_DEBUG_PRINT_INF("WINDSPD: windspd = %.1f\n\n", gTaskWindspdContext.windspdValue);
/*
	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.windspd.equipid;
			record.sensortype = L3CI_windspd;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.windspd.timeStamp;
			record.dataFormat = rcv.windspd.dataFormat;
			record.windspdValue = rcv.windspd.windspdValue;
			record.gpsx = rcv.windspd.gps.gpsx;
			record.gpsy = rcv.windspd.gps.gpsy;
			record.gpsz = rcv.windspd.gps.gpsz;
			record.ew = rcv.windspd.gps.ew;
			record.ns = rcv.windspd.gps.ns;
			//RECORD还要存入数据库
			sensor_windspd_data_element_t windspdData;
			memset(&windspdData, 0, sizeof(sensor_windspd_data_element_t));
			windspdData.equipid = record.equipid;
			windspdData.timeStamp = record.timestamp;
			windspdData.dataFormat = record.dataFormat;
			windspdData.windspdValue = record.windspdValue;
			windspdData.gps.gpsx = record.gpsx;
			windspdData.gps.gpsy = record.gpsy;
			windspdData.gps.gpsz = record.gpsz;
			windspdData.gps.ew = record.ew;
			windspdData.gps.ns = record.ns;
			windspdData.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuWindspdDataInfo_save(&windspdData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
				HcuErrorPrint("WINDSPD: Can not save data into database!\n");
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
				HcuErrorPrint("WINDSPD: Offline but instance or other control message received!\n");
			}
	}
*/

	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE))
	{
		///////////
		//L2信息
		msg_struct_windspd_cloudvela_data_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_windspd_cloudvela_data_resp_t));
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

		snd.windspd.equipid = rcv.windspd.equipid;
		snd.windspd.timeStamp = rcv.windspd.timeStamp;
		snd.windspd.dataFormat = rcv.windspd.dataFormat;
		snd.windspd.windspdValue = rcv.windspd.windspdValue;
		snd.windspd.gps.gpsx = rcv.windspd.gps.gpsx;
		snd.windspd.gps.gpsy = rcv.windspd.gps.gpsy;
		snd.windspd.gps.gpsz = rcv.windspd.gps.gpsz;
		snd.windspd.gps.ew = rcv.windspd.gps.ew;
		snd.windspd.gps.ns = rcv.windspd.gps.ns;
		snd.windspd.nTimes = rcv.windspd.nTimes;
		snd.windspd.onOffLineFlag = rcv.windspd.onOffLineFlag;
		snd.length = sizeof(msg_struct_windspd_cloudvela_data_resp_t);

		//发送后台
		//if ((HCU_SYSCFG_SENSOR_REPORT_MODE_SET & HCU_SYSCFG_SENSOR_REPORT_MODE_INDIVIDUAL) == TRUE){
		if (HCU_SYSCFG_SENSOR_REPORT_MODE_SET == HCU_SYSCFG_SENSOR_REPORT_MODE_INDIVIDUAL){
			ret = hcu_message_send(MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_WINDSPD, &snd, snd.length);
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
			record.equipid = rcv.windspd.equipid;
			record.sensortype = L3CI_windspd;
			record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
			record.timestamp = rcv.windspd.timeStamp;
			record.dataFormat = rcv.windspd.dataFormat;
			record.windspdValue = rcv.windspd.windspdValue;
			record.gpsx = rcv.windspd.gps.gpsx;
			record.gpsy = rcv.windspd.gps.gpsy;
			record.gpsz = rcv.windspd.gps.gpsz;
			record.ew = rcv.windspd.gps.ew;
			record.ns = rcv.windspd.gps.ns;
			//RECORD还要存入数据库
			sensor_windspd_data_element_t windspdData;
			memset(&windspdData, 0, sizeof(sensor_windspd_data_element_t));
			windspdData.equipid = record.equipid;
			windspdData.timeStamp = record.timestamp;
			windspdData.dataFormat = record.dataFormat;
			windspdData.windspdValue = record.windspdValue;
			windspdData.gps.gpsx = record.gpsx;
			windspdData.gps.gpsy = record.gpsy;
			windspdData.gps.gpsz = record.gpsz;
			windspdData.gps.ew = record.ew;
			windspdData.gps.ns = record.ns;
			windspdData.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuWindspdDataInfo_save(&windspdData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
				HcuErrorPrint("WINDSPD: Can not save data into database!\n");
			}
		}// Period mode OK!
		// Instance mode, no need store!
	}

	//差错情形
	else{
		HcuErrorPrint("WINDSPD: Wrong state of CLOUDVELA when data need send out!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
		//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
		//If this shall work normally, it is too much for each sensor STM process!
		return FAILURE;
	}

	//恢复当前传感器的空闲状态
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwAccess = SENSOR_WINDSPD_HW_ACCESS_IDLE;
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].hwStatus = SENSOR_WINDSPD_HW_STATUS_ACTIVE;
	gTaskWindspdContext.windspd[gTaskWindspdContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskWindspdContext.currentSensorId < 0) || (gTaskWindspdContext.currentSensorId >= (MAX_NUM_OF_SENSOR_WINDSPD_INSTALLED-1))){
		gTaskWindspdContext.currentSensorId = 0;
	}else{
		gTaskWindspdContext.currentSensorId++;
	}

	//State Transfer to FSM_STATE_WINDSPD_ACTIVE
	FsmSetState(TASK_ID_WINDSPD, FSM_STATE_WINDSPD_ACTIVED);
	return SUCCESS;
}

//读取源泉，这个可能是控制命令，也可能是数据瞬时读取命令
//状态机方面，会有点交叉混淆的
//瞬时模式，有待完成，初期的必要性不大
OPSTAT fsm_windspd_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_windspd_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_windspd_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_windspd_data_req_t))){
		HcuErrorPrint("WINDSPD: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDSPD]++;
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

OPSTAT fsm_windspd_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_windspd_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_windspd_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_windspd_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_windspd_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}





