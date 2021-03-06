/*
 * sensortemp.c
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#include "sensortemp.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"
#include "../l2frame/modbus.h"

/*
** FSM of the TEMP
*/
HcuFsmStateItem_t HcuFsmTemp[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_temp_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_temp_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_TEMP_INITED,            		fsm_temp_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_TEMP_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_temp_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_temp_time_out},

    //Normal working status
	{MSG_ID_CLOUDVELA_TEMP_DATA_REQ,    	FSM_STATE_TEMP_ACTIVED,      	 	fsm_temp_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM,    FSM_STATE_TEMP_ACTIVED,      	 	fsm_temp_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_TEMP_CTRL_REQ,        FSM_STATE_TEMP_ACTIVED,         	fsm_temp_cloudvela_ctrl_req},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,			FSM_STATE_TEMP_ACTIVED,      	  	fsm_temp_l3aqyc_exg_ctrl_req},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,			FSM_STATE_TEMP_ACTIVED,      	  	fsm_temp_zhbl3mod_exg_ctrl_req},


    //Wait for Modbus Feedback
	{MSG_ID_MODBUS_TEMP_DATA_REPORT, 		FSM_STATE_TEMP_OPT_WFFB,         	fsm_temp_data_report_from_modbus},
	{MSG_ID_MODBUS_TEMP_CONTROL_FB,    		FSM_STATE_TEMP_OPT_WFFB,         	fsm_temp_modbus_control_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskTempContext_t gTaskTempContext;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_temp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_TEMP, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_temp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_TEMP, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("TEMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TEMP].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_INITED);
	HCU_DEBUG_PRINT_FAT("TEMP: Enter FSM_STATE_TEMP_INITED status, everything goes well!\n");

	//Task global variables init.
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP] = 0;
	memset(&gTaskTempContext, 0, sizeof(gTaskTempContext_t));
	gTaskTempContext.tempFlag = FALSE;

	//目前暂时只有一个TEMP传感器，但程序的框架可以支持无数个传感器
	//未来还需要支持传感器的地址可以被配置，随时被修改，通过后台命令
	for (i=0;i<MAX_NUM_OF_SENSOR_TEMP_INSTALLED;i++){
		gTaskTempContext.temp[i].sensorId = i;
		gTaskTempContext.temp[i].equId = MODBUS_TEMP_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
		gTaskTempContext.temp[i].hwAccess = SENSOR_TEMP_HW_ACCESS_IDLE;
		gTaskTempContext.temp[i].hwStatus = SENSOR_TEMP_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskTempContext.temp[i].busyCount = 0;
	}

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	hcu_timer_start(TASK_ID_TEMP, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_TEMP_PERIOD_READ), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//State Transfer to FSM_STATE_TEMP_ACTIVED
	FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_ACTIVED);
	return SUCCESS;
}

OPSTAT fsm_temp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("TEMP: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_temp_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_temp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("TEMP: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_TEMP]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_PM25, TASK_ID_PM25, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TEMP].taskName, zHcuVmCtrTab.task[TASK_ID_TEMP].taskName);
			return FAILURE;
		}
	}

	//Period time out received, send command to MODBUS/SPIBUSARIES for read
	if ((rcv.timeId == TIMER_ID_1S_TEMP_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_TEMP) != FSM_STATE_TEMP_ACTIVED){
			FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_ACTIVED);
		}
/*
#ifdef TARGET_RASPBERRY_PI3B
		if ((SENSOR_TEMP_RPI_DHT11_PRESENT == SENSOR_TEMP_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_DHT11 == HCU_SENSOR_PRESENT_YES)) func_temp_time_out_read_data_from_dht11();
		if ((SENSOR_TEMP_RPI_SHT20_PRESENT == SENSOR_TEMP_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_SHT20 == HCU_SENSOR_PRESENT_YES)) func_temp_time_out_read_data_from_sht20();
		if ((SENSOR_TEMP_RPI_RHT03_PRESENT == SENSOR_TEMP_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_RHT03 == HCU_SENSOR_PRESENT_YES)) func_temp_time_out_read_data_from_rht03();
		if ((SENSOR_TEMP_RPI_BMP180_PRESENT == SENSOR_TEMP_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_BMP180 == HCU_SENSOR_PRESENT_YES)) func_temp_time_out_read_data_from_bmp180();
		if ((SENSOR_TEMP_RPI_MTH01_PRESENT == SENSOR_TEMP_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_MTH01 == HCU_SENSOR_PRESENT_YES)) func_temp_time_out_read_data_from_mth01();
#endif
*/

		//update for SPIBUSARIES start
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_DAYCG21_RASBERRY_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
		func_temp_time_out_read_data_from_spibusaries();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
		func_temp_time_out_read_data_from_modbus();
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
		func_temp_time_out_read_data_from_modbus();

