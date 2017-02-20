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
char gZhbhjtPolIdName[114][ZHBHJT_PFDT_POLID_NAME_LEN_MAX] = {
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
	char ds[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char cps[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	UINT8 msgidIndex = 0;

	//因为是在ZHBHJT212函数体中，所以根据MN地址，自动设置目标服务器为业务服务器
	gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;

	//参数检查
	if ((inputPar == NULL) || (output == NULL))
		HCU_ERROR_PRINT_ZHBHJTCODEC("CLOUDVELA: input parameter error!\n");
	msgidIndex =  func_cloudvela_zhbhjt212_search_ul2cloud_msgid_by_cncode(inputPar->head.cn);
	if ((msgidIndex <= ZHBHJT_IE_uni_CNcode_MIN_0000) || (msgidIndex >= ZHBHJT_IE_uni_CNcode_MAX_9999))
		HCU_ERROR_PRINT_ZHBHJTCODEC("CLOUDVELA: Un-supported message or CN code to be packed!\n");

	//先pack固定FRAME帧头部分
	memset(ds, 0, sizeof(ds));

	//请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	memset(tmp, 0, sizeof(tmp));
	func_cloudvela_zhbhjt212_convert_u64time_to_ymd(inputPar->head.qn, tmp);
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
	printf("ZHB: Lable=%s\n", zHcuSysEngPar.hwBurnId.equLable);
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
	printf("ZHB: DS=[%s]\n", ds);
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
	char sMsgBuid[ZHBHJT_PFM_CMB2MSG_NBR_MAX][HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX][ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	memset(sMsgBuid, 0, sizeof(char)*ZHBHJT_PFM_CMB2MSG_NBR_MAX*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX*ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX);
	UINT8 IeCmbIndex = 0;
	for (i=0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		IeCmbIndex = gZhbhjtMsgFormatCfgUlResp[msgidIndex].IeCmb[i].ieCmbId;
		for (j=0; j<HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX; j++){
			UINT16 PrjLen = 0;
			for (k=0; k<ZHBHJT_PFM_IE2CMB_NBR_MAX; k++){
				index = ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieId;
				rprTimes = ieList[i*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX+j].ie[k].ieRptNbr;
				if ((index>ZHBHJT_IEID_uni_MIN) && (index<ZHBHJT_IEID_uni_MAX) && (rprTimes>0) && (rprTimes<=HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX)){
					for(m=0; m<rprTimes; m++){
						memset(tmp, 0, sizeof(tmp));
						PrjLen++;  //计算长度
						switch(index){
						case ZHBHJT_IEID_uni_SystemTime:
							printf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_SystemTime].keyLable, inputPar->ul2Cloud.SystemTime);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_CfmQN:
							func_cloudvela_zhbhjt212_convert_u64time_to_ymd(inputPar->cfmQn, tmp);
							strcat(sMsgBuid[i][j], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_CfmQN].keyLable);
							strcat(sMsgBuid[i][j], tmp);
							strcat(sMsgBuid[i][j], ",");
							break;
						case ZHBHJT_IEID_uni_CfmCN:
							break;
						case ZHBHJT_IEID_uni_CfmPNUM:
							break;
						case ZHBHJT_IEID_uni_CfmPNO:
							break;
						case ZHBHJT_IEID_uni_QnRtn:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_QnRtn].keyLable, inputPar->ul2Cloud.QnRtn);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_ExeRtn:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_ExeRtn].keyLable, inputPar->ul2Cloud.ExeRtn);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_RtdInterval:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RtdInterval].keyLable, inputPar->ul2Cloud.RtdInterval);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_Rtd:
							sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Rtd].keyLable, inputPar->ul2Cloud.rtd[k].Rtd);
							strcat(sMsgBuid[i][j], tmp);
							if (IeCmbIndex == ZHBHJT_IEID_cmb_multi_rtd){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Rtd].keyLable, inputPar->ul2Cloud.rtd[k].Rtd);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_value_Min:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Min].keyLable, inputPar->ul2Cloud.min[k].Min);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_value_Avg:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Avg].keyLable, inputPar->ul2Cloud.min[k].Avg);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_value_Max:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_value_Max].keyLable, inputPar->ul2Cloud.min[k].Max);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_ZsRtd:
							break;
						case ZHBHJT_IEID_uni_value_ZsMin:
							break;
						case ZHBHJT_IEID_uni_value_ZsAvg:
							break;
						case ZHBHJT_IEID_uni_value_ZsMax:
							break;
						case ZHBHJT_IEID_uni_polFlag:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_multi_rtd){
								sprintf(tmp, "%s%s%c,", gZhbhjtPolIdName[inputPar->ul2Cloud.rtd[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_polFlag].keyLable, inputPar->ul2Cloud.rtd[k].PolFlag);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_Cou:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_pol_report){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.min[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Cou].keyLable, inputPar->ul2Cloud.min[k].Cou);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_RS:
							sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.RS[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RS].keyLable, inputPar->ul2Cloud.RS[k].RS);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_RT:
							sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.RT[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_RT].keyLable, inputPar->ul2Cloud.RT[k].RT);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_Ala:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alarm_event){
								sprintf(tmp, "%s%s%f,", gZhbhjtPolIdName[inputPar->ul2Cloud.AlarmEvent.PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Ala].keyLable, inputPar->ul2Cloud.AlarmEvent.Ala);
								strcat(sMsgBuid[i][j], tmp);
							}else{
								sprintf(tmp, "%s%s%f,", gZhbhjtPolIdName[inputPar->ul2Cloud.Ala.PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Ala].keyLable, inputPar->ul2Cloud.Ala.Ala);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_UpValue:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alm_lim_rng){
								sprintf(tmp, "%s%s%f,", gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_UpValue].keyLable, inputPar->ul2Cloud.limitation[k].UpValue);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_LowValue:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alm_lim_rng){
								sprintf(tmp, "%s%s%f,", gZhbhjtPolIdName[inputPar->ul2Cloud.limitation[k].PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_LowValue].keyLable, inputPar->ul2Cloud.limitation[k].LowValue);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_Data:
							break;
						case ZHBHJT_IEID_uni_DayData:
							break;
						case ZHBHJT_IEID_uni_NightData:
							break;
						case ZHBHJT_IEID_uni_AlarmTime:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmTime].keyLable, inputPar->ul2Cloud.AlarmTime);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_AlarmType:
							if (IeCmbIndex == ZHBHJT_IEID_cmb_alarm_event){
								sprintf(tmp, "%s%s%d,", gZhbhjtPolIdName[inputPar->ul2Cloud.AlarmEvent.PolId], gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_Ala].keyLable, inputPar->ul2Cloud.AlarmEvent.AlarmType);
								strcat(sMsgBuid[i][j], tmp);
							}else{
								sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmType].keyLable, inputPar->ul2Cloud.AlarmType);
								strcat(sMsgBuid[i][j], tmp);
							}
							break;
						case ZHBHJT_IEID_uni_AlarmTarget:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_AlarmTarget].keyLable, inputPar->ul2Cloud.AlarmTarget);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_PolId:
							break;
						case ZHBHJT_IEID_uni_BeginTime:
							break;
						case ZHBHJT_IEID_uni_EndTime:
							break;
						case ZHBHJT_IEID_uni_DataTime:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_DataTime].keyLable, inputPar->ul2Cloud.DataTime);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_ReportTime:
							sprintf(tmp, "%s%d,", gZhbhjtIeEleCfg[ZHBHJT_IEID_uni_ReportTime].keyLable, inputPar->ul2Cloud.ReportTime);
							strcat(sMsgBuid[i][j], tmp);
							break;
						case ZHBHJT_IEID_uni_DayStdValue:
							break;
						case ZHBHJT_IEID_uni_NightStdValue:
							break;
						case ZHBHJT_IEID_uni_SetPwd:
							break;
						case ZHBHJT_IEID_uni_OverTime:
							break;
						case ZHBHJT_IEID_uni_ReCount:
							break;
						case ZHBHJT_IEID_uni_WarnTime:
							break;
						case ZHBHJT_IEID_uni_CTime:
							break;
						default:
							break;
						}//switch(index)
					}//重复rprTimes
				}//满足进入的条件
			}//k，表达的是子项目重复
			if ((PrjLen > 0) && (strlen(sMsgBuid[i][j]) > 0)){
				//附上;项目分隔符号，而且是将最后的一个逗号改为分号即可
				if (sMsgBuid[i][j][strlen(sMsgBuid[i][j]-1)] == ',') sMsgBuid[i][j][strlen(sMsgBuid[i][j]-1)] = ';';
			}
		}//单独IE
	}//复合组合

	//全部组合
	for (i=0; i<ZHBHJT_PFM_CMB2MSG_NBR_MAX; i++){
		for (j=0; j<HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX; j++){
			strcat(cps, sMsgBuid[i][j]);
		}
	}
	printf("ZHB: CPS=[%s]\n", cps);
	//去掉最后一个分号
	if ((strlen(cps) > 0) && (cps[strlen(cps)-1] == ';'))  cps[strlen(cps)-1] = '\0';

	//组合数据部分
	strcat(ds, "CP=&&");
	strcat(ds, cps);
	strcat(ds, "&&");
	printf("ZHB: DS+CPS=[%s]\n", ds);

	//计算长度
	UINT16 len = strlen(ds);
	if (len > ZHBHJT_PROTOCOL_FRAME_DATA_LEN_MAX ) HCU_ERROR_PRINT_ZHBHJTCODEC("CLOUDVELA: pack error!\n");

	//构造输出
	strcpy(output->curBuf, ZHBHJT_PROTOCOL_FRAME_FIX_HEAD);
	sprintf(output->curBuf, "%04d%s", len, ds);

	UINT16 crc16 = 0;
	hcu_vm_calculate_crc_modbus((UINT8*)ds, len, &crc16);
	sprintf(output->curBuf, "%04x", crc16);

	strcat(output->curBuf, ZHBHJT_PROTOCOL_FRAME_FIX_TAIL);
	output->curLen = strlen(output->curBuf);

	//测试打印
	printf("ZHB: output=[%s]\n", output->curBuf);

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


