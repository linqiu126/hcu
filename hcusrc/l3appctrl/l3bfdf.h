/*
 * l3bfdf.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFDF_H_
#define L3APP_BFDF_H_

#include "../l0comvm/sysconfig.h"
#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3BFDF
{
	FSM_STATE_L3BFDF_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3BFDF_ACTIVED,
	FSM_STATE_L3BFDF_OOS_SCAN,  //进料组合态
	FSM_STATE_L3BFDF_OOS_TTT,  	//出料流程态
	FSM_STATE_L3BFDF_SUSPEND,
	FSM_STATE_L3BFDF_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3bfdf[];

//统一下位机板子的控制信息
typedef struct L3BfdfNodeBoardInfo
{
	UINT8  nodeStatus; 	//无效，空料，有料数值错误，有料待组合，有料待出料
	UINT8  cfgRcvFlag;   	//用来保存CFG_RESP是否收到
	UINT8  resumeRcvFlag;  	//用来保存RESUME_RESP是否收到
	UINT8  suspendRcvFlag;  //用来保存SUSPEND_RESP是否收到
}L3BfdfNodeBoardInfo_t;
#define HCU_L3BFDF_NODE_BOARD_STATUS_NONE			0
#define HCU_L3BFDF_NODE_BOARD_STATUS_OFFLINE		1
#define HCU_L3BFDF_NODE_BOARD_STATUS_HW_ERROR		2
#define HCU_L3BFDF_NODE_BOARD_STATUS_OFFLINE_MAX	9
#define HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MIN		10
#define HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP		11		//下位机上线
#define HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ 	12  	//配置开始
#define HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_CMPL 13  	//配置完成
#define HCU_L3BFDF_NODE_BOARD_STATUS_INIT_ERR 		17  	//初始化错误
#define HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MAX		29
#define HCU_L3BFDF_NODE_BOARD_STATUS_WORK_MIN 		30
#define HCU_L3BFDF_NODE_BOARD_STATUS_VALID 			31
#define HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR 	32
#define HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND 		33
#define HCU_L3BFDF_NODE_BOARD_STATUS_WORK_MAX 		49
#define HCU_L3BFDF_NODE_BOARD_STATUS_INVALID  		0xFF

//分组信息
typedef struct L3BfdfGroupInfo
{
	UINT16  groupId;
	UINT8	groupStatus;
	UINT16	totalHopperNbr;
	UINT16	firstHopperId;	//系统配置以后，最初从哪一个料斗开始
	UINT16	fillHopperId;	//优先从哪一个料斗开始查找，从而加速查找
	double	targetWeight;
	double	targetUpLimit;
	double  rangeLow;
	double  rangeHigh;
	double  rangeAvg;
	double  rangeSigma;
}L3BfdfGroupInfo_t;
//分组状态定义
#define HCU_L3BFDF_GROUP_STATUS_NONE			0
#define HCU_L3BFDF_GROUP_STATUS_DISABLE			1
#define HCU_L3BFDF_GROUP_STATUS_ACTIVE			2
#define HCU_L3BFDF_GROUP_STATUS_INVALID  		255

//料斗信息
typedef struct L3BfdfHopperInfo
{
	UINT16 hopperId;
	UINT16 groupId;
	UINT16 hopperStatus;
	UINT16 preHopperId;
	UINT16 nextHopperId;
	UINT8  basketStatus;
	UINT16 matLackNbr;		//用来计算在特定组别的情况下，采用欠缺算法，需要从多少个开始操控．当打开自动调配小组时，这个参数需要动态刷新．本项目估计暂时不需要．
	UINT16 matLackIndex;     //具体控制欠n的数量
	double  hopperValue;    //料斗总重量
	double  hopperLastMat;  //用来存储称重台到物料入料之间的期间，物料的重量．冲入则需要状态和算法控制．
}L3BfdfHopperInfo_t;
//料斗状态定义
#define HCU_L3BFDF_HOPPER_STATUS_NONE			0
#define HCU_L3BFDF_HOPPER_STATUS_OFFLINE		1
#define HCU_L3BFDF_HOPPER_STATUS_OFFLINE_MAX	9
#define HCU_L3BFDF_HOPPER_STATUS_INIT_MIN		10
#define HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC   11
#define HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC     12
#define HCU_L3BFDF_HOPPER_STATUS_INIT_MAX		29
#define HCU_L3BFDF_HOPPER_STATUS_WORK_MIN 		30
#define HCU_L3BFDF_HOPPER_STATUS_VALID          31
#define HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT     32
#define HCU_L3BFDF_HOPPER_STATUS_FULL_PRE       33  //已经满了，就等待拨杆反馈
#define HCU_L3BFDF_HOPPER_STATUS_FULL           34  //真正满了
#define HCU_L3BFDF_HOPPER_STATUS_VALID_ERR      35
#define HCU_L3BFDF_HOPPER_STATUS_WORK_MAX 		49
#define HCU_L3BFDF_HOPPER_STATUS_INVALID  		0xFF

#define HCU_L3BFDF_HOPPER_BASKET_NONE			0
#define HCU_L3BFDF_HOPPER_BASKET_EMPTY			1
#define HCU_L3BFDF_HOPPER_BASKET_FULL			2
#define HCU_L3BFDF_HOPPER_BASKET_INVALID  		0xFF

//全局定义：为了解决编译不成功的问题
#ifndef HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX
	#define HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX	2  	//流水线数量
#endif
#ifndef HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX
	#define HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX	4
#endif
#ifndef HCU_SYSCFG_BFDF_HOPPER_NBR_MAX
	#define HCU_SYSCFG_BFDF_HOPPER_NBR_MAX		33
#endif
#ifndef HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD
	#define HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD	8
#endif

//配置参数
typedef struct gTaskL3bfdfContextCombinationAlgorithmParamaters
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
}gTaskL3bfdfContextCombinationAlgorithmParamaters_t;
typedef struct gTaskL3bfdfContextCalibration
{
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
}gTaskL3bfdfContextCalibration_t;
#define HCU_L3BFDF_CALIBRATION_WGT_BELT_POINT 	240
typedef struct gTaskL3bfdfContextWeightSensorParamaters
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
	gTaskL3bfdfContextCalibration_t  calibration[HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX][HCU_L3BFDF_CALIBRATION_WGT_BELT_POINT];
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;				//皮重，分为每种
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorFilterCoeff[4];				// NOT for GUI
	UINT32  WeightSensorOutputValue[4];
}gTaskL3bfdfContextWeightSensorParamaters_t;
typedef struct gTaskL3bfdfContextMotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}gTaskL3bfdfContextMotorControlParamaters_t;

//统计信息
typedef struct gTaskL3bfdfContextStaEleMid
{
	double	wsIncMatCntMid;  			//物料数量
	double	wsIncMatWgtMid;  			//物料重量
	double	wsCombTimesMid;  			//总共成功素搜到目标的次数
	double	wsTttTimesMid;  			//TTT次数
	double	wsTgvTimesMid;  			//TGV次数
	double	wsTttMatCntMid;				//TTT物料数量
	double	wsTgvMatCntMid;				//TGV物料数量
	double	wsTttMatWgtMid;				//TTT物料重量
	double	wsTgvMatWgtMid;				//TGV物料重量
	double	wsAvgTttTimesMid;			//TTT平均次数
	double	wsAvgTttMatCntMid;			//TTT平均物料数
	double	wsAvgTttMatWgtMid;			//TTT平均重量
}gTaskL3bfdfContextStaEleMid_t;

//统计周期，为了计算滑动平均数据
#define HCU_L3BFDF_STA_CYCLE_DUR  60000 //1分钟，相当于60S
#define HCU_L3BFDF_STA_UNIT_DUR  500 //500ms的单位，这是统计周期颗粒度，跟TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN保持一致
#define HCU_L3BFDF_STA_BASE_CYCLE  (HCU_L3BFDF_STA_CYCLE_DUR / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_5S_CYCLE  (5000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_1M_CYCLE  (60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_15M_CYCLE  (15 * 60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_60M_CYCLE  (60 * 60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_2H_CYCLE  (2* 60 * 60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_8H_CYCLE  (8* 60 * 60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_24H_CYCLE  (24* 60 * 60 * 1000 / HCU_L3BFDF_STA_UNIT_DUR)
#define HCU_L3BFDF_STA_AGEING_COEF_ALPHA   20  //老化系数，越大表示最近的权重越大，100%的情况就是120，相当于当前周期的统计数据除以统计周期
#define HCU_L3BFDF_STA_AGEING_COEF (float)(((float)(HCU_L3BFDF_STA_BASE_CYCLE-HCU_L3BFDF_STA_AGEING_COEF_ALPHA))/(float)HCU_L3BFDF_STA_BASE_CYCLE)
#define HCU_L3BFDF_STA_24H_IN_SECOND  (24*3600)

#define HCU_L3BFDF_STA_DBI_TABLE_LOCALUI   	"BFDF_STA_LOCAL_UI"
#define HCU_L3BFDF_STA_DBI_TABLE_ONEMIN   	"BFDF_STA_ONE_MIN"
#define HCU_L3BFDF_STA_DBI_TABLE_15MIN   	"BFDF_STA_15_MIN"
#define HCU_L3BFDF_STA_DBI_TABLE_60MIN   	"BFDF_STA_60_MIN"
#define HCU_L3BFDF_STA_DBI_TABLE_2HOUR   	"BFDF_STA_2_HOUR"
#define HCU_L3BFDF_STA_DBI_TABLE_8HOUR   	"BFDF_STA_8_HOUR"
#define HCU_L3BFDF_STA_DBI_TABLE_24HOUR   	"BFDF_STA_24_HOUR"
#define HCU_L3BFDF_STA_DBI_TABLE_UP2NOW   	"BFDF_STA_UP_2_NOW"

//主体上下文
typedef struct gTaskL3bfdfContext
{
	//静态配置参数部分
	UINT16	configId;  												//用来标识系统工作在哪一套配置参数中
	char    configName[20];
	UINT32  start24hStaTimeInUnix;									//系统配置的参数，表示24小时统计的日历起点
	gTaskL3bfdfContextCombinationAlgorithmParamaters_t 	comAlgPar;
	gTaskL3bfdfContextWeightSensorParamaters_t			wgtSnrPar;
	gTaskL3bfdfContextMotorControlParamaters_t			motCtrPar;

	//动态部分
	//nodeDyn的编制原则是：0一定表达WGT板子，1-HCU_L3BFDF_NODE_BOARD_NBR_MAX表达一条流水先上的总共的板子数量
	L3BfdfNodeBoardInfo_t  	nodeDyn[HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX][HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX];
	UINT16					totalGroupNbr[HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX]; //分成多少个组，这个数据不包括第一组，特别注意！
	L3BfdfGroupInfo_t		group[HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX][HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	L3BfdfHopperInfo_t  	hopper[HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX][HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	//板子的编制，也必须从１-N，0表示垃圾桶

	//实时统计部分：均以一个统计周期为单位
	UINT32  startWorkTimeInUnix;		//表示该系统开始工作的时间日程点
	UINT32  elipseCnt;					//所有的统计结果和数据，均以这个为时间统计尺度，时间颗粒度另外定义，假设是500ms为统计周期
	UINT32  elipse24HourCnt;			//24小时的日历计数器
	HcuSysMsgIeL3bfdfContextStaElement_t cur;  		//当前统计基础颗粒中的数值
	gTaskL3bfdfContextStaEleMid_t  		 curAge;		//使用老化算法，需要该域存下中间结果，不然每一次计算均采用近似会导致数据失真

	//统计报告部分
	HcuSysMsgIeL3bfdfContextStaElement_t staLocalUi;  //滑动平均给本地UI的数据
	HcuSysMsgIeL3bfdfContextStaElement_t staOneMin;  	//1分钟统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t sta15Min;   	//15分钟统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t sta60Min;	//60分钟统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t sta2H;		//2H统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t sta8H;		//8H统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t sta24H;		//24H统计结果
	HcuSysMsgIeL3bfdfContextStaElement_t staUp2Now;	//连续工作到目前的统计结果

}gTaskL3bfdfContext_t;
extern gTaskL3bfdfContext_t gTaskL3bfdfContext;

//统计打印报告的频率调整
#define HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY 10

//FSM通用部分
extern OPSTAT fsm_l3bfdf_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_snc_pulliin_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_basket_clean_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//CLOUDVELA后台通信部分
extern OPSTAT fsm_l3bfdf_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//API启动及命令控制部分
void func_l3bfdf_stm_main_recovery_from_fault(void);  //提供了一种比RESTART更低层次的状态恢复方式

//Local API
OPSTAT func_l3bfdf_int_init(void);
OPSTAT func_l3bfdf_time_out_sys_cfg_req_process(void);
OPSTAT func_l3bfdf_time_out_comb_out_req_process(void);
OPSTAT func_l3bfdf_time_out_statistic_scan_process(void);
bool   func_l3bfdf_cacluate_sensor_cfg_start_rcv_complete(void);


//核心双链数据处理
extern bool func_l3bfdf_group_allocation(UINT8 streamId, UINT16 nbrGroup);
extern bool func_l3bfdf_hopper_state_set_init(UINT8 streamId);
extern bool func_l3bfdf_hopper_state_set_valid(UINT8 streamId);
extern bool func_l3bfdf_hopper_state_set_single_hopper_N_coef(UINT8 streamId, UINT16 hid);
extern bool func_l3bfdf_hopper_add_by_tail(UINT8 streamId, UINT16 groupId, UINT16 hopperNewId);
extern bool func_l3bfdf_hopper_add_by_group(UINT8 streamId, UINT16 groupId, UINT16 nbrHopper);
extern bool func_l3bfdf_hopper_add_by_group_in_average_distribution(UINT8 streamId, UINT16 nbrGroup);
extern bool func_l3bfdf_hopper_add_by_group_in_normal_distribution(UINT8 streamId, UINT16 nbrGroup);
extern bool func_l3bfdf_hopper_remove_by_tail(UINT8 streamId, UINT16 groupId);
extern bool func_l3bfdf_hopper_insert_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId, UINT16 hopperNewId);
extern bool func_l3bfdf_hopper_del_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId);
extern int  func_l3bfdf_hopper_dual_chain_audit(void);
extern bool func_l3bfdf_group_auto_alloc_init_range_in_average(UINT8 streamId, UINT16 nbrGroup, double wgtMin, double wgtMax);
extern bool func_l3bfdf_group_auto_alloc_init_target_with_uplimit(UINT8 streamId, double targetWgt, double ulRatio);
extern bool func_l3bfdf_print_all_hopper_status_by_id(UINT8 streamId);
extern bool func_l3bfdf_print_all_hopper_status_by_chain(UINT8 streamId);
extern bool func_l3bfdf_print_all_hopper_ratio_by_weight(UINT8 streamId);

//核心搜索算法
UINT16 func_l3bfdf_new_ws_search_group(UINT8 streamId, double weight);
UINT16 func_l3bfdf_new_ws_search_hopper_full(UINT8 streamId);
UINT16 func_l3bfdf_new_ws_search_hopper_lack_one(UINT8 streamId, UINT16 gid, double weight);
UINT16 func_l3bfdf_new_ws_search_hopper_valid_normal(UINT8 sid, UINT16 gid, double weight);
bool   func_l3bfdf_new_ws_send_out_pullin_message(UINT8 streamId, UINT16 hopperId);
bool   func_l3bfdf_new_ws_send_out_comb_out_message(UINT8 streamId, UINT16 hopperId);


//基础函数
double gaussian(double u, double n);

//External APIs
extern OPSTAT hcu_sps232_qr_printer_init(void);
extern void   hcu_sps232_send_char_to_ext_printer(char *s, int len);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFDF(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFDF_RECOVERY(...) do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++; func_l3bfdf_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L3APP_BFDF_H_ */
