/*
 * syspm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "syspm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"

//#include <stdio.h>


/*
** FSM of the SYSPM
*/
HcuFsmStateItem_t HcuFsmSyspm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_syspm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_syspm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_SYSPM_INITED,            		fsm_syspm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_SYSPM_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            			fsm_syspm_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          				fsm_syspm_time_out},

    //Task level functions
	{MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ,          FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_alarm_req},
	{MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM,      FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_alarm_confirm},
	{MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ,          FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_perfm_req},
	{MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM,      FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_perfm_confirm},
	{MSG_ID_COM_ALARM_REPORT,      				FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_com_alarm_report},
	{MSG_ID_COM_PM_REPORT,      				FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_com_pm_report},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_REQ,         FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_test_command_req},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM,     FSM_STATE_SYSPM_ACTIVED,                 fsm_syspm_cloudvela_test_command_confirm},


    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskSyspmContext_t gTaskSyspmContext;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_syspm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_SYSPM, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_syspm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SYSPM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_INITED);

	//初始化硬件接口
	if (func_syspm_int_init() == FAILURE){
		HcuErrorPrint("SYSPM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] = 0;
	memset(&zHcuSysStaPm.statisCnt, 0, sizeof(HcuGlobalCounter_t));
	hcu_timer_start(TASK_ID_SYSPM, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_SYSPM_PERIOD_WORKING), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//设置状态机到目标状态
	FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_ACTIVED);
	HCU_DEBUG_PRINT_FAT("SYSPM: Enter FSM_STATE_SYSPM_ACTIVED status, Keeping refresh here!\n");

	return SUCCESS;
}

OPSTAT fsm_syspm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SYSPM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_syspm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_syspm_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_syspm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_com_time_out_t);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_SYSPM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_SYSPM, TASK_ID_SYSPM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
			HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SYSPM_PERIOD_WORKING) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_ACTIVED);

		//拷贝zHcuRunErrCnt到目标全局变量中
		memcpy(&zHcuSysStaPm.statisCnt.errCnt[0], &zHcuSysStaPm.taskRunErrCnt[0], (sizeof(UINT32)*HCU_SYSDIM_TASK_NBR_MAX));
		//检查COUNTER的情况，并生成相应的事件。这里暂时空着

		func_syspm_cal_cpu_mem_disk_occupy();
		func_syspm_get_cpu_temp();

		//存储事件到数据库中，形成报告
		if (dbi_HcuSyspmGlobalDataInfo_save() == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
			HcuErrorPrint("SYSPM: Error save PM data into DB!\n");
		}

		//PM report to Cloud added by ZSC
		if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE))
		{
			msg_struct_spspm_cloudvela_perfm_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_perfm_report_t));

			//L2信息
			strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
				sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
			strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
					sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));

			snd.comHead.timeStamp = time(0);
			snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
			strcpy(snd.comHead.funcFlag, "0");

			//CONTENT
			snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
			snd.restartCnt = zHcuSysStaPm.statisCnt.restartCnt;
			snd.socketDiscCnt = zHcuSysStaPm.statisCnt.SocketDiscCnt;
			snd.cpuOccupy = zHcuSysStaPm.statisCnt.cpu_occupy;
			snd.memOccupy = zHcuSysStaPm.statisCnt.mem_occupy;
			snd.diskOccupy = zHcuSysStaPm.statisCnt.disk_occupy;
			snd.cpuTemp = zHcuSysStaPm.statisCnt.cpu_temp;
			snd.timeStamp = time(0);
			snd.length = sizeof(msg_struct_spspm_cloudvela_perfm_report_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM);
		}
	}

	return SUCCESS;
}




void func_syspm_get_memoccupy (PmMemOccupyInfo_t *mem) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    char buff[256];
    PmMemOccupyInfo_t *m;
    m=mem;

    fd = fopen ("/proc/meminfo", "r");
    if (fd == NULL) return;

    fgets (buff, sizeof(buff), fd);
    fgets (buff, sizeof(buff), fd);
    fgets (buff, sizeof(buff), fd);
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %d %s", m->name, &m->used, m->name2);

    fgets (buff, sizeof(buff), fd); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里
    sscanf (buff, "%s %d", m->name2, &m->total);

    fclose(fd);     //关闭文件fd
}

UINT32 func_syspm_cal_cpuoccupy (PmCpuOccupyInfo_t *o, PmCpuOccupyInfo_t *n)
{
    UINT32 od, nd;
    UINT32 id, sd;
    UINT32 cpu_use = 0;

    od = (UINT32) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (UINT32) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od

    id = (UINT32) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (UINT32) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
    if((nd-od) != 0)
    	cpu_use = (UINT32)((sd+id)*100)/((nd-od)==0?0.01:(nd-od)); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给cpu_used
    else cpu_use = 0;

    return cpu_use;
}

void func_syspm_get_cpuoccupy (PmCpuOccupyInfo_t *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    char buff[256];
    PmCpuOccupyInfo_t *cpu_occupy;
    cpu_occupy=cpust;

    fd = fopen ("/proc/stat", "r");
    if (fd == NULL) return;

    fgets (buff, sizeof(buff), fd);

    sscanf (buff, "%s %d %d %d %d", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle);

    fclose(fd);
}