//基础搜索函数
UINT8 func_cloudvela_zhbhjt212_search_polid_by_name(char *PolIdName)
{
	UINT8 index =0;
	while(strcmp(gZhbhjtPolIdName[index], "MAXIMUM") !=0){
		if (strcmp(gZhbhjtPolIdName[index], PolIdName) == 0) return index;
		index++;
	}
	return index;  //如果找不到，最后一项必然是MAXIMUM
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

//输出的out必须超过20字节的字符串，否则出错
void func_cloudvela_zhbhjt212_convert_u64time_to_ymd(UINT64 in, char *out)
{
	UINT32 time2Sec = 0, time2Milsec = 0;
	time2Sec = (UINT32)(in / 1000);
	time2Milsec = (UINT32)(in - (in / 1000)*1000);
	time_t lt = (time_t)time2Sec;
	struct tm *cu;
	cu = localtime(&lt);
	sprintf(out,  "QN=%04d%02d%02d%02d%02d%02d%03d;", (UINT16)(1900+cu->tm_year), (UINT8)(1+cu->tm_mon), (UINT8)cu->tm_mday,\
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec, time2Milsec);
	return;
}

void func_cloudvela_zhbhjt212_test(void)
{
	//int i=0, j=0, k=0, index = 0, gindex = 0;
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

	msg_struct_llczhb_cloudvela_frame_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));
	CloudDataSendBuf_t pMsgOutput;
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	rcv.head.cn = ZHBHJT_IE_uni_CNcode_par_get_field_time_resp_1011;
	//func_cloudvela_zhbhjt212_msg_pack(&rcv, &pMsgOutput);
	printf("ZHB: Lable=%s\n", zHcuSysEngPar.hwBurnId.equLable);

}



