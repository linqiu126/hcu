/*
 * pwm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "../l1hwopt/pwm.h"
#include "../l0service/trace.h"

/*
** FSM of the PWM
*/
HcuFsmStateItem_t FsmPwm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_pwm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_pwm_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_pwm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_PWM_ACTIVIED,            		fsm_pwm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_PWM_ACTIVIED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_PWM_ACTIVIED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_PWM_ACTIVIED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_pwm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_PWM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("PWM: Error Set FSM State at fsm_pwm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_pwm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, conCounter=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_PWM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("PWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_PWM], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_PWM, FSM_STATE_PWM_INITED) == FAILURE){
		HcuErrorPrint("PWM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_pwm_int_init() == FAILURE){
		HcuErrorPrint("PWM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_PWM] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_PWM, FSM_STATE_PWM_ACTIVIED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PWM]++;
		HcuErrorPrint("PWM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("PWM: Enter FSM_STATE_PWM_ACTIVED status, Keeping refresh here!\n");
	}

	int workingCycle = 2;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SYSCFG_SNR_PRESENT_MOTOR_SG90 == HCU_SYSCFG_SENSOR_PRESENT_YES){
			func_pwm_write_data_motor_sg90();
			hcu_sleep(RPI_PWM_SENSOR_WRITE_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SYSCFG_SNR_PRESENT_PWM_LED2PIN == HCU_SYSCFG_SENSOR_PRESENT_YES){
			func_pwm_write_data_led_2pin();
			hcu_sleep(RPI_PWM_SENSOR_WRITE_GAP/workingCycle);
			conCounter++;
		}
		conCounter = workingCycle-conCounter;
		hcu_sleep(RPI_PWM_SENSOR_WRITE_GAP/workingCycle * conCounter);
	}

	return SUCCESS;
}

OPSTAT fsm_pwm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("PWM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_pwm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_pwm_int_init(void)
{
	return SUCCESS;
}

//控制TowerPro SG90马达的过程
//  下面是wiringPi提供的标准PWM函数，据说不好使，因为没法设置工作时钟/周期，导致20ms的SG90周期对不齐
//	pwmSetMode(PWM_MODE_BAL); //或者PWM_MODE_MS
//	pwmSetRange(1024); //默认为1024
//	pwmSetClock(50);
//	pwmWrite(RPI_PWM_PIN_OUTPUT, 1);
//  据说softPwmWrite的周期为6.64us，还不能改动，需要使用pwmWirte实验一番
//  如果的确是，则起0-2ms = 0-2000us对应的输入数值是0-301，再试验一番这个东西。
OPSTAT func_pwm_write_data_motor_sg90(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int pwmValueSg90 = 0;
	int i=0, j=0;

	softPwmCreate(RPI_PWM_PIN_OUTPUT_SG90, pwmValueSg90, RPI_PWM_WORKING_RANGE);
	//循环工作RPI_PWM_WRITE_REPEAT_TIMES个周期
	for (j=0; j<RPI_PWM_WRITE_REPEAT_TIMES; j++){
		for (i=0; i<RPI_PWM_WORKING_RANGE; i++){
			delay(20);
			pwmValueSg90 = 7 + (int)(i*21/255);
			softPwmWrite(RPI_PWM_PIN_OUTPUT_SG90, i);
			delay(20);
			softPwmWrite(RPI_PWM_PIN_OUTPUT_SG90, 0);
		}
		//HcuDebugPrint("PWM: SG90 Index = %d \n", j);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//控制简易LED的过程
OPSTAT func_pwm_write_data_led_2pin(void)
{
#ifdef TARGET_RASPBERRY_PI3B

	int pwmValueLed2pin = 0;
	int i=0, j=0;

	softPwmCreate(RPI_PWM_PIN_OUTPUT_LED2PIN, pwmValueLed2pin, RPI_PWM_WORKING_RANGE);
	//循环工作RPI_PWM_WRITE_REPEAT_TIMES个周期
	for (j=0; j<RPI_PWM_WRITE_REPEAT_TIMES; j++){
		for (i=0; i<RPI_PWM_WORKING_RANGE; i++){
			delay(20);
			softPwmWrite(RPI_PWM_PIN_OUTPUT_LED2PIN, i);
			delay(20);
			softPwmWrite(RPI_PWM_PIN_OUTPUT_LED2PIN, 0);
		}
		//HcuDebugPrint("PWM: LED2PIN Index = %d \n", j);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}