//小技巧，不要这部分，以便加强编译检查
#else
#endif
	}

	//Modbus received FB time out message received
	else if ((rcv.timeId == TIMER_ID_1S_TEMP_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_temp_time_out_processing_no_rsponse();
	}

	return SUCCESS;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_temp_time_out_read_data_from_modbus(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess == SENSOR_TEMP_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount < 0) || (gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount >= SENSOR_TEMP_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess =SENSOR_TEMP_HW_ACCESS_IDLE;
		}else{
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess == SENSOR_TEMP_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to MODBUS
		msg_struct_temp_modbus_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_temp_modbus_data_read_t));
		snd.length = sizeof(msg_struct_temp_modbus_data_read_t);
		snd.equId = gTaskTempContext.temp[gTaskTempContext.currentSensorId].equId;
		snd.cmdId = L3CI_temp;
		snd.optId = L3PO_temp_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_TEMP_MODBUS_DATA_READ, TASK_ID_MODBUS, TASK_ID_TEMP, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TEMP].taskName, zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName);
			return;
		}

		//启动一次性定时器
		hcu_timer_start(TASK_ID_TEMP, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_TEMP_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_BUSY;
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_TEMP_OPT_WFFB
		FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_OPT_WFFB);
	}//SENSOR_TEMP_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_IDLE;
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwStatus = SENSOR_TEMP_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;
	}

	return;
}

//for SPIBUSARIES start
void func_temp_time_out_read_data_from_spibusaries(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess == SENSOR_TEMP_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount < 0) || (gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount >= SENSOR_TEMP_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess =SENSOR_TEMP_HW_ACCESS_IDLE;
		}else{
			gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess == SENSOR_TEMP_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to SPIBUSARIES
		msg_struct_temp_spibusaries_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_temp_spibusaries_data_read_t));
		snd.length = sizeof(msg_struct_temp_spibusaries_data_read_t);
		snd.equId = gTaskTempContext.temp[gTaskTempContext.currentSensorId].equId;
		snd.cmdId = L3CI_temp;
		snd.optId = L3PO_temp_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_TEMP_SPIBUSARIES_DATA_READ, TASK_ID_SPIBUSARIES, TASK_ID_TEMP, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TEMP].taskName, zHcuVmCtrTab.task[TASK_ID_SPIBUSARIES].taskName);
			return;
		}

		//启动一次性定时器
		hcu_timer_start(TASK_ID_TEMP, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_TEMP_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_BUSY;
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_TEMP_OPT_WFFB
		FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_OPT_WFFB);
	}//SENSOR_TEMP_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_IDLE;
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwStatus = SENSOR_TEMP_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;
	}

	return;
}
//for SPIBUSARIES end

//超时失败，走向下一个传感器
void func_temp_time_out_processing_no_rsponse(void)
{
	//恢复当前传感器的空闲状态
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_IDLE;
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwStatus = SENSOR_TEMP_HW_STATUS_DEACTIVE;
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//CurrentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskTempContext.currentSensorId < 0) || (gTaskTempContext.currentSensorId >= (MAX_NUM_OF_SENSOR_TEMP_INSTALLED-1))){
		gTaskTempContext.currentSensorId = 0;
	}else{
		gTaskTempContext.currentSensorId++;
	}

	//暂时啥也不干，未来在瞬时模式下也许需要回一个失败的消息，当然缺省情况下没有反应就是表示失败

	//State Transfer to FSM_STATE_TEMP_ACTIVE
	FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_ACTIVED);
	return;
}

