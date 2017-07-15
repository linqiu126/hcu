/*
 * sensornoise.c
 *
 *  Created on: 2015年12月27日
 *      Author: test
 */

#include "sensornoise.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the NOISE
*/
HcuFsmStateItem_t HcuFsmNoise[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_noise_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_noise_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_NOISE_INITED,            		fsm_noise_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_NOISE_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_noise_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_noise_time_out},

    //Normal working status
	{MSG_ID_CLOUDVELA_NOISE_DATA_REQ,    	FSM_STATE_NOISE_ACTIVED,      			fsm_noise_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM,   FSM_STATE_NOISE_ACTIVED,      			fsm_noise_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_NOISE_CTRL_REQ,    	FSM_STATE_NOISE_ACTIVED,          		fsm_noise_cloudvela_ctrl_req},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,			FSM_STATE_NOISE_ACTIVED,      	  		fsm_noise_l3aqyc_exg_ctrl_req},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,			FSM_STATE_NOISE_ACTIVED,      	  		fsm_noise_zhbl3mod_exg_ctrl_req},


    //Wait for MODBUS Feedback
	{MSG_ID_MODBUS_NOISE_DATA_REPORT, 		FSM_STATE_NOISE_MODBUS_WFFB,        	fsm_noise_data_report_from_modbus},
	{MSG_ID_MODBUS_NOISE_CONTROL_FB,    	FSM_STATE_NOISE_MODBUS_WFFB,     		fsm_noise_modbus_control_fb},

    //Wait for Spsvirgo Feedback
	{MSG_ID_SPSVIRGO_NOISE_DATA_REPORT, 	FSM_STATE_NOISE_SPSVIRGO_WFFB,     		fsm_noise_data_report_from_spsvirgo},
	{MSG_ID_SPSVIRGO_NOISE_CONTROL_FB,    	FSM_STATE_NOISE_SPSVIRGO_WFFB,     		fsm_noise_spsvirgo_control_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskNoiseContext_t gTaskNoiseContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_noise_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("NOISE: Error Set FSM State at fsm_noise_task_entry\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_noise_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_NOISE, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("NOISE: Error Set FSM State at fsm_noise_init\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("NOISE: Enter FSM_STATE_NOISE_INITED status, everything goes well!\n");
	}

	//Task global variables init.
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE] = 0;
	memset(&gTaskNoiseContext, 0, sizeof(gTaskNoiseContext_t));
	gTaskNoiseContext.AlarmFlag = FALSE;

	//目前暂时只有一个NOISE传感器，但程序的框架可以支持无数个传感器
	//未来还需要支持传感器的地址可以被配置，随时被修改，通过后台命令
	for (i=0;i<MAX_NUM_OF_SENSOR_NOISE_INSTALLED;i++){
		gTaskNoiseContext.noise[i].sensorId = i;
//		if (SENSOR_ACTIVE_CHOICE_FINAL == SENSOR_ACTIVE_CHOICE_G20_2)
//		{
//			gTaskNoiseContext.noise[i].equId = MODBUS_NOISE_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
//		}
//		else if (SENSOR_ACTIVE_CHOICE_FINAL == SENSOR_ACTIVE_CHOICE_G20_1)
//		{
//			gTaskNoiseContext.noise[i].equId = SPSVIRGO_NOISE_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
//		}
//		else  //Default取SPSVIRGO
//		{
//			gTaskNoiseContext.noise[i].equId = SPSVIRGO_NOISE_RTU_EQUIPMENT_ID;  //该字段，除了配置命令之外，不能再修改
//		}
		gTaskNoiseContext.noise[i].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
		gTaskNoiseContext.noise[i].hwStatus = SENSOR_NOISE_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskNoiseContext.noise[i].busyCount = 0;
	}

	//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
	i = rand()%TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES;
	hcu_sleep(i);

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_NOISE, TIMER_ID_1S_NOISE_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_NOISE_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error start timer!\n");
		return FAILURE;
	}

	//State Transfer to FSM_STATE_NOISE_ACTIVED
	ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error Set FSM State at fsm_noise_init\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_noise_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("NOISE: Internal error counter reach MAJOR level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_noise_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_noise_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("NOISE: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_NOISE]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_NOISE, TASK_ID_NOISE, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_NOISE].taskName);
			return FAILURE;
		}
	}

	//Period time out received, send command to SPSVIRGO/MODBUS for read
	if ((rcv.timeId == TIMER_ID_1S_NOISE_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_NOISE) != FSM_STATE_NOISE_ACTIVED){
			ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
//		if (SENSOR_ACTIVE_CHOICE_FINAL == SENSOR_ACTIVE_CHOICE_G20_1)
//		{
//			func_noise_time_out_read_data_from_spsvirgo();
//		}
//		else if (SENSOR_ACTIVE_CHOICE_FINAL == SENSOR_ACTIVE_CHOICE_G20_2)
//		{
//			func_noise_time_out_read_data_from_modbus();
//		}
//		else //DEFAULT取标准高配置传感器
//		{
//			func_noise_time_out_read_data_from_spsvirgo();
//		}
	}

	//Spsvirgo received MODBUS time out message received
	else if ((rcv.timeId == TIMER_ID_1S_NOISE_MODBUS_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_noise_time_out_processing_no_rsponse();
	}

	//Spsvirgo received FB time out message received
	else if ((rcv.timeId == TIMER_ID_1S_NOISE_SPSVIRGO_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_noise_time_out_processing_no_rsponse();
	}

	return SUCCESS;
}


//超时失败，走向下一个传感器
void func_noise_time_out_processing_no_rsponse(void)
{
	int ret=0;

	//恢复当前传感器的空闲状态
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwStatus = SENSOR_NOISE_HW_STATUS_DEACTIVE;
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//CurrentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskNoiseContext.currentSensorId < 0) || (gTaskNoiseContext.currentSensorId >= (MAX_NUM_OF_SENSOR_NOISE_INSTALLED-1))){
		gTaskNoiseContext.currentSensorId = 0;
	}else{
		gTaskNoiseContext.currentSensorId++;
	}

	//暂时啥也不干，未来在瞬时模式下也许需要回一个失败的消息，当然缺省情况下没有反应就是表示失败

	//State Transfer to FSM_STATE_NOISE_ACTIVE
	ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error Set FSM State!\n");
		return;
	}//FsmSetState
	return;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_noise_time_out_read_data_from_modbus(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess == SENSOR_NOISE_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount < 0) || (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount >= SENSOR_NOISE_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess =SENSOR_NOISE_HW_ACCESS_IDLE;
		}else{
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess == SENSOR_NOISE_HW_ACCESS_IDLE){
		//Do somemthing
		//Send out message to MODBUS
		msg_struct_noise_modbus_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_noise_modbus_data_read_t));
		snd.length = sizeof(msg_struct_noise_modbus_data_read_t);
		snd.equId = gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].equId;
		snd.cmdId = L3CI_noise;
		snd.optId = L3PO_noise_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_NOISE_MODBUS_DATA_READ, TASK_ID_MODBUS, TASK_ID_NOISE, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName);
			return;
		}

		//启动一次性定时器
		ret = hcu_timer_start(TASK_ID_NOISE, TIMER_ID_1S_NOISE_MODBUS_FB, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_NOISE_MODBUS_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Error start timer!\n");
			return;
		}

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_BUSY;
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_NOISE_OPT_WFFB
		ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_MODBUS_WFFB);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Error Set FSM State!\n");
			return;
		}//FsmSetState
	}//SENSOR_NOISE_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwStatus = SENSOR_NOISE_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;
	}

	return;
}

