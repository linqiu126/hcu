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


//上报周期
#define HCU_L3AQYC_STA_1M_CYCLE  60
#define HCU_L3AQYC_STA_1H_CYCLE  60*60
#define HCU_L3AQYC_STA_1D_CYCLE  24*60*60

#define HCU_L3AQYC_STA_DBI_TABLE_1MIN   	"AQYC_STA_1_MIN"
#define HCU_L3AQYC_STA_DBI_TABLE_1HOUR   	"AQYC_STA_1_HOUR"
#define HCU_L3AQYC_STA_DBI_TABLE_1DAY   	"AQYC_STA_1_DAY"

#define HCU_L3AQYC_STA_DBI_TABLE_1MIN_REPORT   	"AQYC_STA_1_MIN_REPORT"
#define HCU_L3AQYC_STA_DBI_TABLE_1HOUR_REPORT   "AQYC_STA_1_HOUR_REPORT"
#define HCU_L3AQYC_STA_DBI_TABLE_1DAY_REPORT   	"AQYC_STA_1_DAY_REPORT"


//Global variables
extern HcuFsmStateItem_t HcuFsmL3aqycg20[];

typedef struct gTaskL3aqycq20Context
{

	//采集数据起始和结束时间和当前时间的差值
	UINT32 timeBegin;
	UINT32 timeEnd;


	//开始测量标志位
	UINT32 StartMonitorFlag;

	//分钟，小时，天数据报告标志位
	UINT32 MinReportFlag;
	UINT32 HourReportFlag;
	UINT32 DayReportFlag;

	//动态部分
	UINT32  startWorkTimeInUnix;		//表示该系统开始工作的时间日程点
	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是1s为统计周期
	UINT32  elipse24HourCnt;			//24小时的日历计数器

	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3aqycContextStaElement_t cur;  		//当前各传感器上报数值（从数据库取得 or 实时取得，为简化暂时从数据库取的）

	//周期统计报告部分(统计从监测仪启动开始定期扫描)，存数据库备用
	HcuSysMsgIeL3aqycContextStaElement_t staInstant;  	//瞬时值结果，是否需要带Max,Min带商榷
	HcuSysMsgIeL3aqycContextStaElement_t staOneMin;  	//1分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta60Min;		//60分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta24H;		//24H统计结果

	//周期统计报告部分(统计从收到上位机命令开始)
	HcuSysMsgIeL3aqycContextStaElement_t staOneMinReport;  	//1分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta60MinReport;	//60分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta24HReport;		//24H统计结果
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


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3AQYCG20(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L3APP_AQYC_G20_H_ */