//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_temp_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_temp_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_temp_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_temp_data_report_t))){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
		HcuErrorPrint("TEMP: Receive message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	hcu_timer_stop(TASK_ID_TEMP, TIMER_ID_1S_TEMP_FB, TIMER_RESOLUTION_1S);

	gTaskTempContext.tempValue = (float)rcv.temp.tempValue/10;
	HCU_DEBUG_PRINT_INF("TEMP: temp = %.1f\n\n", gTaskTempContext.tempValue);

	//if(gTaskTempContext.tempFlag == FALSE)
	gTaskTempContext.tempFlag = CurrentModusContext.tempFlag;

/*
	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.temp.equipid;
			record.sensortype = L3CI_temp;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.temp.timeStamp;
			record.dataFormat = rcv.temp.dataFormat;
			record.tempValue = rcv.temp.tempValue;
			record.gpsx = rcv.temp.gps.gpsx;
			record.gpsy = rcv.temp.gps.gpsy;
			record.gpsz = rcv.temp.gps.gpsz;
			record.ew = rcv.temp.gps.ew;
			record.ns = rcv.temp.gps.ns;
			//RECORD还要存入数据库
			sensor_temp_data_element_t tempData;
			memset(&tempData, 0, sizeof(sensor_temp_data_element_t));
			tempData.equipid = record.equipid;
			tempData.timeStamp = record.timestamp;
			tempData.dataFormat = record.dataFormat;
			tempData.tempValue = record.tempValue;
			tempData.gps.gpsx = record.gpsx;
			tempData.gps.gpsy = record.gpsy;
			tempData.gps.gpsz = record.gpsz;
			tempData.gps.ew = record.ew;
			tempData.gps.ns = record.ns;
			tempData.onOffLineFlag = record.onOffLine;
			ret = dbi_HcuTempDataInfo_save(&tempData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
				HcuErrorPrint("TEMP: Can not save data into database!\n");
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
				HcuErrorPrint("TEMP: Offline but instance or other control message received!\n");
			}
	}
*/

	if(zHcuSysEngPar.hwBurnId.hwType != HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)
	{
		if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE))
		{
			//L2信息
			msg_struct_temp_cloudvela_data_resp_t snd;
			memset(&snd, 0, sizeof(msg_struct_temp_cloudvela_data_resp_t));
			strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
				sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
			strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
					sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
			snd.comHead.timeStamp = time(0);
			snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
			strcpy(snd.comHead.funcFlag, "0");

			//CONTENT
			snd.baseResp = HUITP_IEID_UNI_COM_REPORT_YES;
			snd.usercmdid = rcv.usercmdid;
			snd.cmdIdBackType = rcv.cmdIdBackType;
			snd.useroptid = rcv.useroptid;

			snd.temp.equipid = rcv.temp.equipid;
			snd.temp.timeStamp = rcv.temp.timeStamp;
			snd.temp.dataFormat = rcv.temp.dataFormat;
			snd.temp.tempValue = rcv.temp.tempValue;
			snd.temp.gps.gpsx = rcv.temp.gps.gpsx;
			snd.temp.gps.gpsy = rcv.temp.gps.gpsy;
			snd.temp.gps.gpsz = rcv.temp.gps.gpsz;
			snd.temp.gps.ew = rcv.temp.gps.ew;
			snd.temp.gps.ns = rcv.temp.gps.ns;
			snd.temp.nTimes = rcv.temp.nTimes;
			snd.temp.onOffLineFlag = rcv.temp.onOffLineFlag;
			snd.length = sizeof(msg_struct_temp_cloudvela_data_resp_t);

			//发送后台
			//if ((HCU_SYSCFG_SENSOR_REPORT_MODE_SET & HCU_SYSCFG_SENSOR_REPORT_MODE_INDIVIDUAL) == TRUE){
			if (HCU_SYSCFG_SENSOR_REPORT_MODE_SET == HCU_SYSCFG_SENSOR_REPORT_MODE_INDIVIDUAL){
				ret = hcu_message_send(MSG_ID_TEMP_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_TEMP, &snd, snd.length);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
					HcuErrorPrint("TEMP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TEMP].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
					return FAILURE;
				}
			}

			//Save to disk as request：在线是为了备份，离线是为了重发给后台
			//该函数，有待完成
			if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
				//存入内存缓冲磁盘，做为本地缓存，未来需要实现磁盘模式
				memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
				record.equipid = rcv.temp.equipid;
				record.sensortype = L3CI_temp;
				record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
				record.timestamp = rcv.temp.timeStamp;
				record.dataFormat = rcv.temp.dataFormat;
				record.tempValue = rcv.temp.tempValue;
				record.gpsx = rcv.temp.gps.gpsx;
				record.gpsy = rcv.temp.gps.gpsy;
				record.gpsz = rcv.temp.gps.gpsz;
				record.ew = rcv.temp.gps.ew;
				record.ns = rcv.temp.gps.ns;
				//RECORD还要存入数据库
				sensor_temp_data_element_t tempData;
				memset(&tempData, 0, sizeof(sensor_temp_data_element_t));
				tempData.equipid = record.equipid;
				tempData.timeStamp = record.timestamp;
				tempData.dataFormat = record.dataFormat;
				tempData.tempValue = record.tempValue;
				tempData.gps.gpsx = record.gpsx;
				tempData.gps.gpsy = record.gpsy;
				tempData.gps.gpsz = record.gpsz;
				tempData.gps.ew = record.ew;
				tempData.gps.ns = record.ns;
				tempData.onOffLineFlag = record.onOffLine;
				ret = dbi_HcuTempDataInfo_save(&tempData);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
					HcuErrorPrint("TEMP: Can not save data into database!\n");
				}
			}// Period mode OK!
			// Instance mode, no need store!
		}

		//差错情形
		else{
			HcuErrorPrint("TEMP: Wrong state of CLOUDVELA when data need send out!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
			//If this shall work normally, it is too much for each sensor STM process!
			return FAILURE;
		}
	}

	//恢复当前传感器的空闲状态
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwAccess = SENSOR_TEMP_HW_ACCESS_IDLE;
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].hwStatus = SENSOR_TEMP_HW_STATUS_ACTIVE;
	gTaskTempContext.temp[gTaskTempContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskTempContext.currentSensorId < 0) || (gTaskTempContext.currentSensorId >= (MAX_NUM_OF_SENSOR_TEMP_INSTALLED-1))){
		gTaskTempContext.currentSensorId = 0;
	}else{
		gTaskTempContext.currentSensorId++;
	}

	//State Transfer to FSM_STATE_TEMP_ACTIVE
	FsmSetState(TASK_ID_TEMP, FSM_STATE_TEMP_ACTIVED);
	return SUCCESS;
}

