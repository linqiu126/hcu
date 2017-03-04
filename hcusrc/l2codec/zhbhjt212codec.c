/**
 ****************************************************************************************
 *
 * @file ZHBHJT212CODEC.c
 *
 * @brief ZHBHJT212CODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "zhbhjt212codec.h"
#include "../l0service/trace.h"

//Global variables

//Task Global variables
extern gTaskCloudvelaContext_t gTaskCloudvelaContext;



/*
 *
 *
 *   IE/IECMB，信息单元／信息复合单元的构造
 *
 *
 */


//静态表
#define ZHBHJT_PFDT_POLID_NAME_ID_MAX 103  //这里的静态定义必须正好是该静态数组的长度，不然后期程序会出错，因为这个也是判定条件
const char gZhbhjtPolIdName[ZHBHJT_PFDT_POLID_NAME_ID_MAX][ZHBHJT_PFDT_POLID_NAME_LEN_MAX] = {
	"MIN", "B03",	"L10",	"L5",	"L50",	"L90",	"L95",	"Ld",	"Ldn",	"Leq",	"LMn",	"LMx",	"Ln",	"S01",	"S02",	"S03",	"S04",
	"S05",	"S06",	"S07",	"S08",	"B02",	"01",	"02",	"03",	"04",	"05",	"06",	"07",	"08",	"09",	"10",	"11",	"12",	"13",
	"14",	"15",	"16",	"17",	"18",	"19",	"20",	"21",	"22",	"23",	"24",	"25",	"26",	"27",	"28",	"29",	"30",	"31",
	"32",	"33",	"34",	"35",	"36",	"37",	"99",	"B01",	"001",	"002",	"003",	"010",	"011",	"015",	"020",	"021",	"022",	"023",
	"024",	"025",	"026",	"027",	"028",	"029",	"030",	"031",	"032",	"033",	"034",	"035",	"036",	"037",	"038",	"039",	"040",	"041",
	"060",	"061",	"065",	"080",	"101",	"a01001",	"a01002",	"a01006",	"a01007",	"a01008",	"a34001",	"a50001",	"102",  "MAX",
};

