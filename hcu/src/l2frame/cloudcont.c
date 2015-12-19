/*
 * cloudconnect.c
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#include "cloudcont.h"
#include "../comvm/trace.h"
#include "../comvm/timer.h"
#include "../l2frame/hwinv.h"

/*
** FSM of the CLOUDCONT
*/
FsmStateItem_t FsmCloudCont[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_cloudcont_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_cloudcont_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_cloudcont_restart},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_CLOUDCONT_INITED,            	fsm_cloudcont_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_CLOUDCONT_INITED,            	fsm_cloudcont_restart},

    //Offline working, 定时重新启动链路，但不接受任何L3消息
    {MSG_ID_COM_RESTART,        FSM_STATE_CLOUDCONT_OFFLINE,            fsm_cloudcont_restart},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_CLOUDCONT_OFFLINE,            fsm_cloudcont_time_out},

    //Online working， 定时检查链路，并安排离线数据的及时上传
    {MSG_ID_COM_RESTART,        			FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_time_out},
	{MSG_ID_EMC_CLOUD_DATA_RESP,   			FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_emc_data_resp},
	{MSG_ID_PM25_CLOUD_DATA_RESP,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_pm25_data_resp},
	{MSG_ID_WINDDIR_CLOUD_DATA_RESP, 		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_winddir_data_resp},
	{MSG_ID_WINDSPD_CLOUD_DATA_RESP,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_windspd_data_resp},
	{MSG_ID_TEMP_CLOUD_DATA_RESP,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_temp_data_resp},
	{MSG_ID_HUMID_CLOUD_DATA_RESP,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_humid_data_resp},

	//Online working， 从后台接收到数据和控制命令，四种均有可能，具体是哪一种起作用，将由HWINV定时扫描并解决互斥问题
	//通过全局变量/本模块任务中心跳检测的共同作用，确定到底是哪一种通信接口在运行，确保不同后连接的PnP即插即用特性
	//所有传感器接口（232/485/BLE/USB-CAMERA/I2S-AUDIO）均直接采用API发送，接受采用阻塞式任务框架
	//所有后台接口（ETHERNET/WIFI/USB-OTG/3G4G）也是发送TX直接API，但RX采用阻塞式任务框架
	//由于实质上四种接口的处理方式完全一样，现使用同一个函数，未来有变化再改变
	{MSG_ID_ETHERNET_CLOUD_DATA_RX,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_ethernet_data_rx},
	{MSG_ID_USBNET_CLOUD_DATA_RX,   		FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_ethernet_data_rx},  //fsm_cloudcont_usbnet_data_rx
	{MSG_ID_WIFI_CLOUD_DATA_RX,   			FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_ethernet_data_rx},  //fsm_cloudcont_wifi_data_rx
	{MSG_ID_3G4G_CLOUD_DATA_RX,   			FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_ethernet_data_rx},  //fsm_cloudcont_3g4g_data_rx

	//HWINV发来了硬件状态的改变，一般是硬件重新插拔造成的PnP状态改变
	{MSG_ID_HWINV_CLOUD_PHY_STATUS_CHG,   	FSM_STATE_CLOUDCONT_ONLINE, 	fsm_cloudcont_hwinv_phy_status_chg},
	{MSG_ID_HWINV_CLOUD_PHY_STATUS_CHG,   	FSM_STATE_CLOUDCONT_OFFLINE, 	fsm_cloudcont_hwinv_phy_status_chg},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
HcuDiscDataSampleStorage_t zHcuMemStorageBuf;
CloudControlTable_t zHcuCloudConTable;
//CURL *zHcuCloudCurlPtrTx;
//CURL *zHcuCloudCurlPtrRx;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_cloudcont_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_CLOUD, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State at fsm_cloudcont_task_entry\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_cloudcont_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
	msg_struct_com_init_feedback_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_feedback_t));
	snd.length = sizeof(msg_struct_com_init_feedback_t);
	ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, TASK_ID_SVRCON, TASK_ID_CLOUD, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_SVRCON]);
		return FAILURE;
	}
	ret = FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State at fsm_cloudcont_init\n");
		return FAILURE;
	}
	#if (TRACE_DEBUG_ON)
		HcuDebugPrint("CLOUD: Enter FSM_STATE_CLOUDCONT_INITED status, everything goes well!\n");
	#endif //(TRACE_DEBUG_ON)

	//Task global variables init，
	//zHcuMemStorageBuf其实还是一个全局变量，但因为这个模块用的多，故而在这里定义
	memset(&zHcuMemStorageBuf, 0, sizeof(HcuDiscDataSampleStorage_t));
	memset(&zHcuCloudConTable, 0, sizeof(CloudControlTable_t));
	//zHcuCloudCurlPtrTx = NULL;
	//zHcuCloudCurlPtrTx = NULL;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_CLOUD, TIMER_ID_1S_CLOUD_PERIOD_HEART_BEAT, CLOUD_TIMER_DURATION_PERIOD_HEART_BEAT, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUD: Error start timer!\n");
		return FAILURE;
	}

	//State Transfer to FSM_STATE_CLOUDCONT_OFFLINE
	ret = FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE);
	if (ret == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State at fsm_cloudcont_init\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudcont_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_cloudcont_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("CLOUD: Receive message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_CLOUD_PERIOD_HEART_BEAT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudcont_time_out_period();
	}

	//定时短时钟进行离线数据回送
	else if ((rcv.timeId == TIMER_ID_1S_CLOUD_SEND_DATA_BACK) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_cloudcont_time_out_sendback_offline_data();
	}

	//这里的ret=FAILURE並不算严重，只不过造成状态机返回差错而已，并不会造成程序崩溃和数据混乱，所以只是程序的自我保护而已
	return ret;
}

