/*
 * ZHBHJT212CODEC.h
 *
 *  Created on: 2016年12月25日
 *      Author: test
 */

#ifndef _ZHBHJT_MSG_IE_DEFINITION_H_
#define _ZHBHJT_MSG_IE_DEFINITION_H_
#include "../l0comvm/vmlayer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//考虑到TCPIP中底层最长1500B的情形，这里先如此定义，不要超过这个限制，所以16进制的缓冲区最好不要超过500字节
#define ZHBHJT_MSG_BUF_WITH_HEAD_MAX_LEN 				134
#define ZHBHJT_MSG_BUF_BODY_ONLY_MAX_LEN 				ZHBHJT_MSG_BUF_WITH_HEAD_MAX_LEN - 4 //MSG头+长度域共4BYTE


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT字典定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//ST系统编码字典：指示的是哪一种污染物体系
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

//CN命令码子编码，指示的是命令，或者我们通常所说的消息ID
typedef enum
{
	//INIT初始化
	ZHBHJT_IE_uni_CNcode_MIN_0000                                  = 0,
	ZHBHJT_IE_uni_CNcode_init_set_resend_time_out_req_1000         = 1000,
	ZHBHJT_IE_uni_CNcode_init_set_uplimit_duration_req_1001        = 1001,
	//参数命令
	ZHBHJT_IE_uni_CNcode_par_get_field_time_req_1011               = 1011,
	ZHBHJT_IE_uni_CNcode_par_get_field_time_resp_1011              = 1011,
	ZHBHJT_IE_uni_CNcode_par_set_field_time_req_1012               = 1012,
	ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_req_1021      = 1021,
	ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_resp_1021     = 1021,
	ZHBHJT_IE_uni_CNcode_par_set_pollution_limitaion_req_1022      = 1022,
	ZHBHJT_IE_uni_CNcode_par_get_field_addr_req_1031               = 1031,
	ZHBHJT_IE_uni_CNcode_par_get_field_addr_resp_1031              = 1031,
	ZHBHJT_IE_uni_CNcode_par_set_field_addr_req_1032               = 1032,
	ZHBHJT_IE_uni_CNcode_par_get_data_report_time_req_1041         = 1041,
	ZHBHJT_IE_uni_CNcode_par_get_data_report_time_resp_1041        = 1041,
	ZHBHJT_IE_uni_CNcode_par_set_data_report_time_req_1042         = 1042,
	ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_req_1061            = 1061,  //实时数据间隔
	ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_resp_1061           = 1061,
	ZHBHJT_IE_uni_CNcode_par_set_rtdi_interval_req_1062            = 1062,
	ZHBHJT_IE_uni_CNcode_par_set_access_pswd_req_1072              = 1072,
	//交互命令
	ZHBHJT_IE_uni_CNcode_cmd_answer_resp_9011                       = 9011,
	ZHBHJT_IE_uni_CNcode_cmd_operation_result_resp_9012             = 9012,
	ZHBHJT_IE_uni_CNcode_cmd_notification_req_9013                  = 9013,
	ZHBHJT_IE_uni_CNcode_cmd_notification_resp_9013                 = 9013,
	ZHBHJT_IE_uni_CNcode_cmd_data_req_9014                          = 9014,
	ZHBHJT_IE_uni_CNcode_cmd_data_resp_9014                         = 9014,
	//数据命令
	ZHBHJT_IE_uni_CNcode_dat_get_rt_data_req_2011                   = 2011,
	ZHBHJT_IE_uni_CNcode_dat_get_rt_data_resp_2011                  = 2011,
	ZHBHJT_IE_uni_CNcode_dat_stop_monitor_rt_data_req_2012          = 2012,
	ZHBHJT_IE_uni_CNcode_dat_get_equ_status_req_2021                = 2021,
	ZHBHJT_IE_uni_CNcode_dat_get_equ_status_resp_2021               = 2021,
	ZHBHJT_IE_uni_CNcode_dat_stop_monitor_equ_status_req_2022       = 2022,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_req_2031        = 2031,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_resp_2031       = 2031,
	ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_req_2041            = 2041,
	ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_resp_2041           = 2041,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_req_2051        = 2051,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_resp_2051       = 2051,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_req_2061       = 2061,//add by shanchun
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_resp_2061      = 2061,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_req_2071    = 2071,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_resp_2071   = 2071,
	ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_event_resp_2072    = 2072,
	//控制命令
	ZHBHJT_IE_uni_CNcode_ctr_calabration_req_3011                   = 3011,
	ZHBHJT_IE_uni_CNcode_ctr_start_sample_req_3012                  = 3012,
	ZHBHJT_IE_uni_CNcode_ctr_equ_operation_req_3013                 = 3013,
	ZHBHJT_IE_uni_CNcode_ctr_set_sample_tti_req_3014                = 3014,
	ZHBHJT_IE_uni_CNcode_MAX_9999                                   = 9999,
}ZhbhjtCNcodeUniDefination;