//读取源泉，这个可能是控制命令，也可能是数据瞬时读取命令
//状态机方面，会有点交叉混淆的
//瞬时模式，有待完成，初期的必要性不大
OPSTAT fsm_temp_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_temp_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_temp_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_temp_data_req_t))){
		HcuErrorPrint("TEMP: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
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

OPSTAT fsm_temp_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_temp_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_temp_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//暂时没考虑发送给后台云平台
OPSTAT func_temp_time_out_read_data_from_dht11(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].fVal >= HCU_SENSOR_TEMP_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].fVal <= HCU_SENSOR_TEMP_VALUE_MAX))
	{
		sensor_temp_dht11_data_element_t tempData;
		memset(&tempData, 0, sizeof(sensor_temp_dht11_data_element_t));
		tempData.equipid = 0;
		tempData.timeStamp = time(0);
		tempData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		tempData.tempValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].fVal*100);

		ret = dbi_HcuTempDht11DataInfo_save(&tempData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Can not save TempDht11 data into database!\n");
		}
	}

	return SUCCESS;
}

OPSTAT func_temp_time_out_read_data_from_sht20(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPSHT20].fVal >= HCU_SENSOR_TEMP_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPSHT20].fVal <= HCU_SENSOR_TEMP_VALUE_MAX))
	{
		sensor_temp_sht20_data_element_t tempData;
		memset(&tempData, 0, sizeof(sensor_temp_sht20_data_element_t));
		tempData.equipid = 0;
		tempData.timeStamp = time(0);
		tempData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		tempData.tempValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPSHT20].fVal*100);

		ret = dbi_HcuTempSht20DataInfo_save(&tempData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Can not save TempSht20 data into database!\n");
		}
	}

	return SUCCESS;
}

OPSTAT func_temp_time_out_read_data_from_rht03(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPRHT03].fVal >= HCU_SENSOR_TEMP_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPRHT03].fVal <= HCU_SENSOR_TEMP_VALUE_MAX))
	{
		sensor_temp_rht03_data_element_t tempData;
		memset(&tempData, 0, sizeof(sensor_temp_rht03_data_element_t));
		tempData.equipid = 0;
		tempData.timeStamp = time(0);
		tempData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		tempData.tempValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPRHT03].fVal*100);

		ret = dbi_HcuTempRht03DataInfo_save(&tempData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Can not save TempRht03 data into database!\n");
		}
	}

	return SUCCESS;
}

OPSTAT func_temp_time_out_read_data_from_bmp180(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPBMP180].fVal >= HCU_SENSOR_TEMP_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPBMP180].fVal <= HCU_SENSOR_TEMP_VALUE_MAX))
	{
		sensor_temp_bmp180_data_element_t tempData;
		memset(&tempData, 0, sizeof(sensor_temp_bmp180_data_element_t));
		tempData.equipid = 0;
		tempData.timeStamp = time(0);
		tempData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		tempData.tempValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_I2CTEMPBMP180].fVal*100);

		ret = dbi_HcuTempBmp180DataInfo_save(&tempData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Can not save TempBmp180 data into database!\n");
		}
	}

	return SUCCESS;
}

OPSTAT func_temp_time_out_read_data_from_mth01(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPMTH01].fVal >= HCU_SENSOR_TEMP_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPMTH01].fVal <= HCU_SENSOR_TEMP_VALUE_MAX))
	{
		sensor_temp_mth01_data_element_t tempData;
		memset(&tempData, 0, sizeof(sensor_temp_mth01_data_element_t));
		tempData.equipid = 0;
		tempData.timeStamp = time(0);
		tempData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		tempData.tempValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_SPITEMPMTH01].fVal*100);

		ret = dbi_HcuTempMth01DataInfo_save(&tempData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TEMP]++;
			HcuErrorPrint("TEMP: Can not save TempMth01 data into database!\n");
		}
	}

	return SUCCESS;
}


OPSTAT fsm_temp_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_temp_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}






