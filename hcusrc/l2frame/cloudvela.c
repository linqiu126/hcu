/*
 * cloudvela.c
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l0comvm/sysversion.h"
#include "../l2frame/cloudvela.h"
#include "../l0webser/ftp.h"

/************************************************************************************************************************
 *
 *  内外部消息命名规则
 *
 *  - DATA_REQ/DATA_RESP：表明是由后台发往前台的数据请求以及应答
 *  - CMD_REQ/CMD_RESP, CTRL_CMD/CTRL_FB：表明是由后台发往前台的命令请求以及应答
 *  - DATA_REPORT/DATA_CONFIRM：表明是由前台发往后台的数据汇报，以及回头的回应
 *
 *************************************************************************************************************************/

/*
** FSM of the CLOUDVELA
*/
HcuFsmStateItem_t HcuFsmCloudvela[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            		fsm_cloudvela_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            			fsm_cloudvela_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            			fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_CLOUDVELA_INITED,         fsm_cloudvela_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_CLOUDVELA_INITED,         fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          			fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            		fsm_cloudvela_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          			fsm_cloudvela_time_out},

    //Offline working, 定时重新启动链路，但不接受任何L3消息 //HWINV发来了硬件状态的改变，一般是硬件重新插拔造成的PnP状态改变
	{MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,   	FSM_STATE_CLOUDVELA_OFFLINE, 		fsm_cloudvela_hwinv_phy_status_chg},
	{MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_hwinv_phy_status_chg},

	//Online working， 从后台接收到数据和控制命令，四种均有可能，具体是哪一种起作用，将由HWINV定时扫描并解决互斥问题
	//CURL方式暂未启动，完全只采用了SOCKET方式
	{MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_curl_data_rx},
	{MSG_ID_USBNET_CLOUDVELA_DATA_RX,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_curl_data_rx},  //fsm_cloudvela_usbnet_data_rx
	{MSG_ID_WIFI_CLOUDVELA_DATA_RX,   			FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_curl_data_rx},  //fsm_cloudvela_wifi_data_rx
	{MSG_ID_3G4G_CLOUDVELA_DATA_RX,   			FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_curl_data_rx},  //fsm_cloudvela_3g4g_data_rx
	{MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,  FSM_STATE_CLOUDVELA_OFFLINE, 		fsm_cloudvela_socket_data_rx},
	{MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,  FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_socket_data_rx},

    //通用服务能力处理部分，UL上行链路处理部分，DL下行在解包函数中自动路由完成
	{MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_syspm_alarm_resp},
	{MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_syspm_alarm_report},
	{MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_syspm_perfm_resp},
	{MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_syspm_perfm_report},
	{MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_inventory_resp},
	{MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT,  FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_inventory_report},
	{MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP,   FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_sw_package_resp},
	{MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT, FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_sw_package_report},
	{MSG_ID_SYSSWM_CLOUDVELA_TIME_SYNC_RESP,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_time_sync_resp},
	{MSG_ID_SYSSWM_CLOUDVELA_TIME_SYNC_REPORT, 	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_sysswm_time_sync_report},

	//一般通用传感器业务部分，UL上行链路处理部分，DL下行在解包函数中自动路由完成
	{MSG_ID_EMC_CLOUDVELA_DATA_RESP,   			FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_emc_data_resp},
	{MSG_ID_EMC_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_emc_contrl_fb},
	{MSG_ID_PM25_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_pm25_data_resp},
	{MSG_ID_PM25_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_pm25_contrl_fb},
	{MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP, 		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_winddir_data_resp},
	{MSG_ID_WINDDIR_CLOUDVELA_CONTROL_FB,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_winddir_contrl_fb},
	{MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_windspd_data_resp},
	{MSG_ID_WINDSPD_CLOUDVELA_CONTROL_FB,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_windspd_contrl_fb},
	{MSG_ID_TEMP_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_temp_data_resp},
	{MSG_ID_TEMP_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_temp_contrl_fb},
	{MSG_ID_HUMID_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_humid_data_resp},
	{MSG_ID_HUMID_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_humid_contrl_fb},
	{MSG_ID_NOISE_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_noise_data_resp},
	{MSG_ID_NOISE_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_noise_contrl_fb},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_hsmmp_data_resp},
	{MSG_ID_HSMMP_CLOUDVELA_CONTROL_FB,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_hsmmp_control_fb},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_hsmmp_data_link_resp},

	//采用分项目控制方式，降低不同项目之间的关联，特别是海量MSGID-STATE这一表的内存压力。UL上行链路处理部分，DL下行在解包函数中自动路由完成
	#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP,         FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_l3bfsc_data_resp},
	{MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT,       FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_l3bfsc_data_report},
	{MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT,      FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_l3bfsc_event_report},
	{MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP,          FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_l3bfsc_cmd_resp},
	{MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT,  FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_l3bfsc_statistic_report},
	#endif

	//结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskCloudvelaContext_t gTaskCloudvelaContext;

//Main Entry
/***************************************************************************************************************************
 *
 * 　FSM初始化部分
 *
 ***************************************************************************************************************************/
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_cloudvela_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUDVELA: Error Set FSM State at fsm_cloudvela_task_entry\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_cloudvela_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_CLOUDVELA, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	ret = FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUDVELA: Error Set FSM State at fsm_cloudvela_init\n");
		return FAILURE;
	}
	HCU_DEBUG_PRINT_FAT("CLOUDVELA: Enter FSM_STATE_CLOUDVELA_INITED status, everything goes well!\n");

	//Task global variables init，
	memset(&gTaskCloudvelaContext, 0, sizeof(gTaskCloudvelaContext_t));
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] = 0;

	//启动周期性心跳定时器
	ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error start timer!\n");
		return FAILURE;
	}

	//For sw&db version report
	ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PRD_SWDB_VER_REP, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_CLOUDVELA_PRD_SWDB_VER_REP].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error start timer!\n");
		return FAILURE;
	}

	//State Transfer to FSM_STATE_CLOUDVELA_OFFLINE
	ret = FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error Set FSM State at fsm_cloudvela_init\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("CLOUDVELA: Internal error counter reach CRITICAL/DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_cloudvela_init(0, 0, NULL, 0);
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　FSM定时器处理部分
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_CLOUDVELA, TASK_ID_CLOUDVELA, &snd0, snd0.length) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudvela_hb_link_main_entry();
	}

	//for sw&db version report
	else if ((rcv.timeId == TIMER_ID_1S_CLOUDVELA_PRD_SWDB_VER_REP) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudvela_time_out_period_for_sw_db_report();
	}

	return ret;
}

/***************************************************************************************************************************
 *
 *  CLOUDVELA链路协议
 *
 *  - 未来至少需要支持DEFAULT/HOME两个链路
 *    其实在任何L3MOD发送给底层进行通信的链路DestUser中已经知晓是哪一个目的地址，所以链路层处理在pack消息时，完全可以自动生成linkid
 *    业务模块就关注于填写正确的目的地就是了，上层不应该涉足具体的链路ID细节
 *  - 链路层上行解码时，已经将srcUser填写的非常清楚，也一起带给了L3MOD，所以也没有必要给出LINKID信息
 *  - 有关限制链路层只能传递某些消息内容，完全没有必要，链路层只提供编解码，传任何内容，都有L3MOD自己负责
 *  - 在CLOUDVELA的上下文中以及系统工参信息中，已经配置了主备两个链路服务器信息，链路层启动的时候，就会不断的尝试并建立双链路
 *  - L2 PACK时需要给出是哪一条链路，从而让底层知晓该发往哪一条链路上
 *
 ***************************************************************************************************************************/
