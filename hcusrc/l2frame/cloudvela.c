/*
 * cloudvela.c
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l0webser/ftp.h"
#include "../l0comvm/sysversion.h"
#include "../l2frame/cloudvela.h"

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

    //Offline working, 定时重新启动链路，但不接受任何L3消息
	{MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,   	FSM_STATE_CLOUDVELA_OFFLINE, 		fsm_cloudvela_hwinv_phy_status_chg},
	{MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,  FSM_STATE_CLOUDVELA_OFFLINE, 		fsm_cloudvela_socket_data_rx},

	//HWINV发来了硬件状态的改变，一般是硬件重新插拔造成的PnP状态改变
	{MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,   	FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_hwinv_phy_status_chg},

	//Online working， 从后台接收到数据和控制命令，四种均有可能，具体是哪一种起作用，将由HWINV定时扫描并解决互斥问题
	//通过全局变量/本模块任务中心跳检测的共同作用，确定到底是哪一种通信接口在运行，确保不同后连接的PnP即插即用特性
	//所有传感器接口（232/485/BLE/USB-CAMERA/I2S-AUDIO）均直接采用API发送，接受采用阻塞式任务框架
	//所有后台接口（ETHERNET/WIFI/USB-OTG/3G4G）也是发送TX直接API，但RX采用阻塞式任务框架
	//由于实质上四种接口的处理方式完全一样，现使用同一个函数，未来有变化再改变
	{MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_data_rx},
	{MSG_ID_USBNET_CLOUDVELA_DATA_RX,   		FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_data_rx},  //fsm_cloudvela_usbnet_data_rx
	{MSG_ID_WIFI_CLOUDVELA_DATA_RX,   			FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_data_rx},  //fsm_cloudvela_wifi_data_rx
	{MSG_ID_3G4G_CLOUDVELA_DATA_RX,   			FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_ethernet_data_rx},  //fsm_cloudvela_3g4g_data_rx
	{MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,  FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_socket_data_rx},

    //Online working， 定时检查链路，并安排离线数据的及时上传
	{MSG_ID_COM_ALARM_REPORT,   				FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_alarm_report},
	{MSG_ID_COM_PM_REPORT,   					FSM_STATE_CLOUDVELA_ONLINE, 		fsm_cloudvela_pm_report},
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

	//采用分项目控制方式，降低不同项目之间的关联，特别是海量MSGID-STATE这一表的内存压力
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
HcuDiscDataSampleStorage_t zHcuMemStorageBuf;
CloudvelaTable_t zHcuCloudvelaTable;

//Task Global variables
extern int zHcuEthConClientFd;
extern int socket_connected;

//Main Entry
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
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Enter FSM_STATE_CLOUDVELA_INITED status, everything goes well!\n");
	}

	//Task global variables init，
	//zHcuMemStorageBuf其实还是一个全局变量，但因为这个模块用的多，故而在这里定义
	memset(&zHcuMemStorageBuf, 0, sizeof(HcuDiscDataSampleStorage_t));
	memset(&zHcuCloudvelaTable, 0, sizeof(CloudvelaTable_t));
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] = 0;

	//启动周期性定时器
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

OPSTAT fsm_cloudvela_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("CLOUDVELA: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_CLOUDVELA, TASK_ID_CLOUDVELA, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
			return FAILURE;
		}
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudvela_time_out_period();
	}

	//定时短时钟进行离线数据回送
	else if ((rcv.timeId == TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudvela_time_out_sendback_offline_data();
	}

	//for sw&db version report
	else if ((rcv.timeId == TIMER_ID_1S_CLOUDVELA_PRD_SWDB_VER_REP) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudvela_time_out_period_for_sw_db_report();
	}

	//这里的ret=FAILURE並不算严重，只不过造成状态机返回差错而已，并不会造成程序崩溃和数据混乱，所以只是程序的自我保护而已
	return ret;
}

//在线状态下，zHcuCloudvelaTable.ethConTry的参数用于控制是否可能进入链路断掉-》重建的情形，该参数在尝试过后会+1,
//但HWINV的状态逆转报告会导致它清零，从而具备再一次尝试的资格。这里还要照顾一种清醒：如果所有链路都没有建立起来，则自然需要不断的+1,也没有大问题
//长周期定时器, 周期性心跳时钟处理机制
OPSTAT func_cloudvela_time_out_period(void)
{
	int ret = 0;

	//检查链路状态
	//离线，则再连接
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		//if (func_cloudvela_http_conn_setup() == SUCCESS){
		if (socket_connected == TRUE && func_cloudvela_socket_conn_setup() == SUCCESS){
			zHcuSysStaPm.statisCnt.cloudVelaConnCnt++;
			//State Transfer to FSM_STATE_CLOUDVELA_ONLINE
			if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("CLOUDVELA: Connect state change, from OFFLINE to ONLINE!\n");

			//启动周期性短定时器，進行数据回传云平台
			if (zHcuMemStorageBuf.offlineNbr>0){
				ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK, \
						zHcuSysEngPar.timer.array[TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Error start timer!\n");
					return FAILURE;
				}
			}
		}
		//如果是失败情况，并不返回错误，属于正常情况
		//当链路不可用时，这个打印结果会非常频繁，放开比较好
		else{
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
				HcuDebugPrint("CLOUDVELA: Try to setup connection with back-cloud, but not success!\n");
			}
			zHcuSysStaPm.statisCnt.cloudVelaConnFailCnt++;
		}
	}

	//在线状态，则检查
	else if(FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		if (func_cloudvela_heart_beat_check() == FAILURE){
			zHcuSysStaPm.statisCnt.cloudVelaDiscCnt++;
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			//State Transfer to FSM_STATE_CLOUDVELA_OFFLINE
			if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("CLOUDVELA: Connect state change, from ONLINE to OFFLINE!\n");
			//并不立即启动连接的建立，而是等待下一个周期带来，否则状态机过于复杂
		}//心跳握手失败
		//在线而且心跳握手正常
		else{
			//先看下是否还有合理的高优先级连接存在，以便断掉当前连接，并试图重连高优先级的物理接口
			//主动断掉链路，需要复位CurrentConnection指示以及Http-Curl全局指针

			//如果当前是3G4G
			if ((zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G) &&
					((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				//如果还没有试图重连过
				if ((zHcuCloudvelaTable.ethConTry == 0) || (zHcuCloudvelaTable.usbnetConTry == 0) || (zHcuCloudvelaTable.wifiConTry == 0)){
					//Disconnect current 3g4g connection!!!
					if (hcu_3g4g_phy_link_disconnect() == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error disconnect 3G4G link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudvelaTable.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
			//如果当前是WIFI
			else if ((zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI) &&
				((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
						(zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				if ((zHcuCloudvelaTable.ethConTry == 0) || (zHcuCloudvelaTable.usbnetConTry == 0)){
					//Disconnect current wifi connection!!!
					if (hcu_wifi_phy_link_disconnect() == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error disconnect WIFI link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudvelaTable.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
			//如果当前是USBNET
			else if ((zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET) &&
					(zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE)){
				if (zHcuCloudvelaTable.ethConTry == 0){
					//Disconnect current usbnet connection!!!
					if (hcu_usbnet_phy_link_disconnect() == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error disconnect USBNET link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudvelaTable.curCon = HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID;
				}
				//不做任何动作
			}
			//如果是ETHERNET，则很好！
			//else do nothing

			//然后再试图启动周期性短定时器，進行数据回传云平台
			if (zHcuMemStorageBuf.offlineNbr>0){
				ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK, \
						zHcuSysEngPar.timer.array[TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Error start timer!\n");
					return FAILURE;
				}
			}//BUFFER还有离线数据
		}//end of 在线而且心跳握手正常
	}//end of 长周期定时在线状态

	//既不在线，也不离线，强制转移到离线状态以便下次恢复，这种情况很难得，一般不会跑到这儿来，这种情况通常发生在初始化期间或者状态机胡乱的情况下
	else{
		if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
			return FAILURE;
		}
	}

	return SUCCESS;
}

//短周期定时器
//短时长周期性时钟处理过程，主要是读取离线数据，回送云后台
OPSTAT func_cloudvela_time_out_sendback_offline_data(void)
{
	int ret = 0;
	HcuDiscDataSampleStorageArray_t record;

	//检查离线数据还有没有未送完的，如果没有，停止回送短定时器
	//先检查并停止，再读取数据，会造成多一次定时到达，不是坏事情，会让扫描更为完整
	if (zHcuMemStorageBuf.offlineNbr<=0){
		zHcuMemStorageBuf.offlineNbr = 0;
		ret = hcu_timer_stop(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error stop timer!\n");
			return FAILURE;
		}
		return SUCCESS;
	}

	//如果离线，一样需要停止定时器，不干活
	 if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE){
		ret = hcu_timer_stop(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error stop timer!\n");
			return FAILURE;
		}
		return SUCCESS;
	 }

	 //读取离线数据，通过参数进去无效，不得不使用返回RETURN FUNC的形式
	memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
	ret = hcu_read_from_storage_mem(&record);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuDebugPrint("CLOUDVELA: No any more off-line data shall be sent to CLOUD!\n");
		return SUCCESS;
	}

	//将发送成功后的数据条目从OFFLINE状态设置为ONLINE状态，表示数据已经发送完成，留着只是为了本地备份
	//实际上，读取的过程已经将OFFLINE状态给设置了，这里不需要再处理，不然差错处理会导致状态机太多的报错
	//在线状态下，读取离线成功，但发送不成功，会导致离线数据不再重复发送给后台，因为已经被设置为ONLINE/BACKUP留作本地备份了
	//考虑到这种概率非常低下，不值得再做复杂的处理，反正数据还留着本地备份，万一需要，可以人工介入

	//RECORD数据参数检查，这里只做最为基本的检查，
	if ((record.equipid <=0) || (record.timestamp<=0) || (record.sensortype <=0) ||(record.onOffLine != DISC_DATA_SAMPLE_OFFLINE)){
		HcuErrorPrint("CLOUDVELA: Invalid record read from MEM-DISC, dropped!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//准备发送数据到后台
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//RECORD数据打包，这里的打包需要根据数据类型进行判定，而且还是按照国家环保部的标准进行
	switch(record.sensortype)
	{
	case L3CI_emc:
		if (func_cloudvela_huanbao_emc_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_emc_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.emcValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: EMC pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_pm25:
		if (func_cloudvela_huanbao_pm25_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_pm25_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.pm1d0Value, record.pm2d5Value, record.pm10Value, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: PM25 pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_windspd:
		if (func_cloudvela_huanbao_winddir_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_windspd_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.windspdValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: WINDSPD pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_winddir:
		if (func_cloudvela_huanbao_windspd_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_winddir_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.winddirValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: WINDDIR pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_temp:
		if (func_cloudvela_huanbao_temp_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_temp_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.tempValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: TEMP pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_humid:
		if (func_cloudvela_huanbao_humid_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_humid_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.humidValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: HUMID pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_noise:
		if (func_cloudvela_huanbao_noise_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_noise_data_report, L3CI_cmdid_back_type_period, record.equipid, record.dataFormat, record.noiseValue, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: NOISE pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	case L3CI_hsmmp:
		if (func_cloudvela_huanbao_hsmmp_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_hsmmp_upload_req, L3CI_cmdid_back_type_period, record.equipid, record.gpsx, record.gpsy, record.gpsz, record.ns, record.ew, record.timestamp, record.hsmmpLink, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: HSMMP pack message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		break;

	default:
		HcuErrorPrint("CLOUDVELA: Not known sensor type MEM storage data found!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		//break;
	}

	//发送到后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send off-line data to back cloud error!\n");
			return FAILURE;
		}
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Under online state, send off-line data to cloud success!\n");
	}
	//State no change

	return SUCCESS;
}

OPSTAT func_cloudvela_time_out_period_for_socket_heart(void)
{
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		if(zHcuEthConClientFd < 0){
				HcuErrorPrint("CLOUDVELA: socket id is not valid!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
		}
	}

	UINT32 echolen = strlen(zHcuSysEngPar.cloud.cloudBhHcuName);
	if (send(zHcuEthConClientFd, zHcuSysEngPar.cloud.cloudBhHcuName, echolen, 0) != echolen)
	{
		HcuErrorPrint("CLOUDVELA: Socket disconnected & Mismatch in number of send bytes!\n\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
	}
	else
	{
		HcuDebugPrint("CLOUDVELA: Socket connected, send HEART_BEAT message to socket server success: %s!\n\n", zHcuSysEngPar.cloud.cloudBhHcuName);
	}

	return SUCCESS;
}

OPSTAT func_cloudvela_time_out_period_for_sw_db_report(void)
{
	UINT32 optId=0, cmdId=0, backType=0, ret=0;

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
		if (FAILURE == func_cloudvela_huanbao_hcu_inventory_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, &zHcuSysEngPar.swDbInvInfo, &buf))
		{
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			//return FAILURE;
		}
		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Online state, send HCU Inventory Resp to cloud failure!\n");
			return FAILURE;
		}

		else{
			HcuDebugPrint("CLOUDVELA: Online state, send HCU Inventory Resp to cloud success!\n");
		}
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_emc_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_emc_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_emc_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive EMC message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.emc.equipid <= 0) || (rcv.usercmdid != L3CI_emc) || (rcv.emc.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_emc_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType,
				rcv.emc.equipid, rcv.emc.dataFormat, rcv.emc.emcValue, rcv.emc.gps.gpsx, rcv.emc.gps.gpsy, rcv.emc.gps.gpsz, rcv.emc.gps.ns, rcv.emc.gps.ew, rcv.emc.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get EMC by ONLINE, but back off line so quick!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period EMC to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_pm25_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive PM25_RESP message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.pm25.equipid <= 0) || (rcv.usercmdid != L3CI_pm25) || (rcv.pm25.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_pm25_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType,
				rcv.pm25.equipid, rcv.pm25.dataFormat, rcv.pm25.pm1d0Value, rcv.pm25.pm2d5Value, rcv.pm25.pm10Value, rcv.pm25.gps.gpsx, rcv.pm25.gps.gpsy,
				rcv.pm25.gps.gpsz, rcv.pm25.gps.ns, rcv.pm25.gps.ew, rcv.pm25.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period PM25 to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}


OPSTAT fsm_cloudvela_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_winddir_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_winddir_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_winddir_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive WIND_DIR_RESP message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.winddir.equipid <= 0) || (rcv.usercmdid != L3CI_winddir) || (rcv.winddir.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_winddir_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.winddir.equipid,
				rcv.winddir.dataFormat, rcv.winddir.winddirValue, rcv.winddir.gps.gpsx, rcv.winddir.gps.gpsy, rcv.winddir.gps.gpsz, rcv.winddir.gps.ns, rcv.winddir.gps.ew, rcv.winddir.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get WINDDIR by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period WINDDIR to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_windspd_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_windspd_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_windspd_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive WINDSPD message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.windspd.equipid <= 0) || (rcv.usercmdid != L3CI_windspd) || (rcv.windspd.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_windspd_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.windspd.equipid,
				rcv.windspd.dataFormat, rcv.windspd.windspdValue, rcv.windspd.gps.gpsx, rcv.windspd.gps.gpsy, rcv.windspd.gps.gpsz, rcv.windspd.gps.ns, rcv.windspd.gps.ew, rcv.windspd.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get WINDSPD by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period WINDSPD to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_temp_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_temp_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_temp_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive TEMPERATURE message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.temp.equipid <= 0) || (rcv.usercmdid != L3CI_temp) || (rcv.temp.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_temp_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.temp.equipid,
				rcv.temp.dataFormat, rcv.temp.tempValue, rcv.temp.gps.gpsx, rcv.temp.gps.gpsy, rcv.temp.gps.gpsz, rcv.temp.gps.ns, rcv.temp.gps.ew, rcv.temp.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get TEMPERATURE by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period TEMPERATURE to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_humid_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_humid_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_humid_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive HUMIDITY message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.humid.equipid <= 0) || (rcv.usercmdid != L3CI_humid) || (rcv.humid.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_humid_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.humid.equipid,
				rcv.humid.dataFormat, rcv.humid.humidValue, rcv.humid.gps.gpsx, rcv.humid.gps.gpsy, rcv.humid.gps.gpsz, rcv.humid.gps.ns, rcv.humid.gps.ew, rcv.humid.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get HUMIDITY by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period HUMIDITY to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_noise_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_noise_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_noise_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_noise_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive NOISE message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.noise.equipid <= 0) || (rcv.usercmdid != L3CI_noise) || (rcv.noise.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_noise_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.noise.equipid,
				rcv.noise.dataFormat, rcv.noise.noiseValue, rcv.noise.gps.gpsx, rcv.noise.gps.gpsy, rcv.noise.gps.gpsz, rcv.noise.gps.ns, rcv.noise.gps.ew, rcv.noise.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get NOISE by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period NOISE to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}


//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_data_link_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_hsmmp_cloudvela_data_link_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hsmmp_cloudvela_data_link_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive HSMMP message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查参数
	if ((rcv.link.linkName == NULL) || ((strlen(rcv.link.linkName) > HCU_SYSDIM_FILE_NAME_LEN_MAX))){
		HcuErrorPrint("HSMMP: Error parameter message received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_hsmmp_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.link.equipid,
				rcv.link.gps.gpsx, rcv.link.gps.gpsy, rcv.link.gps.gpsz, rcv.link.gps.ns, rcv.link.gps.ew, rcv.link.timeStampStart, rcv.link.linkName, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get HSMMP by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period HSMMP to cloud success!\n");
	}
	//State no change

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
//HTTP-CURL的状态（创建/更改/删除）全部在本模块完成，ETHERNET/USBNET/WIFI/CON3G4G只能引用，而不能对其做任何操作
//当前的主要物理链路是Ethernet，链路建立没干啥，真实的HTTP-CURL操作都在接收与发送函数中，未来需要进一步优化
OPSTAT func_cloudvela_http_conn_setup(void)
{
	int ret = FAILURE;
	//3G, ETHERNET, WIFI connection?
	//周期性的检测，随便连上哪一种链路，则自然的搞定
	//后面的Hardware Inventory任务会制作一张实时全局硬件状态表，不需要通过消息发送给不同任务模块，这样谁需要访问，一查便知
	//这种方式下，消息减少，还能方便的实现PnP功能

	//检查任务模块状态
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_OFFLINE){
		HcuErrorPrint("CLOUDVELA: Error task status, can not setup new connection with cloud!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//第一次初始化，或者重新初始化全局HTTP-CURL链路
	if (hcu_cloudvela_http_link_init() == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("ETHERNET: Init Curl Http link failure!\n");
		return FAILURE;
	}

	//调用后台模块提供的函数，进行连接建立
	//第一优先级：连接ETHERNET
	//当前的情况下，ETHERNET物理链路的确啥都不干，只是回复成功，未来可以挂载更多的物理链路处理过程在其中
	if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_ethernet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.ethConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.ethConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET;
		return SUCCESS;
	}

	//第二优先级：连接USBNET
	if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_usbnet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.usbnetConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.usbnetConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET;
		return SUCCESS;
	}

	//第三优先级：连接WIFI
	if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_wifi_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.wifiConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.wifiConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI;
		return SUCCESS;
	}

	//第四优先级：连接3G4G
	if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_3g4g_phy_link_setup();

	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.g3g4ConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.g3g4ConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G;
		return SUCCESS;
	}
	/*
	//保留用作测试目的
	if ((rand()%10)>5)
		return SUCCESS;
	else*/
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: No CLOUD-BH physical link hardware available or not setup successful!\n");
	}
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
	return FAILURE;
}

