/*
 * sensorwinddir.c
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#include "sensorwinddir.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the WINDDIR
*/
HcuFsmStateItem_t FsmWinddir[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_winddir_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_winddir_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_winddir_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_WINDDIR_INITED,            		fsm_winddir_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_WINDDIR_INITED,            		fsm_winddir_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_WINDDIR_INITED,            		fsm_com_do_nothing},

    //Normal working status
    {MSG_ID_COM_RESTART,        		FSM_STATE_WINDDIR_ACTIVED,          fsm_winddir_restart},
    {MSG_ID_COM_INIT_FEEDBACK,			FSM_STATE_WINDDIR_ACTIVED,          fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       		FSM_STATE_WINDDIR_ACTIVED,          fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,       	FSM_STATE_WINDDIR_ACTIVED,          fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       		FSM_STATE_WINDDIR_ACTIVED,          fsm_winddir_time_out},
	{MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,     FSM_STATE_WINDDIR_ACTIVED,      	fsm_winddir_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ,  FSM_STATE_WINDDIR_ACTIVED,          fsm_winddir_cloudvela_ctrl_req},

    //Wait for Modbus Feedback
    {MSG_ID_COM_RESTART,        		FSM_STATE_WINDDIR_OPT_WFFB,         fsm_winddir_restart},
    {MSG_ID_COM_INIT_FEEDBACK,			FSM_STATE_WINDDIR_OPT_WFFB,         fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       		FSM_STATE_WINDDIR_OPT_WFFB,         fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,       	FSM_STATE_WINDDIR_OPT_WFFB,         fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       		FSM_STATE_WINDDIR_OPT_WFFB,         fsm_winddir_time_out},
	{MSG_ID_MODBUS_WINDDIR_DATA_REPORT, FSM_STATE_WINDDIR_OPT_WFFB,        	fsm_winddir_data_report_from_modbus},
	{MSG_ID_MODBUS_WINDDIR_CONTROL_FB,  FSM_STATE_WINDDIR_OPT_WFFB,         fsm_winddir_modbus_control_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Task Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表
gTaskWinddirContext_t zSensorWinddirInfo[MAX_NUM_OF_SENSOR_WINDDIR_INSTALLED];
UINT8 currentSensorWinddirId;
//暂时没有硬盘，现在CLOUDVELA中定义了内存级离线缓冲区
//extern HcuDiscDataSampleStorage_t zHcuMemStorageBuf;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_winddir_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("WINDDIR: Error Set FSM State at fsm_winddir_task_entry\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_winddir_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_WINDDIR, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("WINDDIR: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_WINDDIR], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("WINDDIR: Error Set FSM State at fsm_winddir_init\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("WINDDIR: Enter FSM_STATE_WINDDIR_INITED status, everything goes well!\n");
	}

	//Task global variables init.
	memset(zSensorWinddirInfo, 0, sizeof(gTaskWinddirContext_t));
	currentSensorWinddirId = 0;
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR] = 0;
	//目前暂时只有一个WINDDIR传感器，但程序的框架可以支持无数个传感器
	//未来还需要支持传感器的地址可以被配置，随时被修改，通过后台命令
	for (i=0;i<MAX_NUM_OF_SENSOR_WINDDIR_INSTALLED;i++){
		zSensorWinddirInfo[i].sensorId = i;
		zSensorWinddirInfo[i].equId = MODBUS_WINDDIR_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
		zSensorWinddirInfo[i].hwAccess = SENSOR_WINDDIR_HW_ACCESS_IDLE;
		zSensorWinddirInfo[i].hwStatus = SENSOR_WINDDIR_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		zSensorWinddirInfo[i].busyCount = 0;
	}

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_WINDDIR, TIMER_ID_1S_WINDDIR_PERIOD_READ, zHcuSysEngPar.timer.winddirReqTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		HcuErrorPrint("WINDDIR: Error start timer!\n");
		return FAILURE;
	}

	//State Transfer to FSM_STATE_WINDDIR_ACTIVED
	ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		HcuErrorPrint("WINDDIR: Error Set FSM State at fsm_winddir_init\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_winddir_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("WINDDIR: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_winddir_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_winddir_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("WINDDIR: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_WINDDIR]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_WINDDIR, TASK_ID_WINDDIR, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
			HcuErrorPrint("WINDDIR: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_WINDDIR], zHcuSysCrlTab.taskRun.taskName[TASK_ID_WINDDIR]);
			return FAILURE;
		}
	}

	//Period time out received, send command to MODBUS for read
	if ((rcv.timeId == TIMER_ID_1S_WINDDIR_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_WINDDIR) != FSM_STATE_WINDDIR_ACTIVED){
			ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
				HcuErrorPrint("WINDDIR: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
		func_winddir_time_out_read_data_from_modbus();
	}

	//Modbus received FB time out message received
	else if ((rcv.timeId == TIMER_ID_1S_WINDDIR_MODBUS_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_winddir_time_out_processing_no_rsponse();
	}

	return SUCCESS;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_winddir_time_out_read_data_from_modbus(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (zSensorWinddirInfo[currentSensorWinddirId].hwAccess == SENSOR_WINDDIR_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((zSensorWinddirInfo[currentSensorWinddirId].busyCount < 0) || (zSensorWinddirInfo[currentSensorWinddirId].busyCount >= SENSOR_WINDDIR_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			zSensorWinddirInfo[currentSensorWinddirId].busyCount = 0;
			zSensorWinddirInfo[currentSensorWinddirId].hwAccess =SENSOR_WINDDIR_HW_ACCESS_IDLE;
		}else{
			zSensorWinddirInfo[currentSensorWinddirId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (zSensorWinddirInfo[currentSensorWinddirId].hwAccess == SENSOR_WINDDIR_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to MODBUS
		msg_struct_winddir_modbus_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_winddir_modbus_data_read_t));
		snd.length = sizeof(msg_struct_winddir_modbus_data_read_t);
		snd.equId = zSensorWinddirInfo[currentSensorWinddirId].equId;
		snd.cmdId = L3CI_winddir;
		snd.optId = L3PO_winddir_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_WINDDIR_MODBUS_DATA_READ, TASK_ID_MODBUS, TASK_ID_WINDDIR, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
			HcuErrorPrint("WINDDIR: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_WINDDIR], zHcuSysCrlTab.taskRun.taskName[TASK_ID_MODBUS]);
			return;
		}

		//启动一次性定时器
		ret = hcu_timer_start(TASK_ID_WINDDIR, TIMER_ID_1S_WINDDIR_MODBUS_FB, zHcuSysEngPar.timer.winddirReqTimerFB, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
			HcuErrorPrint("WINDDIR: Error start timer!\n");
			return;
		}

		//设置当前传感器到忙，没反应之前，不置状态
		zSensorWinddirInfo[currentSensorWinddirId].hwAccess = SENSOR_WINDDIR_HW_ACCESS_BUSY;
		zSensorWinddirInfo[currentSensorWinddirId].busyCount = 0;

		//State Transfer to FSM_STATE_WINDDIR_OPT_WFFB
		ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_OPT_WFFB);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
			HcuErrorPrint("WINDDIR: Error Set FSM State!\n");
			return;
		}//FsmSetState
	}//SENSOR_WINDDIR_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		zSensorWinddirInfo[currentSensorWinddirId].hwAccess = SENSOR_WINDDIR_HW_ACCESS_IDLE;
		zSensorWinddirInfo[currentSensorWinddirId].hwStatus = SENSOR_WINDDIR_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		zSensorWinddirInfo[currentSensorWinddirId].busyCount = 0;
	}

	return;
}

//超时失败，走向下一个传感器
void func_winddir_time_out_processing_no_rsponse(void)
{
	int ret=0;

	//恢复当前传感器的空闲状态
	zSensorWinddirInfo[currentSensorWinddirId].hwAccess = SENSOR_WINDDIR_HW_ACCESS_IDLE;
	zSensorWinddirInfo[currentSensorWinddirId].hwStatus = SENSOR_WINDDIR_HW_STATUS_DEACTIVE;
	zSensorWinddirInfo[currentSensorWinddirId].busyCount = 0;

	//当前传感器指针指向下一个
	//CurrentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((currentSensorWinddirId < 0) || (currentSensorWinddirId >= (MAX_NUM_OF_SENSOR_WINDDIR_INSTALLED-1))){
		currentSensorWinddirId = 0;
	}else{
		currentSensorWinddirId++;
	}

	//暂时啥也不干，未来在瞬时模式下也许需要回一个失败的消息，当然缺省情况下没有反应就是表示失败

	//State Transfer to FSM_STATE_WINDDIR_ACTIVE
	ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		HcuErrorPrint("WINDDIR: Error Set FSM State!\n");
		return;
	}//FsmSetState
	return;
}