//进入离线，具备两种方式：被动心跳检测到离线状态，主动断掉链路从而连到更高优先级的链路上
//这两种情形下，都需要复位ConTable对应的状态，不然可能会导致物理接口模块处于不正常的接收状态
//检测后台连接的有效性，是否还依然保持连接状态
//心跳握手检测，搞不好就是一种新状态，这里得想办法做成一个立即返回的过程，不然状态机太复杂，上面的控制程序也不好处理
//这里的发送接受，只是有SAE-CLOUD的反馈就算成功，如果在消息解码中再遇到问题，可以重新设置本模块的状态，比如强行进入离线等等
//所以这里返回成功，以及相应的处理，只能是不完善的，因为发送和接收过程分离的，但99%情况下，这种机制是可以正常工作的
//在线状态下，zHcuCloudvelaTable.ethConTry的参数用于控制是否可能进入链路断掉-》重建的情形，该参数在尝试过后会+1,
//但HWINV的状态逆转报告会导致它清零，从而具备再一次尝试的资格。这里还要照顾一种清醒：如果所有链路都没有建立起来，则自然需要不断的+1,也没有大问题
//长周期定时器, 周期性心跳时钟处理机制

//HEART-BEAT只适用于业务服务器，不得使用于HOME服务器
OPSTAT func_cloudvela_hb_link_main_entry(void)
{
	//int ret = 0;

	//检查链路状态，离线，则再连接
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		if ((gTaskCloudvelaContext.defaultSvrSocketCon == TRUE) && (func_cloudvela_socket_conn_setup() == SUCCESS)){
			zHcuSysStaPm.statisCnt.cloudVelaConnCnt++;
			//State Transfer to FSM_STATE_CLOUDVELA_ONLINE
			if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");
			HCU_DEBUG_PRINT_NOR("CLOUDVELA: Connect state change, from OFFLINE to ONLINE!\n");
		}
		//如果是失败情况，并不返回错误，属于正常情况
		else{
			HCU_DEBUG_PRINT_NOR("CLOUDVELA: Try to setup connection with back-cloud, but not success!\n");
			zHcuSysStaPm.statisCnt.cloudVelaConnFailCnt++;
		}
	}

	//在线状态，则检查
	else if(FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		if (func_cloudvela_hb_link_active_send_signal() == FAILURE){
			zHcuSysStaPm.statisCnt.cloudVelaDiscCnt++;
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			//State Transfer to FSM_STATE_CLOUDVELA_OFFLINE
			if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");
			HCU_DEBUG_PRINT_NOR("CLOUDVELA: Connect state change, from ONLINE to OFFLINE!\n");
			//并不立即启动连接的建立，而是等待下一个周期带来，否则状态机过于复杂
		}//心跳握手失败
		//在线而且心跳握手正常
		else{
			//先看下是否还有合理的高优先级连接存在，以便断掉当前连接，并试图重连高优先级的物理接口
			//主动断掉链路，需要复位CurrentConnection指示以及Http-Curl全局指针

			//如果当前是3G4G
			if ((gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G) &&
					((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				//如果还没有试图重连过
				if ((gTaskCloudvelaContext.ethConTry == 0) || (gTaskCloudvelaContext.usbnetConTry == 0) || (gTaskCloudvelaContext.wifiConTry == 0)){
					//Disconnect current 3g4g connection!!!
					if (hcu_3g4g_phy_link_disconnect() == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error disconnect 3G4G link!\n");
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error set FSM status!\n");
					//设置当前工作物理链路为无效
					gTaskCloudvelaContext.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
			//如果当前是WIFI
			else if ((gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI) &&
				((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
						(zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				if ((gTaskCloudvelaContext.ethConTry == 0) || (gTaskCloudvelaContext.usbnetConTry == 0)){
					//Disconnect current wifi connection!!!
					if (hcu_wifi_phy_link_disconnect() == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error disconnect WIFI link!\n");
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error set FSM status!\n");
					//设置当前工作物理链路为无效
					gTaskCloudvelaContext.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
			//如果当前是USBNET
			else if ((gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET) &&
					(zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE)){
				if (gTaskCloudvelaContext.ethConTry == 0){
					//Disconnect current usbnet connection!!!
					if (hcu_usbnet_phy_link_disconnect() == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error disconnect USBNET link!\n");
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error set FSM status!\n");
					//设置当前工作物理链路为无效
					gTaskCloudvelaContext.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
		}//end of 在线而且心跳握手正常
	}//end of 长周期定时在线状态

	//既不在线，也不离线，强制转移到离线状态以便下次恢复，这种情况很难得，一般不会跑到这儿来，这种情况通常发生在初始化期间或者状态机胡乱的情况下
	else{
		if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");
	}

	return SUCCESS;
}

//主动发送心跳信号
//多链路处理过程，未来需要考虑完善
//链路ID和格式的判定还有些问题，未来再完善
OPSTAT func_cloudvela_hb_link_active_send_signal(void)
{
	//int ret = 0;

	//如果这里的链路已经被动断掉，必须将下面的数据清理掉，不然会出现下层物理接口模块还在接收的状态
	//zHcuCloudvelaTable.curCon = CLOUDVELA_CONTROL_PHY_CON_INVALID;

	//检查链路状态
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_ONLINE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send HEART_BEAT to cloud, get by ONLINE, but back off line so quick!\n");

	//发送数据给后台//初始化变量//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));

	//分格式类型组装
	if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML){
		//这里就相当于L3，HEART-BEAT只支持业务服务器，必然采用SOCKET机制
		strncpy(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(gTaskCloudvelaContext.L2Link.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(gTaskCloudvelaContext.L2Link.destUser));
		//zHcuSysEngPar.cloud.hcuName 代替 zHcuSysEngPar.hwBurnId.equLable，未来待完善
		strncpy(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.hcuName, strlen(zHcuSysEngPar.cloud.hcuName)<\
				sizeof(gTaskCloudvelaContext.L2Link.srcUser)?strlen(zHcuSysEngPar.cloud.hcuName):sizeof(gTaskCloudvelaContext.L2Link.srcUser));
		gTaskCloudvelaContext.L2Link.timeStamp = time(0);
		gTaskCloudvelaContext.L2Link.msgType = HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID;
		strcpy(gTaskCloudvelaContext.L2Link.funcFlag, "0");
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_heart_beat_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_heart_beat_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_heart_beat_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = HUITP_IEID_UNI_COM_REPORT_YES;
		//StrIe_HUITP_IEID_uni_heart_beat_ping_t
		pMsgProc.ping.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_heart_beat_ping);
		pMsgProc.ping.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_heart_beat_ping_t) - 4);
		//留下来用于随机验证：服务器需要原封不动的送回来
		UINT16 randval = rand()%65000;
		pMsgProc.ping.randval = HUITP_ENDIAN_EXG16(randval);
		gTaskCloudvelaContext.defaultHbRand = randval;
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_heart_beat_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		if (func_cloudvela_stdzhb_msg_heart_beat_pack(&pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		if (func_cloudvela_stdzhb_msg_heart_beat_pack(&pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//State no change
	return SUCCESS;
}

//主动发送心跳信号以后，收到对方的反馈
//链路ID和格式的判定还有些问题，未来再完善
OPSTAT func_cloudvela_hb_link_active_rcv_signal_check(UINT16 randval)
{
	//FSM状态检查
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_ONLINE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: FSM State error, taking care of potential risk!\n");

	//HW状态检查，心跳的意义
	if ((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)\
			&&(zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)){
		FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE);
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Hardware ETH/USBNET/WIFI/3G4G connection state error, taking care of potential risk!\n");
	}

	//检查随机数是否相等
	if (randval != gTaskCloudvelaContext.defaultHbRand) HCU_DEBUG_PRINT_INF("CLOUDVELA: Default Server potential risk of feedback value!\n");

	return SUCCESS;
}

//被动接收到对方的HEART-BEAT信号，反馈就是了
//链路ID和格式的判定还有些问题，未来再完善
OPSTAT func_cloudvela_hb_link_passive_rcv_signal_for_react(UINT16 randval)
{
	//int ret = 0;

	//发送数据给后台//初始化变量//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));

	//分格式类型组装
	if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML){
		//这里就相当于L3，HEART-BEAT只支持业务服务器，必然采用SOCKET机制
		strncpy(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(gTaskCloudvelaContext.L2Link.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(gTaskCloudvelaContext.L2Link.destUser));
		//zHcuSysEngPar.cloud.hcuName 代替 zHcuSysEngPar.hwBurnId.equLable，未来待完善
		strncpy(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.hcuName, strlen(zHcuSysEngPar.cloud.hcuName)<\
				sizeof(gTaskCloudvelaContext.L2Link.srcUser)?strlen(zHcuSysEngPar.cloud.hcuName):sizeof(gTaskCloudvelaContext.L2Link.srcUser));
		gTaskCloudvelaContext.L2Link.timeStamp = time(0);
		gTaskCloudvelaContext.L2Link.msgType = HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID;
		strcpy(gTaskCloudvelaContext.L2Link.funcFlag, "0");
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_heart_beat_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_heart_beat_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_heart_beat_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = HUITP_IEID_UNI_COM_REPORT_YES;
		//StrIe_HUITP_IEID_uni_heart_beat_pong_t
		pMsgProc.pong.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_heart_beat_pong);
		pMsgProc.pong.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_heart_beat_pong_t) - 4);
		//留下来用于随机验证：服务器需要原封不动的送回来
		UINT16 randval = rand()%65000;
		pMsgProc.pong.randval = HUITP_ENDIAN_EXG16(randval);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_heart_beat_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//State no change
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 *  核心API函数
 *
 ***************************************************************************************************************************/

//Send to backhawl cloud
OPSTAT func_cloudvela_send_data_to_cloud(CloudDataSendBuf_t *buf)
{
	//参数检查
	if ((buf->curLen <=0) || (buf->curLen >HCU_SYSMSG_COM_MSG_BODY_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error message length to send back for cloud!\n");

	//如果是HOME服务器目标，则调用CURL机制发送到HOME。两个服务器相同则认为是下一种。
	if (gTaskCloudvelaContext.linkId == HCU_SYSCFG_CLOUD_BH_LINK_HOME){
		//调用CURL的函数，但初始化必须先完成。先只考虑使用ETHERNET
		if (gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET){
			if (hcu_ethernet_curl_data_send(buf) == FAILURE){
				HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
			}
		}
		return SUCCESS;
	}

	//然后才是其它链路类型。当业务服务器/双同服务器模式下，均采用default链路。
	//根据系统配置，决定使用那一种后台网络
	if (gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET){
		if (hcu_ethernet_socket_data_send(buf) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
		}
	}
	else if (gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET){
		if (hcu_usbnet_data_send(buf) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
		}
	}
	else if (gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI){
		if (hcu_wifi_data_send(buf) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
		}
	}
	else if (gTaskCloudvelaContext.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G){
		if (hcu_3g4g_data_send(buf) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
		}
	}
	else {
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error status of physical layer link for data send!\n");
	}

	return SUCCESS;
}

/*
 * 后台连接采用如下策略：
 * - HWINV不断更新硬件状态
 * - 本任务的长定时器不断扫描链路状态，并进行心跳检测，目的是维护后台连接的长期性和稳定性
 * - 如果心跳检测没变化，则不动
 * - 如果心态检测发现链路有变化，则根据后台硬件状态，按照ETHERNET > USBNET > WIFI > 3G4G的优先级，进行链路连接建立的发起
 * - 如果全部失败，则继续保持OFFLINE状态
 * - 如果成功，则就进入ONLINE状态
 *
 */
//调用此函数的条件是，已知状态就是OFFLINE了
OPSTAT func_cloudvela_socket_conn_setup(void)
{
	int ret = FAILURE;
	//3G, ETHERNET, WIFI connection?
	//周期性的检测，随便连上哪一种链路，则自然的搞定
	//后面的Hardware Inventory任务会制作一张实时全局硬件状态表，不需要通过消息发送给不同任务模块，这样谁需要访问，一查便知
	//这种方式下，消息减少，还能方便的实现PnP功能

	//检查任务模块状态
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_OFFLINE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error task status, can not setup new connection with cloud!\n");

	//调用后台模块提供的函数，进行连接建立
	//第一优先级：连接ETHERNET
	//当前的情况下，ETHERNET物理链路的确啥都不干，只是回复成功，未来可以挂载更多的物理链路处理过程在其中
	if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_ethernet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.ethConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.ethConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET;
		return SUCCESS;
	}

	//第二优先级：连接USBNET
	if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_usbnet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.usbnetConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.usbnetConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET;
		return SUCCESS;
	}

	//第三优先级：连接WIFI
	if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_wifi_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.wifiConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.wifiConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI;
		return SUCCESS;
	}

	//第四优先级：连接3G4G
	if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_3g4g_phy_link_setup();

	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.g3g4ConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.g3g4ConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G;
		return SUCCESS;
	}

	HCU_DEBUG_PRINT_NOR("CLOUDVELA: No CLOUD-BH physical link hardware available or not setup successful!\n");
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
	return FAILURE;
}

/*
 * 后台连接采用如下策略：
 * - HWINV不断更新硬件状态
 * - 本任务的长定时器不断扫描链路状态，并进行心跳检测，目的是维护后台连接的长期性和稳定性
 * - 如果心跳检测没变化，则不动
 * - 如果心态检测发现链路有变化，则根据后台硬件状态，按照ETHERNET > USBNET > WIFI > 3G4G的优先级，进行链路连接建立的发起
 * - 如果全部失败，则继续保持OFFLINE状态
 * - 如果成功，则就进入ONLINE状态
 *
 */
//为了多线程的安全，CURL参数的初始化只能做一次，做一遍，不能都做，所以采用全局变量让其它线程共享，而不能大家都分别去做初始化
OPSTAT func_cloudvela_http_curl_link_init(void)
{
	//初始化
	curl_global_init(CURL_GLOBAL_ALL);

	return SUCCESS;
}

//当前的主要物理链路是Ethernet，链路建立没干啥，真实的HTTP-CURL操作都在接收与发送函数中，未来需要进一步优化
OPSTAT func_cloudvela_http_curl_conn_setup(void)
{
	int ret = FAILURE;
	//3G, ETHERNET, WIFI connection?
	//周期性的检测，随便连上哪一种链路，则自然的搞定
	//后面的Hardware Inventory任务会制作一张实时全局硬件状态表，不需要通过消息发送给不同任务模块，这样谁需要访问，一查便知
	//这种方式下，消息减少，还能方便的实现PnP功能

	//检查任务模块状态
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_OFFLINE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error task status, can not setup new connection with cloud!\n");

	//第一次初始化，或者重新初始化全局HTTP-CURL链路
	if (func_cloudvela_http_curl_link_init() == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Init Curl Http link failure!\n");

	//调用后台模块提供的函数，进行连接建立
	//第一优先级：连接ETHERNET
	//当前的情况下，ETHERNET物理链路的确啥都不干，只是回复成功，未来可以挂载更多的物理链路处理过程在其中
	if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_ethernet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.ethConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.ethConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET;
		return SUCCESS;
	}

	//第二优先级：连接USBNET
	if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_usbnet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.usbnetConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.usbnetConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET;
		return SUCCESS;
	}

	//第三优先级：连接WIFI
	if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_wifi_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.wifiConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.wifiConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI;
		return SUCCESS;
	}

	//第四优先级：连接3G4G
	if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_3g4g_phy_link_setup();

	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		gTaskCloudvelaContext.g3g4ConTry++;
	}
	if (ret == SUCCESS){
		gTaskCloudvelaContext.g3g4ConTry = 0;
		gTaskCloudvelaContext.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G;
		return SUCCESS;
	}

	HCU_DEBUG_PRINT_NOR("CLOUDVELA: No CLOUD-BH physical link hardware available or not setup successful!\n");
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
	return FAILURE;
}

//This callback function gets called by libcurl as soon as there is  data received that needs to be saved
size_t func_cloudvela_http_curl_write_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	msg_struct_ethernet_cloudvela_data_rx_t *mem = (msg_struct_ethernet_cloudvela_data_rx_t *)userp;

	//For test
	memset(mem, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));
	if(realsize > HCU_SYSMSG_COM_MSG_BODY_LEN_MAX)
	{
		HcuErrorPrint("CLOUDVELA: No enough memory!\n");
		return 0;
	}
	memcpy(mem->buf, buffer, realsize); //mem->length
	mem->length = realsize;

	return realsize;
}

//HOME服务器／CURL只支持HUITPXML协议标准，未来可能扩展到HUITP_JASON
OPSTAT fsm_cloudvela_ethernet_curl_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//参数检查
	if ((param_len <=0) || (param_len >HCU_SYSDIM_MSG_BODY_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error message length received from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
	if (param_ptr == NULL)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive NULL pointer data from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);

	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");

	//接收消息解码
	msg_struct_com_cloudvela_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_cloudvela_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_cloudvela_data_rx_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
	memcpy(rcv.buf, param_ptr, param_len);
	rcv.length = param_len;
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Receive data len=%d, data buffer = [%s], from [%s] module\n\n", rcv.length,  rcv.buf, zHcuVmCtrTab.task[src_id].taskName);

	if (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML){
		//不期望任何目标消息
		if (func_cloudvela_huitpxml_msg_unpack(&rcv, -1) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		}
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");

	return SUCCESS;
}

OPSTAT fsm_cloudvela_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");

	return SUCCESS;
}

OPSTAT fsm_cloudvela_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");

	return SUCCESS;
}

//带buffer不定长的结构体，其长度域，只是为了发送消息的安全，都是结构体的最大长度，所以不可以使用它来界定buffer的实际长度
//发现在接收数据的处理中，使用的是完全相同的msg_struct_com_cloudvela_data_rx_t，故而采用完全相同的接收处理函数
//如果未来不同的物理接口出现变化，必须需要不同的处理，再行改变
//CURL方式暂时没启动，使用的是SOCKET方式
OPSTAT fsm_cloudvela_socket_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//参数检查
	if ((param_len <=0) || (param_len >HCU_SYSDIM_MSG_BODY_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error message length received from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
	if (param_ptr == NULL) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive NULL pointer data from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);

	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error Set FSM State!\n");

	//接收消息解码
	msg_struct_com_cloudvela_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_cloudvela_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_cloudvela_data_rx_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);

	memcpy(rcv.buf, param_ptr, param_len);
	rcv.length = param_len;
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Receive data len=%d, data buffer = [%s], from [%s] module\n\n", rcv.length,  rcv.buf, zHcuVmCtrTab.task[src_id].taskName);

	if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML){
		//不期望任何目标消息
		if (func_cloudvela_huitpxml_msg_unpack(&rcv, -1) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		}
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		if (func_cloudvela_stdxml_msg_unpack(&rcv) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		}
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		if (func_cloudvela_stdzhb_msg_unpack(&rcv) == FAILURE){
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		}
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
	}

	return SUCCESS;
}


OPSTAT fsm_cloudvela_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Receive message and copy to local variable
	msg_struct_hwinv_cloudvela_phy_status_chg_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.ethStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.ethStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) gTaskCloudvelaContext.ethConTry = 0;
	else if ((rcv.usbnetStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.usbnetStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) gTaskCloudvelaContext.usbnetConTry = 0;
	else if ((rcv.wifiStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.wifiStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) gTaskCloudvelaContext.wifiConTry = 0;
	else if ((rcv.g3g4StatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.g3g4StatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) gTaskCloudvelaContext.g3g4ConTry = 0;
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Message received with error content!\n");
	}
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 *  AMARM / PM性能管理部分
 *
 ***************************************************************************************************************************/
OPSTAT func_cloudvela_time_out_period_for_sw_db_report(void)
{
	UINT32 optId=0, cmdId=0, backType=0;

	//命令字
	cmdId = L3CI_hcu_inventory;
	backType = L3CI_cmdid_back_type_period;
    optId = L3PO_hcuinventory_report;
    zHcuSysEngPar.swDbInvInfo.hw_type = zHcuSysEngPar.hwBurnId.hwType;
    zHcuSysEngPar.swDbInvInfo.hw_version = zHcuSysEngPar.hwBurnId.hwPemId;
    zHcuSysEngPar.swDbInvInfo.sw_release = zHcuSysEngPar.hwBurnId.swRelId;
    zHcuSysEngPar.swDbInvInfo.sw_delivery = zHcuSysEngPar.hwBurnId.swVerId;

	// send resp msg to cloud
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));
		//打包数据
		if (FAILURE == func_cloudvela_stdzhb_msg_hcu_inventory_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, &zHcuSysEngPar.swDbInvInfo, &buf))
		{
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
		}

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Online state, send HCU Inventory Resp to cloud failure!\n");
		HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send HCU Inventory Resp to cloud success!\n");
	}

	return SUCCESS;
}

