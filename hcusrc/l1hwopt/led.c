/*
 * led.c
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */


#include "../l1hwopt/led.h"
#include "../l0service/trace.h"

/*
** FSM of the LED
*/
HcuFsmStateItem_t HcuFsmLed[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_led_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_led_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_led_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_LED_ACTIVIED,            		fsm_led_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_LED_ACTIVIED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_LED_ACTIVIED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_TIME_OUT,      	FSM_STATE_LED_ACTIVIED,          		fsm_led_time_out},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_LED_ACTIVIED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Galowag总控表：以秒为单位
//具体的闪烁图案，需要通过ON/OFF/CYCLE自行定义，初始化就在这里提前初始化写好
GlobalGalowagCtrlTable_t zHcuGalogwagCtrlTable[] =
{
//ID,    状态控制          ON/OFF/CYCLE/总工作周期/双计数器, LED_ON的函数， LED_OFF的函数
	{GALOWAG_CTRL_ID_MIN, GALOWAG_CTRL_STATE_MIN, 0, 0, 0, 0, 0, 0, NULL, NULL},//Starting
	{GALOWAG_CTRL_ID_GLOBAL_POWER, GALOWAG_CTRL_STATE_OFF, 0, 0, 0, 0, 0, 0, NULL, NULL},
	{GALOWAG_CTRL_ID_GLOBAL_COMMU, GALOWAG_CTRL_STATE_OFF, 0, 0, 0, 0, 0, 0, NULL, NULL},
	{GALOWAG_CTRL_ID_GLOBAL_WORK_STATE, GALOWAG_CTRL_STATE_OFF, 2, 2, 4, 0xFFFF, 0, 0, func_led_work_state_on, func_led_work_state_off},
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{GALOWAG_CTRL_ID_TEST1, GALOWAG_CTRL_STATE_OFF, 1, 10, 11, 22, 0, 0, func_led_test1_on, func_led_test1_off},
#endif
  {GALOWAG_CTRL_ID_MAX,GALOWAG_CTRL_STATE_MIN, 0, 0, 0, 0, 0, 0, NULL, NULL},//Ending
};


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_led_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_LED, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("LED: Error Set FSM State at fsm_led_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_led_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, conCounter=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_LED, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("LED: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LED].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_LED, FSM_STATE_LED_INITED) == FAILURE){
		HcuErrorPrint("LED: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_led_int_init() == FAILURE){
		HcuErrorPrint("LED: Error initialize interface!\n");
		return FAILURE;
	}

	//Global variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_LED] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LED, FSM_STATE_LED_ACTIVIED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_LED]++;
		HcuErrorPrint("LED: Error Set FSM State!\n");
		return FAILURE;
	}

	//方波扫描输出
	if (HCU_LED_GALOWAG_FUNC_SET == HCU_LED_GALOWAG_FUNC_ACTIVE){
		//TIMER_ID_1S_LED_GALOWAG_SCAN，是为扫描方波信号的生成，标准的1秒为单位
		ret = hcu_timer_start(TASK_ID_LED, TIMER_ID_1S_LED_GALOWAG_SCAN, 1, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HcuErrorPrint("LED: Error start timer!\n");
			return FAILURE;
		}
	}

	//打印信息
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("LED: Enter FSM_STATE_LED_ACTIVED status, Keeping refresh here!\n");
	}

	//正常是不会进入到这儿的
	if (HCU_LED_BLINK_OLD_MODE_SET == HCU_LED_BLINK_OLD_MODE_SET){
		int workingCycle = 1;
		//进入循环工作模式
		while(1){
			conCounter = 0;
			if (HCU_SENSOR_PRESENT_LED_LED2PIN == HCU_SENSOR_PRESENT_YES){
				func_led_write_data_led2pin();
				hcu_sleep(RPI_LED_SENSOR_WRITE_GAP/workingCycle);
				conCounter++;
			}
			conCounter = workingCycle-conCounter;
			hcu_sleep(RPI_LED_SENSOR_WRITE_GAP/workingCycle * conCounter);
		}
	}

	return SUCCESS;
}

OPSTAT fsm_led_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("LED: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_led_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_led_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_led_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("EMC: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_LED]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//Period time out received, send command to MODBUS for read
	if ((rcv.timeId == TIMER_ID_1S_EMC_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		func_led_time_out_galowag_scan();
	}

	return SUCCESS;
}


