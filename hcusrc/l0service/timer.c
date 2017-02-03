 /*
 * timer.c
 *
 *  Created on: 2015年11月15日
 *      Author: test
 */
#include "../l0service/timer.h"

#include "../l0service/trace.h"

/*
** FSM of the TIMER
*/
FsmStateItem_t HcuFsmTimer[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_timer_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_timer_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_timer_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//TIMER并没有继续处理消息的能力，谨慎处理接收这些消息
    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_TIMER_ACTIVED,            	fsm_timer_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_TIMER_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_TIMER_ACTIVED,   				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_TIMER_ACTIVED,   				fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
HcuTimerTable_t zHcuTimerTable;
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_timer_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_TIMER, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("TIMER: Error Set FSM State at fsm_timer_task_entry\n");
	}
	return SUCCESS;
}

OPSTAT fsm_timer_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to HCU-MAIN
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);
		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_TIMER, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//INIT this task global variables zHcuTimerTable
	memset(&zHcuTimerTable, 0, sizeof(HcuTimerTable_t));
	zHcuRunErrCnt[TASK_ID_TIMER] = 0;

	//收到初始化消息后，进入初始化状态
	ret = FsmSetState(TASK_ID_TIMER, FSM_STATE_TIMER_INITED);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		HcuErrorPrint("TIMER: Error Set FSM State at fsm_timer_init\n");
		return FAILURE;
	}

	/*
	 *   方法一：失败告终，目前这种机制跟SLEEP相冲突，蛋疼，留待以后的高手帮助解决
	 */
	//启动信号量级的永恒定时器，颗粒度只支持两种，1S和10MS的
	/*if (signal(SIGPROF, func_timer_routine) == SIG_ERR){
		HcuErrorPrint("TIMER: Error set signal = SIGPROF for 1S resolution timer!\n");
	}
	if (signal(SIGVTALRM, func_timer_routine) == SIG_ERR){
		HcuErrorPrint("TIMER: Error set signal = SIGVTALRM for 1S resolution timer!\n");
	}
	//signal(SIGALRM, func_timer_routine);
	//常用定时器
	struct itimerval t1s, t10ms;
	zHcuTimerSet1s.iniValue.tvSec = HCU_TIMER_TICK_1_SEC;
	zHcuTimerSet1s.iniValue.tvUsec = 0;
	zHcuTimerSet1s.intValue.tvSec = HCU_TIMER_TICK_1_SEC;
	zHcuTimerSet1s.intValue.tvUsec = 0;
	t1s.it_value.tv_sec = 1;
	t1s.it_value.tv_usec = 0;
	t1s.it_interval.tv_sec = 1;
	t1s.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_PROF, &t1s, NULL) == -1){
		HcuErrorPrint("TIMER: Error set ITIMER_PROF!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}
	//L2帧协议定时器
	zHcuTimerSet10ms.iniValue.tvSec = 0;
	zHcuTimerSet10ms.iniValue.tvUsec = HCU_TIMER_TICK_10_MS;
	zHcuTimerSet10ms.intValue.tvSec = 0;
	zHcuTimerSet10ms.intValue.tvUsec = HCU_TIMER_TICK_10_MS;
	t10ms.it_value.tv_sec = 0;
	t10ms.it_value.tv_usec = 100000;
	t10ms.it_interval.tv_sec = 0;
	t10ms.it_interval.tv_usec = 100000;
	if (setitimer(ITIMER_VIRTUAL, &t10ms, NULL) == -1){
		HcuErrorPrint("TIMER: Error set ITIMER_VIRTUAL!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}
	//实时数据流定时器
	zHcuTimerSet1ms.iniValue.tvSec = 0;
	zHcuTimerSet1ms.iniValue.tvUsec = HCU_TIMER_TICK_1_MS;
	zHcuTimerSet1ms.intValue.tvSec = 0;
	zHcuTimerSet1ms.intValue.tvUsec = HCU_TIMER_TICK_1_MS;
	if (setitimer(ITIMER_REAL, &zHcuTimerSet1ms, NULL) == -1){
		HcuErrorPrint("TIMER: Error set ITIMER_REAL\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}*/

	/*
	 *   方法二：sigaction方式，测试很长时间了，虽然出现一些问题，但还是能用的，技巧就是将VM中 void hcu_sleep(UINT32 second)做如下处理
	 *   	//if ((rand()%1000) > 998){
				HcuDebugPrint("HCU-VM: Sleep interrupt by other higher level system call, remaining %d second to be executed\n", second);
			//}
	 *   失败告终，目前这种机制跟SLEEP相冲突，蛋疼，留待以后的高手帮助解决
	 *   相关的处理函数void func_timer_routine(int signo)先保留，毕竟它是能工作的，系统的静态性肯定要更好。
	 */
	//1s级别常用定时器
    /*timer_t timer1;
    struct sigevent evp1;
    struct sigaction act;
    //Init figure
    memset(&act, 0, sizeof(act));
    act.sa_handler = func_timer_routine;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
    	HcuErrorPrint("TIMER: Error set SIGUSR1 sigaction()!\n");
    	zHcuRunErrCnt[TASK_ID_TIMER]++;
    	return FAILURE;
    }
    memset(&evp1, 0, sizeof(struct sigevent));
    evp1.sigev_signo = SIGUSR1;
    evp1.sigev_notify = SIGEV_SIGNAL;
    if (timer_create(CLOCK_REALTIME, &evp1, &timer1) == -1) {
    	HcuErrorPrint("TIMER: Error set SIGUSR1 timer_create()!\n");
    	zHcuRunErrCnt[TASK_ID_TIMER]++;
    	return FAILURE;
    }
    struct itimerspec it;
    it.it_interval.tv_sec = HCU_TIMER_TICK_1_SEC;
    it.it_interval.tv_nsec = 0;
    it.it_value.tv_sec = HCU_TIMER_TICK_1_SEC;
    it.it_value.tv_nsec = 0;
    if (timer_settime(timer1, 0, &it, 0) == -1) {
    	HcuErrorPrint("TIMER: Error set SIGUSR1 timer_settime()!\n");
    	zHcuRunErrCnt[TASK_ID_TIMER]++;
    	return FAILURE;
    }*/


	/*
	 *   方法三：线程机制，实时系统的稳态性肯定要差点，但还是在某种限度内，没有太大的问题。
	 */
    // 1s

	if (HCU_TIMER_CONFIG_START_RESOLUTION_1S == TRUE){
	    struct sigevent evp1s;
	    struct itimerspec ts1s;
	    timer_t timer1s;

	    memset (&evp1s, 0, sizeof (evp1s));
	    evp1s.sigev_value.sival_ptr = &timer1s;
	    evp1s.sigev_notify = SIGEV_THREAD;
	    evp1s.sigev_notify_function = func_timer_routine_handler_1s;
	    evp1s.sigev_value.sival_int = 3; //作为func_timer_routine_handler_1s的参数

	    if (timer_create(CLOCK_REALTIME, &evp1s, &timer1s) == -1) {
	    	HcuErrorPrint("TIMER: Error create timer timer_create()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }

	    ts1s.it_interval.tv_sec = HCU_TIMER_TICK_1_SEC;
	    ts1s.it_interval.tv_nsec = 0;
	    ts1s.it_value.tv_sec = HCU_TIMER_TICK_1_SEC;
	    ts1s.it_value.tv_nsec = 0;

	    if (timer_settime(timer1s, TIMER_ABSTIME, &ts1s, NULL) == -1) {
	    	HcuErrorPrint("TIMER: Error set timer timer_settime()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }
	}

    //10ms帧协议定时器
	if (HCU_TIMER_CONFIG_START_RESOLUTION_10MS == TRUE){
	    struct sigevent evp10ms;
	    struct itimerspec ts10ms;
	    timer_t timer10ms;

	    memset (&evp10ms, 0, sizeof (evp10ms));
	    evp10ms.sigev_value.sival_ptr = &timer10ms;
	    evp10ms.sigev_notify = SIGEV_THREAD;
	    evp10ms.sigev_notify_function = func_timer_routine_handler_10ms;
	    evp10ms.sigev_value.sival_int = 3; //作为func_timer_routine_handler_10ms的参数

	    if (timer_create(CLOCK_REALTIME, &evp10ms, &timer10ms) == -1) {
	    	HcuErrorPrint("TIMER: Error create timer timer_create()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }
	    ts10ms.it_interval.tv_sec = 0;
	    ts10ms.it_interval.tv_nsec = HCU_TIMER_TICK_10_MS_IN_NS;
	    ts10ms.it_value.tv_sec = 0;
	    ts10ms.it_value.tv_nsec = HCU_TIMER_TICK_10_MS_IN_NS;

	    if (timer_settime(timer10ms, TIMER_ABSTIME, &ts10ms, NULL) == -1) {
	    	HcuErrorPrint("TIMER: Error set timer timer_settime()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }
	}

    //1ms时钟，这个应该是挺影响性能的，如果系统中没有用到，可以去掉
	if (HCU_TIMER_CONFIG_START_RESOLUTION_1MS == TRUE){
	    struct sigevent evp1ms;
	    struct itimerspec ts1ms;
	    timer_t timer1ms;

	    memset (&evp1ms, 0, sizeof (evp1ms));
	    evp1ms.sigev_value.sival_ptr = &timer1ms;
	    evp1ms.sigev_notify = SIGEV_THREAD;
	    evp1ms.sigev_notify_function = func_timer_routine_handler_1ms;
	    evp1ms.sigev_value.sival_int = 3; //作为func_timer_routine_handler_1ms的参数

	    if (timer_create(CLOCK_REALTIME, &evp1ms, &timer1ms) == -1) {
	    	HcuErrorPrint("TIMER: Error create timer timer_create()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }
	    ts1ms.it_interval.tv_sec = 0;
	    ts1ms.it_interval.tv_nsec = HCU_TIMER_TICK_10_MS_IN_NS;
	    ts1ms.it_value.tv_sec = 0;
	    ts1ms.it_value.tv_nsec = HCU_TIMER_TICK_10_MS_IN_NS;

	    if (timer_settime(timer1ms, TIMER_ABSTIME, &ts1ms, NULL) == -1) {
	    	HcuErrorPrint("TIMER: Error set timer timer_settime()!\n");
	    	zHcuRunErrCnt[TASK_ID_TIMER]++;
	    	return FAILURE;
	    }
	}

	//进入等待反馈状态
	ret = FsmSetState(TASK_ID_TIMER, FSM_STATE_TIMER_ACTIVED);
	if (ret == FAILURE){
		HcuErrorPrint("TIMER: Error Set FSM State!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("TIMER: Enter FSM_STATE_TIMER_ACTIVED status, Keeping loop of signal sending here!\n");
	}

	while(1) {
		pause();
	  }
	return SUCCESS;
}

OPSTAT fsm_timer_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret = 0;

	//为了提高效率，全靠HEART_BEAT的钩子触发该动作，本模块并不钩住
	HcuErrorPrint("TIMER: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_timer_init(0, 0, NULL, 0);

	//由于时钟是系统工作的基础，该模块启动后，必须让SVRCON启动，并初始化所有任务模块
	msg_struct_com_restart_t snd0;
	memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
	snd0.length = sizeof(msg_struct_com_restart_t);
	ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_SVRCON, TASK_ID_TIMER, &snd0, snd0.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[TASK_ID_SVRCON]);
		return FAILURE;
	}
	return SUCCESS;
}

void func_timer_routine(int signo)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	switch (signo){
	case SIGUSR1:
	{
		//HcuDebugPrint("TIMER: SIGPROF 1S resolution timer enter one time!\n");
		for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_1S;i++)
		{
			//Active status
			if (zHcuTimerTable.timer1s[i].status == TIMER_STATUS_ACTIVE){
				if (zHcuTimerTable.timer1s[i].elapse > zHcuTimerTable.timer1s[i].tDuration){
					HcuErrorPrint("TIMER: Error elapse value meet!\n");
					zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].elapse % zHcuTimerTable.timer1s[i].tDuration;
				}
				if (zHcuTimerTable.timer1s[i].elapse >= 1){
					//防止出现潜在问题，求余再减1
					zHcuTimerTable.timer1s[i].elapse--;
				}else{
					//change ACTIVE to STOP, no
					if (zHcuTimerTable.timer1s[i].timerType == TIMER_TYPE_ONE_TIME){
						zHcuTimerTable.timer1s[i].status = TIMER_STATUS_STOP;
						zHcuTimerTable.timer1s[i].elapse = 0;
					}
					//No change for PERIOD, re-start this timer
					//本轮也算1秒，所以需要在周期的基础上减1
					else if (zHcuTimerTable.timer1s[i].timerType == TIMER_TYPE_PERIOD){
						zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].tDuration-1;
					}
					//Send message time_out
					memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
					snd.length = sizeof(msg_struct_com_time_out_t);
					snd.timeId = zHcuTimerTable.timer1s[i].timerId;
					snd.timeRes = zHcuTimerTable.timer1s[i].timerRes;
					ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer1s[i].taskId, TASK_ID_TIMER, &snd, snd.length);
					if (ret == FAILURE){
						zHcuRunErrCnt[TASK_ID_TIMER]++;
						HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer1s[i].taskId]);
						return;
					}
				}//Elapse <= 0, timeout reach
			}
			//START status
			else if(zHcuTimerTable.timer1s[i].status == TIMER_STATUS_START){
				zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].tDuration;
				zHcuTimerTable.timer1s[i].status = TIMER_STATUS_ACTIVE;
			}//Not meet ACTIVE or START timer
		}//for-MAX_TIMER_NUM_IN_ONE_HCU_1S
	   break;
	}//SIGUSR1
	/*case SIGUSR2:
	{
		HcuDebugPrint("TIMER: SIGVTALRM 10MS resolution timer enter one time!");
		for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_10MS;i++){
			if (zHcuTimerTable.timer10ms[i].status == TIMER_STATUS_ACTIVE){
				if (zHcuTimerTable.timer10ms[i].elapse >= 1){
					//防止出现潜在问题，求余再减1
					zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].elapse % zHcuTimerTable.timer10ms[i].tDuration - 1;
				}else{
					zHcuTimerTable.timer10ms[i].elapse = 0;
					//change ACTIVE to STOP, no
					if (zHcuTimerTable.timer10ms[i].timerType == TIMER_TYPE_ONE_TIME){
						zHcuTimerTable.timer10ms[i].status = TIMER_STATUS_STOP;
						zHcuTimerTable.timer10ms[i].elapse = 0;
					}
					//No change for PERIOD, re-start this timer
					else if (zHcuTimerTable.timer10ms[i].timerType == TIMER_TYPE_PERIOD){
						zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].tDuration;
					}
					//Send message time_out
					memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
					snd.length = sizeof(msg_struct_com_time_out_t);
					snd.timeId = zHcuTimerTable.timer10ms[i].taskId;
					snd.timeRes = zHcuTimerTable.timer10ms[i].timerRes;
					ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer10ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
					if (ret == FAILURE){
						zHcuRunErrCnt[TASK_ID_TIMER]++;
						HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer10ms[i].taskId]);
						return;}
				}//Elapse <= 0, timeout reach
			//START status
			}else if(zHcuTimerTable.timer10ms[i].status == TIMER_STATUS_START){
				zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].tDuration;
				zHcuTimerTable.timer10ms[i].status = TIMER_STATUS_ACTIVE;
			}//Not meet ACTIVE or START timer
		}//for-MAX_TIMER_NUM_IN_ONE_HCU_10MS
		break;
	}//SIGUSR2*/
	/*case SIGALRM:
	{
		HcuDebugPrint("TIMER: SIGALRM 1MS resolution timer enter one time!");
		signal(SIGALRM, func_timer_routine);
		for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_1MS;i++){
			if (zHcuTimerTable.timer1ms[i].status == TIMER_STATUS_ACTIVE){
				if (zHcuTimerTable.timer1ms[i].elapse >= 1){
					//防止出现潜在问题，求余再减1
					zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].elapse % zHcuTimerTable.timer1ms[i].tDuration - 1;
				}else{
					zHcuTimerTable.timer1ms[i].elapse = 0;
					//change ACTIVE to STOP, no
					if (zHcuTimerTable.timer1ms[i].timerType == TIMER_TYPE_ONE_TIME){
						zHcuTimerTable.timer1ms[i].status = TIMER_STATUS_STOP;
						zHcuTimerTable.timer1ms[i].elapse = 0;
					}
					//No change for PERIOD, re-start this timer
					else if (zHcuTimerTable.timer1ms[i].timerType == TIMER_TYPE_PERIOD){
						zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].tDuration;
					}
					//Send message time_out
					memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
					snd.length = sizeof(msg_struct_com_time_out_t);
					snd.timeId = zHcuTimerTable.timer1ms[i].taskId;
					snd.timeRes = zHcuTimerTable.timer1ms[i].timerRes;
					ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer1ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
					if (ret == FAILURE){
						zHcuRunErrCnt[TASK_ID_TIMER]++;
						HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer1ms[i].taskId]);
						return;}
				}//Elapse <= 0, timeout reach
			//START status
			}else if(zHcuTimerTable.timer1ms[i].status == TIMER_STATUS_START){
				zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].tDuration;
				zHcuTimerTable.timer1ms[i].status = TIMER_STATUS_ACTIVE;
			}//Not meet ACTIVE or START ti -l
			mer
		}//for-MAX_TIMER_NUM_IN_ONE_HCU_1MS
		break;
	}//SIGALRM*/
	default:
	{
		break;
	}//default
	return;
	}//switch
}//end of func_timer_routine


