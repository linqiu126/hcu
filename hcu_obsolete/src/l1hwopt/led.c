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
HcuFsmStateItem_t FsmLed[] =
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
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_LED_ACTIVIED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

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
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_LED, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("LED: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_LED], zHcuSysCrlTab.taskRun.taskName[src_id]);
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
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("LED: Enter FSM_STATE_LED_ACTIVED status, Keeping refresh here!\n");
	}

	int workingCycle = 1;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SYSCFG_SNR_PRESENT_LED_LED2PIN == HCU_SYSCFG_SENSOR_PRESENT_YES){
			func_led_write_data_led2pin();
			hcu_sleep(RPI_LED_SENSOR_WRITE_GAP/workingCycle);
			conCounter++;
		}
		conCounter = workingCycle-conCounter;
		hcu_sleep(RPI_LED_SENSOR_WRITE_GAP/workingCycle * conCounter);
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