OPSTAT func_cloudvela_sw_download(char *filename)
{
	FTP_OPT ftp_opt;

	char usrtmp[3] = ":";

	memset( (void *)&ftp_opt, 0, sizeof(FTP_OPT));

	//ftp_opt.user_key = zHcuSysEngPar.cloud.cloudFtpUser;
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpUser);
	strcat(ftp_opt.user_key, usrtmp);
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpPwd);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.user_key: %s \n", ftp_opt.user_key);

	//char filetmp[64] = "swdownload.txt";
	//ftp_opt.url = zHcuSysEngPar.cloud.cloudFtpAdd;
	strcat(ftp_opt.url, zHcuSysEngPar.cloud.cloudFtpAdd);
	strcat(ftp_opt.url, filename);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.url: %s \n", ftp_opt.url);

	//ftp_opt.file = zHcuSysEngPar.swDownload.hcuSwDownloadDir;
	strcat(ftp_opt.file, zHcuSysEngPar.swm.hcuSwDownloadDir);
	strcat(ftp_opt.file, filename);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.file: %s \n", ftp_opt.file);

	if(FTP_DOWNLOAD_SUCCESS == ftp_download(ftp_opt)){
		HCU_DEBUG_PRINT_NOR("CLOUDVELA: HCU SW Download success.\n");
	    // send resp msg to cloud: 01 successful
	    //system("reboot");
		return SUCCESS;
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: HCU SW Download failed.\n");
	}

	return SUCCESS;
}

