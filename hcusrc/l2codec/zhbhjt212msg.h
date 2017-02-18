/*
 * ZHBHJT212CODEC.h
 *
 *  Created on: 2016年12月25日
 *      Author: test
 */

#ifndef _ZHBHJT_MSG_IE_DEFINITION_H_
#define _ZHBHJT_MSG_IE_DEFINITION_H_
#pragma pack (1) //强制1字节对齐
#include "../l0comvm/vmlayer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//考虑到TCPIP中底层最长1500B的情形，这里先如此定义，不要超过这个限制，所以16进制的缓冲区最好不要超过500字节
#define ZHBHJT_MSG_BUF_WITH_HEAD_MAX_LEN 				134
#define ZHBHJT_MSG_BUF_BODY_ONLY_MAX_LEN 				ZHBHJT_MSG_BUF_WITH_HEAD_MAX_LEN - 4 //MSG头+长度域共4BYTE
//HUIXML固定头部常量定义
#define ZHBHJT_MSG_HUIXML_CONSTANT_XML_HEAD_L  			"<xml>"
#define ZHBHJT_MSG_HUIXML_CONSTANT_XML_HEAD_R  			"</xml>"
#define ZHBHJT_MSG_HUIXML_CONSTANT_TO_USER_L  			"<ToUserName><![CDATA["
#define ZHBHJT_MSG_HUIXML_CONSTANT_TO_USER_R  			"]]></ToUserName>"


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT字典定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//ST系统编码字典
typedef enum
{
	ZHBHJT_IE_uni_STcode_earth_water                      = 21,
	ZHBHJT_IE_uni_STcode_air_quality                      = 22,
	ZHBHJT_IE_uni_STcode_noise_monitor                    = 23,
	ZHBHJT_IE_uni_STcode_air_pollutioin_source            = 31,
	ZHBHJT_IE_uni_STcode_on_water_pullution_source        = 32,
	ZHBHJT_IE_uni_STcode_under_water_pullution_source     = 33,
	ZHBHJT_IE_uni_STcode_sea_pollution_source             = 34,
	ZHBHJT_IE_uni_STcode_sand_pollution_source            = 35,
	ZHBHJT_IE_uni_STcode_noise_pollution_source           = 36,
	ZHBHJT_IE_uni_STcode_shake_pollution_source           = 37,
	ZHBHJT_IE_uni_STcode_emission_pollution_source        = 38,
	ZHBHJT_IE_uni_STcode_emc_pollution_source             = 41,
	ZHBHJT_IE_uni_STcode_system_interaction               = 91,
}ZhbhjtSTcodeUniDefination;

//ST系统编码字典
typedef enum
{
	//INIT初始化
	ZHBHJT_IE_uni_CNcode_init_set_resend_time_out_req          = 1000,
	ZHBHJT_IE_uni_CNcode_init_set_uplimit_duration_req         = 1001,
	//参数命令
	ZHBHJT_IE_uni_CNcode_par_get_field_time_req               = 1011,
	ZHBHJT_IE_uni_CNcode_par_get_field_time_resp              = 1011,
	ZHBHJT_IE_uni_CNcode_par_set_field_time_req               = 1012,
	ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_req      = 1021,
	ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_resp     = 1021,
	ZHBHJT_IE_uni_CNcode_par_set_pollution_limitaion_req      = 1022,
	ZHBHJT_IE_uni_CNcode_par_get_field_addr_req               = 1031,
	ZHBHJT_IE_uni_CNcode_par_get_field_addr_resp              = 1031,
	ZHBHJT_IE_uni_CNcode_par_set_field_addr_req               = 1032,
	ZHBHJT_IE_uni_CNcode_par_get_data_report_time_req         = 1041,
	ZHBHJT_IE_uni_CNcode_par_get_data_report_time_resp        = 1041,
	ZHBHJT_IE_uni_CNcode_par_set_data_report_time_req         = 1042,
	ZHBHJT_IE_uni_CNcode_par_get_data_sample_cycle_req        = 1061,  //实时数据间隔
	ZHBHJT_IE_uni_CNcode_par_get_data_sample_cycle_resp       = 1061,
	ZHBHJT_IE_uni_CNcode_par_set_data_sample_cycle_req        = 1062,
	ZHBHJT_IE_uni_CNcode_par_set_access_pswd_req              = 1072,
	//交互命令
	ZHBHJT_IE_uni_CNcode_cmd_answer_resp                       = 9011,
	ZHBHJT_IE_uni_CNcode_cmd_operation_result_resp             = 9012,
	ZHBHJT_IE_uni_CNcode_cmd_notification_req                  = 9013,
	ZHBHJT_IE_uni_CNcode_cmd_notification_resp                 = 9013,
	ZHBHJT_IE_uni_CNcode_cmd_data_req                          = 9014,
	ZHBHJT_IE_uni_CNcode_cmd_data_resp                         = 9014,
	//数据命令
	ZHBHJT_IE_uni_CNcode_dat_get_rt_data_req                   = 2011,
	ZHBHJT_IE_uni_CNcode_dat_get_rt_data_resp                  = 2011,
	ZHBHJT_IE_uni_CNcode_dat_stop_monitor_rt_data_req          = 2012,
	ZHBHJT_IE_uni_CNcode_dat_get_equ_status_req                = 2021,
	ZHBHJT_IE_uni_CNcode_dat_get_equ_status_resp               = 2021,
	ZHBHJT_IE_uni_CNcode_dat_stop_monitor_equ_status_req       = 2022,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_req        = 2031,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_resp       = 2031,
	ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_req            = 2041,
	ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_resp           = 2041,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_req        = 2051,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_resp       = 2051,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_req       = 2061,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_resp      = 2061,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_req    = 2071,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_resp   = 2071,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_event_resp    = 2072,
	//控制命令
	ZHBHJT_IE_uni_CNcode_ctr_calabration_req                   = 3011,
	ZHBHJT_IE_uni_CNcode_ctr_start_sample_req                  = 3012,
	ZHBHJT_IE_uni_CNcode_ctr_equ_operation_req                 = 3013,
	ZHBHJT_IE_uni_CNcode_ctr_set_sample_tti_req                = 3014,
}ZhbhjtCNcodeUniDefination;

//QNRTN
#define ZHBHJT_IE_uni_QNRTN_PREPARE_REQ 1
#define ZHBHJT_IE_uni_QNRTN_REQ_REFUSE 2
#define ZHBHJT_IE_uni_QNRTN_PSWD_ERROR 3

//EXEQTN
#define ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS 1
#define ZHBHJT_IE_uni_EXETRN_EXE_FAILURE 2
#define ZHBHJT_IE_uni_EXETRN_EXE_NO_DATA 100









/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT MSG消息结构定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//标准的消息格式
//为了彻底解决大小端的问题，这里按照比特进行定义
typedef struct StrMsg_ZHBHJT_MSGID_uni_general_head_msgid
{
	UINT8 cmdId;
	UINT8 optId;
}StrMsg_ZHBHJT_MSGID_uni_general_head_msgid_t;

typedef struct StrMsg_ZHBHJT_MSGID_uni_general_message
{
	StrMsg_ZHBHJT_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	UINT8  data[ZHBHJT_MSG_BUF_BODY_ONLY_MAX_LEN];
}StrMsg_ZHBHJT_MSGID_uni_general_message_t;

//无效
//ZHBHJT_MSGID_uni_none                             = 0x0000,



#pragma pack () //取消字节对齐
#endif /* _ZHBHJT_MSG_IE_DEFINITION_H_ */