//QNRTN
#define ZHBHJT_IE_uni_QNRTN_PREPARE_REQ 1
#define ZHBHJT_IE_uni_QNRTN_REQ_REFUSE 2
#define ZHBHJT_IE_uni_QNRTN_PSWD_ERROR 3

//EXEQTN
#define ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS 1
#define ZHBHJT_IE_uni_EXETRN_EXE_FAILURE 2
#define ZHBHJT_IE_uni_EXETRN_EXE_NO_DATA 100

//自定义IE字典
typedef enum
{
	ZHBHJT_IEID_uni_null = 0,
	ZHBHJT_IEID_uni_MIN =  0,
	ZHBHJT_IEID_uni_SystemTime,
	ZHBHJT_IEID_uni_CfmQN,
	ZHBHJT_IEID_uni_CfmCN,
	ZHBHJT_IEID_uni_CfmPNUM,
	ZHBHJT_IEID_uni_CfmPNO,
	ZHBHJT_IEID_uni_QnRtn,
	ZHBHJT_IEID_uni_ExeRtn,
	ZHBHJT_IEID_uni_RtdInterval,
	ZHBHJT_IEID_uni_Rtd,
	ZHBHJT_IEID_uni_value_Min,
	ZHBHJT_IEID_uni_value_Avg,
	ZHBHJT_IEID_uni_value_Max,
	ZHBHJT_IEID_uni_ZsRtd,
	ZHBHJT_IEID_uni_value_ZsMin,
	ZHBHJT_IEID_uni_value_ZsAvg,
	ZHBHJT_IEID_uni_value_ZsMax,
	ZHBHJT_IEID_uni_polFlag,
	ZHBHJT_IEID_uni_Cou,
	ZHBHJT_IEID_uni_RS,
	ZHBHJT_IEID_uni_RT,
	ZHBHJT_IEID_uni_Ala,
	ZHBHJT_IEID_uni_UpValue,
	ZHBHJT_IEID_uni_LowValue,
	ZHBHJT_IEID_uni_Data,
	ZHBHJT_IEID_uni_DayData,
	ZHBHJT_IEID_uni_NightData,
	ZHBHJT_IEID_uni_AlarmTime,
	ZHBHJT_IEID_uni_AlarmType,
	ZHBHJT_IEID_uni_AlarmTarget,
	ZHBHJT_IEID_uni_PolId,
	ZHBHJT_IEID_uni_BeginTime,
	ZHBHJT_IEID_uni_EndTime,
	ZHBHJT_IEID_uni_DataTime,
	ZHBHJT_IEID_uni_ReportTime,
	ZHBHJT_IEID_uni_DayStdValue,
	ZHBHJT_IEID_uni_NightStdValue,
	ZHBHJT_IEID_uni_SetPwd,
	ZHBHJT_IEID_uni_OverTime,
	ZHBHJT_IEID_uni_ReCount,
	ZHBHJT_IEID_uni_WarnTime,
	ZHBHJT_IEID_uni_CTime,
	ZHBHJT_IEID_uni_MAX,
	ZHBHJT_IEID_cmb_MIN,
	ZHBHJT_IEID_cmb_gap_time,
	ZHBHJT_IEID_cmb_alm_lim_rng,
	ZHBHJT_IEID_cmb_multi_ctime,
	ZHBHJT_IEID_cmb_alarm_event,
	ZHBHJT_IEID_cmb_multi_rtd,
	ZHBHJT_IEID_cmb_pol_report,
	ZHBHJT_IEID_cmb_MAX,
}ZhbhjtIeIdUniDefination;