//读取命令，只能操纵当前传感器
//周期定时器是所有传感器操纵的定海神针，在各种情况下，都要确保最终能回归，一旦出错，需要重新初始化
void func_noise_time_out_read_data_from_spsvirgo(void)
{
	int ret = 0;
	//如果当前传感器还处于忙的状态，意味着下面操作还未完成，继续等待，下一次再操作
	//通过多次等待，让离线的设备自动变成长周期读取一次
	if (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess == SENSOR_NOISE_HW_ACCESS_BUSY){
		//多次等待后强行恢复
		if ((gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount < 0) || (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount >= SENSOR_NOISE_HW_ACCESS_BUSY_COUNT_NUM_MAX)){
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess =SENSOR_NOISE_HW_ACCESS_IDLE;
		}else{
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount++;
			//Keep busy status
		}
		//状态机不能动，可能在ACTIVE，也可能在WFB，因为定时随时都可能来的
	}

	//是否可以通过HwInv模块中zHcuHwinvTable全局表，获取当前传感器的最新硬件状态，待确定
	//如果当前传感器处于空闲态，干活！
	else if (gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess == SENSOR_NOISE_HW_ACCESS_IDLE){
		//Do something
		//Send out message to SPSVIRGO
		msg_struct_noise_spsvirgo_data_read_t snd;
		memset(&snd, 0, sizeof(msg_struct_noise_spsvirgo_data_read_t));
		snd.length = sizeof(msg_struct_noise_spsvirgo_data_read_t);
		snd.equId = gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].equId;
		snd.cmdId = L3CI_noise;
		snd.optId = L3PO_noise_data_req;
		snd.cmdIdBackType = L3CI_cmdid_back_type_period;
		ret = hcu_message_send(MSG_ID_NOISE_SPSVIRGO_DATA_READ, TASK_ID_SPSVIRGO, TASK_ID_NOISE, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
			return;
		}
		else
		{
			HcuDebugPrint("NOISE: Send message suceed, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SPSVIRGO].taskName);
		}

		//启动一次性定时器
		ret = hcu_timer_start(TASK_ID_NOISE, TIMER_ID_1S_NOISE_SPSVIRGO_FB, NOISE_TIMER_DURATION_SPSVIRGO_FB, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Error start timer!\n");
			return;
		}

		//设置当前传感器到忙，没反应之前，不置状态
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_BUSY;
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;

		//State Transfer to FSM_STATE_NOISE_OPT_WFFB
		ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_SPSVIRGO_WFFB);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Error Set FSM State!\n");
			return;
		}//FsmSetState
	}//SENSOR_NOISE_HW_ACCESS_IDLE

	//任何其他状态，强制初始化
	else{
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwStatus = SENSOR_NOISE_HW_STATUS_ACTIVE;  //假设缺省为活跃状态
		gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;
	}

	return;
}


