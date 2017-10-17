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

typedef struct L3BfhsSensorWsInfo
{
	UINT8  sensorWsId;
	UINT32 sensorValue;
	UINT32 sensorRepTimes;
	UINT8  sensorStatus; //无效，空料，有料数值错误，有料待组合，有料待出料
	UINT8  cfgRcvFlag;   //用来保存CFG_RESP是否收到
	UINT8  startRcvFlag;  //用来保存START_RESP是否收到
	UINT8  stopRcvFlag;   //用来保存STOP_RESP是否收到
}L3BfhsSensorWsInfo_t;
//秤盘状态定义
#define HCU_L3BFHS_SENSOR_WS_STATUS_INVALID			0  		//秤盘无效
#define HCU_L3BFHS_SENSOR_WS_STATUS_OFFLINE		 	1
#define HCU_L3BFHS_SENSOR_WS_STATUS_HW_ERROR		2
#define HCU_L3BFHS_SENSOR_WS_STATUS_OFFLINE_MAX		9
#define HCU_L3BFHS_SENSOR_WS_STATUS_INIT_MIN		10
#define HCU_L3BFHS_SENSOR_WS_STATUS_STARTUP		 	11		//下位机上线
#define HCU_L3BFHS_SENSOR_WS_STATUS_CFG_REQ 		12  	//配置开始
#define HCU_L3BFHS_SENSOR_WS_STATUS_SUSPEND_REQ 	13  	//暂停开始
#define HCU_L3BFHS_SENSOR_WS_STATUS_INIT_ERR 		17  	//初始化错误
#define HCU_L3BFHS_SENSOR_WS_STATUS_INIT_MAX		29
#define HCU_L3BFHS_SENSOR_WS_STATUS_WORK_MIN 		30
#define HCU_L3BFHS_SENSOR_WS_STATUS_VALIID_EMPTY 	31      //秤盘空
#define HCU_L3BFHS_SENSOR_WS_STATUS_VALID_ERROR 	32 		//秤盘有料数值错误
#define HCU_L3BFHS_SENSOR_WS_STATUS_VALID_TO_COMB 	33 		//秤盘有料待组合
#define HCU_L3BFHS_SENSOR_WS_STATUS_WORK_MAX 		49

#define HCU_L3BFHS_SENSOR_WS_STATUS_INVALID1  		255  	//秤盘无效

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
typedef struct gTaskL3bfhsContextCombinationAlgorithmParamaters
{
	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
	UINT32	MinScaleNumberStartCombination;			//开始查找的最小个数，就是说大于这个个数就开始搜索
	UINT32	TargetCombinationWeight;				//组合目标重量
	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
	UINT32	IsPriorityScaleEnabled;					// 1: Enable, 0: Disable
	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
	UINT32	CombinationBias;						      //每个Scale要求放几个物品
	UINT32	IsRemainDetectionEnable;				  //Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	RemainScaleTreatment;					    // 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
	UINT32	CombinationSpeedMode;					    // 0：SpeedPriority，1: PrecisePriority
	UINT32	CombinationAutoMode;					    // 0: Auto, 1: Manual
	UINT32	MovingAvrageSpeedCount;					  //计算平均速度的时候使用最近多少个组合做统计
}gTaskL3bfhsContextCombinationAlgorithmParamaters_t;
//Proximity
#define HCU_L3BFHS_COMB_ALG_PAR_PROXIMITY_DISABLE  				0
#define HCU_L3BFHS_COMB_ALG_PAR_PROXIMITY_ABOVE_UP_LIMIT  		1
#define HCU_L3BFHS_COMB_ALG_PAR_PROXIMITY_BELOW_DN_LIMIT  		2
//Scale Priority
#define HCU_L3BFHS_COMB_ALG_PAR_PRIORITY_DISABLE  				0
#define HCU_L3BFHS_COMB_ALG_PAR_PRIORITY_ENABLE  				1

typedef struct gTaskL3bfhsContextCalibration
{
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
}gTaskL3bfhsContextCalibration_t;
typedef struct gTaskL3bfhsContextWeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
	UINT32  RemainDetectionTimeSec;
	UINT32	WeightSensorInitOrNot;							// NOT for GUI
	UINT32	WeightSensorAdcSampleFreq;
	UINT32	WeightSensorAdcGain;
	UINT32	WeightSensorAdcBitwidth;						// NOT for GUI
	UINT32  WeightSensorAdcValue;								// NOT for GUI
	//gTaskL3bfhsContextCalibration_t  calibration[HCU_SYSMSG_L3BFHS_MAX_SENSOR_NBR];
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;				//皮重，分为每种
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorFilterCoeff[4];				// NOT for GUI
	UINT32  WeightSensorOutputValue[4];
}gTaskL3bfhsContextWeightSensorParamaters_t;

typedef struct gTaskL3bfhsContextMotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}gTaskL3bfhsContextMotorControlParamaters_t;

//主体上下文
#define HCU_L3BFHS_CONTEXT_OPERATOR_NAME_LEN_MAX    20
#define HCU_L3BFHS_CONTEXT_CONFIG_NAME_LEN_MAX    	20
typedef struct gTaskL3bfhsContext
{
	//静态配置参数部分
	gTaskL3bfhsContextCombinationAlgorithmParamaters_t 	comAlgPar;
	gTaskL3bfhsContextWeightSensorParamaters_t			wgtSnrPar;
	gTaskL3bfhsContextMotorControlParamaters_t			motCtrPar;
	UINT32  start24hStaTimeInUnix;		//系统配置的参数，表示24小时统计的日历起点

	//动态部分
	UINT32  sessionId;
	char    operatorName[HCU_L3BFHS_CONTEXT_OPERATOR_NAME_LEN_MAX];
	UINT16	configId;  					//用来标识系统工作在哪一套配置参数中
	char    configName[HCU_L3BFHS_CONTEXT_CONFIG_NAME_LEN_MAX];
	UINT32  startWorkTimeInUnix;		//表示该系统开始工作的时间日程点
	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是500ms为统计周期
	UINT32  elipse24HourCnt;			//24小时的日历计数器

	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3bfhsContextStaElement_t cur;  			//当前统计基础颗粒中的数值
	gTaskL3bfhsContextStaEleMid_t  		curAge;			//使用老化算法，需要该域存下中间结果，不然每一次计算均采用近似会导致数据失真
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
extern OPSTAT fsm_l3bfhs_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfhs_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

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
OPSTAT func_l3bfhs_time_out_sys_cfg_req_process(void);
OPSTAT func_l3bfhs_time_out_sys_start_req_process(void);
OPSTAT func_l3bfhs_time_out_sys_stop_req_process(void);
OPSTAT func_l3bfhs_time_out_error_inq_process(void);
OPSTAT func_l3bfhs_time_out_statistic_scan_process(void);
void func_l3bfhs_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式

//External APIs
extern OPSTAT hcu_sps232_qr_printer_init(void);
extern void   hcu_sps232_send_char_to_ext_printer(char *s, int len);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFHS(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFHS_RECOVERY(...) do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]++; func_l3bfhs_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
//#define HCU_ERROR_PRINT_L3BFHS	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]++; HcuErrorPrint


#endif /* L3APP_BFHS_H_ */