//自定义污染物标签的统一字典，以便节省内部交互的复杂度和字节成本
typedef enum
{
	ZHBHJT_POLID_null = 0,
	ZHBHJT_POLID_MIN =  0,
	ZHBHJT_POLID_B03,
	ZHBHJT_POLID_L10,
	ZHBHJT_POLID_L5,
	ZHBHJT_POLID_L50,
	ZHBHJT_POLID_L90,
	ZHBHJT_POLID_L95,
	ZHBHJT_POLID_Ld,
	ZHBHJT_POLID_Ldn,
	ZHBHJT_POLID_Leq,
	ZHBHJT_POLID_LMn,
	ZHBHJT_POLID_LMx,
	ZHBHJT_POLID_Ln,
	ZHBHJT_POLID_S01,
	ZHBHJT_POLID_S02,
	ZHBHJT_POLID_S03,
	ZHBHJT_POLID_S04,
	ZHBHJT_POLID_S05,
	ZHBHJT_POLID_S06,
	ZHBHJT_POLID_S07,
	ZHBHJT_POLID_S08,
	ZHBHJT_POLID_B02,
	ZHBHJT_POLID_01,
	ZHBHJT_POLID_02,
	ZHBHJT_POLID_03,
	ZHBHJT_POLID_04,
	ZHBHJT_POLID_05,
	ZHBHJT_POLID_06,
	ZHBHJT_POLID_07,
	ZHBHJT_POLID_08,
	ZHBHJT_POLID_09,
	ZHBHJT_POLID_10,
	ZHBHJT_POLID_11,
	ZHBHJT_POLID_12,
	ZHBHJT_POLID_13,
	ZHBHJT_POLID_14,
	ZHBHJT_POLID_15,
	ZHBHJT_POLID_16,
	ZHBHJT_POLID_17,
	ZHBHJT_POLID_18,
	ZHBHJT_POLID_19,
	ZHBHJT_POLID_20,
	ZHBHJT_POLID_21,
	ZHBHJT_POLID_22,
	ZHBHJT_POLID_23,
	ZHBHJT_POLID_24,
	ZHBHJT_POLID_25,
	ZHBHJT_POLID_26,
	ZHBHJT_POLID_27,
	ZHBHJT_POLID_28,
	ZHBHJT_POLID_29,
	ZHBHJT_POLID_30,
	ZHBHJT_POLID_31,
	ZHBHJT_POLID_32,
	ZHBHJT_POLID_33,
	ZHBHJT_POLID_34,
	ZHBHJT_POLID_35,
	ZHBHJT_POLID_36,
	ZHBHJT_POLID_37,
	ZHBHJT_POLID_99,
	ZHBHJT_POLID_B01,
	ZHBHJT_POLID_001,
	ZHBHJT_POLID_002,
	ZHBHJT_POLID_003,
	ZHBHJT_POLID_010,
	ZHBHJT_POLID_011,
	ZHBHJT_POLID_015,
	ZHBHJT_POLID_020,
	ZHBHJT_POLID_021,
	ZHBHJT_POLID_022,
	ZHBHJT_POLID_023,
	ZHBHJT_POLID_024,
	ZHBHJT_POLID_025,
	ZHBHJT_POLID_026,
	ZHBHJT_POLID_027,
	ZHBHJT_POLID_028,
	ZHBHJT_POLID_029,
	ZHBHJT_POLID_030,
	ZHBHJT_POLID_031,
	ZHBHJT_POLID_032,
	ZHBHJT_POLID_033,
	ZHBHJT_POLID_034,
	ZHBHJT_POLID_035,
	ZHBHJT_POLID_036,
	ZHBHJT_POLID_037,
	ZHBHJT_POLID_038,
	ZHBHJT_POLID_039,
	ZHBHJT_POLID_040,
	ZHBHJT_POLID_041,
	ZHBHJT_POLID_060,
	ZHBHJT_POLID_061,
	ZHBHJT_POLID_065,
	ZHBHJT_POLID_080,
	ZHBHJT_POLID_101,
	ZHBHJT_POLID_a01001,
	ZHBHJT_POLID_a01002,
	ZHBHJT_POLID_a01006,
	ZHBHJT_POLID_a01007,
	ZHBHJT_POLID_a01008,
	ZHBHJT_POLID_a34001,
	ZHBHJT_POLID_a50001,
	ZHBHJT_POLID_MAX,
}ZhbhjtIeIdPolIdDefination;