//在线状态下，zHcuCloudConTable.ethConTry的参数用于控制是否可能进入链路断掉-》重建的情形，该参数在尝试过后会+1,
//但HWINV的状态逆转报告会导致它清零，从而具备再一次尝试的资格。这里还要照顾一种清醒：如果所有链路都没有建立起来，则自然需要不断的+1,也没有大问题
//长周期定时器, 周期性心跳时钟处理机制
OPSTAT func_cloudcont_time_out_period(void)
{
	int ret = 0;

	//检查链路状态
	//离线，则再连接
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_OFFLINE){
		if (func_cloudcont_http_conn_setup() == SUCCESS){
			//State Transfer to FSM_STATE_CLOUDCONT_ONLINE
			if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_ONLINE) == FAILURE){
				HcuErrorPrint("CLOUD: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("CLOUD: Connect state change, from OFFLINE to ONLINE!\n");

			//启动周期性短定时器，進行数据回传云平台
			if (zHcuMemStorageBuf.offlineNbr>0){
				ret = hcu_timer_start(TASK_ID_CLOUD, TIMER_ID_1S_CLOUD_SEND_DATA_BACK, CLOUD_TIMER_DURATION_PERIOD_SEND_DATA_BACK, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					HcuErrorPrint("CLOUD: Error start timer!\n");
					return FAILURE;
				}
			}
		}
		//如果是失败情况，并不返回错误，属于正常情况
		//当链路不可用时，这个打印结果会非常频繁，放开比较好
		else{
		//#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: Try to setup connection with back-cloud, but not success!\n");
		//#endif //TRACE_DEBUG_ON
		}
	}

	//在线状态，则检查
	else if(FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		if (func_cloudcont_heart_beat_check() == FAILURE){
			//State Transfer to FSM_STATE_CLOUDCONT_OFFLINE
			if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
				HcuErrorPrint("CLOUD: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("CLOUD: Connect state change, from ONLINE to OFFLINE!\n");
			//并不立即启动连接的建立，而是等待下一个周期带来，否则状态机过于复杂
		}//心跳握手失败
		//在线而且心跳握手正常
		else{
			//先看下是否还有合理的高优先级连接存在，以便断掉当前连接，并试图重连高优先级的物理接口
			//主动断掉链路，需要复位CurrentConnection指示以及Http-Curl全局指针

			//如果当前是3G4G
			if ((zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_3G4G) &&
					((zHcuHwinvTable.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuHwinvTable.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
							(zHcuHwinvTable.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				//如果还没有试图重连过
				if ((zHcuCloudConTable.ethConTry == 0) || (zHcuCloudConTable.usbnetConTry == 0) || (zHcuCloudConTable.wifiConTry == 0)){
					//Disconnect current 3g4g connection!!!
					if (hcu_3g4g_phy_link_disconnect() == FAILURE){
						HcuErrorPrint("CLOUD: Error disconnect 3G4G link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
						HcuErrorPrint("CLOUD: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudConTable.curCon = CLOUD_CONTROL_PHY_CON_INVALID;
					//清除HTTP-CURL链接
					//if((zHcuCloudCurlPtrTx != NULL) || (zHcuCloudCurlPtrRx != NULL)){
					//	if(zHcuCloudCurlPtrTx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrTx);  //End a libcurl easy session
					//	if(zHcuCloudCurlPtrRx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrRx);  //End a libcurl easy session
					//	curl_global_cleanup();
					//}
				}
				//不做任何动作
			}
			//如果当前是WIFI
			else if ((zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_WIFI) &&
				((zHcuHwinvTable.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE) ||
						(zHcuHwinvTable.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE))){
				if ((zHcuCloudConTable.ethConTry == 0) || (zHcuCloudConTable.usbnetConTry == 0)){
					//Disconnect current wifi connection!!!
					if (hcu_wifi_phy_link_disconnect() == FAILURE){
						HcuErrorPrint("CLOUD: Error disconnect WIFI link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
						HcuErrorPrint("CLOUD: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudConTable.curCon = CLOUD_CONTROL_PHY_CON_INVALID;
					//清除HTTP-CURL链接
					//if((zHcuCloudCurlPtrTx != NULL) || (zHcuCloudCurlPtrRx != NULL)){
					//	if(zHcuCloudCurlPtrTx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrTx);  //End a libcurl easy session
					//	if(zHcuCloudCurlPtrRx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrRx);  //End a libcurl easy session
					//	curl_global_cleanup();
					//}
				}
				//不做任何动作
			}
			//如果当前是USBNET
			else if ((zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_USBNET) &&
					(zHcuHwinvTable.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE)){
				if (zHcuCloudConTable.ethConTry == 0){
					//Disconnect current usbnet connection!!!
					if (hcu_usbnet_phy_link_disconnect() == FAILURE){
						HcuErrorPrint("CLOUD: Error disconnect USBNET link!\n");
						return FAILURE;
					}
					//设置为离线状态，以便下次重连接
					if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
						HcuErrorPrint("CLOUD: Error set FSM status!\n");
						return FAILURE;
					}
					//设置当前工作物理链路为无效
					zHcuCloudConTable.curCon = CLOUD_CONTROL_PHY_CON_INVALID;
					//清除HTTP-CURL链接
					//if((zHcuCloudCurlPtrTx != NULL) || (zHcuCloudCurlPtrRx != NULL)){
					//	if(zHcuCloudCurlPtrTx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrTx);  //End a libcurl easy session
					//	if(zHcuCloudCurlPtrRx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrRx);  //End a libcurl easy session
					//	curl_global_cleanup();
					//}
				}
				//不做任何动作
			}
			//如果是ETHERNET，则很好！
			//else do nothing

			//然后再试图启动周期性短定时器，進行数据回传云平台
			if (zHcuMemStorageBuf.offlineNbr>0){
				ret = hcu_timer_start(TASK_ID_CLOUD, TIMER_ID_1S_CLOUD_SEND_DATA_BACK, CLOUD_TIMER_DURATION_PERIOD_SEND_DATA_BACK, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					HcuErrorPrint("CLOUD: Error start timer!\n");
					return FAILURE;
				}
			}//BUFFER还有离线数据
		}//end of 在线而且心跳握手正常
	}//end of 长周期定时在线状态

	//既不在线，也不离线，强制转移到离线状态以便下次恢复，这种情况很难得，一般不会跑到这儿来，这种情况通常发生在初始化期间或者状态机胡乱的情况下
	else{
		if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
			HcuErrorPrint("CLOUD: Error Set FSM State!\n");
			return FAILURE;
		}
	}

	return SUCCESS;
}

//短周期定时器
//短时长周期性时钟处理过程，主要是读取离线数据，回送云后台
OPSTAT func_cloudcont_time_out_sendback_offline_data(void)
{
	int ret = 0;
	HcuDiscDataSampleStorageArray_t record;

	//检查离线数据还有没有未送完的，如果没有，停止回送短定时器
	//先检查并停止，再读取数据，会造成多一次定时到达，不是坏事情，会让扫描更为完整
	if (zHcuMemStorageBuf.offlineNbr<=0){
		zHcuMemStorageBuf.offlineNbr = 0;
		ret = hcu_timer_stop(TASK_ID_CLOUD, TIMER_ID_1S_CLOUD_SEND_DATA_BACK, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Error stop timer!\n");
			return FAILURE;
		}
		return SUCCESS;
	}

	//如果离线，一样需要停止定时器，不干活
	 if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_OFFLINE){
		ret = hcu_timer_stop(TASK_ID_CLOUD, TIMER_ID_1S_CLOUD_SEND_DATA_BACK, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Error stop timer!\n");
			return FAILURE;
		}
		return SUCCESS;
	 }

	 //读取离线数据，通过参数进去无效，不得不使用返回RETURN FUNC的形式
	memset(&record, 0, sizeof(HcuDiscDataSampleStorageArray_t));
	ret = hcu_read_from_storage_mem(&record);
	if (ret == FAILURE){
		HcuDebugPrint("CLOUD: No any more off-line data shall be sent to CLOUD!\n");
		return SUCCESS;
	}

	//将发送成功后的数据条目从OFFLINE状态设置为ONLINE状态，表示数据已经发送完成，留着只是为了本地备份
	//实际上，读取的过程已经将OFFLINE状态给设置了，这里不需要再处理，不然差错处理会导致状态机太多的报错
	//在线状态下，读取离线成功，但发送不成功，会导致离线数据不再重复发送给后台，因为已经被设置为ONLINE/BACKUP留作本地备份了
	//考虑到这种概率非常低下，不值得再做复杂的处理，反正数据还留着本地备份，万一需要，可以人工介入

	//RECORD数据参数检查，这里只做最为基本的检查，
	if ((record.equipid <=0) || (record.timestamp<=0) || (record.sensortype <=0) ||(record.onOffLine != DISC_DATA_SAMPLE_OFFLINE)){
		HcuErrorPrint("CLOUD: Invalid record read from MEM-DISC, dropped!\n");
		return FAILURE;
	}

	//准备发送数据到后台
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//RECORD数据打包，这里的打包需要根据数据类型进行判定，而且还是按照国家环保部的标准进行
	switch(record.sensortype)
	{
	case L3CI_emc:
		if (func_cloudcont_huanbao_emc_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_emc_data_report, L3CI_cmdid_back_type_period, record.equipid, record.emcValue, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: EMC pack message error!\n");
			return FAILURE;
		}
		break;

	case L3CI_pm25:
		if (func_cloudcont_huanbao_pm25_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_pm25_data_report, L3CI_cmdid_back_type_period, record.equipid, record.pm1d0Value, record.pm2d5Value, record.pm10Value, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: PM25 pack message error!\n");
			return FAILURE;
		}
		break;

	case L3CI_windspd:
		if (func_cloudcont_huanbao_winddir_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_windspd_data_report, L3CI_cmdid_back_type_period, record.equipid, record.windspdValue, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: WINDSPD pack message error!\n");
			return FAILURE;
		}
		break;

	case L3CI_winddir:
		if (func_cloudcont_huanbao_windspd_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_winddir_data_report, L3CI_cmdid_back_type_period, record.equipid, record.winddirValue, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: WINDDIR pack message error!\n");
			return FAILURE;
		}
		break;

	case L3CI_temp:
		if (func_cloudcont_huanbao_temp_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_temp_data_report, L3CI_cmdid_back_type_period, record.equipid, record.tempValue, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: TEMP pack message error!\n");
			return FAILURE;
		}
		break;

	case L3CI_humid:
		if (func_cloudcont_huanbao_humid_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, record.sensortype, L3PO_humid_data_report, L3CI_cmdid_back_type_period, record.equipid, record.humidValue, record.gpsx, record.gpsy, record.gpsz, record.timestamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: HUMID pack message error!\n");
			return FAILURE;
		}
		break;

	default:
		HcuErrorPrint("CLOUD: Not known sensor type MEM storage data found!\n");
		//break;
	}

	//发送到后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Send off-line data to back cloud error!\n");
			return FAILURE;
		}
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Under online state, send off-line data to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change

	return SUCCESS;
}


OPSTAT fsm_cloudcont_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_emc_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_emc_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_emc_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive EMC message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_emc) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_emc_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.emcValue, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get EMC by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period EMC to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudcont_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_pm25_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive PM25_RESP message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_pm25) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_pm25_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.pm1d0Value, rcv.pm2d5Value, rcv.pm10Value, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get PM25 by ONLINE, but back off line so quick!");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period PM25 to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change
	return SUCCESS;
}


OPSTAT fsm_cloudcont_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_winddir_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_winddir_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_winddir_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive WIND_DIR_RESP message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_winddir) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_winddir_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.winddirValue, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get WINDDIR by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period WINDDIR to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudcont_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_windspd_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_windspd_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_windspd_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive WINDSPD message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_windspd) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_windspd_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.windspdValue, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get WINDSPD by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period WINDSPD to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudcont_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_temp_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_temp_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_temp_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive TEMPERATURE message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_temp) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_temp_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.tempValue, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get TEMPERATURE by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period TEMPERATURE to cloud success!\n");
	#endif //TRACE_DEBUG_ON
	//State no change
	return SUCCESS;
}

