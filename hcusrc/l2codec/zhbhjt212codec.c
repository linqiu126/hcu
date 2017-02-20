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

//静态表
char gZhbhjtPolIdName[][10] = {
	"MINIMUM", "B03",	"L10",	"L5",	"L50",	"L90",	"L95",	"Ld",	"Ldn",	"Leq",	"LMn",	"LMx",	"Ln",	"S01",	"S02",	"S03",	"S04",
	"S05",	"S06",	"S07",	"S08",	"B02",	"01",	"02",	"03",	"04",	"05",	"06",	"07",	"08",	"09",	"10",	"11",	"12",	"13",
	"14",	"15",	"16",	"17",	"18",	"19",	"20",	"21",	"22",	"23",	"24",	"25",	"26",	"27",	"28",	"29",	"30",	"31",
	"32",	"33",	"34",	"35",	"36",	"37",	"99",	"B01",	"001",	"002",	"003",	"010",	"011",	"015",	"020",	"021",	"022",	"023",
	"024",	"025",	"026",	"027",	"028",	"029",	"030",	"031",	"032",	"033",	"034",	"035",	"036",	"037",	"038",	"039",	"040",	"041",
	"060",	"061",	"065",	"080",	"101",	"a01001",	"a01002",	"a01006",	"a01007",	"a01008",	"a34001",	"a50001",	"MAXIMUM",
};

//静态配置ZHBHJT212协议的收发消息格式
//IE静态数据单元
ZHBHJT212MsgIeEleStaticCfg_t gZhbhjtIeEleCfg[] = {
	{"",                 ZHBHJT_PFDT_CHAR,     0,   0},
	{"SystemTime=",      ZHBHJT_PFDT_INT32,    14,  0},
	{"QN=",              ZHBHJT_PFDT_INT64,    20,  0},
	{"CN=",              ZHBHJT_PFDT_INT16,    4,   0},
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
	{ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_req_1061      , {{0,0},{0,0},{0,0},{0,0},},},//Done
	{ZHBHJT_IE_uni_CNcode_par_set_rtdi_interval_req_1062      , {{ZHBHJT_IEID_uni_RtdInterval,   1},  {0,0},{0,0},{0,0},},},
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

//消息定义：上行，送到服务器来的消息
ZHBHJT212MsgFormatStaticCfg_t gZhbhjtMsgFormatCfgUlResp[] = {
	{ZHBHJT_IE_uni_CNcode_MIN_0000                                , {{0,0},{0,0},{0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_time_resp_1011            , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_SystemTime, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_resp_1021   , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_cmb_alm_lim_rng,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_field_addr_resp_1031            , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_AlarmTarget, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_data_report_time_resp_1041      , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_ReportTime, 1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_resp_1061     , {{ZHBHJT_IEID_uni_CfmQN,   1},  {ZHBHJT_IEID_uni_RtdInterval, 1}, {0,0},{0,0},},},
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
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_resp_2071, {{ZHBHJT_IEID_uni_DataTime,1},  {ZHBHJT_IEID_uni_Ala,HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_event_resp_2072 , {{ZHBHJT_IEID_uni_AlarmTime,1}, {ZHBHJT_IEID_cmb_alarm_event,1}, {0,0},{0,0},},},
	{ZHBHJT_IE_uni_CNcode_MAX_9999                                , {{0,0},{0,0},{0,0},{0,0},},},
};
//ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_resp: Not exist, add temperay


//PACK函数入口
OPSTAT func_cloudvela_zhbhjt212_msg_pack(msg_struct_llczhb_cloudvela_frame_resp_t *inputPar, CloudDataSendBuf_t *output)
{
	//int i=0, index = 0;
	char tmp[ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	char ds[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char cps[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];

	//因为是在ZHBHJT212函数体中，所以根据MN地址，自动设置目标服务器为业务服务器
	gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;

	//参数检查
	if ((inputPar == NULL) || (output == NULL))
		HCU_ERROR_PRINT_ZHBHJTCODEC("CLOUDVELA: input parameter!\n");

	//先pack固定FRAME帧头部分
	memset(ds, 0, sizeof(ds));

	//请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	UINT32 time2Sec = 0, time2Milsec = 0;
	time2Sec = (UINT32)(inputPar->head.qn / 1000);
	time2Milsec = (UINT32)(inputPar->head.qn - (inputPar->head.qn / 1000)*1000);
	time_t lt = (time_t)time2Sec;
	struct tm *cu;
	cu = localtime(&lt);
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp,  "QN=%04d%02d%02d%02d%02d%02d%03d;", (UINT16)(1900+cu->tm_year), (UINT8)(1+cu->tm_mon), (UINT8)cu->tm_mday,\
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec, time2Milsec);
	strcat(ds, tmp);

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

	//POLID定义有问题




	//返回
	return SUCCESS;
}


//UNPACK函数入口
OPSTAT func_cloudvela_zhbhjt212_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId)
{
	//因为是在ZHBHJT212函数体中，所以根据MN地址，自动设置目标服务器为业务服务器
	gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;




	//返回
	return SUCCESS;
}