//标准消息处理方式
OPSTAT fsm_cloudvela_syspm_alarm_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_spspm_cloudvela_alarm_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_spspm_cloudvela_alarm_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spspm_cloudvela_alarm_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_alarm_info_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_alarm_info_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_alarm_info_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_alarm_info_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_alarm_info_element_t
		pMsgProc.respValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_alarm_info_element);
		pMsgProc.respValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_alarm_info_element_t) - 4);
		pMsgProc.respValue.alarmType = HUITP_ENDIAN_EXG16(rcv.alarmType);
		pMsgProc.respValue.alarmClearFlag = rcv.alarmClearFlag;
		pMsgProc.respValue.alarmClearFlag = rcv.alarmClearFlag;
		pMsgProc.respValue.equID = HUITP_ENDIAN_EXG32(rcv.equID);
		pMsgProc.respValue.causeId = HUITP_ENDIAN_EXG32(rcv.causeId);
		pMsgProc.respValue.alarmContent = HUITP_ENDIAN_EXG32(rcv.alarmContent);
		#if (HUITP_IEID_UNI_ALARM_INFO_ELEMENT_DESC_LEN_MAX < HCU_SYSMSG_SPSPM_ALARM_DESC_LEN_MAX)
			#error HUITP and COMMSG parameter set error!
		#endif
		strncpy(pMsgProc.respValue.alarmDesc, rcv.alarmDesc, strlen(rcv.alarmDesc));
		pMsgProc.respValue.timeStamp = HUITP_ENDIAN_EXG32(rcv.timeStamp);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_alarm_info_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_syspm_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_spspm_cloudvela_alarm_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_spspm_cloudvela_alarm_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spspm_cloudvela_alarm_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_alarm_info_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_alarm_info_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_alarm_info_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_alarm_info_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_alarm_info_element_t
		pMsgProc.reportValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_alarm_info_element);
		pMsgProc.reportValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_alarm_info_element_t) - 4);
		pMsgProc.reportValue.alarmType = HUITP_ENDIAN_EXG16(rcv.alarmType);
		pMsgProc.reportValue.alarmClearFlag = rcv.alarmClearFlag;
		pMsgProc.reportValue.alarmClearFlag = rcv.alarmClearFlag;
		pMsgProc.reportValue.equID = HUITP_ENDIAN_EXG32(rcv.equID);
		pMsgProc.reportValue.causeId = HUITP_ENDIAN_EXG32(rcv.causeId);
		pMsgProc.reportValue.alarmContent = HUITP_ENDIAN_EXG32(rcv.alarmContent);
		#if (HUITP_IEID_UNI_ALARM_INFO_ELEMENT_DESC_LEN_MAX < HCU_SYSMSG_SPSPM_ALARM_DESC_LEN_MAX)
			#error HUITP and COMMSG parameter set error!
		#endif
		strncpy(pMsgProc.reportValue.alarmDesc, rcv.alarmDesc, strlen(rcv.alarmDesc));
		pMsgProc.reportValue.timeStamp = HUITP_ENDIAN_EXG32(rcv.timeStamp);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_alarm_info_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_syspm_perfm_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_spspm_cloudvela_perfm_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_spspm_cloudvela_perfm_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spspm_cloudvela_perfm_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_performance_info_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_performance_info_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_performance_info_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_performance_info_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_performance_info_element_t
		pMsgProc.respValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_performance_info_element);
		pMsgProc.respValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_performance_info_element_t) - 4);
		pMsgProc.respValue.restartCnt = HUITP_ENDIAN_EXG32(rcv.restartCnt);
		pMsgProc.respValue.networkConnCnt = HUITP_ENDIAN_EXG32(rcv.networkConnCnt);
		pMsgProc.respValue.networkConnFailCnt = HUITP_ENDIAN_EXG32(rcv.networkConnFailCnt);
		pMsgProc.respValue.networkDiscCnt = HUITP_ENDIAN_EXG32(rcv.networkDiscCnt);
		pMsgProc.respValue.socketDiscCnt = HUITP_ENDIAN_EXG32(rcv.socketDiscCnt);
		pMsgProc.respValue.cpuOccupy = HUITP_ENDIAN_EXG32(rcv.cpuOccupy);
		pMsgProc.respValue.memOccupy = HUITP_ENDIAN_EXG32(rcv.memOccupy);
		pMsgProc.respValue.diskOccupy = HUITP_ENDIAN_EXG32(rcv.diskOccupy);
		pMsgProc.respValue.timeStamp = HUITP_ENDIAN_EXG32(rcv.timeStamp);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_performance_info_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_syspm_perfm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_spspm_cloudvela_perfm_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_spspm_cloudvela_perfm_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_spspm_cloudvela_perfm_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_performance_info_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_performance_info_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_performance_info_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_performance_info_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_performance_info_element_t
		pMsgProc.reportValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_performance_info_element);
		pMsgProc.reportValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_performance_info_element_t) - 4);
		pMsgProc.reportValue.restartCnt = HUITP_ENDIAN_EXG32(rcv.restartCnt);
		pMsgProc.reportValue.networkConnCnt = HUITP_ENDIAN_EXG32(rcv.networkConnCnt);
		pMsgProc.reportValue.networkConnFailCnt = HUITP_ENDIAN_EXG32(rcv.networkConnFailCnt);
		pMsgProc.reportValue.networkDiscCnt = HUITP_ENDIAN_EXG32(rcv.networkDiscCnt);
		pMsgProc.reportValue.socketDiscCnt = HUITP_ENDIAN_EXG32(rcv.socketDiscCnt);
		pMsgProc.reportValue.cpuOccupy = HUITP_ENDIAN_EXG32(rcv.cpuOccupy);
		pMsgProc.reportValue.memOccupy = HUITP_ENDIAN_EXG32(rcv.memOccupy);
		pMsgProc.reportValue.diskOccupy = HUITP_ENDIAN_EXG32(rcv.diskOccupy);
		pMsgProc.reportValue.timeStamp = HUITP_ENDIAN_EXG32(rcv.timeStamp);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_performance_info_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_sysswm_inventory_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_inventory_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_inventory_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_inventory_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_inventory_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_inventory_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_inventory_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_inventory_element_t
		pMsgProc.respValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_inventory_element);
		pMsgProc.respValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_inventory_element_t) - 4);
		pMsgProc.respValue.hwType = HUITP_ENDIAN_EXG16(rcv.hwType);
		pMsgProc.respValue.hwId = HUITP_ENDIAN_EXG16(rcv.hwId);
		pMsgProc.respValue.swRel = HUITP_ENDIAN_EXG16(rcv.swRel);
		pMsgProc.respValue.swVer = HUITP_ENDIAN_EXG16(rcv.swVer);
		pMsgProc.respValue.upgradeFlag = rcv.upgradeFlag;
		#if (HUITP_IEID_UNI_INVENTORY_ELEMENT_DESC_LEN_MAX < HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX)
			#error HUITP and COMMSG parameter set error!
		#endif
		strncpy(pMsgProc.respValue.desc, rcv.desc, strlen(rcv.desc)<sizeof(pMsgProc.respValue.desc)?strlen(rcv.desc):sizeof(pMsgProc.respValue.desc));
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_inventory_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_sysswm_inventory_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_inventory_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_inventory_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_inventory_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_inventory_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_inventory_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_inventory_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_inventory_element_t
		pMsgProc.reportValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_inventory_element);
		pMsgProc.reportValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_inventory_element_t) - 4);
		pMsgProc.reportValue.hwType = HUITP_ENDIAN_EXG16(rcv.hwType);
		pMsgProc.reportValue.hwId = HUITP_ENDIAN_EXG16(rcv.hwId);
		pMsgProc.reportValue.swRel = HUITP_ENDIAN_EXG16(rcv.swRel);
		pMsgProc.reportValue.swVer = HUITP_ENDIAN_EXG16(rcv.swVer);
		pMsgProc.reportValue.upgradeFlag = rcv.upgradeFlag;
		#if (HUITP_IEID_UNI_INVENTORY_ELEMENT_DESC_LEN_MAX < HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX)
			#error HUITP and COMMSG parameter set error!
		#endif
		strncpy(pMsgProc.reportValue.desc, rcv.desc, strlen(rcv.desc)<sizeof(pMsgProc.reportValue.desc)?strlen(rcv.desc):sizeof(pMsgProc.reportValue.desc));
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_inventory_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_sysswm_sw_package_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_sw_package_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_sw_package_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_sw_package_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_sw_package_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_sw_package_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_sw_package_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_sw_package_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_com_segment_t
		pMsgProc.segValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_segment);
		pMsgProc.segValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_segment_t) - 4);
		pMsgProc.segValue.segIndex = HUITP_ENDIAN_EXG16(rcv.segIndex);
		pMsgProc.segValue.segTotal = HUITP_ENDIAN_EXG16(rcv.segTotal);
		pMsgProc.segValue.segLen = HUITP_ENDIAN_EXG16(rcv.segLen);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_sw_package_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_sysswm_sw_package_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_sw_package_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_sw_package_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_sw_package_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_sw_package_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_sw_package_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_sw_package_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_sw_package_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_com_segment_t
		pMsgProc.segValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_segment);
		pMsgProc.segValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_segment_t) - 4);
		pMsgProc.segValue.segIndex = HUITP_ENDIAN_EXG16(rcv.segIndex);
		pMsgProc.segValue.segTotal = HUITP_ENDIAN_EXG16(rcv.segTotal);
		pMsgProc.segValue.segLen = HUITP_ENDIAN_EXG16(rcv.segLen);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_sw_package_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_sysswm_time_sync_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_time_sync_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_time_sync_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_time_sync_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_time_sync_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_time_sync_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_time_sync_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_time_sync_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_time_sync_value_t
		pMsgProc.timeSync.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_time_sync_value);
		pMsgProc.timeSync.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_time_sync_value_t) - 4);
		pMsgProc.timeSync.timeRegion = rcv.timeRegion;
		pMsgProc.timeSync.timeInUnix = HUITP_ENDIAN_EXG32(rcv.timeInUnix);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_time_sync_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}