OPSTAT fsm_cloudcont_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_humid_cloud_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_humid_cloud_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_humid_cloud_data_resp_t))){
		HcuErrorPrint("CLOUD: Receive HUMIDITY message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equipid <= 0) || (rcv.usercmdid != L3CI_humid) || (rcv.timeStamp <=0)){
		HcuErrorPrint("CLOUD: Receive invalid data!\n");
		return FAILURE;
	}

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_humid_msg_pack(CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8, rcv.usercmdid, rcv.useroptid, rcv.cmdIdBackType, rcv.equipid, rcv.humidValue, rcv.gps_x, rcv.gps_y, rcv.gps_z, rcv.timeStamp, &buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send data to cloud, get HUMIDITY by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send instance/period HUMIDITY to cloud success!\n");
	#endif //TRACE_DEBUG_ON
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
OPSTAT func_cloudcont_http_conn_setup(void)
{
	int ret = FAILURE;
	//3G, ETHERNET, WIFI connection?
	//周期性的检测，随便连上哪一种链路，则自然的搞定
	//后面的Hardware Inventory任务会制作一张实时全局硬件状态表，不需要通过消息发送给不同任务模块，这样谁需要访问，一查便知
	//这种方式下，消息减少，还能方便的实现PnP功能

	//检查任务模块状态
	if (FsmGetState(TASK_ID_CLOUD) != FSM_STATE_CLOUDCONT_OFFLINE){
		HcuErrorPrint("CLOUD: Error task status, can not setup new connection with cloud!\n");
		return FAILURE;
	}

	//如果发现HTTP-CURL已经被初始化了，尽快消灭之
	//这是有可能链路已经到达OFFLINE，但CURL还没有消灭的情况
	//if((zHcuCloudCurlPtrTx != NULL) || (zHcuCloudCurlPtrRx != NULL)){
	//	if(zHcuCloudCurlPtrTx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrTx);  //End a libcurl easy session
	//	if(zHcuCloudCurlPtrRx != NULL) curl_easy_cleanup(zHcuCloudCurlPtrRx);  //End a libcurl easy session
	//	curl_global_cleanup();
	//}

	//第一次初始化，或者重新初始化全局HTTP-CURL链路
	if (hcu_cloud_http_link_init() == FAILURE){
		HcuErrorPrint("ETHERNET: Init Curl Http link failure!\n");
		return FAILURE;
	}

	//调用后台模块提供的函数，进行连接建立
	//第一优先级：连接ETHERNET
	//当前的情况下，ETHERNET物理链路的确啥都不干，只是回复成功，未来可以挂载更多的物理链路处理过程在其中
	if (zHcuHwinvTable.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_ethernet_phy_link_setup();
	}
	if (ret == FAILURE) zHcuCloudConTable.ethConTry++;
	if (ret == SUCCESS){
		zHcuCloudConTable.ethConTry = 0;
		zHcuCloudConTable.curCon =CLOUD_CONTROL_PHY_CON_ETHERNET;
		return SUCCESS;
	}

	//第二优先级：连接USBNET
	if (zHcuHwinvTable.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_usbnet_phy_link_setup();
	}
	if (ret == FAILURE) zHcuCloudConTable.usbnetConTry++;
	if (ret == SUCCESS){
		zHcuCloudConTable.usbnetConTry = 0;
		zHcuCloudConTable.curCon =CLOUD_CONTROL_PHY_CON_USBNET;
		return SUCCESS;
	}

	//第三优先级：连接WIFI
	if (zHcuHwinvTable.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_wifi_phy_link_setup();
	}
	if (ret == FAILURE) zHcuCloudConTable.wifiConTry++;
	if (ret == SUCCESS){
		zHcuCloudConTable.wifiConTry = 0;
		zHcuCloudConTable.curCon =CLOUD_CONTROL_PHY_CON_WIFI;
		return SUCCESS;
	}

	//第四优先级：连接3G4G
	if (zHcuHwinvTable.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
		ret = hcu_3g4g_phy_link_setup();

	}
	if (ret == FAILURE) zHcuCloudConTable.g3g4ConTry++;
	if (ret == SUCCESS){
		zHcuCloudConTable.g3g4ConTry = 0;
		zHcuCloudConTable.curCon =CLOUD_CONTROL_PHY_CON_3G4G;
		return SUCCESS;
	}
	/*
	//保留用作测试目的
	if ((rand()%10)>5)
		return SUCCESS;
	else*/
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: No CLOUD-BH physical link hardware available or not setup successful!\n");
	#endif //TRACE_DEBUG_ON
	return FAILURE;
}

//进入离线，具备两种方式：被动心跳检测到离线状态，主动断掉链路从而连到更高优先级的链路上
//这两种情形下，都需要复位ConTable对应的状态，不然可能会导致物理接口模块处于不正常的接收状态
//检测后台连接的有效性，是否还依然保持连接状态
//心跳握手检测，搞不好就是一种新状态，这里得想办法做成一个立即返回的过程，不然状态机太复杂，上面的控制程序也不好处理
//这里的发送接受，只是有SAE-CLOUD的反馈就算成功，如果在消息解码中再遇到问题，可以重新设置本模块的状态，比如强行进入离线等等
//所以这里返回成功，以及相应的处理，只能是不完善的，因为发送和接收过程分离的，但99%情况下，这种机制是可以正常工作的
OPSTAT func_cloudcont_heart_beat_check(void)
{
	int ret = 0;

	//如果这里的链路已经被动断掉，必须将下面的数据清理掉，不然会出现下层物理接口模块还在接收的状态
	//zHcuCloudConTable.curCon = CLOUD_CONTROL_PHY_CON_INVALID;

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUD) == FSM_STATE_CLOUDCONT_ONLINE){
		//初始化变量
		CloudDataSendBuf_t buf;
		memset(&buf, 0, sizeof(CloudDataSendBuf_t));

		//打包数据
		if (func_cloudcont_huanbao_heart_beat_msg_pack(&buf) == FAILURE){
			HcuErrorPrint("CLOUD: Package message error!\n");
			return FAILURE;
		}

		//Send out
		ret = func_cloudcont_send_data_to_cloud(&buf);
		if ( ret == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}else{
		HcuErrorPrint("CLOUD: Error send HEART_BEAT to cloud, get by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//结束
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Online state, send HEART_BEAT message out to cloud success!\n");
	#endif //TRACE_DEBUG_ON
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
		HcuErrorPrint("CLOUD: Error input of data save to memory, on Sensor type or Time stamp!\n");
		return FAILURE;
	}
	if (((record->onOffLine) != DISC_DATA_SAMPLE_ONLINE) && ((record->onOffLine) != DISC_DATA_SAMPLE_OFFLINE)){
		HcuErrorPrint("CLOUD: Error input of data save to memory, on on/off line attribute!\n");
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
	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Data record save to MEM-DISC, sid=%d, totalNbr=%d, offNbr=%d\n", sid, totalNbr, zHcuMemStorageBuf.offlineNbr);
	#endif //TRACE_DEBUG_ON
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
				HcuErrorPrint("CLOUD: Error occurs during totalNbr/offlineNbr control MEM STORAGE, recover!\n");
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
		HcuDebugPrint("CLOUD: Data read nothing, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
		return FAILURE;
	}

	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Data record read from MEM-DISC, rdCnt=%d, totalNbr = %d, offNbr=%d\n", readCnt, totalNbr, zHcuMemStorageBuf.offlineNbr);
	#endif //TRACE_DEBUG_ON
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


//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
//INT8 tplFormat[] = "<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>"; //send data for debug
OPSTAT func_cloudcont_huanbao_heart_beat_msg_pack(CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", L3CI_heart_beat & 0xFF);
		strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: HEART_BEAT Frame XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		//未来HEART_BEAT帧可能会定义独特的命令类型（char cn[8]; //CN=命令编号），现在暂时没有细节，所以采用数据长度=0表示心跳帧
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_HEART_BEAT);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: Heart Beat Frame ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
//INT8 tplFormat[] = "<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>"; //send data for debug
OPSTAT func_cloudcont_huanbao_emc_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 emcValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conEmc, "%04X", emcValue & 0xFFFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: EMC XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conEmc, "%04X", emcValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: EMC ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudcont_huanbao_pm25_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 pm1d0Value, UINT32 pm2d5Value, UINT32 pm10Value, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conPm1d0, "%08X", pm1d0Value);
		sprintf(xmlFormat.conPm2d5, "%08X", pm2d5Value);
		sprintf(xmlFormat.conPm10d, "%08X", pm10Value);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: PM25 XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conPm2d5, "%08X", pm2d5Value);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: PM25 ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudcont_huanbao_winddir_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 winddirValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conWinddir, "%04X", winddirValue  & 0xFFFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: WINDDIR XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conWinddir, "%04X", winddirValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: WINDDIR ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudcont_huanbao_windspd_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 windspdValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conWindspd, "%04X", windspdValue & 0xFFFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: WINDSPD XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conWindspd, "%04X", windspdValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: WINDSPD ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}


	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudcont_huanbao_temp_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 tempValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conTemp, "%04X", tempValue & 0xFFFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: TEMP XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conTemp, "%04X", tempValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: TEMP ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudcont_huanbao_humid_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 humidValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conHumid, "%04X", humidValue & 0xFFFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUD_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUD_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUD: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudcont_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: HUMID XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conHumid, "%04X", humidValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudcont_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUD: Pack message error!\n");
			return FAILURE;
		}
		#ifdef TRACE_DEBUG_ON
		HcuDebugPrint("CLOUD: Humid ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		#endif //TRACE_DEBUG_ON
	}

	else{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//XML自定义标准的编码函数方式
//完全自己手动编码的方式，未来可以灵活的改动
//也可以考虑使用XML的标准函数来做，但其实最大的内容部分，也是靠手工编码的，并没有省略多少复杂度
OPSTAT func_cloudcont_standard_xml_pack(CloudBhItfDevReportStdXml_t *xmlFormat, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (xmlFormat == NULL){
		HcuErrorPrint("CLOUD: Error CloudBhItfDevReportStdXml_t pointer!\n");
		return FAILURE;
	}
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	//格式固定区域
	strcpy(xmlFormat->xml_l, "<xml>");
	strcpy(xmlFormat->xml_r, "</xml>");
	strcpy(xmlFormat->ToUserName_l, "<ToUserName><![CDATA[");
	strcpy(xmlFormat->ToUserName_r, "]]></ToUserName>");
	strcpy(xmlFormat->FromUserName_l, "<FromUserName><![CDATA[");
	strcpy(xmlFormat->FromUserName_r, "]]></FromUserName>");
	strcpy(xmlFormat->CreateTime_l, "<CreateTime>");
	strcpy(xmlFormat->CreateTime_r, "</CreateTime>");
	strcpy(xmlFormat->MsgType_l, "<MsgType><![CDATA[");
	strcpy(xmlFormat->MsgType_r, "]]></MsgType>");
	strcpy(xmlFormat->Content_l, "<Content><![CDATA[");
	strcpy(xmlFormat->Content_r, "]]></Content>");
	strcpy(xmlFormat->FuncFlag_l, "<FuncFlag>");
	strcpy(xmlFormat->FuncFlag_r, "</FuncFlag>");

	//对于目前来说，数值固定内容
	strcpy(xmlFormat->ToUserName, CLOUD_BH_SERVER_NAME);
	strcpy(xmlFormat->FromUserName, CLOUD_BH_HCU_NAME);
	strcpy(xmlFormat->CreateTime, zCurTimeDate.sMin);
	//MsgType参数，必须由调用函数填入，因为它才能知晓这是什么样的内容体
	//strcpy(xmlFormat->MsgType, CLOUD_BH_MSG_TYPE_DEVICE_REPORT);
	sprintf(xmlFormat->FuncFlag, "%1d", 0);

	//准备接龙字符串成为一整串
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char da[MAX_HCU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));
	char tmp[3] = "";

	//固定头部分
	strcat(s, xmlFormat->xml_l);
	strcat(s, xmlFormat->ToUserName_l);
	strcat(s, xmlFormat->ToUserName);
	strcat(s, xmlFormat->ToUserName_r);
	strcat(s, xmlFormat->FromUserName_l);
	strcat(s, xmlFormat->FromUserName);
	strcat(s, xmlFormat->FromUserName_r);
	strcat(s, xmlFormat->CreateTime_l);
	strcat(s, xmlFormat->CreateTime);
	strcat(s, xmlFormat->CreateTime_r);
	strcat(s, xmlFormat->MsgType_l);
	strcat(s, xmlFormat->MsgType);
	strcat(s, xmlFormat->MsgType_r);
	strcat(s, xmlFormat->Content_l);

	//顺序是编码的黄金规则，千万不能错，否则就无法解开了!!!
	//char conCmdId[3];
	strcat(s, xmlFormat->conCmdId);

	//所有变长部分
	//char conOptId[3]; //1B
	strcat(da, xmlFormat->conOptId);

	//char conBackType[3]; //1B
	strcat(da, xmlFormat->conBackType);

	//char conEqpId[3];  //1B
	strcat(da, xmlFormat->conEqpId);

	//char conEmc[5];   //2B
	strcat(da, xmlFormat->conEmc);

	//char conPm1d0[9];   //4B
	strcat(da, xmlFormat->conPm1d0);

	//char conPm2d5[9];   //4B
	strcat(da, xmlFormat->conPm2d5);

	//char conPm10d[9];   //4B
	strcat(s, xmlFormat->conPm10d);

	//char conWinddir[5];   //2B
	strcat(da, xmlFormat->conWinddir);

	//char conWindspd[5];   //2B
	strcat(da, xmlFormat->conWindspd);

	//char conTemp[5];   //2B
	strcat(da, xmlFormat->conTemp);

	//char conHumid[5];   //2B
	strcat(da, xmlFormat->conHumid);

	//char conGpsx[9];   //4B
	strcat(da, xmlFormat->conGpsx);

	//char conGpsy[9];   //4B
	strcat(da, xmlFormat->conGpsy);

	//char conGpsz[9];   //4B
	strcat(da, xmlFormat->conGpsz);

	//char conTimeStamp[9]; //4B
	strcat(da, xmlFormat->conTimeStamp);

	//char conNtimes[5];   //2B
	strcat(da, xmlFormat->conNtimes);

	//获取变长部分的长度, Len=0的情况存在，比如Heart_Beat消息，这里为了统一处理函数的简化，不做过分的区别对待和处理，尽量让处理函数通用化
	int len = 0;
	len = strlen(da);
	if ((len < 0) || ((len % 2) != 0) || (len > MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: No data to be pack or too long length of data content!!!\n");
		return FAILURE;
	}
	len = len / 2;  //字节长度，而非字符串长度

	//如果长度=0,则正好，包含一个长度域=0的东东，非常好！省得底层收到的长度=1的HEART_BEAT消息
	//char conLen[3];  //1B
	sprintf(tmp, "%02X", len & 0xFF);
	strcat(s, tmp);

	//变长部分
	strcat(s, da);

	//Finish content part
	strcat(s, xmlFormat->Content_r);

	//固定尾部分
	strcat(s, xmlFormat->FuncFlag_l);
	strcat(s, xmlFormat->FuncFlag);
	strcat(s, xmlFormat->FuncFlag_r);
	strcat(s, xmlFormat->xml_r);

	//存入返回参量中
	strcpy(buf->curBuf, s);
	buf->curLen = strlen(s) + 4;
	return SUCCESS;
}

OPSTAT func_cloudcont_standard_zhb_pack(CloudBhItfDevReportStdZhb_t *zhbFormat, CloudDataSendBuf_t *buf)
{
	int i=0;

	//参数检查，其它参数无所谓
	if (zhbFormat == NULL){
		HcuErrorPrint("CLOUD: Error CloudBhItfDevReportStdXml_t pointer!\n");
		return FAILURE;
	}
	if (buf == NULL){
		HcuErrorPrint("CLOUD: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	//获取整体长度
	int len = 0;
	len = strlen(zhbFormat->data.conAirprs) + strlen(zhbFormat->data.conEmc) + strlen(zhbFormat->data.conHumid) + strlen(zhbFormat->data.conPm10d)\
			+ strlen(zhbFormat->data.conPm10d) + strlen(zhbFormat->data.conPm2d5) + strlen(zhbFormat->data.conTemp) + strlen(zhbFormat->data.conWinddir)\
			+ strlen(zhbFormat->data.conWindspd);
	if ((len < 0) || ((len % 2) != 0) || (len > MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: No data to be pack or too long length of data content!!!\n");
		return FAILURE;
	}

	//Len =0 就表示HEART_BEAT消息

	//准备接龙字符串成为一整串
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char tmp[CLOUD_BH_ITF_STD_ZHB_DATA_SEGMENT_MAX_LENGTH];

	//char qn[21]; //请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	memset(tmp, 0, sizeof(tmp));
	time_t lt;
	struct tm *cu;
	lt=time(NULL);
	cu = localtime(&lt);
	sprintf(tmp,  "%04d%02d%02d%02d%02d%02d%03d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday,\
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec, 0);
	strcat(tmp, "___"); //变成固定20个字符长度
	strcat(s, tmp);

	//char st[6]; //ST=系统编号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%05d", 11111);
	strcat(s, tmp);

	//char cn[8]; //CN=命令编号
	//命令编号，必然从上面带下来，故而这里不再做拷贝
	//先看理想情况
	if ((zhbFormat->data.cn[0] != '\0') && (strlen(zhbFormat->data.cn) == CLOUD_BH_ITF_STD_ZHB_CN_FIX_LENGTH)){
		strcat(s, zhbFormat->data.cn);
	}
	//再看长度小了，补"_"符号
	else if ((zhbFormat->data.cn[0] != '\0') && (strlen(zhbFormat->data.cn) < CLOUD_BH_ITF_STD_ZHB_CN_FIX_LENGTH)){
		int i=0, j=0;
		i = strlen(zhbFormat->data.cn);
		for (j=i; j<CLOUD_BH_ITF_STD_ZHB_CN_FIX_LENGTH; j++){
			strcat(zhbFormat->data.cn, "_");
		}
		strcat(s, zhbFormat->data.cn);
	}
	//最后是空的
	else{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%07d", 2222222);
		strcat(s, tmp);
	}

	//char mn[13]; //MN=设备编号
	strcpy(tmp, CLOUD_BH_HCU_NAME);
	if (strlen(tmp) <12){
		for (i = strlen(tmp); i<12; i++)
			strcpy(&tmp[i], "_");
	}
	strcpy(&tmp[12], "\0");
	strcat(s, tmp);

	//char pw[7]; //PW=访问密码
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%06d", 444444);
	strcat(s, tmp);

	//char pnum[5]; //HCU->SAE only, PNUM 指示本次通讯总共包含的包数
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04d", 555);
	strcat(s, tmp);

	//char pno[5]; //HCU->SAE only, PNO 指示当前数据包的包号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04d", 6666);
	strcat(s, tmp);

	char da[MAX_HCU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));

	//数据区域, 气压
	if (strlen(zhbFormat->data.conAirprs) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_AIRPRS);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conAirprs);
		strcat(da, ",");
	}

	//数据区域, EMC
	if (strlen(zhbFormat->data.conEmc) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_EMC);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conEmc);
		strcat(da, ",");
	}

	//数据区域, PM25 => 有关多种PMxy组合成总颗粒物数量，待完善，这个数据的长度为4/UINT32个字节，注意不要弄的不一样
	if (strlen(zhbFormat->data.conPm2d5) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_PM25);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conPm2d5);
		strcat(da, ",");
	}

	//数据区域, WINDDIR
	if (strlen(zhbFormat->data.conWinddir) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_WINDDIR);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conWinddir);
		strcat(da, ",");
	}

	//数据区域, WINDSPD
	if (strlen(zhbFormat->data.conWindspd) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_WINDSPD);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conWindspd);
		strcat(da, ",");
	}

	//数据区域, TEMP
	if (strlen(zhbFormat->data.conTemp) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_TEMP);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conTemp);
		strcat(da, ",");
	}

	//数据区域, HUMID
	if (strlen(zhbFormat->data.conHumid) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_HUMID);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conHumid);
		strcat(da, ",");
	}

	//数据区域, NOISY
	if (strlen(zhbFormat->data.conNoisy) > 0){
		strcat(da, CLOUD_BH_ITF_STD_ZHB_NOISY);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conNoisy);
		strcat(da, ",");
	}

	//纯粹项目内容的数据长度区域，暂时没有不同项目分界线，不然需要使用“;”进行分开表征
	i = strlen(da);
	if ((i<0) || (i> (MAX_HCU_MSG_BUF_LENGTH - strlen(s)) )){
		HcuErrorPrint("CLOUD: Too much data to be packed, exceed system limitation!!!\n");
		return FAILURE;
	}

	//继续接上数据去
	strcat(s, da);
	if(strlen(s) > MAX_HCU_MSG_BUF_LENGTH-12){
		HcuErrorPrint("CLOUD: Too much data to be packed, exceed system limitation!!!\n");
		return FAILURE;
	}

	//固定头部分
	strcpy(zhbFormat->zhb_header, "##");
	strcpy(zhbFormat->zhb_tail, "\r\n");  //<CR><LF>, 	回车（CR 或 ASCII 字符 0x0D（13）） 换行（LF 或 ASCII 字符 0x0A（10））
	sprintf(zhbFormat->dataLen, "%04d", strlen(s));

	//CRC计算，字符串当UINT8做CRC计算，可能会有潜在的风险，未来待检查
	UINT16 crc16=0;
	CheckCRCModBus(s, strlen(s), &crc16);
	sprintf(zhbFormat->crc16, "%04X", crc16);

	//全部成帧
	strcpy(buf->curBuf, zhbFormat->zhb_header);
	strcat(buf->curBuf, zhbFormat->dataLen);
	strcat(buf->curBuf, s);
	strcat(buf->curBuf, zhbFormat->crc16);
	strcat(buf->curBuf, zhbFormat->zhb_tail);
	buf->curLen = strlen(buf->curBuf) + 4;
	if(buf->curLen > MAX_HCU_MSG_BUF_LENGTH){
		HcuErrorPrint("CLOUD: Too much data to be packed, exceed system limitation!!!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//Send to backhawl cloud
OPSTAT func_cloudcont_send_data_to_cloud(CloudDataSendBuf_t *buf)
{
	//参数检查
	if ((buf->curLen <=0) || (buf->curLen >MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error message length to send back for cloud!\n");
		return FAILURE;
	}

	//if (zHcuCloudCurlPtrTx == NULL){
	//	HcuErrorPrint("CLOUD: Physical connection still not yet setup!\n");
	//	return FAILURE;
	//}

	//if (zHcuCloudCurlPtrRx == NULL){
	//	HcuErrorPrint("CLOUD: Physical connection still not yet setup!\n");
	//	return FAILURE;
	//}

	//根据系统配置，决定使用那一种后台网络
	if (zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_ETHERNET){
		if (hcu_ethernet_date_send(buf) == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_USBNET){
		if (hcu_usbnet_data_send(buf) == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_WIFI){
		if (hcu_wifi_data_send(buf) == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else if (zHcuCloudConTable.curCon == CLOUD_CONTROL_PHY_CON_3G4G){
		if (hcu_3g4g_data_send(buf) == FAILURE){
			HcuErrorPrint("CLOUD: Error send data to back-cloud!\n");
			return FAILURE;
		}
	}
	else {
		HcuErrorPrint("CLOUD: Error status of physical layer link for data send!\n");
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
//发现在接收数据的处理中，使用的是完全相同的msg_struct_com_cloud_data_rx_t，故而采用完全相同的接收处理函数
//如果未来不同的物理接口出现变化，必须需要不同的处理，再行改变
OPSTAT fsm_cloudcont_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//参数检查
	if ((param_len <=0) || (param_len >MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("CLOUD: Error message length received from [%s] module!\n", zHcuTaskNameList[src_id]);
		return FAILURE;
	}

	if (param_ptr == NULL){
		HcuErrorPrint("CLOUD: Receive NULL pointer data from [%s] module!\n", zHcuTaskNameList[src_id]);
		return FAILURE;
	}

	//连接态
	if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_ONLINE) == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State!\n");
		return FAILURE;
	}

	//接收消息解码
	msg_struct_com_cloud_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_cloud_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_cloud_data_rx_t))){
		HcuErrorPrint("CLOUD: Receive message error from [%s] module!\n", zHcuTaskNameList[src_id]);
		return FAILURE;
	}
	memcpy(rcv.buf, param_ptr, param_len);
	rcv.length = param_len;

	#ifdef TRACE_DEBUG_ON
	//HcuDebugPrint("CLOUD: Receive data length = %d, data buffer = %c%c%c%c%c%c%c%c\n\n", rcv.length,  rcv.buf[0], rcv.buf[1], rcv.buf[2], rcv.buf[3], rcv.buf[4], rcv.buf[5], rcv.buf[6], rcv.buf[7]);
	HcuDebugPrint("CLOUD: Receive data len=%d, data buffer = [%s], from [%s] module\n", rcv.length,  rcv.buf, zHcuTaskNameList[src_id]);
	#endif //TRACE_DEBUG_ON

	//如果是XML自定义格式
	if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_XML)
	{
		if (func_cloudcont_standard_xml_unpack(&rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Unpack receive message error from [%s] module!\n", zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//如果是ZHB格式
	else if (CLOUD_BH_INTERFACE_STANDARD == CLOUD_BH_INTERFACE_STANDARD_ZHB)
	{
		if (func_cloudcont_standard_zhb_unpack(&rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Unpack receive message error from [%s] module!\n", zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//非法格式
	else
	{
		HcuErrorPrint("CLOUD: Not set CLOUD_BH_INTERFACE_STANDARD rightly!\n");
		return FAILURE;
	}


	return SUCCESS;
}

OPSTAT fsm_cloudcont_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_ONLINE) == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudcont_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_ONLINE) == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudcont_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//连接态
	if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_ONLINE) == FAILURE){
		HcuErrorPrint("CLOUD: Error Set FSM State!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudcont_standard_xml_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=0, cmdId=0;
	//检查参数
	if (rcv == NULL){
		HcuErrorPrint("CLOUD: Invalid received data buffer!\n");
		return FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if ((rcv->length %2) != 0){
		HcuErrorPrint("CLOUD: Received invalid data length by XML content format!\n");
		return FAILURE;
	}

	//获取控制CMDID
	char tmp[3] = "";
	strncpy(tmp, &rcv->buf[index], 2);
	cmdId = strtoul(tmp, NULL, 16);

	if (cmdId ==L3CI_heart_beat){
		if (func_cloudcont_standard_xml_heart_beat_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
		//else{
		//	#ifdef TRACE_DEBUG_ON
		//	HcuDebugPrint("CLOUD: Ack HEART_BEAT successful!\n");
		//	#endif //TRACE_DEBUG_ON
		//}
	}

	else if (cmdId ==L3CI_emc){
		if (func_cloudcont_standard_xml_emc_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else if (cmdId ==L3CI_pm25){
		if (func_cloudcont_standard_xml_pm25_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else if (cmdId ==L3CI_winddir){
		if (func_cloudcont_standard_xml_winddir_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else if (cmdId ==L3CI_windspd){
		if (func_cloudcont_standard_xml_windspd_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else if (cmdId ==L3CI_humid){
		if (func_cloudcont_standard_xml_humid_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else if (cmdId ==L3CI_temp){
		if (func_cloudcont_standard_xml_temp_msg_unpack(rcv) == FAILURE){
			HcuErrorPrint("CLOUD: Error unpack receiving message!\n");
			return FAILURE;
		}
	}

	else{
		HcuErrorPrint("CLOUD: Receive cloud data error with CmdId = %d\n", cmdId);
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT func_cloudcont_standard_xml_heart_beat_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0;  //, cmdId=0
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	//cmdId = L3CI_heart_beat;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<0) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length, received length != embeded length!\n");
		return FAILURE;
	}

	//没有了其它的字段，所以长度=0就对了
	if (len != 0){
		HcuErrorPrint("CLOUD: Error unpack on length, HearT_beat frame, Len!=0!\n");
		return FAILURE;
	}

	//暂时没有层2的帧号，所以不用对帧号进行处理

	//也许会有其它潜在的状态转移困惑，所以这里的ONLINE状态只是为了做一定的检查，防止出现各种奇怪现象，而不是为了设置状态
	if (func_cloudcont_heart_beat_received_handle() == FAILURE){
		HcuErrorPrint("CLOUD: Heart_beat processing error!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudcont_standard_xml_emc_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_emc;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_emc_min) || (it >= L3PO_emc_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_emc_data_req) || (optId == L3PO_emc_data_report)){

		//初始化发送函数
		msg_struct_cloud_emc_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_emc_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		//如果想测试，可以先把这里的严格检查放开，以方便测试
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给EMC任务
		snd.length = sizeof(msg_struct_cloud_emc_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_EMC_DATA_REQ, TASK_ID_EMC, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_EMC]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_emc_set_switch) || (optId == L3PO_emc_set_modbus_address) || (optId == L3PO_emc_set_work_cycle) || \
			(optId == L3PO_emc_set_sample_cycle) || (optId == L3PO_emc_set_sample_number) || (optId == L3PO_emc_read_switch) || \
			(optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) || (optId == L3PO_emc_read_sample_cycle)\
			|| (optId == L3PO_emc_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_emc_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_emc_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_emc_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_emc_read_switch) || (optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) ||\
				(optId == L3PO_emc_read_sample_cycle) || (optId == L3PO_emc_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给EMC任务
		snd1.length = sizeof(msg_struct_cloud_emc_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_EMC_CONTROL_CMD, TASK_ID_EMC, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_EMC]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudcont_standard_xml_pm25_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_pm25;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_pm25_min) || (it >= L3PO_pm25_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_pm25_data_req) || (optId == L3PO_pm25_data_report)){

		//初始化发送函数
		msg_struct_cloud_pm25_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_pm25_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给PM25任务
		snd.length = sizeof(msg_struct_cloud_pm25_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_PM25_DATA_REQ, TASK_ID_PM25, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_PM25]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_pm25_set_switch) || (optId == L3PO_pm25_set_modbus_address) || (optId == L3PO_pm25_set_work_cycle) || \
			(optId == L3PO_pm25_set_sample_cycle) || (optId == L3PO_pm25_set_sample_number) || (optId == L3PO_pm25_read_switch) || \
			(optId == L3PO_pm25_read_modbus_address) || (optId == L3PO_pm25_read_work_cycle) || (optId == L3PO_pm25_read_sample_cycle)\
			|| (optId == L3PO_pm25_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_pm25_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_pm25_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_pm25_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_pm25_read_switch) || (optId == L3PO_pm25_read_modbus_address) || (optId == L3PO_pm25_read_work_cycle) ||\
				(optId == L3PO_pm25_read_sample_cycle) || (optId == L3PO_pm25_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给PM25任务
		snd1.length = sizeof(msg_struct_cloud_pm25_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_PM25_CONTROL_CMD, TASK_ID_PM25, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_PM25]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT func_cloudcont_standard_xml_winddir_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_winddir;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_winddir_min) || (it >= L3PO_winddir_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_winddir_data_req) || (optId == L3PO_winddir_data_report)){

		//初始化发送函数
		msg_struct_cloud_winddir_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_winddir_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给WINDDIR任务
		snd.length = sizeof(msg_struct_cloud_winddir_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_WINDDIR_DATA_REQ, TASK_ID_WINDDIR, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDDIR]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_winddir_set_switch) || (optId == L3PO_winddir_set_modbus_address) || (optId == L3PO_winddir_set_work_cycle) || \
			(optId == L3PO_winddir_set_sample_cycle) || (optId == L3PO_winddir_set_sample_number) || (optId == L3PO_winddir_read_switch) || \
			(optId == L3PO_winddir_read_modbus_address) || (optId == L3PO_winddir_read_work_cycle) || (optId == L3PO_winddir_read_sample_cycle)\
			|| (optId == L3PO_winddir_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_winddir_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_winddir_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_winddir_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_winddir_read_switch) || (optId == L3PO_winddir_read_modbus_address) || (optId == L3PO_winddir_read_work_cycle) ||\
				(optId == L3PO_winddir_read_sample_cycle) || (optId == L3PO_winddir_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给WINDDIR任务
		snd1.length = sizeof(msg_struct_cloud_winddir_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_WINDDIR_CONTROL_CMD, TASK_ID_WINDDIR, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDDIR]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}


OPSTAT func_cloudcont_standard_xml_windspd_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_windspd;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_windspd_min) || (it >= L3PO_windspd_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_windspd_data_req) || (optId == L3PO_windspd_data_report)){

		//初始化发送函数
		msg_struct_cloud_windspd_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_windspd_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给WINDSPD任务
		snd.length = sizeof(msg_struct_cloud_windspd_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_WINDSPD_DATA_REQ, TASK_ID_WINDSPD, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDSPD]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_windspd_set_switch) || (optId == L3PO_windspd_set_modbus_address) || (optId == L3PO_windspd_set_work_cycle) || \
			(optId == L3PO_windspd_set_sample_cycle) || (optId == L3PO_windspd_set_sample_number) || (optId == L3PO_windspd_read_switch) || \
			(optId == L3PO_windspd_read_modbus_address) || (optId == L3PO_windspd_read_work_cycle) || (optId == L3PO_windspd_read_sample_cycle)\
			|| (optId == L3PO_windspd_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_windspd_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_windspd_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_windspd_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_windspd_read_switch) || (optId == L3PO_windspd_read_modbus_address) || (optId == L3PO_windspd_read_work_cycle) ||\
				(optId == L3PO_windspd_read_sample_cycle) || (optId == L3PO_windspd_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给WINDSPD任务
		snd1.length = sizeof(msg_struct_cloud_windspd_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_WINDSPD_CONTROL_CMD, TASK_ID_WINDSPD, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDSPD]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}


OPSTAT func_cloudcont_standard_xml_temp_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_temp;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_temp_min) || (it >= L3PO_temp_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_temp_data_req) || (optId == L3PO_temp_data_report)){

		//初始化发送函数
		msg_struct_cloud_temp_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_temp_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给TEMP任务
		snd.length = sizeof(msg_struct_cloud_temp_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_TEMP_DATA_REQ, TASK_ID_TEMP, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_TEMP]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_temp_set_switch) || (optId == L3PO_temp_set_modbus_address) || (optId == L3PO_temp_set_work_cycle) || \
			(optId == L3PO_temp_set_sample_cycle) || (optId == L3PO_temp_set_sample_number) || (optId == L3PO_temp_read_switch) || \
			(optId == L3PO_temp_read_modbus_address) || (optId == L3PO_temp_read_work_cycle) || (optId == L3PO_temp_read_sample_cycle)\
			|| (optId == L3PO_temp_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_temp_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_temp_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_temp_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_temp_read_switch) || (optId == L3PO_temp_read_modbus_address) || (optId == L3PO_temp_read_work_cycle) ||\
				(optId == L3PO_temp_read_sample_cycle) || (optId == L3PO_temp_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给TEMP任务
		snd1.length = sizeof(msg_struct_cloud_temp_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_TEMP_CONTROL_CMD, TASK_ID_TEMP, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_TEMP]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT func_cloudcont_standard_xml_humid_msg_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[CLOUD_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_humid;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUD: Error unpack on length!\n");
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_humid_min) || (it >= L3PO_humid_max)){
		HcuErrorPrint("CLOUD: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUD: Error unpack on equipment Id!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_humid_data_req) || (optId == L3PO_humid_data_report)){

		//初始化发送函数
		msg_struct_cloud_humid_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_humid_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUD: Error unpack on message length!\n");
			return FAILURE;
		}

		//将消息命令发送给HUMID任务
		snd.length = sizeof(msg_struct_cloud_humid_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUD_HUMID_DATA_REQ, TASK_ID_HUMID, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_HUMID]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_humid_set_switch) || (optId == L3PO_humid_set_modbus_address) || (optId == L3PO_humid_set_work_cycle) || \
			(optId == L3PO_humid_set_sample_cycle) || (optId == L3PO_humid_set_sample_number) || (optId == L3PO_humid_read_switch) || \
			(optId == L3PO_humid_read_modbus_address) || (optId == L3PO_humid_read_work_cycle) || (optId == L3PO_humid_read_sample_cycle)\
			|| (optId == L3PO_humid_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloud_humid_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_humid_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.equId = equId;

		if (optId == L3PO_humid_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		else if ((optId == L3PO_humid_read_switch) || (optId == L3PO_humid_read_modbus_address) || (optId == L3PO_humid_read_work_cycle) ||\
				(optId == L3PO_humid_read_sample_cycle) || (optId == L3PO_humid_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUD: Error unpack on message length!\n");
				return FAILURE;
			}
		}

		//将消息命令发送给HUMID任务
		snd1.length = sizeof(msg_struct_cloud_humid_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUD_HUMID_CONTROL_CMD, TASK_ID_HUMID, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_HUMID]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		HcuErrorPrint("CLOUD: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}

/*
 *  有关后台针对环保协议的几个关键点：
 *  1. 该协议的标准格式终结在本模块中
 *  2. 与该协议相关的独特字段处理，均留给业务模块来完成
 *  3. 到底是应该采用通用的消息来处理不同格式的协议，还是应该使用单独的协议来进行，这是个值得探讨的问题
 *     一般来说，如果两个协议比较接近，可以使用共享协议，否则应该分开做，以解耦他们之间的关联
 *  4. 本次，自定义XML格式和中环保协议采用共享通用消息方式
 *  5. 业务处理以及跟外设的完整过程，需要再完善
 *  6. 有关ZHB的字段内容与自定义XML的融合，需要等待ZHB协议内容的再完善
 *
 */

OPSTAT func_cloudcont_standard_zhb_unpack(msg_struct_com_cloud_data_rx_t *rcv)
{
	UINT32 index=0;
	UINT32 totalLen=0, it=0, ret=0;
	char st[CLOUD_BH_ITF_STD_ZHB_DATA_SEGMENT_MAX_LENGTH] = "";

	//检查参数 (固定包头为12个字符串)
	if ((rcv == NULL) ||(rcv->length<=12) ||(rcv->length>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUD: Invalid received data buffer or length!\n");
		return FAILURE;
	}

	//取头部
	if ((rcv->buf[index] !='#')||(rcv->buf[index+1] !='#')){
		HcuErrorPrint("CLOUD: Invalid received data header!\n");
		return FAILURE;
	}
	index = index+2;

	//取尾巴
	if ((rcv->buf[rcv->length-2] !='\r')||(rcv->buf[rcv->length-2] !='\n')){
		HcuErrorPrint("CLOUD: Invalid received data tail!\n");
		return FAILURE;
	}

	//取长度
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 4);
	it = strtoul(st, NULL, 10); //10进制
	index = index + 4;
	totalLen = rcv->length-12;  //12字符长度的固定包头
	if (it != totalLen){
		HcuErrorPrint("CLOUD: Invalid received data length!\n");
		return FAILURE;
	}

	//取CRC并做比较判断
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[rcv->length-6], 4);
	it = strtoul(st, NULL, 16);  //16进制
	UINT16 crc16=0;
	CheckCRCModBus(&rcv->buf[index], rcv->length-12, &crc16);
	if (it != crc16){
		HcuErrorPrint("CLOUD: Invalid received data CRC16!\n");
		return FAILURE;
	}

	//初始化临时参数
	char   iqn[21]; //中环保标准，20c, QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求, 用于请求命令或通知命令
	char   ist[6];  //中环保标准，5c, ST=系统编号
	char   icn[8];  //中环保标准，7c, 命令编号 CN
	char   ipw[7];  //中环保标准， 6c, PW=访问密码
	char   imn[13]; //中环保标准， 12c, 设备唯一标识MN
	UINT8  iansFlag;//中环保标准， 3c, 数据包是否拆分及应答标志 Flag，从云后台的接收方向
	UINT8 flag = 0;

	//取数据区的请求编码
	if (totalLen <20){
		HcuErrorPrint("CLOUD: Invalid received data on QN!\n");
		return FAILURE;
	}
	memset(iqn, 0, sizeof(iqn));
	strncpy(iqn, &rcv->buf[index], 20);
	index = index+20;
	totalLen = totalLen - 20;

	//取数据区的系统编号
	if (totalLen <5){
		HcuErrorPrint("CLOUD: Invalid received data on QN!\n");
		return FAILURE;
	}
	memset(ist, 0, sizeof(ist));
	strncpy(ist, &rcv->buf[index], 5);
	index = index+5;
	totalLen = totalLen - 5;

	//取数据区的命令编号
	if (totalLen <7){
		HcuErrorPrint("CLOUD: Invalid received data on CN!\n");
		return FAILURE;
	}
	memset(icn, 0, sizeof(icn));
	strncpy(icn, &rcv->buf[index], 7);
	index = index+7;
	totalLen = totalLen - 7;
	if (strcmp(icn, CLOUD_BH_ITF_STD_ZHB_CN_HEART_BEAT) == 0){
		flag = 1;
	}else if (strcmp(icn, CLOUD_BH_ITF_STD_ZHB_CN_NORMAL_PKG) == 0){
		flag = 2;
	}else{
		HcuErrorPrint("CLOUD: Invalid received data on CN!\n");
		return FAILURE;
	}

	//取数据区的访问密码
	if (totalLen <6){
		HcuErrorPrint("CLOUD: Invalid received data on PW!\n");
		return FAILURE;
	}
	memset(ipw, 0, sizeof(ipw));
	strncpy(ipw, &rcv->buf[index], 6);
	index = index+6;
	totalLen = totalLen - 6;

	//取数据区的设备唯一标识MN
	if (totalLen <12){
		HcuErrorPrint("CLOUD: Invalid received data on MN!\n");
		return FAILURE;
	}
	memset(imn, 0, sizeof(imn));
	strncpy(imn, &rcv->buf[index], 6);
	index = index+12;
	totalLen = totalLen - 12;

	//取数据区的数据包是否拆分及应答标志 Flag
	if (totalLen <3){
		HcuErrorPrint("CLOUD: Invalid received data on MN!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 3);
	it = strtoul(st, NULL, 16);
	iansFlag = it & 0x03;  //只有最后的两个比特Bit0/1在使用，其它的未用
	index = index+3;
	totalLen = totalLen - 3;

	//长度=0,表示隐含的心跳检测答复帧
	if (totalLen == 0){
		//心跳接收的特殊处理
		if (flag == 1){
			if (func_cloudcont_heart_beat_received_handle() == FAILURE){
				HcuErrorPrint("CLOUD: Heart_beat processing error!\n");
				return FAILURE;
			}
		}
		else{
			HcuErrorPrint("CLOUD: Heart Beat type but with not-zero length of data!\n");
			return FAILURE;
		}
		return SUCCESS;
	}

	//数据去CP的格式暂时不是特别清晰，等待更为完善的规范
	//先假设每次都只有一个参数进来，例子如下
	//CP=&&数据区&&， 数据区是采用发送一样的编码方式，只有一个传感器类型，将命令参数发往传感器L3模块就为胜利
	if (totalLen <10){
		HcuErrorPrint("CLOUD: Invalid received data on CP command length!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	index = index+2;
	totalLen = totalLen - 2;
	if (strcmp(st, "&&") != 0){
		HcuErrorPrint("CLOUD: Invalid received data on CP command prefix!\n");
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index+6], 2);
	totalLen = totalLen -2;
	if (strcmp(st, "&&") != 0){
		HcuErrorPrint("CLOUD: Invalid received data on CP command pigback!\n");
		return FAILURE;
	}

	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 6);
	index = index + 8; //为下一个参数
	totalLen = totalLen -6;

	//先发送控制消息给传感器任务模块，再判断是否还有后续参数

	//EMC传感器任务
	if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_EMC) == 0){
		//初始化发送函数
		msg_struct_cloud_emc_control_cmd_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloud_emc_control_cmd_t));

		//赋值
		snd.cmdId = L3CI_emc;
		strcpy(snd.qn, iqn);
		strcpy(snd.st, ist);
		strcpy(snd.cn, icn);
		strcpy(snd.pw, ipw);
		strcpy(snd.mn, imn);
		snd.ansFlag = iansFlag;
		snd.length = sizeof(msg_struct_cloud_emc_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_EMC_CONTROL_CMD, TASK_ID_EMC, TASK_ID_CLOUD, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_EMC]);
			return FAILURE;
		}
	}//End of EMC传感器任务

	//TEMP传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_TEMP) == 0){
		//初始化发送函数
		msg_struct_cloud_temp_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloud_temp_control_cmd_t));

		//赋值
		snd1.cmdId = L3CI_temp;
		strcpy(snd1.qn, iqn);
		strcpy(snd1.st, ist);
		strcpy(snd1.cn, icn);
		strcpy(snd1.pw, ipw);
		strcpy(snd1.mn, imn);
		snd1.ansFlag = iansFlag;
		snd1.length = sizeof(msg_struct_cloud_temp_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_TEMP_CONTROL_CMD, TASK_ID_TEMP, TASK_ID_CLOUD, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_TEMP]);
			return FAILURE;
		}
	}//End of TEMP传感器任务

	//HUMID传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_HUMID) == 0){
		//初始化发送函数
		msg_struct_cloud_humid_control_cmd_t snd2;
		memset(&snd2, 0, sizeof(msg_struct_cloud_humid_control_cmd_t));

		//赋值
		snd2.cmdId = L3CI_humid;
		strcpy(snd2.qn, iqn);
		strcpy(snd2.st, ist);
		strcpy(snd2.cn, icn);
		strcpy(snd2.pw, ipw);
		strcpy(snd2.mn, imn);
		snd2.ansFlag = iansFlag;
		snd2.length = sizeof(msg_struct_cloud_humid_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_HUMID_CONTROL_CMD, TASK_ID_HUMID, TASK_ID_CLOUD, &snd2, snd2.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_HUMID]);
			return FAILURE;
		}
	}

	//AIRPRS传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_AIRPRS) == 0){
		HcuErrorPrint("CLOUD: Invalid received data on CP common sensor type NOISY, to be supported!\n");
		return FAILURE;
	}//End of AIRPRS传感器任务

	//WINDSPD传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_WINDSPD) == 0){
		//初始化发送函数
		msg_struct_cloud_windspd_control_cmd_t snd4;
		memset(&snd4, 0, sizeof(msg_struct_cloud_windspd_control_cmd_t));

		//赋值
		snd4.cmdId = L3CI_windspd;
		strcpy(snd4.qn, iqn);
		strcpy(snd4.st, ist);
		strcpy(snd4.cn, icn);
		strcpy(snd4.pw, ipw);
		strcpy(snd4.mn, imn);
		snd4.ansFlag = iansFlag;
		snd4.length = sizeof(msg_struct_cloud_windspd_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_WINDSPD_CONTROL_CMD, TASK_ID_WINDSPD, TASK_ID_CLOUD, &snd4, snd4.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDSPD]);
			return FAILURE;
		}
	}//End of WINDSPD传感器任务

	//WINDDIR传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_WINDDIR) == 0){
		//初始化发送函数
		msg_struct_cloud_winddir_control_cmd_t snd5;
		memset(&snd5, 0, sizeof(msg_struct_cloud_winddir_control_cmd_t));

		//赋值
		snd5.cmdId = L3CI_winddir;
		strcpy(snd5.qn, iqn);
		strcpy(snd5.st, ist);
		strcpy(snd5.cn, icn);
		strcpy(snd5.pw, ipw);
		strcpy(snd5.mn, imn);
		snd5.ansFlag = iansFlag;
		snd5.length = sizeof(msg_struct_cloud_winddir_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_WINDDIR_CONTROL_CMD, TASK_ID_WINDDIR, TASK_ID_CLOUD, &snd5, snd5.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_WINDDIR]);
			return FAILURE;
		}
	}//End of WINDDIR传感器任务

	//PM25传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_PM25) == 0){
		//初始化发送函数
		msg_struct_cloud_pm25_control_cmd_t snd6;
		memset(&snd6, 0, sizeof(msg_struct_cloud_pm25_control_cmd_t));

		//赋值
		snd6.cmdId = L3CI_pm25;
		strcpy(snd6.qn, iqn);
		strcpy(snd6.st, ist);
		strcpy(snd6.cn, icn);
		strcpy(snd6.pw, ipw);
		strcpy(snd6.mn, imn);
		snd6.ansFlag = iansFlag;
		snd6.length = sizeof(msg_struct_cloud_pm25_control_cmd_t);

		//发送
		ret = hcu_message_send(MSG_ID_CLOUD_PM25_CONTROL_CMD, TASK_ID_PM25, TASK_ID_CLOUD, &snd6, snd6.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUD: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUD], zHcuTaskNameList[TASK_ID_PM25]);
			return FAILURE;
		}
	}//End of PM25传感器任务

	//NOISY传感器任务
	else if (strcmp(st, CLOUD_BH_ITF_STD_ZHB_NOISY) == 0){
		HcuErrorPrint("CLOUD: Invalid received data on CP common sensor type NOISY, to be supported!\n");
		return FAILURE;
	}//End of NOISY传感器任务

	else {
		HcuErrorPrint("CLOUD: Invalid received data on CP command sensor type!\n");
		return FAILURE;
	}

	//如果剩余长度不为0, 则意味着目前的CP已经包含了多个CP命令，暂时不支持，报错
	if (totalLen != 0){
		HcuErrorPrint("CLOUD: Invalid received data on CP command with more than support capability!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_cloudcont_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_hwinv_cloud_phy_status_chg_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hwinv_cloud_phy_status_chg_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hwinv_cloud_phy_status_chg_t))){
		HcuErrorPrint("CLOUD: Receive message error!\n");
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.ethStatChg == HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.ethStatChg == HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudConTable.ethConTry = 0;
	else if ((rcv.usbnetStatChg == HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.usbnetStatChg == HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudConTable.usbnetConTry = 0;
	else if ((rcv.wifiStatChg == HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.wifiStatChg == HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudConTable.wifiConTry = 0;
	else if ((rcv.g3g4StatChg == HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE) || (rcv.g3g4StatChg == HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE)) zHcuCloudConTable.g3g4ConTry = 0;
	else{
		HcuErrorPrint("CLOUD: Message received with error content!\n");
		return FAILURE;
	}
	return SUCCESS;
}

//This callback function gets called by libcurl as soon as there is  data received that needs to be saved
size_t hcu_cloud_write_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	msg_struct_ethernet_cloud_data_rx_t *mem = (msg_struct_ethernet_cloud_data_rx_t *)userp;

	//For test
	//HcuDebugPrint("CLOUD: hcu_cloud_write_callback receive data len = %d\n", realsize);
	memset(mem, 0, sizeof(msg_struct_ethernet_cloud_data_rx_t));
	if(realsize > MAX_HCU_MSG_BUF_LENGTH)
	{
		HcuErrorPrint("CLOUD: No enough memory!\n");
		return 0;
	}
	memcpy(mem->buf, buffer, realsize); //mem->length
	mem->length = realsize;
	//HcuDebugPrint("CLOUD: Accumulated len = %d\n", mem->length);

	return realsize;
}

//为了多线程的安全，CURL参数的初始化只能做一次，做一遍，不能都做，所以采用全局变量让其它线程共享，而不能大家都分别去做初始化
OPSTAT hcu_cloud_http_link_init(void)
{
	//初始化
	curl_global_init(CURL_GLOBAL_ALL);
	//zHcuCloudCurlPtrTx = curl_easy_init(); //returns a CURL easy handle to be used in other easy-functions
	//zHcuCloudCurlPtrRx = curl_easy_init(); //returns a CURL easy handle to be used in other easy-functions

	//从结果进行判断
	//if (zHcuCloudCurlPtrTx == NULL){
	//	HcuErrorPrint("ETHERNET: Not able to create Curl Pointer by curl_easy_init()!\n");
	//	return FAILURE;
	//}
	//if (zHcuCloudCurlPtrRx == NULL){
	//	HcuErrorPrint("ETHERNET: Not able to create Curl Pointer by curl_easy_init()!\n");
	//	return FAILURE;
	//}

	return SUCCESS;
}

OPSTAT func_cloudcont_heart_beat_received_handle(void)
{
	//FSM状态检查
	if (FsmGetState(TASK_ID_CLOUD) != FSM_STATE_CLOUDCONT_ONLINE){
		HcuErrorPrint("CLOUD: FSM State error, taking care of potential risk!\n");
		return FAILURE;
	}

	//HW状态检查，心跳的意义
	if ((zHcuHwinvTable.ethernet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuHwinvTable.usbnet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)\
			&&(zHcuHwinvTable.wifi.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE) && (zHcuHwinvTable.g3g4.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE)){
		if (FsmSetState(TASK_ID_CLOUD, FSM_STATE_CLOUDCONT_OFFLINE) == FAILURE){
			HcuErrorPrint("CLOUD: Set FSM state error!\n");
			return FAILURE;
		}
		HcuErrorPrint("CLOUD: Hardware ETH/USBNET/WIFI/3G4G connection state error, taking care of potential risk!\n");
		return FAILURE;
	}

	#ifdef TRACE_DEBUG_ON
	HcuDebugPrint("CLOUD: Ack HEART_BEAT successful!\n");
	#endif //TRACE_DEBUG_ON

	return SUCCESS;
}