//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_noise_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_noise_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_noise_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_noise_data_report_t))){
		HcuErrorPrint("NOISE: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	ret = hcu_timer_stop(TASK_ID_NOISE, TIMER_ID_1S_NOISE_MODBUS_FB, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error stop timer!\n");
		return FAILURE;
	}



	//For HKvision option setting
	HKVisionOption_t HKVisionOption;
	memset( (void *)&HKVisionOption, 0, sizeof(HKVisionOption_t));

	strcat(HKVisionOption.user_key, "admin");
	strcat(HKVisionOption.user_key, ":");
	strcat(HKVisionOption.user_key, "Bxxh!123");

	strcat(HKVisionOption.url_photo, "http://192.168.111.64/Streaming/channels/1/picture");
	strcat(HKVisionOption.url_video_start, "http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/start/tracks/1");
	strcat(HKVisionOption.url_video_stop, "http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1");


	strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curPhotoDir);
	strcat(HKVisionOption.file_photo, "/");
	strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curHikvisionFname);
	strcpy(HKVisionOption.file_photo_pure, zHcuVmCtrTab.clock.curHikvisionPureFname);

	strcat(HKVisionOption.file_video, zHcuVmCtrTab.clock.curPhotoDir);
	strcat(HKVisionOption.file_video, "/");
	strcat(HKVisionOption.file_video, "hkvideo.txt");

	//判断如果Noise超过阀值，若超过，则需要设alarm flag = ON, 启动拍照和录像，并触发告警，告警报告中需要包括告警类型，告警内容，及需要上传照片的文件名（包含设备名字日期时间）和录像的开始日期、时间和停止的日期、时间。
	HCU_DEBUG_PRINT_INF("NOISE: noise = %d\n\n\n\n", (rcv.noise.noiseValue));
	if(rcv.noise.noiseValue >= (HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD*10))
	//if(rcv.noise.noiseValue >= zHcuSysEngPar.serialport.SeriesPortForGPS) //for debug
	{
		ret = hcu_hsmmp_photo_capture_start(HKVisionOption);
		if(FAILURE == ret){
			HcuErrorPrint("NOISE: Start HK photo capture error!\n\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		}

		if(FALSE == gTaskNoiseContext.AlarmFlag)
		{
			if(FAILURE == hcu_hsmmp_video_capture_start(HKVisionOption)){
				HcuErrorPrint("NOISE: Start HK video capture error!\n\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			}
		}

		gTaskNoiseContext.AlarmFlag = TRUE;

		//send alarm report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.noise.equipid;
		snd.alarmType = ALARM_TYPE_NOISE_VALUE;
		snd.alarmContent = ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD;
		if(FAILURE == ret){
			strcpy(snd.alarmDesc, "NOISE: Start HK photo capture error!");
		}else{
			strcpy(snd.alarmDesc, HKVisionOption.file_photo_pure);
		}

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_PM25, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_NOISE, "NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
	}

	//若没超过阀值，而且alarm flag = TRUE, 则将alarm flag = FALSE，停止拍照和录像，然后需要发告警清除报告
	if((rcv.noise.noiseValue <= HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD*10) && (gTaskNoiseContext.AlarmFlag == TRUE))

	{
		gTaskNoiseContext.AlarmFlag = FALSE;

		ret = hcu_hsmmp_video_capture_stop(HKVisionOption);
		if(FAILURE == ret)
		{
			HcuErrorPrint("NOISE: Stop HK video capture error!\n\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		}

		//send alarm clear report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.noise.equipid;
		snd.alarmType = ALARM_TYPE_NOISE_VALUE;
		snd.alarmContent = ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD;
		strcpy(snd.alarmDesc, HKVisionOption.file_photo_pure);

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_NOISE, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_NOISE, "NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
	}


/*
	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.noise.equipid;
			record.sensortype = L3CI_noise;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.noise.timeStamp;
			record.dataFormat = rcv.noise.dataFormat;
			record.noiseValue = rcv.noise.noiseValue;
			record.gpsx = rcv.noise.gps.gpsx;
			record.gpsy = rcv.noise.gps.gpsy;
			record.gpsz = rcv.noise.gps.gpsz;
			record.ew = rcv.noise.gps.ew;
			record.ns = rcv.noise.gps.ns;
			//RECORD还要存入数据库
			sensor_noise_data_element_t noiseData;
			memset(&noiseData, 0, sizeof(sensor_noise_data_element_t));
			noiseData.equipid = record.equipid;
			noiseData.timeStamp = record.timestamp;
			noiseData.dataFormat = record.dataFormat;
			noiseData.noiseValue = record.noiseValue;
			noiseData.gps.gpsx = record.gpsx;
			noiseData.gps.gpsy = record.gpsy;
			noiseData.gps.gpsz = record.gpsz;
			noiseData.gps.ew = record.ew;
			noiseData.gps.ns = record.ns;
			noiseData.onOffLineFlag = record.onOffLine;

			HCU_DEBUG_PRINT_INF("NOISE: noiseValue=%d\n\n\n\n", noiseData.noiseValue);

			ret = dbi_HcuNoiseDataInfo_save(&noiseData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Can not save data into database!\n");
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Offline but instance or other control message received!\n");
			}
	}
*/
	//在线模式
	//else if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
	//在线模式
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE))

	{
		//Online processing
		//赋值给发送消息

		/*
				msg_struct_noise_cloudvela_data_resp_t snd;
				memset(&snd, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
				snd.length = sizeof(msg_struct_noise_cloudvela_data_resp_t);
				snd.noise.equipid = rcv.noise.equipid;
				snd.noise.timeStamp = rcv.noise.timeStamp;
				snd.usercmdid = rcv.usercmdid;
				snd.cmdIdBackType = rcv.cmdIdBackType;
				snd.useroptid = rcv.useroptid;
				snd.noise.dataFormat = rcv.noise.dataFormat;
				snd.noise.noiseValue = rcv.noise.noiseValue;
				snd.noise.gps.gpsx = rcv.noise.gps.gpsx;
				snd.noise.gps.gpsy = rcv.noise.gps.gpsy;
				snd.noise.gps.gpsz = rcv.noise.gps.gpsz;
				snd.noise.gps.ew = rcv.noise.gps.ew;
				snd.noise.gps.ns = rcv.noise.gps.ns;
				ret = hcu_message_send(MSG_ID_NOISE_CLOUDVELA_DATA_RESP, TASK_ID_CLOUDVELA, TASK_ID_NOISE, &snd, snd.length);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
					HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
					return FAILURE;
				}
		*/
				///////////
				//L2信息
				msg_struct_noise_cloudvela_data_resp_t snd;
				memset(&snd, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
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

				snd.noise.equipid = rcv.noise.equipid;
				snd.noise.timeStamp = rcv.noise.timeStamp;
				snd.noise.dataFormat = rcv.noise.dataFormat;
				snd.noise.noiseValue = rcv.noise.noiseValue;
				snd.noise.gps.gpsx = rcv.noise.gps.gpsx;
				snd.noise.gps.gpsy = rcv.noise.gps.gpsy;
				snd.noise.gps.gpsz = rcv.noise.gps.gpsz;
				snd.noise.gps.ew = rcv.noise.gps.ew;
				snd.noise.gps.ns = rcv.noise.gps.ns;
				snd.noise.nTimes = rcv.noise.nTimes;
				snd.noise.onOffLineFlag = rcv.noise.onOffLineFlag;
				snd.length = sizeof(msg_struct_noise_cloudvela_data_resp_t);

				ret = hcu_message_send(MSG_ID_NOISE_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_NOISE, &snd, snd.length);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
					HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
					return FAILURE;
				}
				///////////

				//Save to disk as request：在线是为了备份，离线是为了重发给后台
				//该函数，有待完成
				if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
					//RECORD还要存入数据库
					memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
					record.equipid = rcv.noise.equipid;
					record.sensortype = L3CI_noise;
					record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
					record.timestamp = rcv.noise.timeStamp;
					record.dataFormat = rcv.noise.dataFormat;
					record.noiseValue = rcv.noise.noiseValue;
					record.gpsx = rcv.noise.gps.gpsx;
					record.gpsy = rcv.noise.gps.gpsy;
					record.gpsz = rcv.noise.gps.gpsz;
					record.ew = rcv.noise.gps.ew;
					record.ns = rcv.noise.gps.ns;
					//RECORD还要存入数据库
					sensor_noise_data_element_t noiseData;
					memset(&noiseData, 0, sizeof(sensor_noise_data_element_t));
					noiseData.equipid = record.equipid;
					noiseData.timeStamp = record.timestamp;
					noiseData.dataFormat = record.dataFormat;
					noiseData.noiseValue = record.noiseValue;
					noiseData.gps.gpsx = record.gpsx;
					noiseData.gps.gpsy = record.gpsy;
					noiseData.gps.gpsz = record.gpsz;
					noiseData.gps.ew = record.ew;
					noiseData.gps.ns = record.ns;
					noiseData.onOffLineFlag = record.onOffLine;

					//HCU_DEBUG_PRINT_INF("NOISE: noiseValue=%d\n\n\n\n", noiseData.noiseValue);

					ret = dbi_HcuNoiseDataInfo_save(&noiseData);
					if (ret == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
						HcuErrorPrint("NOISE: Can not save data into database!\n");
					}
				}// Period mode OK!
				// Instance mode, no need store!
			}

			//差错情形
			else{
				HcuErrorPrint("NOISE: Wrong state of CLOUDVELA when data need send out!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
				//If this shall work normally, it is too much for each sensor STM process!
				return FAILURE;
			}

			//恢复当前传感器的空闲状态
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwStatus = SENSOR_NOISE_HW_STATUS_ACTIVE;
			gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;

			//当前传感器指针指向下一个
			//Finished, then currentSensor+1 do firstly
			//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
			if ((gTaskNoiseContext.currentSensorId < 0) || (gTaskNoiseContext.currentSensorId >= (MAX_NUM_OF_SENSOR_NOISE_INSTALLED-1))){
				gTaskNoiseContext.currentSensorId = 0;
			}else{
				gTaskNoiseContext.currentSensorId++;
			}

			//State Transfer to FSM_STATE_NOISE_ACTIVE
			ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Error Set FSM State!\n");
				return FAILURE;
			}

			return SUCCESS;

}


//离线时，数据完全存储到后台硬件盘，让CLOUDVELA任务一旦在线时，自动送往后台
//在线时，直接将数据送往后台，由于系统备份的需要，这里必须也需要保存在硬盘中一定的时间长度
//通过查阅CLOUDVELA的状态来判断，是在线还是离线
OPSTAT fsm_noise_data_report_from_spsvirgo(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//这种申明方法，已经分配了完整的内存空间，不用再MALLOC单独申请内存了
	HcuDiscDataSampleStorageArray_t record;

	msg_struct_spsvirgo_noise_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_spsvirgo_noise_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spsvirgo_noise_data_report_t))){
		HcuErrorPrint("NOISE: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器
	ret = hcu_timer_stop(TASK_ID_NOISE, TIMER_ID_1S_NOISE_SPSVIRGO_FB, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error stop timer!\n");
		return FAILURE;
	}


	//For HKvision option setting
	HKVisionOption_t HKVisionOption;
	memset( (void *)&HKVisionOption, 0, sizeof(HKVisionOption_t));

	strcat(HKVisionOption.user_key, "admin");
	strcat(HKVisionOption.user_key, ":");
	strcat(HKVisionOption.user_key, "Bxxh!123");

	strcat(HKVisionOption.url_photo, "http://192.168.111.64/Streaming/channels/1/picture");
	strcat(HKVisionOption.url_video_start, "http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/start/tracks/1");
	strcat(HKVisionOption.url_video_stop, "http://192.168.111.64/ISAPI/ContentMgmt/record/control/manual/stop/tracks/1");


	strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curPhotoDir);
	strcat(HKVisionOption.file_photo, "/");
	strcat(HKVisionOption.file_photo, zHcuVmCtrTab.clock.curHikvisionFname);
	strcpy(HKVisionOption.file_photo_pure, zHcuVmCtrTab.clock.curHikvisionPureFname);

	strcat(HKVisionOption.file_video, zHcuVmCtrTab.clock.curPhotoDir);
	strcat(HKVisionOption.file_video, "/");
	strcat(HKVisionOption.file_video, "hkvideo.txt");

	//判断如果Noise超过阀值，若超过，则需要设alarm flag = ON, 启动拍照和录像，并触发告警，告警报告中需要包括告警类型，告警内容，及需要上传照片的文件名（包含设备名字日期时间）和录像的开始日期、时间和停止的日期、时间。
	HCU_DEBUG_PRINT_INF("NOISE: noise = %d\n\n\n\n", (rcv.noise.noiseValue));
	if(rcv.noise.noiseValue >= (HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD*10))
	//if(rcv.noise.noiseValue >= zHcuSysEngPar.serialport.SeriesPortForGPS) //for debug
	{
		ret = hcu_hsmmp_photo_capture_start(HKVisionOption);
		if(FAILURE == ret){
			HcuErrorPrint("NOISE: Start HK photo capture error!\n\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		}

		if(FALSE == gTaskNoiseContext.AlarmFlag)
		{
			if(FAILURE == hcu_hsmmp_video_capture_start(HKVisionOption)){
				HcuErrorPrint("NOISE: Start HK video capture error!\n\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			}
		}

		gTaskNoiseContext.AlarmFlag = TRUE;

		//send alarm report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.noise.equipid;
		snd.alarmType = ALARM_TYPE_NOISE_VALUE;
		snd.alarmContent = ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD;
		if(FAILURE == ret){
			strcpy(snd.alarmDesc, "NOISE: Start HK photo capture error!");
		}else{
			strcpy(snd.alarmDesc, HKVisionOption.file_photo_pure);
		}

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_PM25, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_NOISE, "NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
	}

	//若没超过阀值，而且alarm flag = TRUE, 则将alarm flag = FALSE，停止拍照和录像，然后需要发告警清除报告
	if((rcv.noise.noiseValue <= HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD*10) && (gTaskNoiseContext.AlarmFlag == TRUE))

	{
		gTaskNoiseContext.AlarmFlag = FALSE;

		ret = hcu_hsmmp_video_capture_stop(HKVisionOption);
		if(FAILURE == ret)
		{
			HcuErrorPrint("NOISE: Stop HK video capture error!\n\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		}

		//send alarm clear report
		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.noise.equipid;
		snd.alarmType = ALARM_TYPE_NOISE_VALUE;
		snd.alarmContent = ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD;
		strcpy(snd.alarmDesc, HKVisionOption.file_photo_pure);

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_NOISE, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_NOISE, "NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
	}


/*
	//离线模式
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//完整的处理情形，有待完成，存入磁盘还为实现
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.noise.equipid;
			record.sensortype = L3CI_noise;
			record.onOffLine = DISC_DATA_SAMPLE_OFFLINE;
			record.timestamp = rcv.noise.timeStamp;
			record.dataFormat = rcv.noise.dataFormat;
			record.noiseValue = rcv.noise.noiseValue;
			record.gpsx = rcv.noise.gps.gpsx;
			record.gpsy = rcv.noise.gps.gpsy;
			record.gpsz = rcv.noise.gps.gpsz;
			record.ew = rcv.noise.gps.ew;
			record.ns = rcv.noise.gps.ns;
			//RECORD还要存入数据库
			sensor_noise_data_element_t noiseData;
			memset(&noiseData, 0, sizeof(sensor_noise_data_element_t));
			noiseData.equipid = record.equipid;
			noiseData.timeStamp = record.timestamp;
			noiseData.dataFormat = record.dataFormat;
			noiseData.noiseValue = record.noiseValue;
			noiseData.gps.gpsx = record.gpsx;
			noiseData.gps.gpsy = record.gpsy;
			noiseData.gps.gpsz = record.gpsz;
			noiseData.gps.ew = record.ew;
			noiseData.gps.ns = record.ns;
			noiseData.onOffLineFlag = record.onOffLine;

			HCU_DEBUG_PRINT_INF("NOISE: noiseValue=%d\n\n", noiseData.noiseValue);


			ret = dbi_HcuNoiseDataInfo_save(&noiseData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Can not save data into database!\n");
			}
		}//周期模式
		else{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Offline but instance or other control message received!\n");
			}
	}
*/
	//在线模式
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE))
	{//debug by shanchun
		//Online processing
		//赋值给发送消息
/*
		msg_struct_noise_cloudvela_data_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
		snd.length = sizeof(msg_struct_noise_cloudvela_data_resp_t);
		snd.noise.equipid = rcv.noise.equipid;
		snd.noise.timeStamp = rcv.noise.timeStamp;
		snd.usercmdid = rcv.usercmdid;
		snd.cmdIdBackType = rcv.cmdIdBackType;
		snd.useroptid = rcv.useroptid;
		snd.noise.dataFormat = rcv.noise.dataFormat;
		snd.noise.noiseValue = rcv.noise.noiseValue;
		snd.noise.gps.gpsx = rcv.noise.gps.gpsx;
		snd.noise.gps.gpsy = rcv.noise.gps.gpsy;
		snd.noise.gps.gpsz = rcv.noise.gps.gpsz;
		snd.noise.gps.ew = rcv.noise.gps.ew;
		snd.noise.gps.ns = rcv.noise.gps.ns;
		ret = hcu_message_send(MSG_ID_NOISE_CLOUDVELA_DATA_RESP, TASK_ID_CLOUDVELA, TASK_ID_NOISE, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
			return FAILURE;
		}
*/
		///////////
		//L2信息
		msg_struct_noise_cloudvela_data_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
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

		snd.noise.equipid = rcv.noise.equipid;
		snd.noise.timeStamp = rcv.noise.timeStamp;
		snd.noise.dataFormat = rcv.noise.dataFormat;
		snd.noise.noiseValue = rcv.noise.noiseValue;
		snd.noise.gps.gpsx = rcv.noise.gps.gpsx;
		snd.noise.gps.gpsy = rcv.noise.gps.gpsy;
		snd.noise.gps.gpsz = rcv.noise.gps.gpsz;
		snd.noise.gps.ew = rcv.noise.gps.ew;
		snd.noise.gps.ns = rcv.noise.gps.ns;
		snd.noise.nTimes = rcv.noise.nTimes;
		snd.noise.onOffLineFlag = rcv.noise.onOffLineFlag;
		snd.length = sizeof(msg_struct_noise_cloudvela_data_resp_t);

		ret = hcu_message_send(MSG_ID_NOISE_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_NOISE, &snd, snd.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
			HcuErrorPrint("NOISE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_NOISE].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
			return FAILURE;
		}
		///////////

		//Save to disk as request：在线是为了备份，离线是为了重发给后台
		//该函数，有待完成
		if (rcv.cmdIdBackType == L3CI_cmdid_back_type_period){
			//RECORD还要存入数据库
			memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
			record.equipid = rcv.noise.equipid;
			record.sensortype = L3CI_noise;
			record.onOffLine = DISC_DATA_SAMPLE_ONLINE;
			record.timestamp = rcv.noise.timeStamp;
			record.dataFormat = rcv.noise.dataFormat;
			record.noiseValue = rcv.noise.noiseValue;
			record.gpsx = rcv.noise.gps.gpsx;
			record.gpsy = rcv.noise.gps.gpsy;
			record.gpsz = rcv.noise.gps.gpsz;
			record.ew = rcv.noise.gps.ew;
			record.ns = rcv.noise.gps.ns;
			//RECORD还要存入数据库
			sensor_noise_data_element_t noiseData;
			memset(&noiseData, 0, sizeof(sensor_noise_data_element_t));
			noiseData.equipid = record.equipid;
			noiseData.timeStamp = record.timestamp;
			noiseData.dataFormat = record.dataFormat;
			noiseData.noiseValue = record.noiseValue;
			noiseData.gps.gpsx = record.gpsx;
			noiseData.gps.gpsy = record.gpsy;
			noiseData.gps.gpsz = record.gpsz;
			noiseData.gps.ew = record.ew;
			noiseData.gps.ns = record.ns;
			noiseData.onOffLineFlag = record.onOffLine;

			//HCU_DEBUG_PRINT_INF("NOISE: noiseValue=%d\n\n\n\n", noiseData.noiseValue);

			ret = dbi_HcuNoiseDataInfo_save(&noiseData);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
				HcuErrorPrint("NOISE: Can not save data into database!\n");
			}
		}// Period mode OK!
		// Instance mode, no need store!
	}

	//差错情形
	else{
		HcuErrorPrint("NOISE: Wrong state of CLOUDVELA when data need send out!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		//CLOUDCOUNT not normal, here Sensor might work normally, to be further check!
		//If this shall work normally, it is too much for each sensor STM process!
		return FAILURE;
	}

	//恢复当前传感器的空闲状态
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwAccess = SENSOR_NOISE_HW_ACCESS_IDLE;
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].hwStatus = SENSOR_NOISE_HW_STATUS_ACTIVE;
	gTaskNoiseContext.noise[gTaskNoiseContext.currentSensorId].busyCount = 0;

	//当前传感器指针指向下一个
	//Finished, then currentSensor+1 do firstly
	//SensorId+1处理，Linux-C下模操作可能会出错，算法以后待处理优化
	if ((gTaskNoiseContext.currentSensorId < 0) || (gTaskNoiseContext.currentSensorId >= (MAX_NUM_OF_SENSOR_NOISE_INSTALLED-1))){
		gTaskNoiseContext.currentSensorId = 0;
	}else{
		gTaskNoiseContext.currentSensorId++;
	}

	//State Transfer to FSM_STATE_NOISE_ACTIVE
	ret = FsmSetState(TASK_ID_NOISE, FSM_STATE_NOISE_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
		HcuErrorPrint("NOISE: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//读取源泉，这个可能是控制命令，也可能是数据瞬时读取命令
//状态机方面，会有点交叉混淆的
//瞬时模式，有待完成，初期的必要性不大
OPSTAT fsm_noise_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_noise_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_noise_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_noise_data_req_t))){
		HcuErrorPrint("NOISE: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_NOISE]++;
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

OPSTAT fsm_noise_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_noise_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_noise_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_noise_spsvirgo_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_noise_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_noise_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}