OPSTAT fsm_cloudvela_sysswm_time_sync_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_sysswm_cloudvela_time_sync_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_cloudvela_time_sync_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_cloudvela_time_sync_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_time_sync_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_time_sync_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_time_sync_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_time_sync_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_time_sync_value_t
		pMsgProc.timeSync.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_time_sync_value);
		pMsgProc.timeSync.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_time_sync_value_t) - 4);
		pMsgProc.timeSync.timeRegion = rcv.timeRegion;
		pMsgProc.timeSync.timeInUnix = HUITP_ENDIAN_EXG32(rcv.timeInUnix);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_time_sync_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　EMC业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_emc_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_emc_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_emc_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive EMC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.emc.equipid <= 0) || (rcv.usercmdid != L3CI_emc) || (rcv.emc.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_emc_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType,
				rcv.emc.equipid, rcv.emc.dataFormat, rcv.emc.emcValue, rcv.emc.gps.gpsx, rcv.emc.gps.gpsy, rcv.emc.gps.gpsz, rcv.emc.gps.ns, rcv.emc.gps.ew, rcv.emc.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to back-cloud!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get EMC by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period EMC to cloud success!\n");
	//State no change
	return SUCCESS;
}


OPSTAT fsm_cloudvela_emc_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 * 　PM25业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_pm25_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive PM25_RESP message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.pm25.equipid <= 0) || (rcv.usercmdid != L3CI_pm25) || (rcv.pm25.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_pm25_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType,
				rcv.pm25.equipid, rcv.pm25.dataFormat, rcv.pm25.pm1d0Value, rcv.pm25.pm2d5Value, rcv.pm25.pm10Value, rcv.pm25.gps.gpsx, rcv.pm25.gps.gpsy,
				rcv.pm25.gps.gpsz, rcv.pm25.gps.ns, rcv.pm25.gps.ew, rcv.pm25.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period PM25 to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_pm25_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Adding by Shanchun for control cmd
	//int ret=0;
	msg_struct_pm25_cloudvela_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_cloudvela_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_cloudvela_control_fb_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive PM25_CMD_RESP message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.opt.equId <= 0) || (rcv.cmdId != L3CI_pm25))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//func_cloudvela_stdzhb_msg_pm25_cmd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 powerOnOff, UINT32 interSample, UINT32 meausTimes, UINT32 newEquId, UINT32 workCycle,CloudDataSendBuf_t *buf)
		//打包数据
		if (func_cloudvela_stdzhb_msg_pm25_cmd_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, rcv.cmdId, rcv.optId, rcv.backType,
				rcv.opt.equId, rcv.opt.powerOnOff, rcv.opt.interSample, rcv.opt.meausTimes, rcv.opt.newEquId, rcv.opt.workCycle, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send PM25 Sensor Control CMD ACK to cloud success!\n");
	//State no change
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　WINDDIR业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_winddir_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_winddir_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_winddir_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive WIND_DIR_RESP message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.winddir.equipid <= 0) || (rcv.usercmdid != L3CI_winddir) || (rcv.winddir.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_winddir_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.winddir.equipid,
				rcv.winddir.dataFormat, rcv.winddir.winddirValue, rcv.winddir.gps.gpsx, rcv.winddir.gps.gpsy, rcv.winddir.gps.gpsz, rcv.winddir.gps.ns, rcv.winddir.gps.ew, rcv.winddir.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get WINDDIR by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period WINDDIR to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_winddir_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 * 　WINDSPD消息处理函数
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_windspd_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_windspd_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_windspd_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive WINDSPD message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.windspd.equipid <= 0) || (rcv.usercmdid != L3CI_windspd) || (rcv.windspd.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_windspd_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.windspd.equipid,
				rcv.windspd.dataFormat, rcv.windspd.windspdValue, rcv.windspd.gps.gpsx, rcv.windspd.gps.gpsy, rcv.windspd.gps.gpsz, rcv.windspd.gps.ns, rcv.windspd.gps.ew, rcv.windspd.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get WINDSPD by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period WINDSPD to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_windspd_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 * 　TEMP业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_temp_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_temp_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_temp_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive TEMPERATURE message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.temp.equipid <= 0) || (rcv.usercmdid != L3CI_temp) || (rcv.temp.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_temp_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.temp.equipid,
				rcv.temp.dataFormat, rcv.temp.tempValue, rcv.temp.gps.gpsx, rcv.temp.gps.gpsy, rcv.temp.gps.gpsz, rcv.temp.gps.ns, rcv.temp.gps.ew, rcv.temp.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get TEMPERATURE by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period TEMPERATURE to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_temp_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}



