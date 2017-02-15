/**
 ****************************************************************************************
 *
 * @file huixmlcodec.h
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_HUIXMLCODEC_H_
#define L2FRAME_HUIXMLCODEC_H_

#include "../l2codec/huitp.h"
#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

//XML message pack/unpack
extern OPSTAT func_cloudvela_huitpxml_msg_pack(UINT16 msgId, StrMsg_HUITP_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output);
extern OPSTAT func_cloudvela_huitpxml_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId);
extern void   func_cloudvela_huitpxml_msg_generate_test_data(void);

//公共服务模块消息处理函数
extern OPSTAT func_cloudvela_huitpxml_msg_heart_beat_req_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_alarm_info_req_received_handle(StrMsg_HUITP_MSGID_uni_alarm_info_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_alarm_info_confirm_received_handle(StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm_info_req_received_handle(StrMsg_HUITP_MSGID_uni_performance_info_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm_info_confirm_received_handle(StrMsg_HUITP_MSGID_uni_performance_info_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_inventory_req_received_handle(StrMsg_HUITP_MSGID_uni_inventory_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_uni_inventory_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_sw_package_req_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *rcv);

//通用传感器模块处理函数
extern OPSTAT func_cloudvela_huitpxml_msg_emc_data_req_received_handle(StrMsg_HUITP_MSGID_uni_emc_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_emc_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_emc_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_emc_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_co1_data_req_received_handle(StrMsg_HUITP_MSGID_uni_co1_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_co1_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_co1_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_hcho_data_req_received_handle(StrMsg_HUITP_MSGID_uni_hcho_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_hcho_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_hcho_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_alcohol_data_req_received_handle(StrMsg_HUITP_MSGID_uni_alcohol_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_alcohol_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm25_data_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25_data_req_t  *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm25_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t  *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm25_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm25sp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_pm25sp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_windspd_data_req_received_handle(StrMsg_HUITP_MSGID_uni_windspd_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_windspd_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_windspd_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_winddir_data_req_received_handle(StrMsg_HUITP_MSGID_uni_winddir_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_winddir_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_winddir_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_temp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_temp_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_temp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_temp_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_temp_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_humid_data_req_received_handle(StrMsg_HUITP_MSGID_uni_humid_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_humid_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_humid_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_humid_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_airprs_data_req_received_handle(StrMsg_HUITP_MSGID_uni_airprs_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_airprs_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_noise_data_req_received_handle(StrMsg_HUITP_MSGID_uni_noise_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_noise_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_noise_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_noise_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_hsmmp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_hsmmp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_hsmmp_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_lightstr_data_req_received_handle(StrMsg_HUITP_MSGID_uni_lightstr_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_lightstr_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_toxicgas_data_req_received_handle(StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_toxicgas_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t *rcv);

//分项目模块处理函数
extern OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_req_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_event_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_cmd_req_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_req_t *rcv);
extern OPSTAT func_cloudvela_huitpxml_msg_bfsc_statistic_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t *rcv);



//大小端变换宏定义
#define HUITP_CURRENT_PROCESSOR_ENDIAN_SMALL 1
#define HUITP_CURRENT_PROCESSOR_ENDIAN_BIG 2
#define HUITP_CURRENT_PROCESSOR_ENDIAN_SET HUITP_CURRENT_PROCESSOR_ENDIAN_SMALL
#if (HUITP_CURRENT_PROCESSOR_ENDIAN_SET == HUITP_CURRENT_PROCESSOR_ENDIAN_BIG)
   #define HUITP_ENDIAN_EXG16(x) (x)
   #define HUITP_ENDIAN_EXG32(x) (x)
#elif (HUITP_CURRENT_PROCESSOR_ENDIAN_SET == HUITP_CURRENT_PROCESSOR_ENDIAN_SMALL)
   #define HUITP_ENDIAN_EXG16(x) ((((x)&0xFF00)>>8) | (((x)&0x00FF)<<8))
   #define HUITP_ENDIAN_EXG32(x) ((((x)&0xFF000000)>>24) | (((x)&0x00FF0000)>>8) | (((x)&0x0000FF00)<<8) | (((x)&0x000000FF)<<24))
#else
   #error Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both.
#endif


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_HUIXMLCODEC(...)	do{zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//Test Data：纯粹是为了测试下位机，所以没有将ToUser/FromUser设置的非常规整
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_HEAD_VALID   				"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1477323943</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
400183]]></Content><FuncFlag>XXXX</FuncFlag></xml>" 
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_IND 						"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4D90003400010001014D01002B0100FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_RESP_YES				"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4D10000A00020001014D02000101]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_AUTH_RESP_NO					"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4D10000A00020001014D02000102]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_PERIOD	"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E81006800030001014D0000060404010101014D00000604040101010148000001024B000001024600000102470000010245000001024900000303FFFF4A00000303FFFF450100030300124E0000030371064E010003038FF54400000303E2504E02000303967C4E03000101]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_CLOSE		"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033642</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E81006800030001014D0000060404010101014D00000604040101010148000001024B000001024600000102470000010245000001024900000303FFFF4A00000303FFFF450100030300124E0000030371064E010003038FF54400000303E2504E02000303967C4E03000102]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_REPORT_FAULT		"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E81006800030001014D0000060404010101014D00000604040101010148000001024B000001024600000102470000010245000001024900000303FFFF4A00000303FFFF450100030300134E00000303BCFA4E01000303F4894400000303E2504E020003032D304E03000103]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_PERIOD	"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E01000A00040001014E03000101]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_CLOSE	"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033642</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E01000A00040001014E03000102]]></Content><FuncFlag>0</FuncFlag></xml>"
#define HUITP_MSG_HUIXML_TEST_DATA_CCL_STATE_CONFIRM_FAULT	"<xml><ToUserName><![CDATA[HCU_G801_BFSC_SH001]]></ToUserName>\
<FromUserName><![CDATA[XHZN_HCU]]></FromUserName><CreateTime>1485033641</CreateTime><MsgType><![CDATA[huitp_text]]></MsgType><Content><![CDATA[\
4E01000A00040001014E03000103]]></Content><FuncFlag>0</FuncFlag></xml>"


#endif /* L2FRAME_HUIXMLCODEC_H_ */
