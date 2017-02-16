/*
 * l3bfsc.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFSC_H_
#define L3APP_BFSC_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3BFSC
{
	FSM_STATE_L3BFSC_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3BFSC_ACTIVED,
	FSM_STATE_L3BFSC_OPR_CFG,  	//人工配置状态
	FSM_STATE_L3BFSC_WS_INIT,  	//初始化下位机
	FSM_STATE_L3BFSC_OOS_SCAN,  //进料组合态
	FSM_STATE_L3BFSC_OOS_TTT,  	//出料流程态
	FSM_STATE_L3BFSC_OOS_TGU,  	//放弃物料态
	FSM_STATE_L3BFSC_ERROR_INQ, //数据差错，重新采样所有数据
	FSM_STATE_L3BFSC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3bfsc[];


typedef struct L3BfscSensorWsInfo
{
	UINT8  sensorWsId;
	UINT32 sensorValue;
	UINT8  sensorStatus; //无效，空料，有料数值错误，有料待组合，有料待出料
}L3BfscSensorWsInfo_t;
//秤盘状态定义
#define HCU_L3BFSC_SENSOR_WS_STATUS_INVALID 	0  		//秤盘无效
#define HCU_L3BFSC_SENSOR_WS_STATUS_INVALID1 255  		//秤盘无效
#define HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY 1       		//秤盘空
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR 2 		//秤盘有料数值错误
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB 3 	//秤盘有料待组合
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT 4		//秤盘有料待出料
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU 5		//秤盘有料待抛弃

//统计周期，为了计算滑动平均数据
#define HCU_L3BFSC_STA_CYCLE_DUR  60000 //1分钟，相当于60S
#define HCU_L3BFSC_STA_UNIT_DUR  500 //500ms的单位，这是统计周期颗粒度，跟TIMER_ID_10MS_L3BFSC_PERIOD_STA_SCAN保持一致
#define HCU_L3BFSC_STA_BASE_CYCLE  (HCU_L3BFSC_STA_CYCLE_DUR / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_1M_CYCLE  (60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_15M_CYCLE  (15 * 60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_60M_CYCLE  (60 * 60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_2H_CYCLE  (2* 60 * 60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_8H_CYCLE  (8* 60 * 60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_24H_CYCLE  (24* 60 * 60 * 1000 / HCU_L3BFSC_STA_UNIT_DUR)
#define HCU_L3BFSC_STA_AGEING_COEF_ALPHA   20  //老化系数，越大表示最近的权重越大，100%的情况就是120，相当于当前周期的统计数据除以统计周期
#define HCU_L3BFSC_STA_AGEING_COEF (float)(((float)(HCU_L3BFSC_STA_BASE_CYCLE-HCU_L3BFSC_STA_AGEING_COEF_ALPHA))/(float)HCU_L3BFSC_STA_BASE_CYCLE)
#define HCU_L3BFSC_STA_24H_IN_SECOND  24*3600

//组合目标的控制表
typedef struct gTaskL3bfscContextStaElement
{
	UINT32	wsIncMatCnt;  			//物料数量
	float	wsIncMatWgt;  			//物料重量
	UINT32	wsCombTimes;  			//总共成功素搜到目标的次数
	UINT32	wsTttTimes;  			//TTT次数
	UINT32	wsTgvTimes;  			//TGV次数
	UINT32	wsTttMatCnt;			//TTT物料数量
	UINT32	wsTgvMatCnt;			//TGV物料数量
	float	wsTttMatWgt;			//TTT物料重量
	float	wsTgvMatWgt;			//TGV物料重量
	UINT32	wsAvgTttTimes;			//TTT平均次数
	UINT32	wsAvgTttMatCnt;			//TTT平均物料数
	float	wsAvgTttMatWgt;			//TTT平均重量
}gTaskL3bfscContextStaElement_t;
//临时数据暂存内容
typedef struct gTaskL3bfscContextStaEleMid
{
	float	wsIncMatCntMid;  			//物料数量
	float	wsIncMatWgtMid;  			//物料重量
	float	wsCombTimesMid;  			//总共成功素搜到目标的次数
	float	wsTttTimesMid;  			//TTT次数
	float	wsTgvTimesMid;  			//TGV次数
	float	wsTttMatCntMid;				//TTT物料数量
	float	wsTgvMatCntMid;				//TGV物料数量
	float	wsTttMatWgtMid;				//TTT物料重量
	float	wsTgvMatWgtMid;				//TGV物料重量
	float	wsAvgTttTimesMid;			//TTT平均次数
	float	wsAvgTttMatCntMid;			//TTT平均物料数
	float	wsAvgTttMatWgtMid;			//TTT平均重量
}gTaskL3bfscContextStaEleMid_t;
//主体上下文
typedef struct gTaskL3bfscContext
{
	//搜索部分
	UINT32 	wsRrSearchStart; 			//搜索算法从哪一个搜索系数开始
	UINT8   *SearchCoefficientPointer;
	UINT32  searchSpaceTotalNbr; 		//搜索的长度，12对应4096
	//参数部分
	UINT8	minWsNbr;
	UINT8	maxWsNbr;
	UINT32  targetValue;
	UINT32	targetUpLimit;
	UINT32  start24hStaTimeInUnix;		//系统配置的参数，表示24小时统计的日历起点
	//动态部分
	UINT32  startWorkTimeInUnix;		//表示该系统开始工作的时间日程点
	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是100ms为统计周期
	UINT32  elipse24HourCnt;			//24小时的日历计数器
	L3BfscSensorWsInfo_t	sensorWs[HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX];
	UINT8  	wsValueNbrFree;  			//空闲的0值秤盘数量
	UINT8   wsValueNbrWeight;			//空闲有值的秤盘数量
	UINT8 	wsValueNbrTtt;  			//待出料有值秤盘数量
	UINT8 	wsValueNbrTgu;  			//待出料有值秤盘数量
	UINT8 	wsBitmap[HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX];  //组合出的秤盘标示
	//实时统计部分：均以一个统计周期为单位
	gTaskL3bfscContextStaElement_t cur;  		//当前统计基础颗粒中的数值
	gTaskL3bfscContextStaEleMid_t  curAge;		//使用老化算法，需要该域存下中间结果，不然每一次计算均采用近似会导致数据失真
	//gTaskL3bfscContextStaElement_t curArray[HCU_L3BFSC_STA_BASE_CYCLE]; //不使用桶形算法，抑制该数据。未来确定不用，可删去
	//统计报告部分
	gTaskL3bfscContextStaElement_t staLocalUi;  //滑动平均给本地UI的数据
	gTaskL3bfscContextStaElement_t staOneMin;  	//1分钟统计结果
	gTaskL3bfscContextStaElement_t sta15Min;   	//15分钟统计结果
	gTaskL3bfscContextStaElement_t sta60Min;	//60分钟统计结果
	gTaskL3bfscContextStaElement_t sta2H;		//2H统计结果
	gTaskL3bfscContextStaElement_t sta8H;		//8H统计结果
	gTaskL3bfscContextStaElement_t sta24H;		//24H统计结果
	gTaskL3bfscContextStaElement_t staUp2Now;	//连续工作到目前的统计结果
}gTaskL3bfscContext_t;

//统计打印报告的频率调整
#define HCU_L3BFSC_STATISTIC_PRINT_FREQUENCY 10

//API
extern OPSTAT fsm_l3bfsc_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_error_inq_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_period_read_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_general_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_init_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_param_set_result(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//CLOUDVELA后台通信部分
extern OPSTAT fsm_l3bfsc_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3bfsc_int_init(void);
void func_l3bfsc_cacluate_sensor_ws_valid_value(void);
INT32 func_l3bfsc_ws_sensor_search_combination(void);
void func_l3bfsc_ws_sensor_search_give_up(void);
UINT32 func_l3bfsc_cacluate_sensor_ws_bitmap_valid_number(void);
float func_l3bfsc_cacluate_sensor_ws_bitmap_valid_weight(void);
OPSTAT func_l3bfsc_time_out_ws_init_req_process(void);
OPSTAT func_l3bfsc_time_out_ttt_wait_fb_process(void);
OPSTAT func_l3bfsc_time_out_tgu_wait_fb_process(void);
OPSTAT func_l3bfsc_time_out_error_scan_process(void);
OPSTAT func_l3bfsc_time_out_period_read_process(void);
OPSTAT func_l3bfsc_time_out_statistic_scan_process(void);
void func_l3bfsc_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFSC(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFSC_RECOVERY(...) do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC]++; func_l3bfsc_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
//#define HCU_ERROR_PRINT_L3BFSC	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC]++; HcuErrorPrint


#endif /* L3APP_BFSC_H_ */