/***************************************************************************************************************************
 *
 * 　HUMID业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_humid_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_humid_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_humid_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive HUMIDITY message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.humid.equipid <= 0) || (rcv.usercmdid != L3CI_humid) || (rcv.humid.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_humid_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.humid.equipid,
				rcv.humid.dataFormat, rcv.humid.humidValue, rcv.humid.gps.gpsx, rcv.humid.gps.gpsy, rcv.humid.gps.gpsz, rcv.humid.gps.ns, rcv.humid.gps.ew, rcv.humid.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get HUMIDITY by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period HUMIDITY to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_humid_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 * 　NOISE业务处理
 *
 ***************************************************************************************************************************/
OPSTAT fsm_cloudvela_noise_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_noise_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_noise_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive NOISE message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.noise.equipid <= 0) || (rcv.usercmdid != L3CI_noise) || (rcv.noise.timeStamp <=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive invalid data!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_noise_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.noise.equipid,
				rcv.noise.dataFormat, rcv.noise.noiseValue, rcv.noise.gps.gpsx, rcv.noise.gps.gpsy, rcv.noise.gps.gpsz, rcv.noise.gps.ns, rcv.noise.gps.ew, rcv.noise.timeStamp, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get NOISE by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period NOISE to cloud success!\n");
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_noise_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}



/***************************************************************************************************************************
 *
 * 　HSMMP业务处理
 *
 ***************************************************************************************************************************/
//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_data_link_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_hsmmp_cloudvela_data_link_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive HSMMP message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//检查参数
	if ((rcv.link.linkName == NULL) || ((strlen(rcv.link.linkName) > HCU_SYSDIM_FILE_NAME_LEN_MAX)))
		HCU_ERROR_PRINT_CLOUDVELA("HSMMP: Error parameter message received!\n");

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_stdzhb_msg_hsmmp_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.link.equipid,
				rcv.link.gps.gpsx, rcv.link.gps.gpsy, rcv.link.gps.gpsz, rcv.link.gps.ns, rcv.link.gps.ew, rcv.link.timeStampStart, rcv.link.linkName, &buf) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");

		//Send out
		if (func_cloudvela_send_data_to_cloud(&buf) == FAILURE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error send data to cloud, get HSMMP by ONLINE, but back off line so quick!\n");
	}

	//结束
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Online state, send instance/period HSMMP to cloud success!\n");
	//State no change
	return SUCCESS;
}


