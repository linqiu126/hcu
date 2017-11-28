/*
 * l3bfhs.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFHS_H_
#define L3APP_BFHS_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3BFHS
{
	FSM_STATE_L3BFHS_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3BFHS_ACTIVED,
	FSM_STATE_L3BFHS_OOS_SCAN,  //进料态
	FSM_STATE_L3BFHS_SUSPEND,
	FSM_STATE_L3BFHS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3bfhs[];

typedef struct L3BfhsNodeBoardInfo
{
	UINT8  nodeStatus; 	//无效，空料，有料数值错误，有料待组合，有料待出料
	UINT8  cfgRcvFlag;   	//用来保存CFG_RESP是否收到
	UINT8  resumeRcvFlag;  	//用来保存RESUME_RESP是否收到
	UINT8  suspendRcvFlag;  //用来保存SUSPEND_RESP是否收到
}L3BfhsNodeBoardInfo_t;
#define HCU_L3BFHS_NODE_BOARD_STATUS_NONE			0
#define HCU_L3BFHS_NODE_BOARD_STATUS_OFFLINE		1
#define HCU_L3BFHS_NODE_BOARD_STATUS_HW_ERROR		2
#define HCU_L3BFHS_NODE_BOARD_STATUS_OFFLINE_MAX	9
#define HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MIN		10
#define HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP		11		//下位机上线
#define HCU_L3BFHS_NODE_BOARD_STATUS_CFG_START_REQ 	12  	//配置开始
#define HCU_L3BFHS_NODE_BOARD_STATUS_CFG_START_CMPL 13  	//配置完成
#define HCU_L3BFHS_NODE_BOARD_STATUS_INIT_ERR 		17  	//初始化错误
#define HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MAX		29
#define HCU_L3BFHS_NODE_BOARD_STATUS_WORK_MIN 		30
#define HCU_L3BFHS_NODE_BOARD_STATUS_VALID 			31
#define HCU_L3BFHS_NODE_BOARD_STATUS_VALID_ERROR 	32
#define HCU_L3BFHS_NODE_BOARD_STATUS_SUSPEND 		33
#define HCU_L3BFHS_NODE_BOARD_STATUS_WORK_MAX 		49
#define HCU_L3BFHS_NODE_BOARD_STATUS_INVALID  		0xFF

//统计周期，为了计算滑动平均数据
#define HCU_L3BFHS_STA_CYCLE_DUR  60000 //1分钟，相当于60S
#define HCU_L3BFHS_STA_UNIT_DUR  500 //500ms的单位，这是统计周期颗粒度，跟TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN保持一致
#define HCU_L3BFHS_STA_BASE_CYCLE  (HCU_L3BFHS_STA_CYCLE_DUR / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_5S_CYCLE  (5000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_1M_CYCLE  (60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_15M_CYCLE  (15 * 60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_60M_CYCLE  (60 * 60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_2H_CYCLE  (2* 60 * 60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_8H_CYCLE  (8* 60 * 60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_24H_CYCLE  (24* 60 * 60 * 1000 / HCU_L3BFHS_STA_UNIT_DUR)
#define HCU_L3BFHS_STA_AGEING_COEF_ALPHA   20  //老化系数，越大表示最近的权重越大，100%的情况就是120，相当于当前周期的统计数据除以统计周期
#define HCU_L3BFHS_STA_AGEING_COEF (float)(((float)(HCU_L3BFHS_STA_BASE_CYCLE-HCU_L3BFHS_STA_AGEING_COEF_ALPHA))/(float)HCU_L3BFHS_STA_BASE_CYCLE)
#define HCU_L3BFHS_STA_24H_IN_SECOND  (24*3600)

#define HCU_L3BFHS_STA_DBI_TABLE_LOCALUI   	"BFHS_STA_LOCAL_UI"
#define HCU_L3BFHS_STA_DBI_TABLE_ONEMIN   	"BFHS_STA_ONE_MIN"
#define HCU_L3BFHS_STA_DBI_TABLE_15MIN   	"BFHS_STA_15_MIN"
#define HCU_L3BFHS_STA_DBI_TABLE_60MIN   	"BFHS_STA_60_MIN"
#define HCU_L3BFHS_STA_DBI_TABLE_2HOUR   	"BFHS_STA_2_HOUR"
#define HCU_L3BFHS_STA_DBI_TABLE_8HOUR   	"BFHS_STA_8_HOUR"
#define HCU_L3BFHS_STA_DBI_TABLE_24HOUR   	"BFHS_STA_24_HOUR"
#define HCU_L3BFHS_STA_DBI_TABLE_UP2NOW   	"BFHS_STA_UP_2_NOW"

//临时数据暂存内容
typedef struct gTaskL3bfhsContextStaEleMid
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
}gTaskL3bfhsContextStaEleMid_t;

//配置参数
typedef struct gTaskL3bfhsContextWgtSnrParamaters
{
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
    UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT32	MaxAllowedWeight;				//称重物品的范围上限 NF2 format
	UINT32  MinAllowedWeight;        //称重物品的范围下限 NF2 format
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;
    UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //location of decimal point for Preload Compensation, for example,6.25%, this value is 2.
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT8   WeightSensorMeasurementRangeNo; //object 0x2040, Default is 0, set measurement range no(totally 3),which is displayed in 0x2041
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   WeightSensorTimeGrid;  //object 0x2222, send weight value in a fixed time grid.
	UINT8   WeightSensorAlgoSelect;  //weight algorithm select
}gTaskL3bfhsContextWgtSnrParamaters_t;

typedef struct gTaskL3bfhsContextMotoCtrlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
}gTaskL3bfhsContextMotoCtrlParamaters_t;

typedef struct gTaskL3bfhsContextArmCtrlParamaters
{
	UINT32	ArmRollingStartMs;						//how long do the arm rolling for start action
	UINT32	ArmRollingStopMs;							//how long do the arm rolling for stop action
	UINT32	ArmRollingInveralMs;					//If the arm is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	ArmFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	ArmFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}gTaskL3bfhsContextArmCtrlParamaters_t;

//主体上下文
#define HCU_L3BFHS_CONTEXT_OPERATOR_NAME_LEN_MAX    20
#define HCU_L3BFHS_CONTEXT_CONFIG_NAME_LEN_MAX    	20
typedef struct gTaskL3bfhsContext
{
	//静态配置参数部分
	gTaskL3bfhsContextWgtSnrParamaters_t 			wgtSnrPar;
	gTaskL3bfhsContextMotoCtrlParamaters_t			motoCtrlPar;
	gTaskL3bfhsContextArmCtrlParamaters_t			armCtrlPar;
	UINT32  start24hStaTimeInUnix;		//系统配置的参数，表示24小时统计的日历起点

	//动态部分
	UINT32  callCellId;					//批次数据
	char    operatorName[HCU_L3BFHS_CONTEXT_OPERATOR_NAME_LEN_MAX];
	UINT16	configId;  					//用来标识系统工作在哪一套配置参数中
	char    configName[HCU_L3BFHS_CONTEXT_CONFIG_NAME_LEN_MAX];
	UINT32  startWorkTimeInUnix;		//表示该系统开始工作的时间日程点
	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是500ms为统计周期
	UINT32  elipse24HourCnt;			//24小时的日历计数器
	L3BfhsNodeBoardInfo_t	sensorWs[1];   //固定为0

	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3bfhsContextStaElement_t cur;  			//当前统计基础颗粒中的数值
	gTaskL3bfhsContextStaEleMid_t  		 curAge;	    //使用老化算法，需要该域存下中间结果，不然每一次计算均采用近似会导致数据失真
	//统计报告部分
	HcuSysMsgIeL3bfhsContextStaElement_t staLocalUi;  	//滑动平均给本地UI的数据
	HcuSysMsgIeL3bfhsContextStaElement_t staOneMin;  	//1分钟统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t sta15Min;   	//15分钟统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t sta60Min;		//60分钟统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t sta2H;			//2H统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t sta8H;			//8H统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t sta24H;		//24H统计结果
	HcuSysMsgIeL3bfhsContextStaElement_t staUp2Now;		//连续工作到目前的统计结果
}gTaskL3bfhsContext_t;
extern gTaskL3bfhsContext_t gTaskL3bfhsContext;

//统计打印报告的频率调整
#define HCU_L3BFHS_STATISTIC_PRINT_FREQUENCY 10

//API通用部分
extern OPSTAT fsm_l3bfhs_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//API启动及命令控制部分
extern OPSTAT fsm_l3bfhs_uicomm_ctrl_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_cal_zero_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_cal_full_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_startup_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_fault_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_heart_beat_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//API组合部分
extern OPSTAT fsm_l3bfhs_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_error_inq_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//CLOUDVELA后台通信部分
extern OPSTAT fsm_l3bfhs_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3bfhs_int_init(void);
OPSTAT func_l3bfhs_time_out_sys_cfg_start_wait_fb_process(void);
OPSTAT func_l3bfhs_time_out_sys_suspend_wait_fb_process(void);
OPSTAT func_l3bfhs_time_out_sys_resume_wait_fb_process(void);
OPSTAT func_l3bfhs_time_out_cal_zero_wait_fb_process(void);
OPSTAT func_l3bfhs_time_out_cal_full_wait_fb_process(void);
OPSTAT func_l3bfhs_time_out_error_inq_process(void);
OPSTAT func_l3bfhs_time_out_statistic_scan_process(void);
void func_l3bfhs_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式

//External APIs
extern OPSTAT hcu_sps232_qr_printer_init(void);
extern void   hcu_sps232_send_char_to_ext_printer(char *s, int len);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFHS(...)	           	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFHS_RECOVERY(...)   	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]++; func_l3bfhs_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L3APP_BFHS_H_ */
