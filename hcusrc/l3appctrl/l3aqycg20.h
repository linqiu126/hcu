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


//分钟扫描计算标志位
#define HCU_L3AQYC_STA_1M_CYCLE  60

//上报周期
#define HCU_L3AQYC_STA_1M_REPORT_DURATION  60
#define HCU_L3AQYC_STA_5M_REPORT_DURATION  60*5
#define HCU_L3AQYC_STA_1H_REPORT_DURATION   60*60
#define HCU_L3AQYC_STA_1D_REPORT_DURATION   24*60*60

#define HCU_L3AQYC_STA_DBI_TABLE_1MIN   	"AQYC_STA_1_MIN"
#define HCU_L3AQYC_STA_DBI_TABLE_1HOUR   	"AQYC_STA_1_HOUR"
#define HCU_L3AQYC_STA_DBI_TABLE_1DAY   	"AQYC_STA_1_DAY"

#define HCU_L3AQYC_STA_DBI_TABLE_1MIN_REPORT   	"AQYC_STA_1_MIN_REPORT"
#define HCU_L3AQYC_STA_DBI_TABLE_1HOUR_REPORT   "AQYC_STA_1_HOUR_REPORT"
#define HCU_L3AQYC_STA_DBI_TABLE_1DAY_REPORT   	"AQYC_STA_1_DAY_REPORT"

//扬尘监测的统计报表数据标识符判断值缺省常量
//颗粒物监测仪量程
#define HCU_L3AQYC_A34001_RANGE_MAX		30.00
#define HCU_L3AQYC_A34001_RANGE_MIN		0.01

//噪声监测仪量程
#define HCU_L3AQYC_A50001_RANGE_MAX		130.0
#define HCU_L3AQYC_A50001_RANGE_MIN		30.0

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

//最大风速值（噪声仪失效）
#define HCU_L3AQYC_WINDTHRESHOLD_FOR_NOISE		5.0 //当风速大于5m/s时噪声数据无效（设噪声数据为S）


typedef struct gTaskL3aqycq20Context
{

	//采集数据起始和结束时间和当前时间的差值
	UINT32 timeBegin_1Min;
	UINT32 timeEnd_1Min;
	UINT32 timeBegin_1Hour;
	UINT32 timeEnd_1Hour;
	UINT32 timeBegin_1Day;
	UINT32 timeEnd_1Day;

	//分钟，小时，天数据报告标志位
	UINT32 MinReportFlag;
	UINT32 HourReportFlag;
	UINT32 DayReportFlag;

	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是1s为统计周期

	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3aqycContextStaElement_t cur;  		//当前各传感器上报数值（从数据库取得 or 实时取得，为简化暂时从数据库取的）

	//周期统计报告部分(统计从监测仪启动开始定期扫描)，是否存数据库待定
	HcuSysMsgIeL3aqycContextStaElement_t staInstant;  	//瞬时值结果，是否需要带Max,Min带商榷
	HcuSysMsgIeL3aqycContextStaElement_t staOneMin;  	//1分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta60Min;		//60分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta24H;		//24H统计结果

	HcuSysL3aqycValueJudgement_t valueJudge;

}gTaskL3aqycq20Context_t;



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
void func_l3aqycg20_judge_value_init(void);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3AQYCG20(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L3APP_AQYC_G20_H_ */