//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 * 　BFSC消息处理函数
 *
 ***************************************************************************************************************************/
//BFSC项目的处理过程
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
OPSTAT fsm_cloudvela_l3bfsc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_data_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_bfsc_comb_scale_data_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_bfsc_comb_scale_data_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_scale_weight_value_t
		pMsgProc.respValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_value);
		pMsgProc.respValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_value_t) - 4);
		pMsgProc.respValue.type = rcv.type;
		pMsgProc.respValue.dataFormat = rcv.dataFormat;
		pMsgProc.respValue.snrCfgNbrMax = rcv.snrCfgNbrMax;
		pMsgProc.respValue.snrUsedBitmap = HUITP_ENDIAN_EXG32(rcv.snrUsedBitmap);
		#if (HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX > HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX)
			#error Error message structure or system parameter set!
		#endif
		memcpy(pMsgProc.respValue.snrValue, rcv.snrValue, HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
		//不能采取memcpy的方式，因为既有可能两边定义的结构不一致，更有可能pack()导致的sizeof()不一样，甚至有大小端的问题
		pMsgProc.respValue.sta.combTimes = HUITP_ENDIAN_EXG32(rcv.sta.combTimes);
		pMsgProc.respValue.sta.tttTimes = HUITP_ENDIAN_EXG32(rcv.sta.tttTimes);
		pMsgProc.respValue.sta.tgvTimes = HUITP_ENDIAN_EXG32(rcv.sta.tgvTimes);
		pMsgProc.respValue.sta.combMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.combMatNbr);
		pMsgProc.respValue.sta.tttMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tttMatNbr);
		pMsgProc.respValue.sta.tgvMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tgvMatNbr);
		pMsgProc.respValue.sta.combAvgSpeed = HUITP_ENDIAN_EXG32(rcv.sta.combAvgSpeed);
		pMsgProc.respValue.sta.totalWeight = HUITP_ENDIAN_EXG32(rcv.sta.totalWeight);
		pMsgProc.respValue.sta.totalMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.totalMatNbr);
		pMsgProc.respValue.sta.totalWorkMin = HUITP_ENDIAN_EXG32(rcv.sta.totalWorkMin);
		pMsgProc.respValue.sta.timeInUnix = HUITP_ENDIAN_EXG32(rcv.sta.timeInUnix);
		pMsgProc.respValue.sta.errNbr = HUITP_ENDIAN_EXG32(rcv.sta.errNbr);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_bfsc_comb_scale_data_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_data_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_bfsc_comb_scale_data_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_bfsc_comb_scale_data_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_scale_weight_value_t
		pMsgProc.reportValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_value);
		pMsgProc.reportValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_value_t) - 4);
		pMsgProc.reportValue.type = rcv.type;
		pMsgProc.reportValue.dataFormat = rcv.dataFormat;
		pMsgProc.reportValue.snrCfgNbrMax = rcv.snrCfgNbrMax;
		pMsgProc.reportValue.snrUsedBitmap = HUITP_ENDIAN_EXG32(rcv.snrUsedBitmap);
		#if (HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX > HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX)
			#error Error message structure or system parameter set!
		#endif
		memcpy(pMsgProc.reportValue.snrValue, rcv.snrValue, HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
		//不能采取memcpy的方式，因为既有可能两边定义的结构不一致，更有可能pack()导致的sizeof()不一样，甚至有大小端的问题
		pMsgProc.reportValue.sta.combTimes = HUITP_ENDIAN_EXG32(rcv.sta.combTimes);
		pMsgProc.reportValue.sta.tttTimes = HUITP_ENDIAN_EXG32(rcv.sta.tttTimes);
		pMsgProc.reportValue.sta.tgvTimes = HUITP_ENDIAN_EXG32(rcv.sta.tgvTimes);
		pMsgProc.reportValue.sta.combMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.combMatNbr);
		pMsgProc.reportValue.sta.tttMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tttMatNbr);
		pMsgProc.reportValue.sta.tgvMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tgvMatNbr);
		pMsgProc.reportValue.sta.combAvgSpeed = HUITP_ENDIAN_EXG32(rcv.sta.combAvgSpeed);
		pMsgProc.reportValue.sta.totalWeight = HUITP_ENDIAN_EXG32(rcv.sta.totalWeight);
		pMsgProc.reportValue.sta.totalMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.totalMatNbr);
		pMsgProc.reportValue.sta.totalWorkMin = HUITP_ENDIAN_EXG32(rcv.sta.totalWorkMin);
		pMsgProc.reportValue.sta.timeInUnix = HUITP_ENDIAN_EXG32(rcv.sta.timeInUnix);
		pMsgProc.reportValue.sta.errNbr = HUITP_ENDIAN_EXG32(rcv.sta.errNbr);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_bfsc_comb_scale_data_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_event_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_event_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_event_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_event_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_bfsc_comb_scale_event_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_bfsc_comb_scale_event_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_scale_weight_event_t
		pMsgProc.reportEvent.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_event);
		pMsgProc.reportEvent.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_event_t) - 4);
		pMsgProc.reportEvent.eventType = rcv.eventType;
		pMsgProc.reportEvent.sensorId = rcv.sensorId;
		pMsgProc.reportEvent.eventValue = HUITP_ENDIAN_EXG16(rcv.eventValue);
		#if (HCU_SYSMSG_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN > HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN)
			#error msg_struct_l3bfsc_cloudvela_event_report_t parameter set error!
		#endif
		memcpy(pMsgProc.reportEvent.userName, rcv.userName, HCU_SYSMSG_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN);
		pMsgProc.reportEvent.cause = HUITP_ENDIAN_EXG16(rcv.cause);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_bfsc_comb_scale_event_report, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_resp_t
		pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
		pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
		pMsgProc.baseResp.comResp = rcv.baseResp;
		//StrIe_HUITP_IEID_uni_scale_weight_cmd_t
		pMsgProc.cmdResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_cmd);
		pMsgProc.cmdResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_cmd_t) - 4);
		pMsgProc.cmdResp.scaleWeightCmd = rcv.scaleWeightCmd;
		//StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t
		pMsgProc.cfgResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_cfg_par);
		pMsgProc.cfgResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t) - 4);
		pMsgProc.cfgResp.workMode = rcv.workMode;
		pMsgProc.cfgResp.scaleWeightNbr = rcv.scaleWeightNbr;
		pMsgProc.cfgResp.staStartTime = HUITP_ENDIAN_EXG32(rcv.staStartTime);
		pMsgProc.cfgResp.staReportDuration = HUITP_ENDIAN_EXG32(rcv.staReportDuration);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_statistic_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_statistic_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_statistic_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_statistic_report_t)))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Receive L3BFSC message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//分格式类型组装
	if ((zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML) || (zHcuSysEngPar.cloud.svrBhItfFrameStdHome == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML)){
		memcpy(&(gTaskCloudvelaContext.L2Link), &(rcv.comHead), sizeof(msgie_struct_bh_com_head_t));
		//准备组装发送消息
		StrMsg_HUITP_MSGID_uni_bfsc_statistic_report_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_bfsc_statistic_report_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgId.cmdId = (HUITP_MSGID_uni_bfsc_statistic_report>>8)&0xFF;
		pMsgProc.msgId.optId = HUITP_MSGID_uni_bfsc_statistic_report&0xFF;
		pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		//StrIe_HUITP_IEID_uni_com_report_t
		pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
		pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
		pMsgProc.baseReport.comReport = rcv.baseReport;
		//StrIe_HUITP_IEID_uni_scale_weight_statistic_t
		pMsgProc.staRep.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_scale_weight_statistic);
		pMsgProc.staRep.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_scale_weight_statistic_t) - 4);
		pMsgProc.staRep.staRepType = rcv.staRepType;
		pMsgProc.staRep.dataFormat = rcv.dataFormat;
		//不能采取memcpy的方式，因为既有可能两边定义的结构不一致，更有可能pack()导致的sizeof()不一样，甚至有大小端的问题
		pMsgProc.staRep.sta.combTimes = HUITP_ENDIAN_EXG32(rcv.sta.combTimes);
		pMsgProc.staRep.sta.tttTimes = HUITP_ENDIAN_EXG32(rcv.sta.tttTimes);
		pMsgProc.staRep.sta.tgvTimes = HUITP_ENDIAN_EXG32(rcv.sta.tgvTimes);
		pMsgProc.staRep.sta.combMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.combMatNbr);
		pMsgProc.staRep.sta.tttMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tttMatNbr);
		pMsgProc.staRep.sta.tgvMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.tgvMatNbr);
		pMsgProc.staRep.sta.combAvgSpeed = HUITP_ENDIAN_EXG32(rcv.sta.combAvgSpeed);
		pMsgProc.staRep.sta.totalWeight = HUITP_ENDIAN_EXG32(rcv.sta.totalWeight);
		pMsgProc.staRep.sta.totalMatNbr = HUITP_ENDIAN_EXG32(rcv.sta.totalMatNbr);
		pMsgProc.staRep.sta.totalWorkMin = HUITP_ENDIAN_EXG32(rcv.sta.totalWorkMin);
		pMsgProc.staRep.sta.timeInUnix = HUITP_ENDIAN_EXG32(rcv.sta.timeInUnix);
		pMsgProc.staRep.sta.errNbr = HUITP_ENDIAN_EXG32(rcv.sta.errNbr);
		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		if (func_cloudvela_huitpxml_msg_pack(HUITP_MSGID_uni_bfsc_comb_scale_cmd_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}

	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_JASON){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else{
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");
	}

	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	//返回
	return SUCCESS;
}