/*
 *
http://blog.csdn.net/david_xtd/article/details/18252463
POSIX Timer
 *
间隔定时器 setitimer 有一些重要的缺点，POSIX Timer 对 setitimer 进行了增强，克服了 setitimer 的诸多问题：
首先，一个进程同一时刻只能有一个 timer。假如应用需要同时维护多个 Interval 不同的计时器，必须自己写代码来维护。这非常不方便。使用 POSIX Timer，一个进程可以创建任意多个 Timer。
setitmer 计时器时间到达时，只能使用信号方式通知使用 timer 的进程，而 POSIX timer 可以有多种通知方式，比如信号，或者启动线程。
使用 setitimer 时，通知信号的类别不能改变：SIGALARM，SIGPROF 等，而这些都是传统信号，而不是实时信号，因此有 timer overrun 的问题；而 POSIX Timer 则可以使用实时信号。
setimer 的精度是 ms，POSIX Timer 是针对有实时要求的应用所设计的，接口支持 ns 级别的时钟精度。
表 2. POSIX Timer 函数
函数名 	功能描述
timer_create 	创建一个新的 Timer；并且指定定时器到时通知机制
timer_delete 	删除一个 Timer
timer_gettime 	Get the time remaining on a POSIX.1b interval timer
timer_settime 	开始或者停止某个定时器。
timer_getoverrun 	获取丢失的定时通知个数。
 *
 */

