/*
 * l3aqycg20.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_AQYC_G20_H_
#define L3APP_AQYC_G20_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l1hwopt/spsapi.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3AQYCG20
{
	FSM_STATE_L3AQYCG20_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3AQYCG20_ACTIVED,
	FSM_STATE_L3AQYCG20_WFFB_CTRL,
	FSM_STATE_L3AQYCG20_WFFB_DATA,
	FSM_STATE_L3AQYCG20_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


//Global variables
extern HcuFsmStateItem_t HcuFsmL3aqycg20[];


//扫描时间最小颗粒度为30s
#define HCU_L3AQYC_STA_DURATION  30

//上报周期
#define HCU_L3AQYC_STA_1M_REPORT_DURATION   60
#define HCU_L3AQYC_STA_5M_REPORT_DURATION   60*5
#define HCU_L3AQYC_STA_1H_REPORT_DURATION   60*60
#define HCU_L3AQYC_STA_1D_REPORT_DURATION   24*60*60

#define HCU_L3AQYC_STA_UNIT_DUR  30 //1分钟
//#define HCU_L3AQYC_STA_1M_REPORT_CYCLE 	 (66/HCU_L3AQYC_STA_UNIT_DUR) // test for LED
#define HCU_L3AQYC_STA_1M_REPORT_CYCLE 	 (60/HCU_L3AQYC_STA_UNIT_DUR)
#define HCU_L3AQYC_STA_2M_REPORT_CYCLE 	 (60*2/HCU_L3AQYC_STA_UNIT_DUR)
#define HCU_L3AQYC_STA_3M_REPORT_CYCLE 	 (60*3*2/HCU_L3AQYC_STA_UNIT_DUR)
#define HCU_L3AQYC_STA_1H_REPORT_CYCLE   (60*60*2/HCU_L3AQYC_STA_UNIT_DUR)
#define HCU_L3AQYC_STA_1D_REPORT_CYCLE   (24*60*60/HCU_L3AQYC_STA_UNIT_DUR)

#define HCU_L3AQYC_STA_DBI_TABLE_1MIN   	"AQYC_STA_1_MIN"
#define HCU_L3AQYC_STA_DBI_TABLE_1HOUR   	"AQYC_STA_1_HOUR"
#define HCU_L3AQYC_STA_DBI_TABLE_1DAY   	"AQYC_STA_1_DAY"

//扬尘监测各传感器LLC标识符
#define HCU_L3AQYC_A01001_INDEX 94
#define HCU_L3AQYC_A01002_INDEX 95
#define HCU_L3AQYC_A01006_INDEX 96
#define HCU_L3AQYC_A01007_INDEX 97
#define HCU_L3AQYC_A01008_INDEX 98
#define HCU_L3AQYC_A34001_INDEX 99
#define HCU_L3AQYC_A50001_INDEX 100


//扬尘监测的统计报表数据标识符判断值缺省常量
//颗粒物监测仪量程
#define HCU_L3AQYC_A34001_RANGE_MAX		30.00
#define HCU_L3AQYC_A34001_RANGE_MIN		0.01

//噪声监测仪量程
#define HCU_L3AQYC_A50001_RANGE_MAX		130.0
#define HCU_L3AQYC_A50001_RANGE_MIN		20.0

//温度监测仪量程
#define HCU_L3AQYC_A01001_RANGE_MAX		50.0
#define HCU_L3AQYC_A01001_RANGE_MIN		-10.0

//湿度监测仪量程
#define HCU_L3AQYC_A01002_RANGE_MAX		100.0
#define HCU_L3AQYC_A01002_RANGE_MIN		0

//风速监测仪量程
#define HCU_L3AQYC_A01007_RANGE_MAX		30.0
#define HCU_L3AQYC_A01007_RANGE_MIN		0

//风向监测仪量程
#define HCU_L3AQYC_A01008_RANGE_MAX		359.0
#define HCU_L3AQYC_A01008_RANGE_MIN		0

//气压监测仪量程
#define HCU_L3AQYC_A01006_RANGE_MAX		100.0//not used
#define HCU_L3AQYC_A01006_RANGE_MIN		0
/////////////////////////////////////

//颗粒物监测仪数据设定范围
#define HCU_L3AQYC_A34001_RANGE_SET_MAX		HCU_L3AQYC_A34001_RANGE_MAX
#define HCU_L3AQYC_A34001_RANGE_SET_MIN		HCU_L3AQYC_A34001_RANGE_MIN

//噪声监测仪数据设定范围
#define HCU_L3AQYC_A50001_RANGE_SET_MAX		HCU_L3AQYC_A50001_RANGE_MAX
#define HCU_L3AQYC_A50001_RANGE_SET_MIN		HCU_L3AQYC_A50001_RANGE_MIN

//温度监测仪数据设定范围
#define HCU_L3AQYC_A01001_RANGE_SET_MAX		HCU_L3AQYC_A01001_RANGE_MAX
#define HCU_L3AQYC_A01001_RANGE_SET_MIN		HCU_L3AQYC_A01001_RANGE_MIN

//湿度监测仪数据设定范围
#define HCU_L3AQYC_A01002_RANGE_SET_MAX		HCU_L3AQYC_A01002_RANGE_MAX
#define HCU_L3AQYC_A01002_RANGE_SET_MIN		HCU_L3AQYC_A01002_RANGE_MIN

//风速监测仪数据设定范围
#define HCU_L3AQYC_A01007_RANGE_SET_MAX		HCU_L3AQYC_A01007_RANGE_MAX
#define HCU_L3AQYC_A01007_RANGE_SET_MIN		HCU_L3AQYC_A01007_RANGE_MIN

//风向监测仪数据设定范围
#define HCU_L3AQYC_A01008_RANGE_SET_MAX		HCU_L3AQYC_A01008_RANGE_MAX
#define HCU_L3AQYC_A01008_RANGE_SET_MIN		HCU_L3AQYC_A01008_RANGE_MIN

//气压监测仪数据设定范围
#define HCU_L3AQYC_A01006_RANGE_SET_MAX		HCU_L3AQYC_A01006_RANGE_MAX
#define HCU_L3AQYC_A01006_RANGE_SET_MIN		HCU_L3AQYC_A01006_RANGE_MIN



#define HCU_L3AQYC_A34001_ALAMR_MAX		3.00
#define HCU_L3AQYC_A34001_ALAMR_MIN		HCU_L3AQYC_A34001_RANGE_MIN
#define HCU_L3AQYC_A50001_ALAMR_MAX		80.00
#define HCU_L3AQYC_A50001_ALAMR_MIN		HCU_L3AQYC_A50001_RANGE_MIN
#define HCU_L3AQYC_A01001_ALAMR_MAX		HCU_L3AQYC_A01001_RANGE_MAX
#define HCU_L3AQYC_A01001_ALAMR_MIN		HCU_L3AQYC_A01001_RANGE_MAX
#define HCU_L3AQYC_A01002_ALAMR_MAX		HCU_L3AQYC_A01002_RANGE_MIN
#define HCU_L3AQYC_A01002_ALAMR_MIN		HCU_L3AQYC_A01002_RANGE_MIN
#define HCU_L3AQYC_A01007_ALAMR_MAX		5.00
#define HCU_L3AQYC_A01007_ALAMR_MIN		HCU_L3AQYC_A01007_RANGE_MIN
#define HCU_L3AQYC_A01008_ALAMR_MAX		HCU_L3AQYC_A01008_RANGE_MAX
#define HCU_L3AQYC_A01008_ALAMR_MIN		HCU_L3AQYC_A01008_RANGE_MIN
#define HCU_L3AQYC_A01006_ALAMR_MAX		HCU_L3AQYC_A01006_RANGE_MAX
#define HCU_L3AQYC_A01006_ALAMR_MIN		HCU_L3AQYC_A01006_RANGE_MIN
/////////////////////////////////////


//颗粒监测仪校准标志位
#define HCU_L3AQYC_A34001_CALIBRATION_FLAG		FALSE
//噪声监测仪校准标志位
#define HCU_L3AQYC_A50001_CALIBRATION_FLAG		FALSE
//温度监测仪校准标志位
#define HCU_L3AQYC_A01001_CALIBRATION_FLAG		FALSE
//湿度监测仪校准标志位
#define HCU_L3AQYC_A01002_CALIBRATION_FLAG		FALSE
//风速监测仪校准标志位
#define HCU_L3AQYC_A01007_CALIBRATION_FLAG		FALSE
//风向监测仪校准标志位
#define HCU_L3AQYC_A01008_CALIBRATION_FLAG		FALSE
//气压监测仪校准标志位
#define HCU_L3AQYC_A01006_CALIBRATION_FLAG		FALSE
//雨雪雷电标志位
#define HCU_L3AQYC_RAIN_SNOW_FLAG		FALSE



typedef struct gTaskL3aqycq20Context
{
	//PM10报警标志位
	UINT32 PM10AlarmOnOff;
	//噪声报警标志位
	UINT32 NoiseAlarmOnOff;

	//设备运行状态标志位
	UINT32 EquStatusReportFlag;

	//实时采样数据上报间隔
	UINT16 RtdInterval;

	//数据上报时间
	UINT16 ReportTime;

	//监控设备较零较满污染物ID
	UINT8 singlePolId;

	//超限报警时间
	UINT32 WarnTime;

	//采集数据起始和结束时间和当前时间的差值
	UINT32 timeBegin_Min;
	UINT32 timeEnd_Min;
	UINT32 timeBegin_Hour;
	UINT32 timeEnd_Hour;
	UINT32 timeBegin_Day;
	UINT32 timeEnd_Day;
	UINT32 timeBegin_Ala;
	UINT32 timeEnd_Ala;

	//即时，分钟，小时，天数据报告标志位
	UINT32 InstReportFlag;
	UINT32 MinReportFlag;
	UINT32 HourReportFlag;
	UINT32 DayReportFlag;
	//UINT32 AlaReportFlag;

	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是1s为统计周期

	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3aqycContextCurrentElement_t cur;  		//当前各传感器上报数值（从数据库取得 or 实时取得，为简化暂时从数据库取的）

	//周期统计报告部分(统计从监测仪启动开始定期扫描)，是否存数据库待定
	HcuSysMsgIeL3aqycContextStaElement_t staInstant;  	//瞬时值结果(为数据完备性，暂时取一定取样的时间均值)
	HcuSysMsgIeL3aqycContextStaElement_t staMin;  	//1分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t staHour;		//60分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t staDay;		//24H统计结果

	HcuSysL3aqycValueJudgement_t valueJudge;

	HcuSysMsgIeL3aqycContextEqtStatusElement_t eqtStatus;

	HcuSysL3aqycAlarmLimit_t alarmLimit;

}gTaskL3aqycq20Context_t;



typedef struct SerialLedMsgBuf
{
	UINT32 curLen;
	UINT8  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
}SerialLedMsgBuf_t;

//层三Context
extern gTaskL3aqycq20Context_t		gTaskL3aqycq20Context;

//API
extern OPSTAT fsm_l3aqycg20_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_huil3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_huil3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_llczhb_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_zhbl3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3aqycg20_int_init(void);
OPSTAT func_l3aqyc_time_out_aggregation_process(void);
OPSTAT func_l3aqyc_pm10_noise_value_alarm_judge_2072(HcuSysMsgIeL3aqycContextStaElement_t *staInstant);
void func_l3aqycg20_judge_value_init(void);
void func_l3aqycg20_eqt_rs_init(void);
void func_l3aqycg20_alarm_limit_init(void);
void func_l3aqyc_measurement_value_flag_judge_inst(HcuSysMsgIeL3aqycContextCurrentElement_t *aggReport);
void func_l3aqyc_measurement_value_flag_judge_min(HcuSysMsgIeL3aqycContextStaElement_t *aggReport);



//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3AQYCG20(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L3APP_AQYC_G20_H_ */