//静态配置ZHBHJT212协议的收发消息格式
//IE静态数据单元
ZHBHJT212MsgIeEleStaticCfg_t gZhbhjtIeEleCfg[] = {
	{"",                 ZHBHJT_PFDT_CHAR,     0,   0},
	{"SystemTime=",      ZHBHJT_PFDT_INT32,    14,  0},
	{"QN=",              ZHBHJT_PFDT_INT64,    20,  0},
	{"CN=",              ZHBHJT_PFDT_INT16,    4,   0},
	{"PNUM=",            ZHBHJT_PFDT_INT16,    4,   0},
	{"PNO=",             ZHBHJT_PFDT_INT16,    4,   0},
	{"QnRtn=",           ZHBHJT_PFDT_UINT8,    3,   0},
	{"ExeRtn=",          ZHBHJT_PFDT_UINT8,    3,   0},
	{"RtdInterval=",     ZHBHJT_PFDT_INT16,    4,   0},
	{"-Rtd=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Min=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Avg=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Max=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-ZsRtd=",          ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-ZsMin=",          ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-ZsAvg=",          ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-ZsMax=",          ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Flag=",           ZHBHJT_PFDT_CHAR,     1,   1},
	{"-Cou=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-RS=",             ZHBHJT_PFDT_UINT8,    1,   1},
	{"-RT=",             ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Ala=",            ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-UpValue=",        ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-LowValue=",       ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-Data=",           ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-DayData=",        ZHBHJT_PFDT_DBFLT,    18,  1},
	{"-NightData=",      ZHBHJT_PFDT_DBFLT,    18,  1},
	{"AlarmTime=",       ZHBHJT_PFDT_INT32,    14,  0},
	{"AlarmType=",       ZHBHJT_PFDT_UINT8,    1,   0},
	{"AlarmTarget=",     ZHBHJT_PFDT_INT64,    20,  0},
	{"PolId=",           ZHBHJT_PFDT_CHAR,     8,   0},
	{"BeginTime=",       ZHBHJT_PFDT_INT32,    14,  0},
	{"EndTime=",         ZHBHJT_PFDT_INT32,    14,  0},
	{"DataTime=",        ZHBHJT_PFDT_INT32,    14,  0},
	{"ReportTime=",      ZHBHJT_PFDT_INT16,    4,   0},
	{"DayStdValue=",     ZHBHJT_PFDT_INT32,    14,  0},
	{"NightStdValue=",   ZHBHJT_PFDT_INT32,    14,  0},
	{"PW=",              ZHBHJT_PFDT_CHAR,     6,   0},
	{"OverTime=",        ZHBHJT_PFDT_INT32,    5,   0},
	{"ReCount=",         ZHBHJT_PFDT_UINT8,    2,   0},
	{"WarnTime=",        ZHBHJT_PFDT_INT16,    5,   0},
	{"CTime=",           ZHBHJT_PFDT_UINT8,    2,   0},
	{"",                 ZHBHJT_PFDT_CHAR,     0,   0},
};

//组合IE的定义
/*
ZHBHJT212MsgIeCmbStaticCfg_t gZhbhjtIeCmbCfg[] = {
	{ZHBHJT_IEID_cmb_MIN,           {{0,                         0}, {0,                         0}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_gap_time,      {{ZHBHJT_IEID_uni_BeginTime, 1}, {ZHBHJT_IEID_uni_EndTime,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_alm_lim_rng,   {{ZHBHJT_IEID_uni_LowValue,  1}, {ZHBHJT_IEID_uni_UpValue,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_multi_ctime,   {{ZHBHJT_IEID_uni_PolId,     1}, {ZHBHJT_IEID_uni_CTime,     HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX},  {0,0},{0,                   0},},},
	{ZHBHJT_IEID_cmb_alarm_event,   {{ZHBHJT_IEID_uni_Ala,       1}, {ZHBHJT_IEID_uni_AlarmType, 1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_multi_rtd,     {{ZHBHJT_IEID_uni_Rtd,       1}, {ZHBHJT_IEID_uni_polFlag,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_pol_report,    {{ZHBHJT_IEID_uni_Cou,       1}, {ZHBHJT_IEID_uni_value_Min, 1}, {ZHBHJT_IEID_uni_value_Avg, 1}, {ZHBHJT_IEID_uni_value_Max,1},},},
	{ZHBHJT_IEID_cmb_MAX,           {{0,                         0}, {0,                         0}, {0,                        0}, {0,                         0},},},
};
*/
//test for shanchun

ZHBHJT212MsgIeCmbStaticCfg_t gZhbhjtIeCmbCfg[] = {
	{ZHBHJT_IEID_cmb_MIN,           {{0,                         0}, {0,                         0}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_gap_time,      {{ZHBHJT_IEID_uni_BeginTime, 1}, {ZHBHJT_IEID_uni_EndTime,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_alm_lim_rng,   {{ZHBHJT_IEID_uni_LowValue,  1}, {ZHBHJT_IEID_uni_UpValue,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_multi_ctime,   {{ZHBHJT_IEID_uni_PolId,     1}, {ZHBHJT_IEID_uni_CTime,     HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX},  {0,0},{0,                   0},},},
	{ZHBHJT_IEID_cmb_alarm_event,   {{ZHBHJT_IEID_uni_Ala,       1}, {ZHBHJT_IEID_uni_AlarmType, 1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_multi_rtd,     {{ZHBHJT_IEID_uni_Rtd,       1}, {ZHBHJT_IEID_uni_polFlag,   1}, {0,                        0}, {0,                         0},},},
	{ZHBHJT_IEID_cmb_pol_report,    {{ZHBHJT_IEID_uni_polFlag,       1}, {ZHBHJT_IEID_uni_value_Avg, 1}, {ZHBHJT_IEID_uni_value_Max, 1}, {ZHBHJT_IEID_uni_value_Min,1},},},
	{ZHBHJT_IEID_cmb_MAX,           {{0,                         0}, {0,                         0}, {0,                        0}, {0,                         0},},},
};



/*
 *
 *
 *   消息格式的编码构造采用初始化表格，因为结构的CAST机制用不上，而且内部消息采用容器机制，也没有对应关系。
 *
 * 　相比结构申明，这其实没有太大的不一样，但这种方式有可能避免每一个函数的单独处理，而采用较为复杂的处理算法
 *   它的问题是，需要为每一种IE和IECMB进行独立的处理，或者也将其构造出完善的处理方式，但同样算法比较复杂
 *
 *
 */

//下表的含义是，即便单IE结构，依然表达为项目循环，应该使用;进行分离。如果需要在单IE中进行重复组合，使用逗号进行区分，那就在组合IE区域，形成独立的复合IE结构，所以不存在不明确的情形
//消息定义：上行，送到服务器来的消息
ZHBHJT212MsgFormatStaticCfg_t gZhbhjtMsgFormatCfgUlResp[] = {
	{ZHBHJT_IE_uni_CNcode_MIN_0000                                , {{0,0},{0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_time_resp_1011            , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_SystemTime, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_resp_1021   , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_cmb_alm_lim_rng,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_addr_resp_1031            , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_AlarmTarget, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_data_report_time_resp_1041      , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_ReportTime, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_resp_1061         , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_RtdInterval, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_answer_resp_9011                    , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_QnRtn,    1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_operation_result_resp_9012          , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_ExeRtn,   1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_notification_resp_9013              , {{ZHBHJT_IEID_uni_CfmQN,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_rt_data_resp_2011               , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_cmb_multi_rtd,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_data_resp_9014                      , {{0,0},{0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_equ_status_resp_2021            , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_uni_RS,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_resp_2031    , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_cmb_pol_report,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_resp_2041        , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_uni_RT,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_resp_2051    , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_cmb_pol_report,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_resp_2061   , {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_cmb_pol_report,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_resp_2071, {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_uni_Ala,1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_event_resp_2072 , {{ZHBHJT_IEID_uni_AlarmTime,1}, {ZHBHJT_IEID_cmb_alarm_event,1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_MAX_9999                                , {{0,0},{0,0},{0,0},{0,0},},},
};
//ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_resp: Not exist, add temperay


//消息定义：下行，从服务器来的消息
ZHBHJT212MsgFormatStaticCfg_t gZhbhjtMsgFormatCfgDlReq[] = {
	{ZHBHJT_IE_uni_CNcode_MIN_0000                                , {{0,0},{0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_init_set_resend_time_out_req_1000       , {{ZHBHJT_IEID_uni_OverTime,   1},  {ZHBHJT_IEID_uni_ReCount,   1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_init_set_uplimit_duration_req_1001      , {{ZHBHJT_IEID_uni_WarnTime,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_time_req_1011             , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_par_set_field_time_req_1012             , {{ZHBHJT_IEID_uni_SystemTime,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_req_1021    , {{ZHBHJT_IEID_uni_PolId,   HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_set_pollution_limitaion_req_1022    , {{ZHBHJT_IEID_cmb_alm_lim_rng,   HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_addr_req_1031             , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_par_set_field_addr_req_1032             , {{ZHBHJT_IEID_uni_AlarmTarget,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_data_report_time_req_1041       , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_par_set_data_report_time_req_1042       , {{ZHBHJT_IEID_uni_ReportTime,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_req_1061          , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_par_set_rtdi_interval_req_1062          , {{ZHBHJT_IEID_uni_RtdInterval,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_set_access_pswd_req_1072            , {{ZHBHJT_IEID_uni_SetPwd,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_notification_req_9013               , {{ZHBHJT_IEID_uni_CfmQN,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_cmd_data_req_9014                       , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_CfmCN,   1},{ZHBHJT_IEID_uni_CfmPNUM,   0xFF},{ZHBHJT_IEID_uni_CfmPNO,   0xFF},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_rt_data_req_2011                , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_dat_stop_monitor_rt_data_req_2012       , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_dat_get_equ_status_req_2021             , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_dat_stop_monitor_equ_status_req_2022    , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_req_2031     , {{ZHBHJT_IEID_cmb_gap_time,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_req_2041         , {{ZHBHJT_IEID_cmb_gap_time,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_req_2051     , {{ZHBHJT_IEID_cmb_gap_time,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_req_2061    , {{ZHBHJT_IEID_cmb_gap_time,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_req_2071 , {{ZHBHJT_IEID_cmb_gap_time,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_ctr_calabration_req_3011                , {{ZHBHJT_IEID_uni_PolId,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_ctr_start_sample_req_3012               , {{ZHBHJT_IEID_uni_PolId,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_ctr_equ_operation_req_3013              , {{0,0},{0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_ctr_set_sample_tti_req_3014             , {{ZHBHJT_IEID_cmb_multi_ctime,   1},  {0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_MAX_9999                                , {{0,0},{0,0},{0,0},{0,0},},},
};


//PACK函数入口
OPSTAT func_cloudvela_zhbhjt212_msg_pack(msg_struct_llczhb_cloudvela_frame_resp_t *inputPar, CloudDataSendBuf_t *output)
{
	int i=0, j = 0, k = 0, m=0, index = 0;
	char tmp[ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	char tmpSecond[ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	char ds[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char cps[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	UINT8 msgidIndex = 0;

	//因为是在ZHBHJT212函数体中，所以根据MN地址，自动设置目标服务器为业务服务器
	gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;

	//参数检查
	if ((inputPar == NULL) || (output == NULL))
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: input parameter error!\n");
	msgidIndex =  func_cloudvela_zhbhjt212_search_ul2cloud_msgid_by_cncode(inputPar->head.cn);
	if ((msgidIndex <= ZHBHJT_IE_uni_CNcode_MIN_0000) || (msgidIndex >= ZHBHJT_IE_uni_CNcode_MAX_9999))
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Un-supported message or CN code to be packed!\n");

	//先pack固定FRAME帧头部分
	memset(ds, 0, sizeof(ds));

	//请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	memset(tmp, 0, sizeof(tmp));
	HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms=%lu\n\n", inputPar->head.qn);
	func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(inputPar->head.qn, tmp);
	//func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(&inputPar->cfmQn, tmp);//LLC终结的消息立即返回时头里不带QN的值，L3回的消息头带QN??
	HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms=%s\n\n", tmp);
	strcat(ds, "QN=");
	strcat(ds, tmp);
	strcat(ds, ";");

	//ST=系统编号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "ST=%d;", inputPar->head.st);
	strcat(ds, tmp);

	//CN=命令编号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "CN=%d;", inputPar->head.cn);
	strcat(ds, tmp);

	//PW=访问密码
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "PW=%s;", inputPar->head.pw);
	strcat(ds, tmp);

	//MN=设备编号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "MN=%s;", zHcuSysEngPar.hwBurnId.zhbMnLable);
	strcat(ds, tmp);

	//FLAG应答标识：可能不存在
	if (inputPar->head.ansFlag != 0xFF){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "Flag=%d;", inputPar->head.ansFlag);
		strcat(ds, tmp);
	}

	//PNUM 指示本次通讯总共包含的包数：可能不存在
	if (inputPar->head.pnum != 0xFFFF){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "PNUM=%d;", inputPar->head.pnum);
		strcat(ds, tmp);
	}

	//PNO 指示当前数据包的包号：可能不存在
	if (inputPar->head.pno != 0xFFFF){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "PNO=%d;", inputPar->head.pno);
		strcat(ds, tmp);
	}

	//CP数据内容部分
	memset(cps, 0, sizeof(cps));

	//先将复合结构拷贝到单一IE区域，简化后续处理过程
	ZHBHJT212IeList_t ieList[ZHBHJT_PFM_CMB2MSG_NBR_MAX * HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	memset(&ieList, 0, sizeof(ZHBHJT212IeList_t)*ZHBHJT_PFM_CMB2MSG_NBR_MAX*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	UINT8 rprTimes = 0;
	//复合IE的多个区域
	for (i = 0; i < ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		index = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
		rprTimes = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbRptNbr;
		if (rprTimes == 0xFF) rprTimes = 1;  //表示是否存在的选项

		//为满秩的复合IE做独特处理=>这里是属于自行计算
		if (rprTimes == HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX){
			if (index == ZHBHJT_IEID_cmb_alm_lim_rng) rprTimes = inputPar->ul2Cloud.nbrOfAlmLim;
			else if (index == ZHBHJT_IEID_cmb_multi_rtd) rprTimes = inputPar->ul2Cloud.nbrOfCRtd;
			else if (index == ZHBHJT_IEID_cmb_pol_report) rprTimes = inputPar->ul2Cloud.nbrOfCMinRpt;
			else if (index == ZHBHJT_IEID_uni_RT) rprTimes = inputPar->ul2Cloud.nbrOfRT;
			else if (index == ZHBHJT_IEID_uni_RS) rprTimes = inputPar->ul2Cloud.nbrOfRS;
		}

		//单个IE结构
		if ((index > ZHBHJT_IEID_uni_MIN) && (index < ZHBHJT_IEID_uni_MAX) && (rprTimes > 0) && (rprTimes <= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX)){
			for (j=0; j<rprTimes; j++){
				ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[0].ieId = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
				ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[0].ieRptNbr = 1;
			}
		}
		//复合IE结构
		else if ((index > ZHBHJT_IEID_cmb_MIN) && (index < ZHBHJT_IEID_cmb_MAX) && (rprTimes > 0) && (rprTimes <= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX)){
			for (j=0; j<rprTimes; j++){
				UINT8 cmb2IeIndex = index - ZHBHJT_IEID_cmb_MIN;
				for (k=0; k<ZHBHJT_PFM_IE2CMB_NBR_MAX; k++){
					ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieId = (gZhbhjtIeCmbCfg[cmb2IeIndex].ie[k].ieId);
					ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr = (gZhbhjtIeCmbCfg[cmb2IeIndex].ie[k].ieRptNbr);
					//if ((index == ZHBHJT_IEID_cmb_multi_ctime) && (gZhbhjtIeCmbCfg[cmb2IeIndex].ie[k].ieRptNbr == HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX))
					//	ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr = inputPar->ul2Cloud.NbrofCTimes;
				}
			}
		}
	}

	//申请中间消息结构体，准备制造消息
	char sMsgBuild[ZHBHJT_PFM_CMB2MSG_NBR_MAX][HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX][ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	memset(sMsgBuild, 0, sizeof(char)*ZHBHJT_PFM_CMB2MSG_NBR_MAX*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX*ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX);
	UINT8 IeCmbIndex = 0;
	UINT16 PrjLen = 0;
	for (i=0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		IeCmbIndex = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
		for (j=0; j<HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX; j++){
			PrjLen = 0;
			for (k=0; k<ZHBHJT_PFM_IE2CMB_NBR_MAX; k++){
				index = ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieId;
				rprTimes = ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr;
				if ((index>ZHBHJT_IEID_uni_MIN) && (index<ZHBHJT_IEID_uni_MAX) && (rprTimes>0) && (rprTimes<=HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX)){
					for(m=0; m<rprTimes; m++){
						memset(tmp, 0, sizeof(tmp));
						memset(tmpSecond, 0, sizeof(tmpSecond));
						PrjLen++;  //计算长度
						//HCU_DEBUG_PRINT_INF("ZHBHJT212: Index=[%d], rptTimes = %d\n", index, rprTimes);
						switch(index){
						case ZHBHJT_IEID_uni_SystemTime:
							func_cloudvela_zhbhjt212_convert_u32time_to_ymd_wo_ms(inputPar->ul2Cloud.SystemTime, tmpSecond);
							sprintf(tmp, "%s%s,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_SystemTime].keyLable, tmpSecond);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_CfmQN:
							func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(inputPar->cfmQn, tmpSecond);
							sprintf(tmp, "%s%s,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_CfmQN].keyLable, tmpSecond);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_CfmCN:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_CfmPNUM:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_CfmPNO:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_QnRtn:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_QnRtn].keyLable, inputPar->ul2Cloud.QnRtn);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_ExeRtn:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_ExeRtn].keyLable, inputPar->ul2Cloud.ExeRtn);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_RtdInterval:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RtdInterval].keyLable, inputPar->ul2Cloud.RtdInterval);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_Rtd:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_multi_rtd){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Rtd].keyLable, inputPar->ul2Cloud.rtd[j].Rtd);
								strcat(sMsgBuild[i][j], tmp);
							}else{
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Rtd].keyLable, inputPar->ul2Cloud.rtd[j].Rtd);
								strcat(sMsgBuild[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_value_Min:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Min].keyLable, inputPar->ul2Cloud.min[j].Min);
								strcat(sMsgBuild[i][j], tmp);
								HCU_DEBUG_PRINT_INF("ZHBHJT212: Min j=%d, Polid=%d, name=%s, keylable=%s, min=%4.2f\n\n", j, inputPar->ul2Cloud.min[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId],gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Min].keyLable, inputPar->ul2Cloud.min[j].Min);

							}
							break;
						case ZHBHJT_IEID_uni_value_Avg:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Avg].keyLable, inputPar->ul2Cloud.min[j].Avg);
								strcat(sMsgBuild[i][j], tmp);
								HCU_DEBUG_PRINT_INF("ZHBHJT212: Avg j=%d, Polid=%d, name=%s, keylable=%s, avg=%4.2f\n\n", j, inputPar->ul2Cloud.min[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId],gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Avg].keyLable, inputPar->ul2Cloud.min[j].Avg);
							}
							break;
						case ZHBHJT_IEID_uni_value_Max:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Max].keyLable, inputPar->ul2Cloud.min[j].Max);
								strcat(sMsgBuild[i][j], tmp);
								HCU_DEBUG_PRINT_INF("ZHBHJT212: Max j=%d, Polid=%d, name=%s, keylable=%s, max=%4.2f\n\n", j, inputPar->ul2Cloud.min[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId],gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Max].keyLable, inputPar->ul2Cloud.min[j].Max);

							}
							break;
						case ZHBHJT_IEID_uni_ZsRtd:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_value_ZsMin:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_value_ZsAvg:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_value_ZsMax:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;

						case ZHBHJT_IEID_uni_polFlag:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_multi_rtd){
								sprintf(tmp, "%s%s%c,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_polFlag].keyLable, inputPar->ul2Cloud.rtd[j].PolFlag);
								strcat(sMsgBuild[i][j], tmp);
							}

							else if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%c,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_polFlag].keyLable, inputPar->ul2Cloud.min[j].PolFlag);
								strcat(sMsgBuild[i][j], tmp);
								HCU_DEBUG_PRINT_INF("ZHBHJT212: Flag j=%d, Polid=%d, name=%s, keylable=%s, polflag=%c\n\n", j, inputPar->ul2Cloud.min[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId],gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_polFlag].keyLable, inputPar->ul2Cloud.min[j].PolFlag);

							}

							break;

							//test by shanchun
							/*
						case ZHBHJT_IEID_uni_polFlag:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%c,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_polFlag].keyLable, inputPar->ul2Cloud.min[j].PolFlag);
								strcat(sMsgBuild[i][j], tmp);
							}
							break;
							*/


						case ZHBHJT_IEID_uni_Cou:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								//sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Cou].keyLable, inputPar->ul2Cloud.min[j].Cou);
								//strcat(sMsgBuild[i][j], tmp);
							}
							break;

						case ZHBHJT_IEID_uni_RS:
							sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.RS[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RS].keyLable, inputPar->ul2Cloud.RS[j].RS);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_RT:
							sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.RT[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RT].keyLable, inputPar->ul2Cloud.RT[j].RT);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_Ala:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alarm_event){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.AlarmEvent.PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Ala].keyLable, inputPar->ul2Cloud.AlarmEvent.Ala);
								strcat(sMsgBuild[i][j], tmp);
							}else{
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.Ala.PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Ala].keyLable, inputPar->ul2Cloud.Ala.Ala);
								strcat(sMsgBuild[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_UpValue:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alm_lim_rng){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_UpValue].keyLable, inputPar->ul2Cloud.limitation[j].UpValue);
								strcat(sMsgBuild[i][j], tmp);
								//HCU_DEBUG_PRINT_INF("ZHBHJT212: Upvalue j=%d, Polid=%d, name=%s\n", j, inputPar->ul2Cloud.limitation[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[j].PolId]);
							}
							break;
						case ZHBHJT_IEID_uni_LowValue:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alm_lim_rng){
								sprintf(tmp, "%s%s%4.2f,", gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[j].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_LowValue].keyLable, inputPar->ul2Cloud.limitation[j].LowValue);
								strcat(sMsgBuild[i][j], tmp);
								//HCU_DEBUG_PRINT_INF("ZHBHJT212: Lowvalue j=%d, Polid=%d, name=%s\n", j, inputPar->ul2Cloud.limitation[j].PolId, gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[j].PolId]);
							}
							break;
						case ZHBHJT_IEID_uni_Data:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_DayData:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_NightData:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_AlarmTime:
							func_cloudvela_zhbhjt212_convert_u32time_to_ymd_wo_ms(inputPar->ul2Cloud.AlarmTime, tmpSecond);
							sprintf(tmp, "%s%s,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmTime].keyLable, tmpSecond);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_AlarmType:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmType].keyLable, inputPar->ul2Cloud.AlarmType);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_AlarmTarget:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmTarget].keyLable, inputPar->ul2Cloud.AlarmTarget);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_PolId:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_BeginTime:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_EndTime:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_DataTime:
							func_cloudvela_zhbhjt212_convert_u32time_to_ymd_wo_ms(inputPar->ul2Cloud.DataTime, tmpSecond);
							sprintf(tmp, "%s%s,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_DataTime].keyLable, tmpSecond);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_ReportTime:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_ReportTime].keyLable, inputPar->ul2Cloud.ReportTime);
							strcat(sMsgBuild[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_DayStdValue:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_NightStdValue:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_SetPwd:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_OverTime:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_ReCount:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_WarnTime:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						case ZHBHJT_IEID_uni_CTime:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						default:
							HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
							break;
						}//switch(index)
					}//重复rprTimes
				}//满足进入的条件
			}//k，表达的是子项目重复
			if ((PrjLen > 0) && (strlen(sMsgBuild[i][j]) > 0)){
				//附上;项目分隔符号，而且是将最后的一个逗号改为分号即可
				if (sMsgBuild[i][j][strlen(sMsgBuild[i][j])-1] == ',') {sMsgBuild[i][j][strlen(sMsgBuild[i][j])-1] = ';'; }
				//HCU_DEBUG_PRINT_INF("ZHBHJT212: I/J=%d/%d, String=[%s]\n", i, j, sMsgBuid[i][j]);
			}
		}//单独IE
	}//复合组合

	//全部组合
	for (i=0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		for (j=0; j<HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX; j++){
			strcat(cps, sMsgBuild[i][j]);
			//HCU_DEBUG_PRINT_INF("ZHBHJT212: sMsgBuild[%d][%d]=%s\n", i,j,sMsgBuild[i][j]);
		}
	}
	//去掉最后一个分号
	if ((strlen(cps) > 0) && (cps[strlen(cps)-1] == ';'))  cps[strlen(cps)-1] = '\0';

	//组合数据部分
    //test by shanchun
	UINT16 dslen = strlen(ds);
	UINT16 cpslen = strlen(cps);
	HCU_DEBUG_PRINT_INF("ZHBHJT212: lengh of DS=[%d]\n\n", dslen);
	HCU_DEBUG_PRINT_INF("ZHBHJT212: lengh of CPS=[%d]\n\n", cpslen);
	HCU_DEBUG_PRINT_INF("ZHBHJT212: DS=[%s]\n\n", ds);
	HCU_DEBUG_PRINT_INF("ZHBHJT212: CPS=[%s]\n\n", cps);

	if ((strlen(ds) + strlen(cps) + 20) > HCU_SYSMSG_COM_MSG_BODY_LEN_MAX) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: pack error!\n");
	strcat(ds, "CP=&&");
	strcat(ds, cps);
	strcat(ds, "&&");

	//计算长度
	UINT16 len = strlen(ds);
	if (len > ZHBHJT_PROTOCOL_FRAME_DATA_LEN_MAX ) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: pack error!\n");

	//构造输出
	sprintf(output->curBuf, "%s%04d%s", ZHBHJT_PROTOCOL_FRAME_FIX_HEAD, len, ds);
	UINT16 crc16 = 0;
	hcu_vm_calculate_crc_modbus((UINT8*)ds, len, &crc16);
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04X", crc16);
	strcat(output->curBuf, tmp);
	strcat(output->curBuf, ZHBHJT_PROTOCOL_FRAME_FIX_TAIL);
	output->curLen = strlen(output->curBuf);

/*for test start
	memset(ds, 0, sizeof(ds));
	len = strlen(ZHBHJT_MSG_TEST_DATA_SEND_MIN_RPT1);
	strcat(ds, ZHBHJT_MSG_TEST_DATA_SEND_MIN_RPT1);
	sprintf(output->curBuf, "%s%04d%s", ZHBHJT_PROTOCOL_FRAME_FIX_HEAD, len, ds);
	crc16 = 0;
	hcu_vm_calculate_crc_modbus((UINT8*)ds, len, &crc16);
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04X", crc16);
	strcat(output->curBuf, tmp);
	strcat(output->curBuf, ZHBHJT_PROTOCOL_FRAME_FIX_TAIL);
	output->curLen = strlen(output->curBuf);
*/

	//测试打印
	HCU_DEBUG_PRINT_INF("ZHBHJT212: output=[%s]\n\n", output->curBuf);

	//返回
	return SUCCESS;
}


//UNPACK函数入口
OPSTAT func_cloudvela_zhbhjt212_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId)
{
	int i=0, j = 0, k = 0, m = 0, index = 0;
	UINT32 totalLen=0, it=0, tempLen = 0;
	UINT64 u64Tmp = 0;
	UINT8 msgidIndex = 0;
	char tmp[ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	char ds[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char cps[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char *p1, *p2, *p3;

	//因为是在ZHBHJT212函数体中，所以根据MN地址，自动设置目标服务器为业务服务器
	gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;

	//参数检查
	//忽略expectMsgId的输入条件，这个变量是为了兼容HUITPXML的函数格式
	if ((rcv == NULL) || (rcv->length != strlen(rcv->buf)) || (rcv->length < 12) || (rcv->length > HCU_SYSMSG_COM_MSG_BODY_LEN_MAX))
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data header!\n");

	index = 0;
	//取头部
	if ((rcv->buf[index] !='#')||(rcv->buf[index+1] !='#')) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data header!\n");
	index = index+2;

	//取尾巴  \r = 0D, \n = 0A, should be 0D 0A
	if ((rcv->buf[rcv->length-2] !='\r')||(rcv->buf[rcv->length-1] !='\n')) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data header!\n");

	//取长度
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &rcv->buf[index], 4);
	it = strtoul(tmp, NULL, 10); //10进制
	index = index + 4;
	totalLen = rcv->length-12;  //12字符长度的固定包头，这个表示了cp域的具体长度
	if (it != totalLen) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data length!\n");

	//取CRC并做比较判断
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &rcv->buf[rcv->length-6], 4);
	it = strtoul(tmp, NULL, 16);  //16进制
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus((UINT8*)&rcv->buf[index], rcv->length-12, &crc16);
	if (it != crc16) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data CRC16!\n");

	//申明输出结构
	msg_struct_cloudvela_llczhb_frame_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_llczhb_frame_req_t));
	snd.length = sizeof(msg_struct_cloudvela_llczhb_frame_req_t);

	//取数据区的请求编码：CP具备长度
	memset(ds, 0, sizeof(ds));
	memset(cps, 0, sizeof(cps));
	p1 = strstr(&rcv->buf[index], "CP=&&");
	if (p1 != NULL) p2 = p1 + strlen("CP=&&"); else p2 = NULL;
	if (p2!=NULL) p3 = strstr(p2, "&&"); else p3 =NULL;
	if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p2 > p3))
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data CP domain!\n");
	tempLen = p1 - &(rcv->buf[index]);
	strncpy(ds, &(rcv->buf[index]), tempLen);
	tempLen = p3 - p2;
	strncpy(cps, p2, tempLen);

	//解MN：不能为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "MN=");
	if (p1!=NULL) p2 = p1 + strlen("MN="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p2 >= p3)) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received MN head!\n");
	tempLen = p3 - p2;
	if (tempLen > 14) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received MN head!\n");
	strncpy(tmp, p2, tempLen);
	//if (strcmp(zHcuSysEngPar.hwBurnId.zhbMnLable, tmp) != 0) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received MN head!\n");

	//解QN域：允许为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "QN=");
	if (p1!=NULL) p2 = p1 + strlen("QN="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 != NULL) && (p2 != NULL) && (p3 != NULL) && (p3 > p2)){
		tempLen = p3 - p2;
		if (tempLen != 17) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received QN head!\n");
		strncpy(tmp, p2, tempLen);
		HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_ymd_with_ms_to_u64time=%s\n\n", tmp);//for test
		func_cloudvela_zhbhjt212_convert_ymd_with_ms_to_u64time(tmp, &u64Tmp);
		snd.head.qn = u64Tmp;
		HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_ymd_with_ms_to_u64time=%lu\n", u64Tmp);//for test

		//for test by shanchun start
		memset(tmp, 0, sizeof(tmp));
		func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(snd.head.qn, tmp);
		HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms=%s\n\n", tmp);

		memset(tmp, 0, sizeof(tmp));
		u64Tmp = 1488519070;
		func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(u64Tmp, tmp);
		HCU_DEBUG_PRINT_INF("CODEC: func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms from =%lu to =%s\n\n", u64Tmp, tmp);


	}

	//解ST领域：不允许为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "ST=");
	if (p1!=NULL) p2 = p1 + strlen("ST="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p2 >= p3)) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received ST head!\n");
	tempLen = p3 - p2;
	if (tempLen > 5) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received ST head!\n");
	strncpy(tmp, p2, tempLen);
	it = strtoul(tmp, NULL, 10) & 0xFFFF;
	if ((it != ZHBHJT_IE_uni_STcode_system_interaction) && (it != HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE))
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received ST head!\n");
	snd.head.st = it;

	//解CN领域：不允许为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "CN=");
	if (p1!=NULL) p2 = p1 + strlen("CN="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p2 >= p3)) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received CN head!\n");
	tempLen = p3 - p2;
	if (tempLen > 7) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received QN head!\n");
	strncpy(tmp, p2, tempLen);
	it = strtoul(tmp, NULL, 10) & 0xFFFF;
	//这里采用搜索技巧，不在静态表中的CNcode，都是非法CNcode
	msgidIndex = func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(it);
	i = func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(ZHBHJT_IE_uni_CNcode_MIN_0000);
	j = func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(ZHBHJT_IE_uni_CNcode_MAX_9999);
	if ((msgidIndex == i) || (msgidIndex == j)) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received QN head!\n");
	snd.head.cn = it;

	//解密码：允许为空，交给LLC上层控制
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "PW=");
	if (p1!=NULL) p2 = p1 + strlen("PW="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p2 > p3)) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received PW head!\n");
	tempLen = p3 - p2;
	if (tempLen > 6) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received PW head!\n");
	strncpy(snd.head.pw, p2, tempLen);

	//解PNUM领域：可以为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "PNUM=");
	if (p1!=NULL) p2 = p1 + strlen("PNUM="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 != NULL) && (p2 != NULL) && (p3 != NULL) && (p3 > p2)){
		tempLen = p3 - p2;
		if (tempLen > 4) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received PNUM head!\n");
		strncpy(tmp, p2, tempLen);
		snd.head.pnum = strtoul(tmp, NULL, 10) & 0xFFFF;
	}else{
		snd.head.pnum = 0xFFFF;
	}

	//解PNO领域：可以为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "PNO=");
	if (p1!=NULL) p2 = p1 + strlen("PNO="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 != NULL) && (p2 != NULL) && (p3 != NULL) && (p3 > p2)){
		tempLen = p3 - p2;
		if (tempLen > 4) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received PNO head!\n");
		strncpy(tmp, p2, tempLen);
		snd.head.pno = strtoul(tmp, NULL, 10) & 0xFFFF;
	}else{
		snd.head.pno = 0xFFFF;
	}

	//解Flag领域：可以为空
	memset(tmp, 0, sizeof(tmp));
	p1 = strstr(ds, "Flag=");
	if (p1!=NULL) p2 = p1 + strlen("Flag="); else p2=NULL;
	if (p2!=NULL) p3 = strstr(p2, ";"); else p3 =NULL;
	if ((p1 != NULL) && (p2 != NULL) && (p3 != NULL) && (p3 > p2)){
		tempLen = p3 - p2;
		if (tempLen > 3) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received Flag head!\n");
		strncpy(tmp, p2, tempLen);
		snd.head.ansFlag = strtoul(tmp, NULL, 10) & 0xFF;
	}else{
		snd.head.ansFlag = 0xFF;
	}

	//准备解cps域，并按照;的项目切到数组中去
	if (strlen(cps) == 0){
		if(hcu_message_send(MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ, TASK_ID_LLCZHB, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		return SUCCESS;
	}

	//先切分到项目中
	#define ZHBHJT_PFM_CMBIE_MAX    ZHBHJT_PFM_CMB2MSG_NBR_MAX * HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX
	char sMsgPrj[ZHBHJT_PFM_CMBIE_MAX][ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	memset(sMsgPrj, 0, sizeof(char)*ZHBHJT_PFM_CMBIE_MAX*ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX);
	index = 0;
	totalLen = strlen(cps);
	p1 = (char*)&(cps[0]);

	while (totalLen > 0){
		p2 = strstr(p1, ";");
		//最后一段
		if (p2 == NULL){
			strncpy(sMsgPrj[index], p1, strlen(p1)<ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX?strlen(p1):ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX);
			totalLen = 0;
		}
		//太长长度的字符串，非法
		else if ((p2-p1) >= ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX){
			HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
		}
		//正常长度
		else{
			tempLen = p2 - p1;
			strncpy(sMsgPrj[index], p1, tempLen);
			p1 = p2+1;
			totalLen = totalLen - tempLen -1;
			index++;
			//太长非法
			if (index >= ZHBHJT_PFM_CMBIE_MAX)
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
		}
	}

	//再切分到子项目中：LV1考虑到循环数量的复合IE结构，LV2是因为还存在类似于CTime的变态重复
	char sMsgCut[ZHBHJT_PFM_CMBIE_MAX][HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX][ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	memset(sMsgCut, 0, sizeof(char)*ZHBHJT_PFM_CMBIE_MAX*HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX*ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX);
	for (i=0; i<ZHBHJT_PFM_CMBIE_MAX; i++){
		if (strlen(sMsgPrj[i]) == 0) continue;
		p1 = (char*)&(sMsgPrj[i][0]);
		totalLen = strlen(p1);
		index = 0;
		while(totalLen>0){
			p2 = strstr(p1, ",");
			//最后一段
			if (p2 == NULL){
				strcpy(sMsgCut[i][index], p1);
				totalLen = 0;
			}
			//无效的逗号分隔符
			else if (p2 == p1){
				p1 = p2+1;
				totalLen = totalLen - 1;
			}
			//正常长度
			else{
				tempLen = p2 - p1 - 1;
				strncpy(sMsgCut[i][index], p1, tempLen);
				p1 = p2+1;
				totalLen = totalLen - tempLen -1;
				index++;
				//太长非法
				if (index >= HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			}
		}//while(totalLen>0){
	}//i循环

	//转化数据
	UINT32 u32Tmp = 0;
	INT32 int32Tmp = 0;
	UINT8 u8Tmp = 0;
	UINT8 PolId = 0;
	float floatTmp = 0;
	UINT8 limNewRoundFlag = 0;
	for (i=0; i<ZHBHJT_PFM_CMBIE_MAX; i++){
		for(j=0; j<HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX; j++){
			if (strlen(sMsgCut[i][j]) == 0) continue;
			//找到了第k个
			p2 = NULL;
			for(k=1; k<ZHBHJT_IEID_uni_MAX; k++){
				p2 = strstr(sMsgCut[i][j], gZhbhjtIeEleCfg[k].keyLable);
				if(p2 != NULL) break;
			}
			//如果未能找到(或者p2==NULL)
			if (k == ZHBHJT_IEID_uni_MAX) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");

			//找到了，先确定长度是否合法
			totalLen = strlen(sMsgCut[i][j]);
			tempLen = strlen(gZhbhjtIeEleCfg[k].keyLable);
			if (gZhbhjtIeEleCfg[k].backwardMatchFlag == 1) tempLen += ZHBHJT_PFDT_POLID_NAME_LEN_MAX;
			tempLen += gZhbhjtIeEleCfg[k].maxLen;
			if (totalLen > tempLen) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			p1 = (char*)&(sMsgCut[i][j][0]);
			p3 = p2 + strlen(gZhbhjtIeEleCfg[k].keyLable);
			tempLen = totalLen - (p3 - p1);
			//printf("String[%s/%s], i=%d/j=%d/k=%d, p1=%x, p2=%x, p3=%x, tempLen=%d, totalLen = %d\n", sMsgCut[i][j], sMsgCut[i+1][j],i, j, k, (UINT32)p1, (UINT32)p2, (UINT32)p3, tempLen, totalLen);
			//严格检查前后字符串的长度
			if ((p2 == NULL) || (p3 == NULL) || (p2 >= p3) || (tempLen == 0) || (tempLen > 20) || ((p2-p1) > ZHBHJT_PFDT_POLID_NAME_LEN_MAX))
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			//前缀只有在本该有前缀的时候有
			if ((((p2-p1) > 0) && (gZhbhjtIeEleCfg[k].backwardMatchFlag == 0)) || (((p2-p1) == 0) && (gZhbhjtIeEleCfg[k].backwardMatchFlag == 1)))
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			//不得超过规定的最长长度
			if (tempLen > gZhbhjtIeEleCfg[k].maxLen) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			//寻找PolId
			if ((p2-p1) > 0){
				memset(tmp, 0, sizeof(tmp));
				strncpy(tmp, p1, (p2-p1));
				PolId = func_cloudvela_zhbhjt212_search_polid_by_name(tmp);
				if ((PolId == 0) || (PolId == (ZHBHJT_PFDT_POLID_NAME_ID_MAX-1))) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			}
			//转化
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, p3, tempLen);
			if (gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_CHAR){
			}else if (gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_UINT8){
				u8Tmp = strtoul(tmp, NULL, 10) & 0xFF;
			}else if ((gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_INT16) || (gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_INT32)){
				if ((k == ZHBHJT_IEID_uni_SystemTime) || (k == ZHBHJT_IEID_uni_AlarmTime) || (k == ZHBHJT_IEID_uni_BeginTime) \
						|| (k == ZHBHJT_IEID_uni_EndTime) || (k == ZHBHJT_IEID_uni_DataTime)) {
					u32Tmp = 0;
					func_cloudvela_zhbhjt212_convert_ymd_wo_ms_to_u32time(tmp, &u32Tmp);
				}else{
					int32Tmp = strtoul(tmp, NULL, 10) & 0xFFFFFFFF;
				}
			}else if (gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_INT64){
				if (k == ZHBHJT_IEID_uni_CfmQN) {
					u64Tmp = 0;
					func_cloudvela_zhbhjt212_convert_ymd_with_ms_to_u64time(tmp, &u64Tmp);
				}else if (k == ZHBHJT_IEID_uni_AlarmTarget){  //目前暂时是将AlarmTarget转化为INT32的
					int32Tmp = strtoul(tmp, NULL, 10) & 0xFFFFFFFF;
				}else {
					HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				}
			//暂时不考虑双精度的浮点，实际中并没有太多的价值
			}else if ((gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_FLOAT) || (gZhbhjtIeEleCfg[k].dataType == ZHBHJT_PFDT_DBFLT)){
				floatTmp = strtof(tmp, NULL);
			}else{
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
			}

			//再分类处理
			switch(k){
			case ZHBHJT_IEID_uni_SystemTime:
				snd.dl2Self.SystemTime = u32Tmp;
				break;
			case ZHBHJT_IEID_uni_CfmQN:
				snd.cfmQn = u64Tmp;
				break;
			case ZHBHJT_IEID_uni_CfmCN:
				snd.cfmCN = int32Tmp & 0xFFFF;
				break;
			case ZHBHJT_IEID_uni_CfmPNUM:
				snd.cfmPNUM = int32Tmp & 0xFFFF;
				break;
			case ZHBHJT_IEID_uni_CfmPNO:
				snd.cfmPNO = int32Tmp & 0xFFFF;
				break;
			case ZHBHJT_IEID_uni_QnRtn:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_ExeRtn:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_RtdInterval:
				snd.dl2Self.RtdInterval = int32Tmp;
				break;
			case ZHBHJT_IEID_uni_Rtd:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_Min:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_Avg:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_Max:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_ZsRtd:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_ZsMin:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_ZsAvg:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_value_ZsMax:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_polFlag:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_Cou:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_RS:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_RT:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_Ala:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_UpValue:
				if (snd.dl2Self.nbrOfLimitation >= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				if (limNewRoundFlag == 0){
					 limNewRoundFlag = 1;
					 snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].UpValue = floatTmp;
					 snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].PolId = PolId;
				}else{
					limNewRoundFlag = 0;
					if (snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].PolId != PolId) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
					snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].UpValue = floatTmp;
					snd.dl2Self.nbrOfLimitation++;
				}
				break;
			case ZHBHJT_IEID_uni_LowValue:
				if (snd.dl2Self.nbrOfLimitation >= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				if (limNewRoundFlag == 0){
					 limNewRoundFlag = 1;
					 snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].LowValue = floatTmp;
					 snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].PolId = PolId;
				}else{
					limNewRoundFlag = 0;
					if (snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].PolId != PolId) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
					snd.dl2Self.limitation[snd.dl2Self.nbrOfLimitation].LowValue = floatTmp;
					snd.dl2Self.nbrOfLimitation++;
				}
				break;
			case ZHBHJT_IEID_uni_Data:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_DayData:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_NightData:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_AlarmTime:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_AlarmType:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_AlarmTarget:
				snd.dl2Self.AlarmTarget = int32Tmp;
				break;
			case ZHBHJT_IEID_uni_PolId:
				//独特的技巧：sMsgCut[i][0]必须是PolId，sMsgCut[i][1]以后必须是CTime，从而将polId分开
				//如果只有单独的PolId在sMsgCut[i][0]，这意味着它是PolId，而不是Ctime
				PolId = func_cloudvela_zhbhjt212_search_polid_by_name(tmp);
				if ((PolId <=0) || (PolId >= (ZHBHJT_PFDT_POLID_NAME_ID_MAX-1))) HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				if ((strstr(sMsgCut[i][1], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_CTime].keyLable) != NULL) && (j == 0)){
					snd.dl2Self.ctime.PolId = PolId;
				}else if (snd.dl2Self.nbrOfPolId < HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX){
					//不考虑同时存在单个和多个的情况：此时，单个和多个，是跟消息的配置静态表相关
					UINT8 flagNbr = 0;
					for (m=0; m<ZHBHJT_PFM_CMB2MSG_NBR_MAX; m++){
						if (gZhbhjtMsgFormatCfgDlReq[msgidIndex].IeCmb[m].ieCmbId == ZHBHJT_IEID_uni_PolId)
							flagNbr = gZhbhjtMsgFormatCfgDlReq[msgidIndex].IeCmb[m].ieCmbRptNbr;
					}
					if (flagNbr == 1) snd.dl2Self.singlePolId = PolId;
					else if (flagNbr > 1){
						snd.dl2Self.multiPolid[snd.dl2Self.nbrOfPolId] = PolId;
						snd.dl2Self.nbrOfPolId++;
					}else{
						HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
					}
				}else{
					HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				}
				break;
			case ZHBHJT_IEID_uni_BeginTime:
				snd.dl2Self.gapTime.BeginTime = u32Tmp;
				break;
			case ZHBHJT_IEID_uni_EndTime:
				snd.dl2Self.gapTime.EndTime = u32Tmp;
				break;
			case ZHBHJT_IEID_uni_DataTime:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_ReportTime:
				snd.dl2Self.ReportTime = int32Tmp & 0xFFFF;
				break;
			case ZHBHJT_IEID_uni_DayStdValue:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_NightStdValue:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			case ZHBHJT_IEID_uni_SetPwd:
				strncpy(snd.setpw, tmp, strlen(tmp)<sizeof(snd.setpw)?strlen(tmp):sizeof(snd.setpw));
				break;
			case ZHBHJT_IEID_uni_OverTime:
				snd.dl2Self.OverTime = int32Tmp;
				break;
			case ZHBHJT_IEID_uni_ReCount:
				snd.dl2Self.ReCount = u8Tmp;
				break;
			case ZHBHJT_IEID_uni_WarnTime:
				snd.dl2Self.WarnTime = int32Tmp;
				break;
			case ZHBHJT_IEID_uni_CTime:
				//独特的技巧：sMsgCut[i][0]必须是PolId，sMsgCut[i][1]以后必须是CTime，从而将polId分开
				//如果只有单独的PolId在sMsgCut[i][0]，这意味着它是PolId，而不是Ctime
				if ((snd.dl2Self.nbrOfCTime < HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX) && (strstr(sMsgCut[i][0], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_PolId].keyLable) != NULL)){
					snd.dl2Self.ctime.CTime[snd.dl2Self.nbrOfCTime] = u8Tmp;
					snd.dl2Self.nbrOfCTime++;
				}
				break;
			default:
				HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Invalid received data!\n");
				break;
			}
		}
	}

	//发送消息到上层LLC
	if(hcu_message_send(MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ, TASK_ID_LLCZHB, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_ZHBHJTCODEC("ZHBHJT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);

	//返回
	return SUCCESS;
}


//基础搜索函数
UINT8 func_cloudvela_zhbhjt212_search_polid_by_name(char *PolIdName)
{
	UINT8 index =0;
	while(strcmp(gZhbhjtPolIdName[index], "MAX") !=0){
		if (strcmp(gZhbhjtPolIdName[index], PolIdName) == 0) return index;
		index++;
	}
	return index;  //如果找不到，最后一项必然是MAX
}

UINT8 func_cloudvela_zhbhjt212_search_ul2cloud_msgid_by_cncode(UINT16 cnid)
{
	UINT8 index = 0;
	while(gZhbhjtMsgFormatCfgUlResp[index].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){
		if (gZhbhjtMsgFormatCfgUlResp[index].cnCode == cnid) return index;
		index++;
	}
	return index;  //如果找不到，最后一项必然是ZHBHJT_IE_uni_CNcode_MAX_9999
}

UINT8 func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(UINT16 cnid)
{
	UINT8 index =0;
	while(gZhbhjtMsgFormatCfgDlReq[index].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){
		if (gZhbhjtMsgFormatCfgDlReq[index].cnCode == cnid) return index;
		index++;
	}
	return index;  //如果找不到，最后一项必然是ZHBHJT_IE_uni_CNcode_MAX_9999
}

UINT16 func_cloudvela_zhbhjt212_caculate_ul2cloud_msg_size_max(UINT16 cnId)
{
	UINT16 totalRes = 0, ieRes = 0, IeCmbRes = 0;
	UINT8 MsgIndex = 0;
	UINT8 i=0, j=0;
	UINT8 IeOrIeCmbId = 0;
	UINT8 IePureId = 0;
	UINT8 IeCmbId = 0;

	//先找到目标消息的Index
	MsgIndex = func_cloudvela_zhbhjt212_search_ul2cloud_msgid_by_cncode(cnId);

	//然后通过gZhbhjtMsgFormatCfgUlResp寻找所有的IeId
	totalRes = 0;
	for (i = 0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		//单体IE处理方式
		IeOrIeCmbId = gZhbhjtMsgFormatCfgUlResp[MsgIndex].IeCmb[i].ieCmbId;
		IeCmbRes = 0;
		if (IeOrIeCmbId <= ZHBHJT_IEID_uni_MAX){
			//处理以下表格
			ieRes = 0;
			if (gZhbhjtIeEleCfg[IeOrIeCmbId].backwardMatchFlag == 1) ieRes += (ZHBHJT_PFDT_POLID_NAME_LEN_MAX + strlen(gZhbhjtIeEleCfg[IeOrIeCmbId].keyLable));
			else ieRes += strlen(gZhbhjtIeEleCfg[IeOrIeCmbId].keyLable);
			ieRes += gZhbhjtIeEleCfg[IeOrIeCmbId].maxLen;
			IeCmbRes = ieRes;
		}
		//复合IE处理方式
		else{
			//得到复合IECMB的相对Index
			IeCmbId = IeOrIeCmbId - ZHBHJT_IEID_cmb_MIN;
			for (j=0; j<ZHBHJT_PFM_IE2CMB_NBR_MAX; j++){
				ieRes = 0;
				IePureId = gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieId;
				if (gZhbhjtIeEleCfg[IePureId].backwardMatchFlag == 1) ieRes += (ZHBHJT_PFDT_POLID_NAME_LEN_MAX + strlen(gZhbhjtIeEleCfg[IePureId].keyLable));
				ieRes += gZhbhjtIeEleCfg[IePureId].maxLen;
				//IECMB的RepNbr=0xFF表示不定长
				if (gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieRptNbr == 0xFF) IeCmbRes += ieRes;
				else IeCmbRes += ieRes * gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieRptNbr ;
			}
		}//分类处理完成
		//MSG的RepNbr=0xFF表示不定长，对付IE以及IeCMB两种形态
		if (gZhbhjtMsgFormatCfgUlResp[MsgIndex].IeCmb[i].ieCmbRptNbr == 0xFF) totalRes += IeCmbRes;
		else totalRes += IeCmbRes * gZhbhjtMsgFormatCfgUlResp[MsgIndex].IeCmb[i].ieCmbRptNbr;
	}

	return totalRes;
}

UINT16 func_cloudvela_zhbhjt212_caculate_dl2hcu_msg_size_max(UINT16 cnId)
{
	UINT16 totalRes = 0, ieRes = 0, IeCmbRes = 0;
	UINT8 MsgIndex = 0;
	UINT8 i=0, j=0;
	UINT8 IeOrIeCmbId = 0;
	UINT8 IePureId = 0;
	UINT8 IeCmbId = 0;

	//先找到目标消息的Index
	MsgIndex = func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(cnId);

	//然后通过gZhbhjtMsgFormatCfgUlResp寻找所有的IeId
	totalRes = 0;
	for (i = 0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		//单体IE处理方式
		IeOrIeCmbId = gZhbhjtMsgFormatCfgDlReq[MsgIndex].IeCmb[i].ieCmbId;
		IeCmbRes = 0;
		if (IeOrIeCmbId <= ZHBHJT_IEID_uni_MAX){
			//处理以下表格
			ieRes = 0;
			if (gZhbhjtIeEleCfg[IeOrIeCmbId].backwardMatchFlag == 1) ieRes += (ZHBHJT_PFDT_POLID_NAME_LEN_MAX + strlen(gZhbhjtIeEleCfg[IeOrIeCmbId].keyLable));
			ieRes += strlen(gZhbhjtIeEleCfg[IeOrIeCmbId].keyLable);
			ieRes += gZhbhjtIeEleCfg[IeOrIeCmbId].maxLen;
			IeCmbRes = ieRes;
		}
		//复合IE处理方式
		else{
			//得到复合IECMB的相对Index
			IeCmbId = IeOrIeCmbId - ZHBHJT_IEID_cmb_MIN;
			for (j=0; j<ZHBHJT_PFM_IE2CMB_NBR_MAX; j++){
				IePureId = gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieId;
				ieRes = 0;
				if (gZhbhjtIeEleCfg[IePureId].backwardMatchFlag == 1) ieRes += (ZHBHJT_PFDT_POLID_NAME_LEN_MAX + strlen(gZhbhjtIeEleCfg[IePureId].keyLable));
				ieRes += gZhbhjtIeEleCfg[IePureId].maxLen;
				//IECMB的RepNbr=0xFF表示不定长
				if (gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieRptNbr == 0xFF) IeCmbRes += ieRes;
				else IeCmbRes += ieRes * gZhbhjtIeCmbCfg[IeCmbId].ie[j].ieRptNbr ;
			}
		}
		//MSG的RepNbr=0xFF表示不定长
		if (gZhbhjtMsgFormatCfgDlReq[MsgIndex].IeCmb[i].ieCmbRptNbr == 0xFF) totalRes += IeCmbRes;
		else totalRes += IeCmbRes * gZhbhjtMsgFormatCfgDlReq[MsgIndex].IeCmb[i].ieCmbRptNbr;
	}

	return totalRes;
}

//输出的out=17字节的字符串，否则出错
void func_cloudvela_zhbhjt212_convert_u64time_to_ymd_with_ms(UINT64 in, char *out)
{
	UINT32 time2Sec = 0, time2Milsec = 0;
	time2Sec = (UINT32)(in / 1000);
	time2Milsec = (UINT32)(in - (in / 1000)*1000);
	time_t lt = (time_t)time2Sec;
	struct tm *cu;
	cu = localtime(&lt);
	sprintf(out,  "%04d%02d%02d%02d%02d%02d%03d", (UINT16)(1900+cu->tm_year), (UINT8)(1+cu->tm_mon), (UINT8)cu->tm_mday,\
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec, time2Milsec);
	return;
}

//输出的out=14字节的字符串，否则出错
void func_cloudvela_zhbhjt212_convert_u32time_to_ymd_wo_ms(UINT32 in, char *out)
{
	struct tm *cu;
	time_t lt = (time_t)in;

	cu = localtime(&lt);
	sprintf(out, "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)(1+cu->tm_mon), (UINT8)cu->tm_mday, \
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	return;
}

//输入必须是17个字符串，输出的U64的数值，否则出错
void func_cloudvela_zhbhjt212_convert_ymd_with_ms_to_u64time(char *in, UINT64 *out)
{
	char s[5];
	struct tm cu;
	UINT16 t=0;
	UINT32 time2Milsec = 0;
	UINT64 tout = 0;

	int len = strlen (in);
	if ((len > 17) || (len < 12)) return;

	memset(&cu, 0, sizeof(struct tm));
	memset(s, 0, sizeof(s));
	strncpy(s, in, 4);
	t = strtoul(s, NULL, 10) & 0xFFFF;
	cu.tm_year = t-1900;

	memset(s, 0, sizeof(s));
	strncpy(s, in+4, 2);
	t = strtoul(s, NULL, 10) & 0xFF;
	cu.tm_mon = t-1;

	memset(s, 0, sizeof(s));
	strncpy(s, in+6, 2);
	t = strtoul(s, NULL, 10) & 0xFF;
	cu.tm_mday = t;

	if (len == 17){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+12, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+14, 3);
		t = strtoul(s, NULL, 10) & 0xFFFF;
		time2Milsec = t;
	}
	else if (len == 16){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+11, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+13, 3);
		t = strtoul(s, NULL, 10) & 0xFFFF;
		time2Milsec = t;
	}
	else if (len == 15){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+12, 3);
		t = strtoul(s, NULL, 10) & 0xFFFF;
		time2Milsec = t;
	}
	else if (len == 14){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+12, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		time2Milsec = 0;
	}
	else if (len == 13){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+11, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		time2Milsec = 0;
	}
	else if (len == 12){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;

		time2Milsec = 0;
	}

	time_t lt;

	lt = mktime(&cu);
	tout = lt * 1000 + time2Milsec;
	*out = tout;

	return;
}

//输入必须是14个字符串，输出的U32的数值，否则出错
void func_cloudvela_zhbhjt212_convert_ymd_wo_ms_to_u32time(char *in, UINT32 *out)
{
	char s[5];
	struct tm cu;
	UINT16 t=0;

	int len = strlen (in);
	if ((len > 14) && (len < 12)) return;

	memset(&cu, 0, sizeof(struct tm));
	memset(s, 0, sizeof(s));
	strncpy(s, in, 4);
	t = strtoul(s, NULL, 10) & 0xFFFF;
	cu.tm_year = t-1900;

	memset(s, 0, sizeof(s));
	strncpy(s, in+4, 2);
	t = strtoul(s, NULL, 10) & 0xFF;
	cu.tm_mon = t-1;

	memset(s, 0, sizeof(s));
	strncpy(s, in+6, 2);
	t = strtoul(s, NULL, 10) & 0xFF;
	cu.tm_mday = t;

	if (len == 14){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+12, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;
	}
	else if (len == 13){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+11, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;
	}
	else if (len == 12){
		memset(s, 0, sizeof(s));
		strncpy(s, in+8, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_hour = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+9, 1);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_min = t;

		memset(s, 0, sizeof(s));
		strncpy(s, in+10, 2);
		t = strtoul(s, NULL, 10) & 0xFF;
		cu.tm_sec = t;
	}

	time_t lt;

	lt = mktime(&cu);
	*out = lt;

	return;
}

void func_cloudvela_zhbhjt212_test(void)
{
	int i=0, j=0, k=0, index = 0, gindex = 0;
	/*
	UINT8 index = 0;
	while(gZhbhjtMsgFormatCfgUlResp[index].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){
		printf("Index=[%d], CnCode=[%d], Frame Len Max[%d]\n", index, gZhbhjtMsgFormatCfgUlResp[index].cnCode, func_cloudvela_zhbhjt212_caculate_ul2cloud_msg_size_max(gZhbhjtMsgFormatCfgUlResp[index].cnCode));
		index++;
	}
	printf("Index=[%d], CnCode=[%d], Frame Len Max[%d]\n", index, gZhbhjtMsgFormatCfgUlResp[index].cnCode, func_cloudvela_zhbhjt212_caculate_ul2cloud_msg_size_max(gZhbhjtMsgFormatCfgUlResp[index].cnCode));

	index = 0;
	while(gZhbhjtMsgFormatCfgDlReq[index].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){
		printf("Index=[%d], CnCode[%d], Frame Len Max[%d]\n", index, gZhbhjtMsgFormatCfgDlReq[index].cnCode, func_cloudvela_zhbhjt212_caculate_dl2hcu_msg_size_max(gZhbhjtMsgFormatCfgDlReq[index].cnCode));
		index++;
	}
	printf("Index=[%d], CnCode[%d], Frame Len Max[%d]\n", index, gZhbhjtMsgFormatCfgDlReq[index].cnCode, func_cloudvela_zhbhjt212_caculate_dl2hcu_msg_size_max(gZhbhjtMsgFormatCfgDlReq[index].cnCode));
	*/

	/*
	while(gZhbhjtMsgFormatCfgUlResp[gindex].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){

		UINT8 msgidIndex =  func_cloudvela_zhbhjt212_search_dl2hcu_msgid_by_cncode(gZhbhjtMsgFormatCfgUlResp[gindex].cnCode);


		ZHBHJT212IeList_t ieList[ZHBHJT_PFM_CMB2MSG_NBR_MAX * HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
		memset(&ieList, 0, sizeof(ZHBHJT212IeList_t)*ZHBHJT_PFM_CMB2MSG_NBR_MAX*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		UINT8 rprTimes = 0;
		//复合IE的多个区域
		for (i = 0; i < ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
			index = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
			rprTimes = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbRptNbr;
			if (rprTimes == 0xFF) rprTimes = 1;  //表示是否存在的选项

			//为满秩的复合IE做独特处理=>这里是属于自行计算
			if (rprTimes == HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX){
				if (index == ZHBHJT_IEID_cmb_alm_lim_rng) rprTimes = 6;
				else if (index == ZHBHJT_IEID_cmb_multi_rtd) rprTimes = 6;
				else if (index == ZHBHJT_IEID_cmb_pol_report) rprTimes = 6;
				else if (index == ZHBHJT_IEID_uni_RT) rprTimes = 6;
				else if (index == ZHBHJT_IEID_uni_RS) rprTimes = 6;
			}

			//单个IE结构
			if ((index > ZHBHJT_IEID_uni_MIN) && (index < ZHBHJT_IEID_uni_MAX) && (rprTimes > 0) && (rprTimes <= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX)){
				for (j=0; j<rprTimes; j++){
					ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[0].ieId = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
					ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[0].ieRptNbr = 1;
				}
			}
			//复合IE结构
			else if ((index > ZHBHJT_IEID_cmb_MIN) && (index < ZHBHJT_IEID_cmb_MAX) && (rprTimes > 0) && (rprTimes <= HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX)){
				for (j=0; j<rprTimes; j++){
					UINT8 cmb2IeIndex = index - ZHBHJT_IEID_cmb_MIN;
					for (k=0; k<ZHBHJT_PFM_IE2CMB_NBR_MAX; k++){
						ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieId = (gZhbhjtIeCmbCfg[cmb2IeIndex].ie[k].ieId);
						ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr = (gZhbhjtIeCmbCfg[cmb2IeIndex].ie[k].ieRptNbr);
					}
				}
			}
		}

		for (i = 0; i < ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
			for (j = 0; j<HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX; j++){
				printf("gIndex/CnCode=[%d/%d], I/J=%d/%d=> ", gindex, gZhbhjtMsgFormatCfgUlResp[gindex].cnCode, i, j);
				for (k=0; k<ZHBHJT_PFM_IE2CMB_NBR_MAX; k++){
					printf("(%d)=%d/%d, ", k, ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieId, ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr);
				}
				printf("\n");
			}
		}
		gindex++;
	}*/

	//发送消息编码
	/*
	msg_struct_llczhb_cloudvela_frame_resp_t rcv;
	CloudDataSendBuf_t pMsgOutput;
	memset(&rcv, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	rcv.ul2Cloud.nbrOfAlmLim = 5;
	rcv.ul2Cloud.nbrOfCMinRpt = 7;
	rcv.ul2Cloud.nbrOfCRtd = 4;
	rcv.ul2Cloud.nbrOfRS = 5;
	rcv.ul2Cloud.nbrOfRT = 2;
	rcv.ul2Cloud.SystemTime = 1234;
	rcv.ul2Cloud.limitation[0].PolId = 91;
	rcv.ul2Cloud.limitation[1].PolId = 92;
	rcv.ul2Cloud.limitation[2].PolId = 93;
	rcv.ul2Cloud.limitation[3].PolId = 94;
	rcv.ul2Cloud.limitation[4].PolId = 95;
	rcv.ul2Cloud.limitation[5].PolId = 96;
	rcv.ul2Cloud.limitation[6].PolId = 97;
	rcv.ul2Cloud.min[0].PolId = 94;
	rcv.ul2Cloud.min[1].PolId = 95;
	rcv.ul2Cloud.min[2].PolId = 96;
	rcv.ul2Cloud.min[3].PolId = 97;
	rcv.ul2Cloud.min[4].PolId = 98;
	rcv.ul2Cloud.min[5].PolId = 99;
	rcv.ul2Cloud.min[6].PolId = 100;
	strcpy(rcv.head.pw, "abcd#1");
	rcv.ul2Cloud.rtd[0].PolFlag='Y';
	rcv.ul2Cloud.rtd[1].PolFlag='N';
	rcv.ul2Cloud.rtd[2].PolFlag='N';
	rcv.ul2Cloud.rtd[3].PolFlag='Y';
	rcv.ul2Cloud.rtd[4].PolFlag='Y';
	rcv.ul2Cloud.rtd[5].PolFlag='N';
	rcv.ul2Cloud.rtd[6].PolFlag='Y';
	rcv.ul2Cloud.RS[0].PolId = 94;
	rcv.ul2Cloud.RS[0].RS = 11;
	rcv.ul2Cloud.RS[1].PolId = 95;
	rcv.ul2Cloud.RS[2].PolId = 96;
	rcv.ul2Cloud.RS[3].PolId = 97;
	rcv.ul2Cloud.RS[4].PolId = 98;
	rcv.ul2Cloud.RS[5].PolId = 99;
	rcv.ul2Cloud.RS[6].PolId = 100;
	rcv.ul2Cloud.RT[0].PolId = 94;
	rcv.ul2Cloud.RT[1].PolId = 95;
	rcv.ul2Cloud.RT[2].PolId = 96;
	rcv.ul2Cloud.RT[3].PolId = 97;
	rcv.ul2Cloud.RT[4].PolId = 98;
	rcv.ul2Cloud.RT[5].PolId = 99;
	rcv.ul2Cloud.RT[6].PolId = 100;
	rcv.ul2Cloud.AlarmEvent.PolId = 90;

	//	rcv.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_resp_2031;
	//func_cloudvela_zhbhjt212_msg_pack(&rcv, &pMsgOutput);
	gindex = 0;
	while(gZhbhjtMsgFormatCfgUlResp[gindex].cnCode != ZHBHJT_IE_uni_CNcode_MAX_9999){
		memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
		rcv.head.cn = gZhbhjtMsgFormatCfgUlResp[gindex].cnCode;
		func_cloudvela_zhbhjt212_msg_pack(&rcv, &pMsgOutput);
		gindex++;
	}
	*/

	//计算所有输入消息的CRC数据

	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_PSWD);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_PSWD);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_FT_TIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_FT_TIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_RTD);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_STOP_MONIROT);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_EQU_RUN);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_STOP_EQU_RUN);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_MIN_RPT1);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_POL_DAY);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_HIS_DAY);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_POL_RCD);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_NOTIFY);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_LIM);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_LIM);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_ADDR);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_ADDR);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_CTIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_RPT_TIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_ZERO);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_GAP_TIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_GET_GAP_TIME);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_INIT_OT);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_INIT_WARNING);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_INST_SMLP);
	ZHBHJT_PRINT_SIZE_AND_CRC(ZHBHJT_MSG_TEST_DATA_SET_SLMP_CYCLE);

	/*
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_PSWD);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_PSWD);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_FT_TIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_FT_TIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_RTD);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_STOP_MONIROT);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_EQU_RUN);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_STOP_EQU_RUN);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_MIN_RPT1);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_POL_DAY);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_HIS_DAY);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_POL_RCD);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_NOTIFY);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_LIM);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_LIM);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_ADDR);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_ADDR);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_CTIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_RPT_TIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_ZERO);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_GAP_TIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_GET_GAP_TIME);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_INIT_OT);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_INIT_WARNING);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_INST_SMLP);
	printf("ZHB: %s\n", ZHBHJT_MSG_TEST_DATA_SET_SLMP_CYCLE);
*/
}