/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ZHBHJT MSG消息结构定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//标准的消息格式
//为了彻底解决大小端的问题，这里按照比特进行定义
typedef struct StrMsg_ZHBHJT_MSGID_uni_general_head_msgid
{
	UINT16 msgId;
	UINT16 msgLen;
}StrMsg_ZHBHJT_MSGID_uni_general_head_msgid_t;

typedef struct StrMsg_ZHBHJT_MSGID_uni_general_message
{
	StrMsg_ZHBHJT_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	UINT8  data[ZHBHJT_MSG_BUF_BODY_ONLY_MAX_LEN];
}StrMsg_ZHBHJT_MSGID_uni_general_message_t;

//无效
//ZHBHJT_MSGID_uni_none                             = 0x0000,

//Test Data
#define ZHBHJT_MSG_TEST_DATA_SET_PSWD         "##0086QN=20170516010101001;ST=22;CN=1072;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&PW=654321&&E476\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_PSWD         "##0077QN=20170516010101001;ST=22;CN=1011;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&8411\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_FT_TIME      "##0102QN=20170516010101001;ST=22;CN=1012;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&SystemTime=20170516010101&&E1C4\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_FT_TIME      "##0077QN=20170516010101001;ST=22;CN=2011;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&30A5\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_RTD          "##0077QN=20170516010101001;ST=22;CN=2011;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&30A5\r\n"
#define ZHBHJT_MSG_TEST_DATA_STOP_MONIROT     "##0070QN=20170516010101001;ST=22;CN=2012;PW=123456;MN=ABCDEFG1234567;CP=&&&&B388\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_EQU_RUN      "##0077QN=20170516010101001;ST=22;CN=2021;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&C116\r\n"
#define ZHBHJT_MSG_TEST_DATA_STOP_EQU_RUN     "##0070QN=20170516010101001;ST=22;CN=2022;PW=123456;MN=ABCDEFG1234567;CP=&&&&A938\r\n"
//#define ZHBHJT_MSG_TEST_DATA_GET_MIN_RPT1     "##0124QN=20170516010101001;ST=22;CN=2051;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&BeginTime=20170506000000,EndTime=20170510000000&&4F61\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_MIN_RPT1     "##0124QN=20170302150101001;ST=22;CN=2051;PW=123456;MN=AQHJ0QP0100001;Flag=3;CP=&&BeginTime=20170302000000,EndTime=20170303000000&&0137\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_POL_DAY      "##0124QN=20170516010101001;ST=22;CN=2031;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&BeginTime=20170506000000,EndTime=20170510000000&&2BE2\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_HIS_DAY      "##0124QN=20170516010101001;ST=22;CN=2041;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&BeginTime=20170506000000,EndTime=20170510000000&&FEA0\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_POL_RCD      "##0124QN=20170516010101001;ST=22;CN=2071;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&BeginTime=20170506010001,EndTime=20170506150030&&2EF7\r\n"
#define ZHBHJT_MSG_TEST_DATA_NOTIFY           "##0076ST=91;CN=9013;PW=123456;MN=ABCDEFG1234567;Flag=0;CP=&&QN=20170516010101001&&1CBA\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_LIM          "##0142QN=20170516010101001;ST=22;CN=1022;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&101-LowValue=1.1,101-UpValue=9.9;102-LowValue=1.1,102-UpValue=9.9&&D413\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_LIM          "##0096QN=20170516010101001;ST=22;CN=1021;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&PolId=101;PolId=102&&7FAA\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_ADDR         "##0096QN=20170516010101001;ST=22;CN=1032;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&AlarmTarget=3882566&&B83C\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_ADDR         "##0077QN=20170516010101001;ST=22;CN=1031;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&2533\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_CTIME        "##0092QN=20170516010101001;ST=22;CN=1042;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&ReportTime=0101&&7201\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_RPT_TIME     "##0077QN=20170516010101001;ST=22;CN=1041;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&D6C7\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_ZERO         "##0086QN=20170516010101001;ST=22;CN=3011;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&PolId=101&&8CF8\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_GAP_TIME     "##0091QN=20170516010101001;ST=22;CN=1062;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&RtdInterval=30&&EF98\r\n"
#define ZHBHJT_MSG_TEST_DATA_GET_GAP_TIME     "##0077QN=20170516010101001;ST=22;CN=1061;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&&&77E5\r\n"
#define ZHBHJT_MSG_TEST_DATA_INIT_OT          "##0097QN=20170516010101001;ST=22;CN=1000;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&OverTime=5;ReCount=3&&2D61\r\n"
#define ZHBHJT_MSG_TEST_DATA_INIT_WARNING     "##0087QN=20170516010101001;ST=22;CN=1001;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&WarnTime=5&&0323\r\n"
#define ZHBHJT_MSG_TEST_DATA_INST_SMLP        "##0086QN=20170516010101001;ST=22;CN=3012;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&PolId=101&&2253\r\n"
#define ZHBHJT_MSG_TEST_DATA_SET_SLMP_CYCLE   "##0122QN=20170516010101001;ST=22;CN=3014;PW=123456;MN=ABCDEFG1234567;Flag=3;CP=&&PolId=101,CTime=04,CTime=10,CTime=14,CTime=16&&526F\r\n"
#define ZHBHJT_MSG_TEST_DATA_SEND_MIN_RPT1    "QN=20170303200101001;ST=22;CN=2011;PW=123456;MN=AQHJ0QP0100001;CP=&&DataTime=20170303200101001;a34001-Avg=46.6,a34001-Max=47.6,a34001-Min=46.0,a34001-Flag=N;a50001-Avg=50.6,a50001-Max=67.6,a50001-Min=46.0,a50001-Flag=N;a01001-Avg=7.2,a01001-Max=9.70;a01001-Min=2.3,a01001-Flag=N;a01002-Avg=33.0,a01002-Max=45.0;a01002-Min=12.0,a01002-Flag=N;a01006-Avg=33.0,a01006-Max=45.0,a01006-Min=23.0,a01006-Flag=N;a01007-Avg=45.0,a01007-Max=46.0,a01007-Min=30.0,a01007-Flag=N;a01008-Avg=0.69,a01008-Max=0.79,a01008-Min=0.59,a01008-Flag=N&&"