#endif  //(HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)


/***************************************************************************************************************************
 *
 * 　不再使用的API，但由于其久经考验的程序价值，暂时放在这儿
 *
 ***************************************************************************************************************************/
/*
typedef struct HcuDiscDataSampleStorage
{
	UINT32 rdCnt;
	UINT32 wrtCnt;
	UINT32 recordNbr;
	UINT32 offlineNbr;
	UINT32 lastSid;
	HcuDiscDataSampleStorageArray_t recordItem[DISC_DATA_SAMPLE_STORAGE_NBR_MAX];
}HcuDiscDataSampleStorage_t;

//存入数据到本地内存磁盘
OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record)
{
	//int ret=0;
	UINT32 readCnt=0, wrtCnt=0, totalNbr=0, sid=0;

	//先检查输入数据的合法性，以下三项必须填写，其它的无所谓
	if (((record->sensortype)<=0) || ((record->timestamp)<=0))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error input of data save to memory, on Sensor type or Time stamp!\n");
	if (((record->onOffLine) != DISC_DATA_SAMPLE_ONLINE) && ((record->onOffLine) != DISC_DATA_SAMPLE_OFFLINE))
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error input of data save to memory, on on/off line attribute!\n");

	//取得缓冲区数据的状态
	readCnt = zHcuMemStorageBuf.rdCnt;
	wrtCnt = zHcuMemStorageBuf.wrtCnt;
	totalNbr = zHcuMemStorageBuf.recordNbr;
	sid = zHcuMemStorageBuf.lastSid+1; //达到32位的最大值，需要连续工作几百年，几乎不可能

	//先清零原先的记录，然后再贯穿写，不然之前的记录会污染当前的记录
	//但是一股脑儿的全部拷贝，反而不会出错，就不需要清零原来的记录了
	memcpy(&zHcuMemStorageBuf.recordItem[wrtCnt], record, sizeof(HcuDiscDataSampleStorageArray_t));
	if ((record->onOffLine) == DISC_DATA_SAMPLE_OFFLINE) {
		zHcuMemStorageBuf.offlineNbr = zHcuMemStorageBuf.offlineNbr + 1;
		if (zHcuMemStorageBuf.offlineNbr >= DISC_DATA_SAMPLE_STORAGE_NBR_MAX){
			zHcuMemStorageBuf.offlineNbr = DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
		}
	}

	//判断是否满，从而回写
	wrtCnt++;
	wrtCnt = wrtCnt % DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
	if (totalNbr >= DISC_DATA_SAMPLE_STORAGE_NBR_MAX)
	{
		totalNbr = DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
		readCnt++;
		readCnt = readCnt % DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
	}else{
		totalNbr++;
	}
	zHcuMemStorageBuf.rdCnt = readCnt;
	zHcuMemStorageBuf.wrtCnt = wrtCnt;
	zHcuMemStorageBuf.recordNbr = totalNbr;
	zHcuMemStorageBuf.lastSid = sid;  //最新一个写入记录的SID数值

	//Always successful, as the storage is a cycle buffer!
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Data record save to MEM-DISC, sid=%d, totalNbr=%d, offNbr=%d\n", sid, totalNbr, zHcuMemStorageBuf.offlineNbr);
	return SUCCESS;
}

//从本地内存磁盘，读取数据并送往后台
OPSTAT hcu_read_from_storage_mem(HcuDiscDataSampleStorageArray_t *record)
{
	//int ret=0;
	UINT32 readCnt=0, totalNbr=0;

	readCnt = zHcuMemStorageBuf.rdCnt;
	totalNbr = zHcuMemStorageBuf.recordNbr;
	memset(record, 0, sizeof(HcuDiscDataSampleStorageArray_t));

	//先检查输入数据的合法性，以下三项必须填写，其它的无所谓
	//总控数量优先
	if (totalNbr <=0 ) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error during MEM storage!\n");

	//读取一个记录
	//总数totalNbr和离线计数offlineNbr双重控制，如果出现不一致，一定是存储读取错误
	BOOL flag = FALSE;
	while(totalNbr>0){
		if (zHcuMemStorageBuf.recordItem[readCnt].onOffLine == DISC_DATA_SAMPLE_OFFLINE){
			memcpy(record, &zHcuMemStorageBuf.recordItem[readCnt], sizeof(HcuDiscDataSampleStorageArray_t));
			zHcuMemStorageBuf.recordItem[readCnt].onOffLine = DISC_DATA_SAMPLE_ONLINE; //used as backup
			if ((zHcuMemStorageBuf.offlineNbr <=0) || (zHcuMemStorageBuf.offlineNbr > totalNbr))
				HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Error occurs during totalNbr/offlineNbr control MEM STORAGE, recover!\n");
			totalNbr--;
			readCnt++;
			readCnt = readCnt % DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
			zHcuMemStorageBuf.rdCnt = readCnt;
			zHcuMemStorageBuf.recordNbr = totalNbr;
			zHcuMemStorageBuf.offlineNbr--;
			flag = TRUE;
			break;
		}else{
			totalNbr--;
			readCnt++;
			readCnt = readCnt % DISC_DATA_SAMPLE_STORAGE_NBR_MAX;
			zHcuMemStorageBuf.rdCnt = readCnt;
			zHcuMemStorageBuf.recordNbr = totalNbr;
		}
	}
	//如果offlineNbr>0，数据又没有读到，这个也是正常情况，因为Offline数据可能被桶形ONLINE数据给覆盖掉了
	//所以没有读到数据的情形，也得当正常情形来看待，而不能当出错，不然处理方式就不对
	if (flag == FALSE) HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Data read nothing, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: Data record read from MEM-DISC, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
	return SUCCESS;
}

//记录多少条，是否超限，过时清理等等，都在这儿完成
OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen)
{
	return hcu_disk_write(fId, dataBuffer, dataLen);
}

OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen)
{
	return SUCCESS;
}

*/