OPSTAT func_led_write_data_led2pin(void)
{
#ifdef TARGET_RASPBERRY_PI3B

	int i=0;

    pinMode(RPI_LED_PIN_OUTPUT_LED2PIN, OUTPUT);
    delay(25);
    digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, 0);
    delay(25);

    //慢闪图案
    for (i=0;i<RPI_LED_WRITE_REPEAT_TIMES;i++){
    	delay(500);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, LOW);
    	delay(500);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, HIGH);
    }

    //快闪图案
    for (i=0;i<RPI_LED_WRITE_REPEAT_TIMES*5;i++){
    	delay(100);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, LOW);
    	delay(100);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, HIGH);
    }

    //少亮图案
    for (i=0;i<RPI_LED_WRITE_REPEAT_TIMES*2;i++){
    	delay(500);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, LOW);
    	delay(100);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, HIGH);
    }

    //多亮图案
    for (i=0;i<RPI_LED_WRITE_REPEAT_TIMES*2;i++){
    	delay(100);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, LOW);
    	delay(500);
    	digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, HIGH);
    }

    //结束，复位到灭灯
    delay(25);
    digitalWrite(RPI_LED_PIN_OUTPUT_LED2PIN, 0);

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//WORK_STATE灯开
void func_led_work_state_on(void)
{
	return;
}

//WORK_STATE灯灭
void func_led_work_state_off(void)
{
	return;
}

//TEST1灯开
void func_led_test1_on(void)
{
	return;
}

//TEST1灯灭
void func_led_test1_off(void)
{
	return;
}

//GALOWAG的主循环程序，扫描所有设定的波形生成器
void func_led_time_out_galowag_scan(void)
{
	int i = 0;
	for (i = GALOWAG_CTRL_ID_MIN+1; i<GALOWAG_CTRL_ID_MAX; i++){
		if (zHcuGalogwagCtrlTable[i].galState == GALOWAG_CTRL_STATE_ON){
			zHcuGalogwagCtrlTable[i].galTotalWorkCounter++;
			if (zHcuGalogwagCtrlTable[i].galTotalWorkCounter >= zHcuGalogwagCtrlTable[i].galTotalDuration) zHcuGalogwagCtrlTable[i].galState = GALOWAG_CTRL_STATE_OFF;
			if (zHcuGalogwagCtrlTable[i].galPeriodWorkCounter == 0){
				(void)zHcuGalogwagCtrlTable[i].galFuncOn();
			}
			if (zHcuGalogwagCtrlTable[i].galPeriodWorkCounter == zHcuGalogwagCtrlTable[i].galPattenOn){
				(void)zHcuGalogwagCtrlTable[i].galFuncOff();
			}
			//放到下一次，是为了先激活灯，避免点亮需要等待一个周期满
			zHcuGalogwagCtrlTable[i].galPeriodWorkCounter = (zHcuGalogwagCtrlTable[i].galTotalWorkCounter + 1) % zHcuGalogwagCtrlTable[i].galPattenCycle;
		}
	}

	return;
}

//暂时不考虑周期可设，都是在程序初始化的时候HARD_CODE进去
OPSTAT hcu_led_galowag_start(UINT8 galId, UINT32 galDur)
{
	//检查参数
	if ((galId <= GALOWAG_CTRL_ID_MIN || galId >= GALOWAG_CTRL_ID_MAX))
		HCU_ERROR_PRINT_TASK(TASK_ID_LED, "LED: Receive parameter error!\n");

	//设置并启动
	zHcuGalogwagCtrlTable[galId].galTotalDuration = galDur;
	zHcuGalogwagCtrlTable[galId].galState = GALOWAG_CTRL_STATE_ON;

	return SUCCESS;
}

OPSTAT hcu_led_galowag_stop(UINT8 galId)
{
	//检查参数
	if ((galId <= GALOWAG_CTRL_ID_MIN || galId >= GALOWAG_CTRL_ID_MAX))
		HCU_ERROR_PRINT_TASK(TASK_ID_LED, "LED: Receive parameter error!\n");

	//设置并启动
	zHcuGalogwagCtrlTable[galId].galState = GALOWAG_CTRL_STATE_OFF;

	return SUCCESS;
}