//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_winddir_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_winddir_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_winddir_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_winddir_data_report_t))){
		HcuErrorPrint("WINDDIR: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	ret = hcu_timer_stop(TASK_ID_WINDDIR, TIMER_ID_1S_WINDDIR_MODBUS_FB, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		HcuErrorPrint("WINDDIR: Error stop timer!\n");
		return FAILURE;
	}

	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.winddir.equipid;
			record.sensortype = L3CI_winddir;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.winddir.timeStamp;
			record.dataFormat = rcv.winddir.dataFormat;
			record.winddirValue = rcv.winddir.winddirValue;
			record.gpsx = rcv.winddir.gps.gpsx;
			record.gpsy = rcv.winddir.gps.gpsy;
			record.gpsz = rcv.winddir.gps.gpsz;
			record.ew = rcv.winddir.gps.ew;
			record.ns = rcv.winddir.gps.ns;
			//RECORD存入内存盘
			if (HCU_SENSOR_DATA_SAVE_TO_MEMDISK_SET == HCU_SYSCFG_SENSOR_SAVE_TO_MEMDISK_FLAG_YES)
			{
				ret = hcu_save_to_storage_mem(&record);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into memory buffer, might par error!\n");
				}
			}
			//RECORD存入硬盘
			if (HCU_SYSCFG_SNR_DATA_SAVE_TO_FLASH_DISK_SET == HCU_SYSCFG_SENSOR_SAVE_TO_FLASH_DISK_FLAG_YES)
			{
				ret = hcu_save_to_storage_disc(FILE_OPERATION_TYPE_SENSOR, &record, sizeof(HcuDiscDataSampleStorageArray_t));
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into hard disk!\n");
				}
			}
			//RECORD还要存入数据库
			if (HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_SYSCFG_SENSOR_SAVE_TO_LOCAL_DB_FLAG_YES)
			{
				sensor_winddir_data_element_t winddirData;
				memset(&winddirData, 0, sizeof(sensor_winddir_data_element_t));
				winddirData.equipid = record.equipid;
				winddirData.timeStamp = record.timestamp;
				winddirData.dataFormat = record.dataFormat;
				winddirData.winddirValue = record.winddirValue;
				winddirData.gps.gpsx = record.gpsx;
				winddirData.gps.gpsy = record.gpsy;
				winddirData.gps.gpsz = record.gpsz;
				winddirData.gps.ew = record.ew;
				winddirData.gps.ns = record.ns;
				winddirData.onOffLineFlag = record.onOffLine;
				ret = dbi_HcuWinddirDataInfo_save(&winddirData);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into database!\n");
				}
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
				HcuErrorPrint("WINDDIR: Offline but instance or other control message received!\n");
			}
	}

	//在线模式
	else if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//Online processing
		//赋值给发送消息
		msg_struct_winddir_cloudvela_data_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_winddir_cloudvela_data_resp_t));
		snd.length = sizeof(msg_struct_winddir_cloudvela_data_resp_t);
		snd.winddir.equipid = rcv.winddir.equipid;
		snd.winddir.timeStamp = rcv.winddir.timeStamp;
		snd.usercmdid = rcv.usercmdid;
		snd.cmdIdBackType = rcv.cmdIdBackType;
		snd.useroptid = rcv.useroptid;
		snd.winddir.dataFormat = rcv.winddir.dataFormat;
		snd.winddir.winddirValue = rcv.winddir.winddirValue;
		snd.winddir.gps.gpsx = rcv.winddir.gps.gpsx;
		snd.winddir.gps.gpsy = rcv.winddir.gps.gpsy;
		snd.winddir.gps.gpsz = rcv.winddir.gps.gpsz;
		snd.winddir.gps.ew = rcv.winddir.gps.ew;
		snd.winddir.gps.ns = rcv.winddir.gps.ns;
		ret = hcu_message_send(MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP, TASK_ID_CLOUDVELA, TASK_ID_WINDDIR, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
			HcuErrorPrint("WINDDIR: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_WINDDIR], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
			return FAILURE;
		}

		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//该函数，有待完成
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			//存入内存缓冲磁盘，做为本地缓存，未来需要实现磁盘模式
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.winddir.equipid;
			record.sensortype = L3CI_winddir;
			record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
			record.timestamp = rcv.winddir.timeStamp;
			record.dataFormat = rcv.winddir.dataFormat;
			record.winddirValue = rcv.winddir.winddirValue;
			record.gpsx = rcv.winddir.gps.gpsx;
			record.gpsy = rcv.winddir.gps.gpsy;
			record.gpsz = rcv.winddir.gps.gpsz;
			record.ew = rcv.winddir.gps.ew;
			record.ns = rcv.winddir.gps.ns;
			//RECORD存入内存盘
			if (HCU_SENSOR_DATA_SAVE_TO_MEMDISK_SET == HCU_SYSCFG_SENSOR_SAVE_TO_MEMDISK_FLAG_YES)
			{
				ret = hcu_save_to_storage_mem(&record);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into memory buffer, might par error!\n");
				}
			}
			//RECORD存入硬盘
			if (HCU_SYSCFG_SNR_DATA_SAVE_TO_FLASH_DISK_SET == HCU_SYSCFG_SENSOR_SAVE_TO_FLASH_DISK_FLAG_YES)
			{
				ret = hcu_save_to_storage_disc(FILE_OPERATION_TYPE_SENSOR, &record, sizeof(HcuDiscDataSampleStorageArray_t));
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into hard disk!\n");
				}
			}
			//RECORD还要存入数据库
			if (HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_SYSCFG_SENSOR_SAVE_TO_LOCAL_DB_FLAG_YES)
			{
				sensor_winddir_data_element_t winddirData;
				memset(&winddirData, 0, sizeof(sensor_winddir_data_element_t));
				winddirData.equipid = record.equipid;
				winddirData.timeStamp = record.timestamp;
				winddirData.dataFormat = record.dataFormat;
				winddirData.winddirValue = record.winddirValue;
				winddirData.gps.gpsx = record.gpsx;
				winddirData.gps.gpsy = record.gpsy;
				winddirData.gps.gpsz = record.gpsz;
				winddirData.gps.ew = record.ew;
				winddirData.gps.ns = record.ns;
				winddirData.onOffLineFlag = record.onOffLine;
				ret = dbi_HcuWinddirDataInfo_save(&winddirData);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
					HcuErrorPrint("WINDDIR: Can not save data into database!\n");
				}
			}
		}// Period mode OK!
		// Instance mode, no need store!
	}

	//差错情形
	else{
		HcuErrorPrint("WINDDIR: Wrong state of CLOUDVELA when data need send out!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
		//If this shall work normally, it is too much for each sensor STM process!
		return FAILURE;
	}

	//恢复当前传感器的空闲状态
	zSensorWinddirInfo[currentSensorWinddirId].hwAccess = SENSOR_WINDDIR_HW_ACCESS_IDLE;
	zSensorWinddirInfo[currentSensorWinddirId].hwStatus = SENSOR_WINDDIR_HW_STATUS_ACTIVE;
	zSensorWinddirInfo[currentSensorWinddirId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((currentSensorWinddirId < 0) || (currentSensorWinddirId >= (MAX_NUM_OF_SENSOR_WINDDIR_INSTALLED-1))){
		currentSensorWinddirId = 0;
	}else{
		currentSensorWinddirId++;
	}

	//State Transfer to FSM_STATE_WINDDIR_ACTIVE
	ret = FsmSetState(TASK_ID_WINDDIR, FSM_STATE_WINDDIR_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
		HcuErrorPrint("WINDDIR: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//读取源泉，这个可能是控制命令，也可能是数据瞬时读取命令
//状态机方面，会有点交叉混淆的
//瞬时模式，有待完成，初期的必要性不大
OPSTAT fsm_winddir_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_winddir_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_winddir_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_winddir_data_req_t))){
		HcuErrorPrint("WINDDIR: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_WINDDIR]++;
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

OPSTAT fsm_winddir_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_winddir_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}