void func_syspm_get_diskoccupy(void)
{
	struct statfs diskInfo;
	statfs("/", &diskInfo);
	unsigned long long totalBlocks = diskInfo.f_bsize;
	unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
	size_t mbTotalsize = totalSize>>20;
	unsigned long long freeDisk = diskInfo.f_bfree * totalBlocks;
	size_t mbFreedisk = freeDisk>>20;

	float r = (float)(mbTotalsize - mbFreedisk)*100/(mbTotalsize==0?0.01:mbTotalsize);
	zHcuSysStaPm.statisCnt.disk_occupy = r;
	HCU_DEBUG_PRINT_NOR("SYSPM: Disk total = %dMB, free=%dMB, usage/total ratio =%.2f%%, diskoccupy=%d\n", mbTotalsize, mbFreedisk,r, zHcuSysStaPm.statisCnt.disk_occupy);
}

void func_syspm_cal_cpu_mem_disk_occupy(void)
{
	PmCpuOccupyInfo_t cpu_stat1;
	PmCpuOccupyInfo_t cpu_stat2;
	PmMemOccupyInfo_t mem_stat;


    //获取内存
    func_syspm_get_memoccupy ((PmMemOccupyInfo_t *)&mem_stat);
    //HcuDebugPrint("syspm:  mem_stat.total= %d, mem_stat.used = %d\n", mem_stat.total, mem_stat.used);

    zHcuSysStaPm.statisCnt.mem_occupy = mem_stat.used*100/((mem_stat.total == 0)?0.01:mem_stat.total);

    func_syspm_get_diskoccupy();


    //第一次获取cpu使用情况
    func_syspm_get_cpuoccupy((PmCpuOccupyInfo_t *)&cpu_stat1);
    sleep(10);

    //第二次获取cpu使用情况
    func_syspm_get_cpuoccupy((PmCpuOccupyInfo_t *)&cpu_stat2);

    //计算cpu使用率
    zHcuSysStaPm.statisCnt.cpu_occupy = func_syspm_cal_cpuoccupy ((PmCpuOccupyInfo_t *)&cpu_stat1, (PmCpuOccupyInfo_t *)&cpu_stat2);

}

void func_syspm_get_cpu_temp(void)
{

	FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (fp == NULL) return;
	(void)fscanf(fp,"%u", &zHcuSysStaPm.statisCnt.cpu_temp);
	zHcuSysStaPm.statisCnt.cpu_temp = (UINT32)(zHcuSysStaPm.statisCnt.cpu_temp)/1000;
	//printf("CPU Temprature = %u \n",zHcuSysStaPm.statisCnt.cpu_temp);
    fclose(fp);

	//exit(1);

}

OPSTAT fsm_syspm_cloudvela_alarm_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_spspm_alarm_req_t);

	return SUCCESS;
}

OPSTAT fsm_syspm_cloudvela_alarm_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_spspm_alarm_confirm_t);

	return SUCCESS;
}

OPSTAT fsm_syspm_cloudvela_perfm_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_spspm_perfm_req_t);

	return SUCCESS;
}

OPSTAT fsm_syspm_cloudvela_perfm_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_spspm_perfm_confirm_t);

	return SUCCESS;
}

//未来需要集中到本地表单，然后通过一定的统计机制触发并汇报告警。当前暂时采用单件汇报机制。
OPSTAT fsm_syspm_com_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_com_alarm_report_t);

	//参数检查
	if ((rcv.equID <= 0) || (rcv.usercmdid != L3CI_alarm) || (rcv.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("SYSPM: Receive invalid data!\n");

	if(dbi_HcuSysAlarmInfo_save(&rcv) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSPM, "SYSPM: Can not save data into database!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){//如果只发给Home,应该是Curl的连接状态
	//if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE)){//debug by shanchun

		msg_struct_spspm_cloudvela_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_alarm_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		//snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.alarmType = rcv.alarmType;
		snd.equID = rcv.equID;
		snd.alarmServerity = rcv.alarmServerity;
		snd.alarmClearFlag = rcv.alarmClearFlag;
		snd.causeId = 0;
		snd.timeStamp = rcv.timeStamp;
		snd.alarmContent = rcv.alarmContent;
		snd.length = sizeof(msg_struct_spspm_cloudvela_alarm_report_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM);
//		if (hcu_message_send(MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_TASK(TASK_ID_SYSPM, "SYSPM:: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//State no change
	return SUCCESS;
}

//未来需要集中到本地表单，然后通过一定的统计机制触发并汇报性能。当前暂时采用单件汇报机制。
OPSTAT fsm_syspm_com_pm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_com_pm_report_t);

	//参数检查
	if ((rcv.usercmdid != L3CI_performance) || (rcv.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("SYSPM: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){//如果只发给Home,应该是Curl的连接状态
	//if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE)){//debug by shanchun

		msg_struct_spspm_cloudvela_perfm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_perfm_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.restartCnt = rcv.TaskRestartCnt;
		//snd.networkConnCnt = rcv.CloudVelaConnCnt;
		//snd.networkConnFailCnt = rcv.CloudVelaConnFailCnt;
		//snd.networkDiscCnt = rcv.CloudVelaDiscCnt;
		snd.socketDiscCnt = rcv.SocketDiscCnt;
		snd.cpuOccupy = rcv.cpu_occupy;
		snd.memOccupy = rcv.mem_occupy;
		snd.diskOccupy = rcv.disk_occupy;
		snd.cpuTemp = rcv.cpu_temp;
		snd.timeStamp = rcv.timeStamp;
		snd.length = sizeof(msg_struct_spspm_cloudvela_perfm_report_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM);
//		if (hcu_message_send(MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_TASK(TASK_ID_PM25, "SYSPM:: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//State no change
	return SUCCESS;
}

OPSTAT fsm_syspm_cloudvela_test_command_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_test_command_req_t);

	return SUCCESS;
}

OPSTAT fsm_syspm_cloudvela_test_command_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_SYSPM, msg_struct_cloudvela_test_command_confirm_t);

	return SUCCESS;
}