#define ZHBHJT_MSG_TEST_DATA_SEND_MIN_RPT2    "QN=20160715025900000;ST=22;CN=2011;PW=123456;MN=SHHBZB1111;CP=&&DataTime=20160715025900;a34001-Avg=0.000,a34001-Max=0.000,a34001-Min=0.000,a34001-Flag=N;a50001-Avg=30.0,a50001-Max=30.0,a50001-Min=30.0,a50001-Flag=N;a01001-Avg=25.3,a01001-Max=25.3,a01001-Min=25.3,a01001-Flag=N;a01002-Avg=88.6,a01002-Max=88.6,a01002-Min=88.6,a01002-Flag=N;a01006-Avg=0.0,a01006-Max=0.0,a01006-Min=0.0,a01006-Flag=N;a01007-Avg=0.0,a01007-Max=0.0,a01007-Min=0.0,a01007-Flag=D;a01008-Avg=348,a01008-Max=348,a01008-Min=348,a01008-Flag=N&&"

//数据处理的宏
#define ZHBHJT_PRINT_SIZE_AND_CRC(s)   do{UINT16 crc16=0;\
	hcu_vm_calculate_crc_modbus((UINT8*)(s+6), strlen(s)-12, &crc16);\
	printf("ZHB: SIZE=%04d, CRC=%04X\n", strlen(s)-12, crc16);\
	}while(0)

#endif /* _ZHBHJT_MSG_IE_DEFINITION_H_ */
