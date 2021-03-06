/*
 * cloudvela.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L2FRAME_CLOUDVELA_H_
#define L2FRAME_CLOUDVELA_H_

#include "../l1com/l1comdef.h"
#include "../l2codec/huixmlcodec.h"
#include "../l2codec/zhbhjt212codec.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_CLOUDVELA
{
	FSM_STATE_CLOUDVELA_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CLOUDVELA_OFFLINE,  //离线只干一件事，就是想办法连上，通过定时心跳检测的方式来进行
	FSM_STATE_CLOUDVELA_ONLINE,
	FSM_STATE_CLOUDVELA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmCloudvela[];

//在CLOUD模块中定义的公共数据
typedef struct gTaskCloudvelaContext
{
	UINT8  curCon;  					//表示当前是哪一种物理接口在工作
	UINT32 ethConTry;  					//用于断链的处理，确保高优先级物理链路可以处于优势地位，节省用户的链路成本消耗
	UINT32 usbnetConTry; 				//下同
	UINT32 wifiConTry;					//下同
	UINT32 g3g4ConTry;					//下同
	//[特别说明：因为业务服务器DEFAULT和HOME服务器已经使用了SOCKET/CURL不同机制分别工作，LINKID只有func_cloudvela_send_data_to_cloud才有作用]
	UINT8  linkId;						//用来标识当前处理的链路是哪一个
	int    defaultSvrSocketCon;  		//业务服务器SOCKET连接标识
	int    defaultSvrethConClientFd;    //业务服务器eth客户标签
	int    homeSvrSocketCon;  			//家庭服务器SOCKET连接标识
	int    homeSvrethConClientFd;       //家庭服务器eth客户标签
	msgie_struct_bh_com_head_t L2Link;  //层2的链路上下文，用于编解码时带到API函数中去
	UINT16 defaultHbRand;    			//用于业务心跳链路的随机数检查，只用于HCU发起主动心跳，不适用于被动心跳过程
}gTaskCloudvelaContext_t;

extern gTaskCloudvelaContext_t gCloudvelaTaskContex;

//FSM CORE API
extern OPSTAT fsm_cloudvela_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_ethernet_curl_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_socket_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//公共服务模块消息处理函数
extern OPSTAT fsm_cloudvela_syspm_alarm_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_syspm_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_syspm_perfm_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_syspm_perfm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_syspm_test_command_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_syspm_test_command_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_sysswm_inventory_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_sysswm_inventory_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_sysswm_sw_package_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_sysswm_sw_package_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//通用传感器模块处理函数
extern OPSTAT fsm_cloudvela_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_emc_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_emc_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_temp_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_temp_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_humid_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_humid_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_winddir_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_winddir_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_windspd_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_windspd_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_picture_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_noise_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_noise_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_noise_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_ycjk_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_ycjk_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_ycjk_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//通用传感器模块处理函数
extern OPSTAT fsm_cloudvela_airprs_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_airprs_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_alcohol_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_alcohol_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_co1_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_co1_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hcho_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hcho_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_lightstr_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_lightstr_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25sp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25sp_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_toxicgas_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_toxicgas_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//LLCZHB技术标准
extern OPSTAT fsm_cloudvela_llczhb_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//分项目模块处理函数
extern OPSTAT fsm_cloudvela_l3bfsc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfsc_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfsc_event_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfsc_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfsc_statistic_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//分项目模块处理函数
extern OPSTAT fsm_cloudvela_l3bfdf_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfdf_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfdf_event_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfdf_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfdf_statistic_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//分项目模块处理函数
extern OPSTAT fsm_cloudvela_l3bfhs_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfhs_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfhs_event_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfhs_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_l3bfhs_statistic_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);


//L2 LINK 链路功能
OPSTAT func_cloudvela_hb_link_main_entry(void);
OPSTAT func_cloudvela_hb_link_active_send_signal(void);
OPSTAT func_cloudvela_hb_link_active_rcv_signal_check(UINT16 randval);
OPSTAT func_cloudvela_hb_link_passive_rcv_signal_for_react(UINT16 randval);

//L2 LINK 链路功能支撑函数
extern OPSTAT func_cloudvela_send_data_to_cloud(CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_socket_conn_setup(void);
extern OPSTAT func_cloudvela_http_curl_conn_setup(void);
extern OPSTAT func_cloudvela_http_curl_link_init(void);
extern size_t func_cloudvela_http_curl_write_callback(void *buffer, size_t size, size_t nmemb, void *userp);

//给外部使用的函数
extern bool hcu_cloudvela_check_online_status(void);

//引用外部函数
extern UINT32 hcu_disk_write(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_ethernet_curl_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_usbnet_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_wifi_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_3g4g_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_ethernet_phy_link_setup(void);
extern OPSTAT hcu_usbnet_phy_link_setup(void);
extern OPSTAT hcu_wifi_phy_link_setup(void);
extern OPSTAT hcu_3g4g_phy_link_setup(void);
extern OPSTAT hcu_ethernet_phy_link_disconnect(void);
extern OPSTAT hcu_usbnet_phy_link_disconnect(void);
extern OPSTAT hcu_wifi_phy_link_disconnect(void);
extern OPSTAT hcu_3g4g_phy_link_disconnect(void);
extern void hcu_vm_calculate_crc_modbus(UINT8* pDataIn, UINT32 iLenIn, UINT16* pCRCOut);
extern void func_hwinv_scan_date(void);
extern OPSTAT hcu_ethernet_socket_data_send(CloudDataSendBuf_t *buf);



//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CLOUDVELA(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL_CLOUDVELA(par) 	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_CLOUDVELA, par)

//申明输出变量
#define HCU_CLOUDVELA_OUTPUT_MSG_DECLARITION()\
	CloudDataSendBuf_t pMsgOutput;\
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));\
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));\

//编码消息并准备发送到HUITP接口
#define HCU_CLOUDVELA_GEN_NEW_HUITP_MSG_RESP(strHuiMsg, huiMsgId)\
	strHuiMsg pMsgProc;\
	UINT16 msgProcLen = sizeof(strHuiMsg);\
	memset(&pMsgProc, 0, msgProcLen);\
	pMsgProc.msgId.cmdId = (huiMsgId>>8)&0xFF;\
	pMsgProc.msgId.optId = huiMsgId&0xFF;\
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);\
	pMsgProc.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);\
	pMsgProc.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);\
	pMsgProc.baseResp.comResp = rcv.baseResp;\

#define HCU_CLOUDVELA_GEN_NEW_HUITP_MSG_REPORT(strHuiMsg, huiMsgId)\
	strHuiMsg pMsgProc;\
	UINT16 msgProcLen = sizeof(strHuiMsg);\
	memset(&pMsgProc, 0, msgProcLen);\
	pMsgProc.msgId.cmdId = (huiMsgId>>8)&0xFF;\
	pMsgProc.msgId.optId = huiMsgId&0xFF;\
	pMsgProc.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);\
	pMsgProc.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);\
	pMsgProc.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);\
	pMsgProc.baseReport.comReport = rcv.baseReport;\

//PACK过程
#define HCU_CLOUDVELA_PACK_HUITP_MSG(huiMsgId)\
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;\
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));\
	memcpy(&pMsgInput, &pMsgProc, msgProcLen);\
	if (func_cloudvela_huitpxml_msg_pack(huiMsgId, &pMsgInput, msgProcLen, &pMsgOutput) == FAILURE)\
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Package message error!\n");\

//其它潜在编码处理过程
#define HCU_CLOUDVELA_PROCESSING_OTHER_CODEC_PROTOCOL()\
	else if (zHcuSysEngPar.cloud.svrBhItfFrameStdDefault == HCU_SYSCFG_CLOUD_BH_ITF_STD_ZHB_HJT212){\
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not support transmit protocol!\n");\
	}\
	else{\
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Not set back-haul transmit protocol rightly!\n");\
	}\

#endif /* L2FRAME_CLOUDVELA_H_ */