//线程机制，搞定时器
void func_timer_routine_handler_1s(union sigval v)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	if (v.sival_int !=3) return;

	//HcuDebugPrint("TIMER: 1s resolution timer enter one time!\n");
	//HcuDebugPrint("TIMER: SIGPROF 1S resolution timer enter one time!\n");
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_1S;i++)
	{
		//Active status
		if (zHcuTimerTable.timer1s[i].status == TIMER_STATUS_ACTIVE){
			if (zHcuTimerTable.timer1s[i].elapse > zHcuTimerTable.timer1s[i].tDuration){
				HcuErrorPrint("TIMER: Error elapse value meet!\n");
				zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].elapse % zHcuTimerTable.timer1s[i].tDuration;
			}
			if (zHcuTimerTable.timer1s[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zHcuTimerTable.timer1s[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zHcuTimerTable.timer1s[i].timerType == TIMER_TYPE_ONE_TIME){
					zHcuTimerTable.timer1s[i].status = TIMER_STATUS_STOP;
					zHcuTimerTable.timer1s[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1秒，所以需要在周期的基础上减1
				else if (zHcuTimerTable.timer1s[i].timerType == TIMER_TYPE_PERIOD){
					zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zHcuTimerTable.timer1s[i].timerId;
				snd.timeRes = zHcuTimerTable.timer1s[i].timerRes;
				ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer1s[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_TIMER]++;
					HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer1s[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zHcuTimerTable.timer1s[i].status == TIMER_STATUS_START){
			zHcuTimerTable.timer1s[i].elapse = zHcuTimerTable.timer1s[i].tDuration;
			zHcuTimerTable.timer1s[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_HCU_1S
}


//线程机制，搞定时器
void func_timer_routine_handler_10ms(union sigval v)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	if (v.sival_int !=3) return;

	//HcuDebugPrint("TIMER: 10ms resolution timer enter one time!\n");
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_10MS;i++)
	{
		//Active status
		if (zHcuTimerTable.timer10ms[i].status == TIMER_STATUS_ACTIVE){
			if (zHcuTimerTable.timer10ms[i].elapse > zHcuTimerTable.timer10ms[i].tDuration){
				HcuErrorPrint("TIMER: Error elapse value meet!\n");
				zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].elapse % zHcuTimerTable.timer10ms[i].tDuration;
			}
			if (zHcuTimerTable.timer10ms[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zHcuTimerTable.timer10ms[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zHcuTimerTable.timer10ms[i].timerType == TIMER_TYPE_ONE_TIME){
					zHcuTimerTable.timer10ms[i].status = TIMER_STATUS_STOP;
					zHcuTimerTable.timer10ms[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1次，所以需要在周期的基础上减1，不然会出现问题
				else if (zHcuTimerTable.timer10ms[i].timerType == TIMER_TYPE_PERIOD){
					zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zHcuTimerTable.timer10ms[i].timerId;
				snd.timeRes = zHcuTimerTable.timer10ms[i].timerRes;
				ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer10ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_TIMER]++;
					HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer10ms[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zHcuTimerTable.timer10ms[i].status == TIMER_STATUS_START){
			zHcuTimerTable.timer10ms[i].elapse = zHcuTimerTable.timer10ms[i].tDuration;
			zHcuTimerTable.timer10ms[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_HCU_10MS
}

//线程机制，搞定时器
void func_timer_routine_handler_1ms(union sigval v)
{
	int i=0, ret=0;
	msg_struct_com_time_out_t snd;

	if (v.sival_int !=3) return;

	//HcuDebugPrint("TIMER: 1ms resolution timer enter one time!\n");
	for (i=0;i<MAX_TIMER_NUM_IN_ONE_HCU_1MS;i++)
	{
		//Active status
		if (zHcuTimerTable.timer1ms[i].status == TIMER_STATUS_ACTIVE){
			if (zHcuTimerTable.timer1ms[i].elapse > zHcuTimerTable.timer1ms[i].tDuration){
				HcuErrorPrint("TIMER: Error elapse value meet!\n");
				zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].elapse % zHcuTimerTable.timer1ms[i].tDuration;
			}
			if (zHcuTimerTable.timer1ms[i].elapse >= 1){
				//防止出现潜在问题，求余再减1
				zHcuTimerTable.timer1ms[i].elapse--;
			}else{
				//change ACTIVE to STOP, no
				if (zHcuTimerTable.timer1ms[i].timerType == TIMER_TYPE_ONE_TIME){
					zHcuTimerTable.timer1ms[i].status = TIMER_STATUS_STOP;
					zHcuTimerTable.timer1ms[i].elapse = 0;
				}
				//No change for PERIOD, re-start this timer
				//本轮也算1次，所以需要在周期的基础上减1，不然会出现问题
				else if (zHcuTimerTable.timer1ms[i].timerType == TIMER_TYPE_PERIOD){
					zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].tDuration-1;
				}
				//Send message time_out
				memset(&snd, 0, sizeof(msg_struct_com_time_out_t));
				snd.length = sizeof(msg_struct_com_time_out_t);
				snd.timeId = zHcuTimerTable.timer1ms[i].timerId;
				snd.timeRes = zHcuTimerTable.timer1ms[i].timerRes;
				ret = hcu_message_send(MSG_ID_COM_TIME_OUT, zHcuTimerTable.timer1ms[i].taskId, TASK_ID_TIMER, &snd, snd.length);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_TIMER]++;
					HcuErrorPrint("TIMER: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_TIMER], zHcuTaskNameList[zHcuTimerTable.timer1ms[i].taskId]);
					return;
				}
			}//Elapse <= 0, timeout reach
		}
		//START status
		else if(zHcuTimerTable.timer1ms[i].status == TIMER_STATUS_START){
			zHcuTimerTable.timer1ms[i].elapse = zHcuTimerTable.timer1ms[i].tDuration;
			zHcuTimerTable.timer1ms[i].status = TIMER_STATUS_ACTIVE;
		}//Not meet ACTIVE or START timer
	}//for-MAX_TIMER_NUM_IN_ONE_HCU_1MS
}


//由于SIGALRM信号量太强大，没法中断，故而先不实现1MS精度的时钟，未来再考虑方式
OPSTAT hcu_timer_start(UINT32 task_id, UINT32 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res)
{
	//检查task_id是否合法
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("TIMER: Error on timer start src_id =%d [%s]!!!\n", task_id, zHcuTaskNameList[task_id]);
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查timer_duartion是否合法
	if ((t_dur > MAX_TIMER_SET_DURATION) || (t_dur <=0)){
		HcuErrorPrint("TIMER: Error on timer start timer duration!!!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_type是否合法
	if ((t_type != TIMER_TYPE_ONE_TIME) && (t_type != TIMER_TYPE_PERIOD)){
		HcuErrorPrint("TIMER: Error on timer start type!!!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_resolution & timer_id 是否合法
	if (t_res == TIMER_RESOLUTION_1S){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_1S) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer start 1S timerId!!!\n");
			zHcuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_10MS) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer start 10MS timerId!!!\n");
			zHcuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}
	else if (t_res == TIMER_RESOLUTION_1MS){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_1MS) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer start 1MS timerId!!!\n");
			return FAILURE;
		}
	}
	else{
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		HcuErrorPrint("TIMER: Error on timer start timer resolution!!!\n");
		return FAILURE;
	}

	//设置进全局数据表单
	if (t_res == TIMER_RESOLUTION_1S){
		zHcuTimerTable.timer1s[timer_id].taskId = task_id;
		zHcuTimerTable.timer1s[timer_id].timerId = timer_id;
		zHcuTimerTable.timer1s[timer_id].tDuration = t_dur;
		zHcuTimerTable.timer1s[timer_id].timerType = t_type;
		zHcuTimerTable.timer1s[timer_id].timerRes = TIMER_RESOLUTION_1S;
		zHcuTimerTable.timer1s[timer_id].status = TIMER_STATUS_START;
	}
	else if (t_res == TIMER_RESOLUTION_10MS){
		zHcuTimerTable.timer10ms[timer_id].taskId = task_id;
		zHcuTimerTable.timer10ms[timer_id].timerId = timer_id;
		zHcuTimerTable.timer10ms[timer_id].tDuration = t_dur;
		zHcuTimerTable.timer10ms[timer_id].timerType = t_type;
		zHcuTimerTable.timer10ms[timer_id].timerRes = TIMER_RESOLUTION_10MS;
		zHcuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_START;
	}else if (t_res == TIMER_RESOLUTION_1MS){
		zHcuTimerTable.timer1ms[timer_id].taskId = task_id;
		zHcuTimerTable.timer1ms[timer_id].timerId = timer_id;
		zHcuTimerTable.timer1ms[timer_id].tDuration = t_dur;
		zHcuTimerTable.timer1ms[timer_id].timerType = t_type;
		zHcuTimerTable.timer1ms[timer_id].timerRes = TIMER_RESOLUTION_1MS;
		zHcuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_START;
	}

	return SUCCESS;
}

OPSTAT hcu_timer_stop(UINT32 task_id, UINT32 timer_id, UINT8 t_res)
{
	//检查task_id是否合法
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("TIMER: Error on timer stop src_id =%d [%s]!!!\n", task_id, zHcuTaskNameList[task_id]);
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//检查t_resolution & timer_id 是否合法
	if (t_res == TIMER_RESOLUTION_1S){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_1S) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer stop 1S timerId!!!\n");
			zHcuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_10MS) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer stop 10MS timerId!!!\n");
			zHcuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else if (t_res == TIMER_RESOLUTION_1MS){
		if ((timer_id > MAX_TIMER_NUM_IN_ONE_HCU_1MS) || (timer_id <0)){
			HcuErrorPrint("TIMER: Error on timer stop 1MS timerId!!!\n");
			zHcuRunErrCnt[TASK_ID_TIMER]++;
			return FAILURE;
		}
	}else{
		HcuErrorPrint("TIMER: Error on timer stop timer resolution!!!\n");
		zHcuRunErrCnt[TASK_ID_TIMER]++;
		return FAILURE;
	}

	//设置进全局数据表单
	if (t_res == TIMER_RESOLUTION_1S){
		if ((zHcuTimerTable.timer1s[timer_id].taskId == task_id) && (zHcuTimerTable.timer1s[timer_id].timerId == timer_id)){
			zHcuTimerTable.timer1s[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zHcuTimerTable.timer1s[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}else if (t_res == TIMER_RESOLUTION_10MS){
		if ((zHcuTimerTable.timer10ms[timer_id].taskId == task_id) && (zHcuTimerTable.timer10ms[timer_id].timerId == timer_id)){
			zHcuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zHcuTimerTable.timer10ms[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}else if (t_res == TIMER_RESOLUTION_1MS){
		if ((zHcuTimerTable.timer1ms[timer_id].taskId == task_id) && (zHcuTimerTable.timer1ms[timer_id].timerId == timer_id)){
			zHcuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_STOP;
		}else{
			zHcuTimerTable.timer1ms[timer_id].status = TIMER_STATUS_DEACTIVE;
		}
	}
	return SUCCESS;
}