//进入离线，具备两种方式：被动心跳检测到离线状态，主动断掉链路从而连到更高优先级的链路上
//这两种情形下，都需要复位ConTable对应的状态，不然可能会导致物理接口模块处于不正常的接收状态
//检测后台连接的有效性，是否还依然保持连接状态
//心跳握手检测，搞不好就是一种新状态，这里得想办法做成一个立即返回的过程，不然状态机太复杂，上面的控制程序也不好处理
//这里的发送接受，只是有SAE-CLOUD的反馈就算成功，如果在消息解码中再遇到问题，可以重新设置本模块的状态，比如强行进入离线等等
//所以这里返回成功，以及相应的处理，只能是不完善的，因为发送和接收过程分离的，但99%情况下，这种机制是可以正常工作的
OPSTAT func_cloudvela_heart_beat_check(void)
{
	int ret = 0;

	//如果这里的链路已经被动断掉，必须将下面的数据清理掉，不然会出现下层物理接口模块还在接收的状态
	//zHcuCloudvelaTable.curCon = CLOUDVELA_CONTROL_PHY_CON_INVALID;

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_heart_beat_msg_pack(&buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send HEART_BEAT to cloud, get by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send HEART_BEAT message out to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

//for cmd control by Shanchun
OPSTAT func_cloudvela_cmd_control_check(void)
{
	int ret = 0;

	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_cmd_control_msg_pack(&buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}

		//for GPS test
		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: NS is %c, latitude is %d, EW is %c, longitude is %d, Status is %c, speed is %d, high is %d.\n",
					zHcuVmCtrTab.hwinv.gps.NS, zHcuVmCtrTab.hwinv.gps.gpsY, zHcuVmCtrTab.hwinv.gps.EW, zHcuVmCtrTab.hwinv.gps.gpsX, zHcuVmCtrTab.hwinv.gps.status, zHcuVmCtrTab.hwinv.gps.speed, zHcuVmCtrTab.hwinv.gps.gpsZ);
			HcuDebugPrint("CLOUDVELA: year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d\n", zHcuVmCtrTab.hwinv.gps.D.year, zHcuVmCtrTab.hwinv.gps.D.month, zHcuVmCtrTab.hwinv.gps.D.day, zHcuVmCtrTab.hwinv.gps.D.hour, zHcuVmCtrTab.hwinv.gps.D.minute, zHcuVmCtrTab.hwinv.gps.D.second);
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Can not send CMD CONTROL CHECK to cloud due to offline status!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send CMD CONTROL CHECK message out to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

//存入数据到本地内存磁盘
OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record)
{
	//int ret=0;
	UINT32 readCnt=0, wrtCnt=0, totalNbr=0, sid=0;

	//先检查输入数据的合法性，以下三项必须填写，其它的无所谓
	if (((record->sensortype)<=0) || ((record->timestamp)<=0)){
		HcuErrorPrint("CLOUDVELA: Error input of data save to memory, on Sensor type or Time stamp!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	if (((record->onOffLine) != DISC_DATA_SAMPLE_ONLINE) && ((record->onOffLine) != DISC_DATA_SAMPLE_OFFLINE)){
		HcuErrorPrint("CLOUDVELA: Error input of data save to memory, on on/off line attribute!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

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
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Data record save to MEM-DISC, sid=%d, totalNbr=%d, offNbr=%d\n", sid, totalNbr, zHcuMemStorageBuf.offlineNbr);
	}
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
	if (totalNbr <=0 ){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//读取一个记录
	//总数totalNbr和离线计数offlineNbr双重控制，如果出现不一致，一定是存储读取错误
	BOOL flag = FALSE;
	while(totalNbr>0){
		if (zHcuMemStorageBuf.recordItem[readCnt].onOffLine == DISC_DATA_SAMPLE_OFFLINE){
			memcpy(record, &zHcuMemStorageBuf.recordItem[readCnt], sizeof(HcuDiscDataSampleStorageArray_t));
			zHcuMemStorageBuf.recordItem[readCnt].onOffLine = DISC_DATA_SAMPLE_ONLINE; //used as backup
			if ((zHcuMemStorageBuf.offlineNbr <=0) || (zHcuMemStorageBuf.offlineNbr > totalNbr)){
				HcuErrorPrint("CLOUDVELA: Error occurs during totalNbr/offlineNbr control MEM STORAGE, recover!\n");
				zHcuMemStorageBuf.offlineNbr = totalNbr;//强制将offlineNbr设置为跟totalNbr一致，但基本上没啥作用，而且还不影响继续发送数据到后台
			}
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
	if (flag == FALSE){
		HcuDebugPrint("CLOUDVELA: Data read nothing, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Data record read from MEM-DISC, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
	}
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


/*
 *
 *  发送部分
 *
 *
 */

//Send to backhawl cloud
OPSTAT func_cloudvela_send_data_to_cloud(CloudDataSendBuf_t *buf)
{
	//参数检查
	if ((buf->curLen <=0) || (buf->curLen >HCU_SYSMSG_COM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("CLOUDVELA: Error message length to send back for cloud!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//根据系统配置，决定使用那一种后台网络
	if (zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET){
		//if (hcu_ethernet_date_send(buf) == FAILURE){
		if (hcu_ethernet_socket_date_send(buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET){
		if (hcu_usbnet_data_send(buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI){
		if (hcu_wifi_data_send(buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudvelaTable.curCon == HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G){
		if (hcu_3g4g_data_send(buf) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else {
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error status of physical layer link for data send!\n");
		return FAILURE;
	}

	return SUCCESS;
}


/*
 *
 *  接收部分
 *
 *
 */

//四种情况下，如何判定当前后台网络的优先级，不然可能会造成混乱
//带buffer不定长的结构体，其长度域，只是为了发送消息的安全，都是结构体的最大长度，所以不可以使用它来界定buffer的实际长度
//发现在接收数据的处理中，使用的是完全相同的msg_struct_com_cloudvela_data_rx_t，故而采用完全相同的接收处理函数
//如果未来不同的物理接口出现变化，必须需要不同的处理，再行改变
OPSTAT fsm_cloudvela_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//参数检查
	if ((param_len <=0) || (param_len >HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("CLOUDVELA: Error message length received from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (param_ptr == NULL){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Receive NULL pointer data from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		return FAILURE;
	}

	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
		return FAILURE;
	}

	//接收消息解码
	msg_struct_com_cloudvela_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_cloudvela_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_cloudvela_data_rx_t))){
		HcuErrorPrint("CLOUDVELA: Receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	memcpy(rcv.buf, param_ptr, param_len);
	rcv.length = param_len;

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Receive data len=%d, data buffer = [%s], from [%s] module\n\n", rcv.length,  rcv.buf, zHcuVmCtrTab.task[src_id].taskName);
		//int i;
		//for(i =0; i<rcv.length; i++) HcuDebugPrint("CLOUDVELA: Receive data len=%d, data buffer = [%c], from [%s] module\n", rcv.length,  rcv.buf[i], zHcuTaskInfo[src_id].taskName);
	}

	//如果是XML自定义格式
	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML)
	{
		if (func_cloudvela_standard_xml_unpack(&rcv) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//如果是ZHB格式 //to be update for CMD if itf standard is ZHB
	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB)
	{
		if (func_cloudvela_standard_zhb_unpack(&rcv) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//非法格式
	else
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_ONLINE) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT fsm_cloudvela_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_hwinv_cloudvela_phy_status_chg_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t))){
		HcuErrorPrint("CLOUDVELA: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.ethStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.ethStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudvelaTable.ethConTry = 0;
	else if ((rcv.usbnetStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.usbnetStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudvelaTable.usbnetConTry = 0;
	else if ((rcv.wifiStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.wifiStatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudvelaTable.wifiConTry = 0;
	else if ((rcv.g3g4StatChg == HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.g3g4StatChg == HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudvelaTable.g3g4ConTry = 0;
	else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Message received with error content!\n");
		return FAILURE;
	}
	return SUCCESS;
}

//This callback function gets called by libcurl as soon as there is  data received that needs to be saved
size_t hcu_cloudvela_write_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	msg_struct_ethernet_cloudvela_data_rx_t *mem = (msg_struct_ethernet_cloudvela_data_rx_t *)userp;

	//For test
	//HcuDebugPrint("CLOUDVELA: hcu_cloudvela_write_callback receive data len = %d\n", realsize);
	memset(mem, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));
	if(realsize > HCU_SYSMSG_COM_MSG_BODY_LEN_MAX)
	{
		HcuErrorPrint("CLOUDVELA: No enough memory!\n");
		return 0;
	}
	memcpy(mem->buf, buffer, realsize); //mem->length
	mem->length = realsize;
	//HcuDebugPrint("CLOUDVELA: Accumulated len = %d\n", mem->length);

	return realsize;
}

//为了多线程的安全，CURL参数的初始化只能做一次，做一遍，不能都做，所以采用全局变量让其它线程共享，而不能大家都分别去做初始化
OPSTAT hcu_cloudvela_http_link_init(void)
{
	//初始化
	curl_global_init(CURL_GLOBAL_ALL);

	return SUCCESS;
}

OPSTAT func_cloudvela_heart_beat_received_handle(void)
{
	//FSM状态检查
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_ONLINE){
		HcuErrorPrint("CLOUDVELA: FSM State error, taking care of potential risk!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//HW状态检查，心跳的意义
	if ((zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)\
			&&(zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)){
		if (FsmSetState(TASK_ID_CLOUDVELA, FSM_STATE_CLOUDVELA_OFFLINE) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Set FSM state error!\n");
			return FAILURE;
		}
		HcuErrorPrint("CLOUDVELA: Hardware ETH/USBNET/WIFI/3G4G connection state error, taking care of potential risk!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Ack HEART_BEAT successful!\n");
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
	HcuDebugPrint("CLOUDVELA: ftp_opt.user_key: %s \n", ftp_opt.user_key);

	//char filetmp[64] = "swdownload.txt";
	//ftp_opt.url = zHcuSysEngPar.cloud.cloudFtpAdd;
	strcat(ftp_opt.url, zHcuSysEngPar.cloud.cloudFtpAdd);
	strcat(ftp_opt.url, filename);
	HcuDebugPrint("CLOUDVELA: ftp_opt.url: %s \n", ftp_opt.url);

	//ftp_opt.file = zHcuSysEngPar.swDownload.hcuSwDownloadDir;
	strcat(ftp_opt.file, zHcuSysEngPar.swm.hcuSwDownloadDir);
	strcat(ftp_opt.file, filename);
	HcuDebugPrint("CLOUDVELA: ftp_opt.file: %s \n", ftp_opt.file);

	if(FTP_DOWNLOAD_SUCCESS == ftp_download(ftp_opt)){
		//HcuDebugPrint("CLOUDVELA: HCU SW Download success.\n");
	    // send resp msg to cloud: 01 successful
	    //system("reboot");
		return SUCCESS;
	}

	else
	{
		//HcuErrorPrint("CLOUDVELA: HCU SW Download failed.\n");
	    // send resp msg to cloud: 00 failure
		return FAILURE;

	}

}

OPSTAT func_cloudvela_av_upload(char *filename)
{
	FTP_OPT ftp_opt;

	char usrtmp[3] = ":";

	memset( (void *)&ftp_opt, 0, sizeof(FTP_OPT));

	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpUserVideo);
	strcat(ftp_opt.user_key, usrtmp);
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpPwdVideo);
	HcuDebugPrint("CLOUDVELA: ftp_opt.user_key: %s \n", ftp_opt.user_key);

	strcat(ftp_opt.url, zHcuSysEngPar.cloud.cloudFtpAdd);
	strcat(ftp_opt.url, filename);
	HcuDebugPrint("CLOUDVELA: ftp_opt.url: %s \n", ftp_opt.url);

	strcat(ftp_opt.file, zHcuSysEngPar.videoSev.hcuVideoServerDir);
	strcat(ftp_opt.file, filename);
	HcuDebugPrint("CLOUDVELA: ftp_opt.file: %s \n", ftp_opt.file);

///
	char filepath[HCU_CLOUDVELA_PATH_MAX];
	memset(filepath,0,HCU_CLOUDVELA_PATH_MAX);
	//char *file = "/usr/local/apache_arm/htdocs/avorion/sensor20160528.dat";

	//int i;
	int rslt = readlink(ftp_opt.file,filepath,HCU_CLOUDVELA_PATH_MAX-1);
	if(rslt<0 || (rslt >=HCU_CLOUDVELA_PATH_MAX-1))
	{
		HcuErrorPrint("CLOUDVELA: read file path failure %d!\n", rslt);

		//return NULL;
	}
	else
	{
		filepath[rslt] = '\0';
		memset(ftp_opt.file,0,HCU_SYSDIM_FILE_NAME_LEN_MAX);
		strcpy(ftp_opt.file,filepath);

	}
	HcuDebugPrint("CLOUDVELA: AV file path: %s!\n", ftp_opt.file);

///

	if(FTP_UPLOAD_SUCCESS == ftp_upload(ftp_opt)){
		return SUCCESS;
	}

	else
	{
		return FAILURE;
	}

}

OPSTAT fsm_cloudvela_emc_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_pm25_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Adding by Shanchun for control cmd
	int ret=0;
	msg_struct_pm25_cloudvela_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_cloudvela_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_cloudvela_control_fb_t))){
		HcuErrorPrint("CLOUDVELA: Receive PM25_CMD_RESP message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.opt.equId <= 0) || (rcv.cmdId != L3CI_pm25)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
/*
	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_pm25_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType,
				rcv.pm25.equipid, rcv.pm25.pm1d0Value, rcv.pm25.pm2d5Value, rcv.pm25.pm10Value, rcv.pm25.gps.gpsx, rcv.pm25.gps.gpsy,
				rcv.pm25.gps.gpsz, rcv.pm25.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
		return FAILURE;
	}

*/

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));
		/*
		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("cloudvela: control command cmdId= %d\n", rcv.cmdId);
			HcuDebugPrint("cloudvela: control command optId= %d\n", rcv.optId);
			HcuDebugPrint("cloudvela: control command backType = %d\n", rcv.backType);
			HcuDebugPrint("cloudvela: control command equId= %d\n", rcv.opt.equId);
			HcuDebugPrint("cloudvela: control command powerOnOff= %d\n", rcv.opt.powerOnOff);
			HcuDebugPrint("cloudvela: control command newEquId= %d\n", rcv.opt.newEquId);
			HcuDebugPrint("cloudvela: control command workCycle= %d\n", rcv.opt.workCycle);
			HcuDebugPrint("cloudvela: control command interSample= %d\n", rcv.opt.interSample);
			HcuDebugPrint("cloudvela: control command meausTimes= %d\n", rcv.opt.meausTimes);
		}
		*/

		//func_cloudvela_huanbao_pm25_cmd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 powerOnOff, UINT32 interSample, UINT32 meausTimes, UINT32 newEquId, UINT32 workCycle,CloudDataSendBuf_t *buf)
		//打包数据
		if (func_cloudvela_huanbao_pm25_cmd_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, rcv.cmdId, rcv.optId, rcv.backType,
				rcv.opt.equId, rcv.opt.powerOnOff, rcv.opt.interSample, rcv.opt.meausTimes, rcv.opt.newEquId, rcv.opt.workCycle, &buf) == FAILURE)
		{
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send PM25 Sensor Control CMD ACK to cloud success!\n");
	}
	//State no change
	return SUCCESS;

}

OPSTAT fsm_cloudvela_winddir_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_windspd_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_temp_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_humid_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_noise_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自HSMMP的数据，处理后发送数据到后台云
OPSTAT fsm_cloudvela_hsmmp_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}



OPSTAT fsm_cloudvela_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)//to create a seperate task in L3APP to handle all alarm stuff
{
	int ret=0;
	msg_struct_alarm_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_alarm_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_alarm_report_t))){
		HcuErrorPrint("CLOUDVELA: Receive Alarm message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equID <= 0) || (rcv.usercmdid != L3CI_alarm) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if(dbi_HcuSysAlarmInfo_save(&rcv) == FAILURE)
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Can not save data into database!\n");
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据

		if (func_cloudvela_huanbao_alarm_msg_pack(CLOUDVELA_BH_MSG_TYPE_ALARM_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.alarmType, rcv.alarmContent, rcv.equID, rcv.alarmServerity, rcv.alarmClearFlag, rcv.photofileName, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get ALARM by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send alarm info to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}


OPSTAT fsm_cloudvela_pm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_pm_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm_report_t))){
		HcuErrorPrint("CLOUDVELA: Receive PM message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.usercmdid != L3CI_performance) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}


	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudvela_huanbao_pm_msg_pack(CLOUDVELA_BH_MSG_TYPE_PM_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.CloudVelaConnCnt, rcv.CloudVelaConnFailCnt, rcv.CloudVelaDiscCnt, rcv.SocketDiscCnt, rcv.TaskRestartCnt, rcv.cpu_occupy, rcv.mem_occupy, rcv.disk_occupy, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get PM by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send PM info to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudvela_socket_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//参数检查
	if ((param_len <=0) || (param_len >HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("CLOUDVELA: Error message length received from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (param_ptr == NULL){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Receive NULL pointer data from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		return FAILURE;
	}

	//接收消息解码
	msg_struct_com_cloudvela_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_cloudvela_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_cloudvela_data_rx_t))){
		HcuErrorPrint("CLOUDVELA: Receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	memcpy(rcv.buf, param_ptr, param_len);
	rcv.length = param_len;

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Receive data len=%d, data buffer = [%s], from [%s] module\n\n", rcv.length,  rcv.buf, zHcuVmCtrTab.task[src_id].taskName);
	}

	//如果是XML自定义格式
	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML)
	{
		if (func_cloudvela_standard_xml_unpack(&rcv) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//如果是ZHB格式 //to be update for CMD if itf standard is ZHB
	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB)
	{
		if (func_cloudvela_standard_zhb_unpack(&rcv) == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Unpack receive message error from [%s] module!\n", zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//非法格式
	else
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
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
	if (FsmGetState(TASK_ID_CLOUDVELA) != FSM_STATE_CLOUDVELA_OFFLINE){
		HcuErrorPrint("CLOUDVELA: Error task status, can not setup new connection with cloud!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//调用后台模块提供的函数，进行连接建立
	//第一优先级：连接ETHERNET
	//当前的情况下，ETHERNET物理链路的确啥都不干，只是回复成功，未来可以挂载更多的物理链路处理过程在其中
	if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_ethernet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.ethConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.ethConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET;
		return SUCCESS;
	}

	//第二优先级：连接USBNET
	if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_usbnet_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.usbnetConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.usbnetConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET;
		return SUCCESS;
	}

	//第三优先级：连接WIFI
	if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_wifi_phy_link_setup();
	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.wifiConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.wifiConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI;
		return SUCCESS;
	}

	//第四优先级：连接3G4G
	if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_3g4g_phy_link_setup();

	}
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		zHcuCloudvelaTable.g3g4ConTry++;
	}
	if (ret == SUCCESS){
		zHcuCloudvelaTable.g3g4ConTry = 0;
		zHcuCloudvelaTable.curCon =HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G;
		return SUCCESS;
	}
	/*
	//保留用作测试目的
	if ((rand()%10)>5)
		return SUCCESS;
	else*/
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: No CLOUD-BH physical link hardware available or not setup successful!\n");
	}
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
	return FAILURE;
}

OPSTAT func_cloudvela_msg_heart_beat_req_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_heart_beat_req_t received!\n");
}

OPSTAT func_cloudvela_msg_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t received!\n");
}

OPSTAT func_cloudvela_msg_inventory_req_received_handle(StrMsg_HUITP_MSGID_uni_inventory_req_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_inventory_req_t received!\n");
}

OPSTAT func_cloudvela_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_uni_inventory_confirm_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_inventory_confirm_t received!\n");
}

OPSTAT func_cloudvela_msg_sw_package_req_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_req_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_sw_package_req_t received!\n");
}

OPSTAT func_cloudvela_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *rcv)
{
	HCU_ERROR_PRINT_CLOUDVELA("SPSVIRGO: Un-supported message but known message StrMsg_HUITP_MSGID_uni_sw_package_confirm_t received!\n");
}

//BFSC项目的处理过程
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
OPSTAT fsm_cloudvela_l3bfsc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	msg_struct_l3bfsc_cloudvela_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_data_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive L3BFSC message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//申明发送消息
	CloudDataSendBuf_t pMsgOutput;

	//分格式类型组装
	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_HUITP_XML){
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
		pMsgProc.respValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(HUITP_IEID_uni_scale_weight_value) - 4);
		pMsgProc.respValue.type = rcv.type;
		pMsgProc.respValue.dataFormat = rcv.dataFormat;
		pMsgProc.respValue.snrCfgNbrMax = rcv.snrCfgNbrMax;
		pMsgProc.respValue.snrUsedBitmap = rcv.snrUsedBitmap;
		memcpy(pMsgProc.respValue.snrValue, rcv.snrValue, HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
		//不能采取memcpy的方式，因为既有可能两边定义的结构不一致，更有可能pack()导致的sizeof()不一样
		pMsgProc.respValue.sta.combTimes = rcv.sta.combTimes;
		pMsgProc.respValue.sta.tttTimes = rcv.sta.tttTimes;
		pMsgProc.respValue.sta.tgvTimes = rcv.sta.tgvTimes;
		pMsgProc.respValue.sta.combMatNbr = rcv.sta.combMatNbr;
		pMsgProc.respValue.sta.tttMatNbr = rcv.sta.tttMatNbr;
		pMsgProc.respValue.sta.tgvMatNbr = rcv.sta.tgvMatNbr;
		pMsgProc.respValue.sta.combAvgSpeed = rcv.sta.combAvgSpeed;
		pMsgProc.respValue.sta.totalWeight = rcv.sta.totalWeight;
		pMsgProc.respValue.sta.totalMatNbr = rcv.sta.totalMatNbr;
		pMsgProc.respValue.sta.totalWorkMin = rcv.sta.totalWorkMin;
		pMsgProc.respValue.sta.timeInUnix = rcv.sta.timeInUnix;
		pMsgProc.respValue.sta.errNbr = rcv.sta.errNbr;

		//Pack message
		StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
		memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		memcpy(&pMsgInput, &pMsgProc, msgProcLen);
		memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
		if (func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_bfsc_comb_scale_data_resp, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)
			HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");
	}
	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_XML){
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");
	}
	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB){
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
	int ret=0;
	msg_struct_l3bfsc_cloudvela_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_data_report_t))){
		HcuErrorPrint("CLOUDVELA: Receive L3BFSC message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//发送数据给后台
	//初始化变量
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));

	//打包数据：这里引用了比较高潮的技巧，将传感器数组传递到目标
/*	if (func_cloudvela_huanbao_bfsc_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.cmdid, rcv.optid, rcv.optpar, rcv.eqpid,
			rcv.dataFormat, 0, rcv.sensorNbr, rcv.sensorWsValue, rcv.timestamp, &buf) == FAILURE){
		HcuErrorPrint("CLOUDVELA: Package message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}*/







	//Send out
	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error!\n");

	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_event_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_l3bfsc_cloudvela_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t))){
		HcuErrorPrint("CLOUDVELA: Receive L3BFSC message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
/*	if ((rcv.cmdid != L3CI_bfsc_comb_scale) || (rcv.timestamp <=0)){
		HcuErrorPrint("CLOUDVELA: Receive invalid data!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}*/

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
/*		if (func_cloudvela_huanbao_bfsc_msg_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, rcv.cmdid, rcv.optid, rcv.optpar, rcv.eqpid,
				rcv.dataFormat, rcv.modbusVal, 0, NULL, rcv.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Package message error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}*/

		//Send out
		ret = func_cloudvela_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error!\n");
			return FAILURE;
		}
	}else{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error send data to cloud, get NOISE by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("CLOUDVELA: Online state, send instance/period L3BFSC to cloud success!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_cloudvela_l3bfsc_statistic_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

#endif


