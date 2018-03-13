/*
 * l3bfdf.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFDF_H_
#define L3APP_BFDF_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l2frame/cloudvela.h"
#include "../l2codec/huicobus.h"
#include "../l2codec/huicobuscodec.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3BFDF
{
	FSM_STATE_L3BFDF_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3BFDF_ACTIVED,
	FSM_STATE_L3BFDF_OOS_SCAN,  //进料组合态
	FSM_STATE_L3BFDF_SUSPEND,
	FSM_STATE_L3BFDF_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3bfdf[];

/*
 * 全局标签
   CANID:
			0 => 第1条流水线固定垃圾桶
			1 => 第1条流水线WGT板子
			2-7 => 第1条流水线SNR板子
			8 => 第2条流水线固定垃圾桶
			9 => 第2条流水线WGT板子
			10-15 => 第2条流水线SNR板子
			16 => 第3条流水线固定垃圾桶
			17 => 第3条流水线WGT板子
			18-23 => 第3条流水线SNR板子
			24 => 第4条流水线固定垃圾桶
			25 => 第4条流水线WGT板子
			26-31 => 第4条流水线SNR板子
			注意：线路LINE/STREAMLINE的编号还是从0-N的，而非从１开始
	BOARDID:　等同于CANID

	//下面的定义有问题，后续需要进一步改进：问题所在是每条线都需要垃圾桶，
	 或者是HOPPERID只是线路下的子空间，不是全局空间，这样０表示垃圾桶，１－３２表示正常料斗
	 另外一个逻辑是用U16的高位表示流水线，低位表示料斗号，这样更加安全
	 ＝＞由于HUITP.H中以及使用了1-32/33-64的分类，这里就按照这个方式进行编码
	AP/HOOPERID:
			0: =>固定垃圾桶
			1-32: 	=>fix for all lines
*/
//统一下位机板子的控制信息
typedef struct L3BfdfNodeBoardInfo
{
	UINT8  nodeStatus; 	//无效，空料，有料数值错误，有料待组合，有料待出料
	UINT8  cfgRcvFlag;   	//用来保存CFG_RESP/RESUME_RESP/SUSPEND_RESP是否收到
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
#define HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND_REQ	33
#define HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND 		34
#define HCU_L3BFDF_NODE_BOARD_STATUS_RESUME_REQ		35
#define HCU_L3BFDF_NODE_BOARD_STATUS_WORK_MAX 		49
#define HCU_L3BFDF_NODE_BOARD_STATUS_INVALID  		0xFF

//整个系统全部采用NF2的整数形式进行处理

//分组信息
typedef struct L3BfdfGroupInfo
{
	UINT16  groupId;
	UINT8	groupStatus;
	UINT16	totalHopperNbr;
	UINT16	firstHopperId;	//系统配置以后，最初从哪一个料斗开始
	UINT16	fillHopperId;	//优先从哪一个料斗开始查找，从而加速查找
	UINT32	targetWeight;	//目标重量
	UINT32	targetUpLimit;	//上限比例部分，小数，不是最大值，所以这里定义的并不是目标最大值，而是最大值与目标之间的差值
	UINT32	bufWgtTarget;	//篮筐之外的缓冲区
	UINT32  rangeLow;
	UINT32  rangeHigh;
	UINT32  rangeAvg;
	UINT32  rangeSigma;
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
	UINT16 matLackNbr;		//用来计算在特定组别的情况下，采用欠缺算法，需要从多少个开始操控
	UINT16 matLackIndex;     //具体控制欠n的数量
	UINT32  hopperValue;    //料斗总重量
	//UINT32  hopperLastMat;  //用来存储称重台到物料入料之间的期间，物料的重量．冲入则需要状态和算法控制：为了改造COMB_FB成为可重入工作模式，该工作机制将被废弃
	UINT32  buferValue;		//用来控制buffer的重量数值
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
#define HCU_L3BFDF_HOPPER_STATUS_VALID          31	//状态转换到工作态，空料斗
#define HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT     32	//开始有物料，直接进篮筐
#define HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL    33  //篮筐满了
#define HCU_L3BFDF_HOPPER_STATUS_BUF_CONT       34  //BUF继续
#define HCU_L3BFDF_HOPPER_STATUS_BUF_FULL       35  //上层BUFFER也满了，一旦收到清零篮筐，必然回到HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT状态，因为篮筐不可能直接满的
#define HCU_L3BFDF_HOPPER_STATUS_VALID_ERR      36
#define HCU_L3BFDF_HOPPER_STATUS_WORK_MAX 		49
#define HCU_L3BFDF_HOPPER_STATUS_INVALID  		0xFF

//配置参数
typedef struct gTaskL3bfdfContextWeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//BFDF: 如果发现超过这个最大值，说明Sensor出错
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorMovingEverageSample;
}gTaskL3bfdfContextWeightSensorParamaters_t;
typedef struct gTaskL3bfdfContextMotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}gTaskL3bfdfContextMotorControlParamaters_t;
typedef struct gTaskL3bfdfContextActionControlParamaters
{
	UINT16  TWeightInd;                     /* After INFRA INT, Wait for how long to send WEIGHT_IND, unit is 10ms Tick */
	UINT16  T0bis;                          /* After INFRA INT, INFRA INT sent to Node 2 to 5, unit is 10ms Tick */
	UINT16  TA0;                            /* After INFRA INT, Deay to AP01 */
	UINT16  TActCmd;
	UINT16  TArmStart;
	UINT16  TArmStop;
	UINT16  TDoorCloseLightOn;
	UINT16  TApIntervalMin;
	UINT16  TApInterval[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	UINT16  TLocalAp[HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD];
	UINT16  DelayNode1ToX;
	UINT16  DelayUpHcuAlgoDl;
}gTaskL3bfdfContextActionControlParamaters_t;
typedef struct gTaskL3bfdfContextDynCalibrationParamaters
{
    UINT8            zero_cal_iteration;    /* 8 for ZERO, 4 for FULL */
    UINT8            full_cal_iteration;
    UINT16           TWeightInd;
    UINT32           full_weight; /* in 0.01g */
    UINT32           motor_speed;
    UINT32           adc_sample_freq;
    UINT32           adc_gain;
    UINT32           noise_floor_filter_factor;   /* 0 - 100: 0.00 to 1.00, y = factor * x(n) + (1-factor) * x(n-1), 0 means disable */
    UINT32           max_allowed_weight; /* in 0.01g */
    UINT32           WeightSensorTailorValue;
}gTaskL3bfdfContextDynCalibrationParamaters_t;

//统计信息：为了提高本地计算的精度
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
	double  wsCallCellTimesMid;			//成功出料到外部的次数
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
	gTaskL3bfdfContextWeightSensorParamaters_t			wgtSnrPar;
	gTaskL3bfdfContextMotorControlParamaters_t			motMainPar;
	gTaskL3bfdfContextMotorControlParamaters_t			motSecondPar;
	gTaskL3bfdfContextActionControlParamaters_t			actionCtrlPar;
	gTaskL3bfdfContextDynCalibrationParamaters_t		dynCalPar;
	UINT32  start24hStaTimeInUnix;						//系统配置的参数，表示24小时统计的日历起点

	//动态部分
	UINT32  sessionId;								//批次数据
	char    operatorName[HCU_L3BFDF_CONTEXT_OPERATOR_NAME_LEN_MAX];
	UINT16	configId;  								//用来标识系统工作在哪一套配置参数中
	UINT8   engModeSwitch;  //Engineering mode switch
	char    configName[HCU_L3BFDF_CONTEXT_CONFIG_NAME_LEN_MAX];
	UINT8   nbrStreamLine;			//缺省为2，最大不能超过4
	UINT8   nbrIoBoardPerLine;		//缺省为4，最大不能超过4
	float   combAlgoSpaceCtrlRatio;  //解空间控制系数，降低算法的敏感度

	//nodeDyn的编制原则是：0一定表达WGT板子，1-HCU_L3BFDF_NODE_BOARD_NBR_MAX表达一条流水先上的总共的板子数量
	//Line从0开始，正常0-1
	//GroupId从1开始
	//HopperId从1-32，只有本地意义。0表示垃圾桶
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
#define HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL		(gTaskL3bfdfContext.nbrStreamLine)
#define HCU_L3BFDF_MAX_IO_BOARD_NBR_ACTUAL		(gTaskL3bfdfContext.nbrIoBoardPerLine + 2)
#define HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL	(gTaskL3bfdfContext.nbrIoBoardPerLine* HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD + 1)

//统计打印报告的频率调整
#define HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY 10

//FSM通用部分
extern OPSTAT fsm_l3bfdf_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_uicomm_ctrl_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_dyn_cal_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_basket_clean_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_startup_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_fault_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfdf_canitf_heart_beat_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

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
OPSTAT func_l3bfdf_time_out_statistic_scan_process(void);
bool   func_l3bfdf_cacluate_sensor_cfg_start_rcv_complete(void);
bool   func_l3bfdf_cacluate_sensor_suspend_rcv_complete(void);
bool   func_l3bfdf_cacluate_sensor_resume_rcv_complete(void);
bool   func_l3bfdf_is_there_any_board_not_yet_startup(void);
OPSTAT func_l3bfdf_send_out_suspend_message_to_all(void);
OPSTAT func_l3bfdf_send_out_resume_message_to_all(void);
OPSTAT func_l3bfdf_send_out_cfg_start_message_to_all(void);
bool   func_l3bfdf_print_all_board_status(void);

//核心双链数据处理
extern bool func_l3bfdf_group_allocation(UINT8 streamId, UINT16 nbrGroup);
extern bool func_l3bfdf_hopper_state_set_init(UINT8 streamId);
extern bool func_l3bfdf_hopper_state_set_valid(UINT8 streamId);
extern bool func_l3bfdf_hopper_update_N_coef(UINT8 streamId, UINT16 hid);
extern bool func_l3bfdf_hopper_add_by_tail(UINT8 streamId, UINT16 groupId, UINT16 hopperNewId);
extern bool func_l3bfdf_hopper_add_by_group_element(UINT8 streamId, UINT16 groupId, UINT16 nbrHopper);
extern bool func_l3bfdf_hopper_add_by_grp_in_average_distribution(UINT8 streamId, UINT16 nbrGroup);
extern bool func_l3bfdf_hopper_add_by_grp_in_normal_distribution(UINT8 streamId, UINT16 nbrGroup);  //比较复杂，所以暂时没有使用
extern bool func_l3bfdf_hopper_remove_by_tail(UINT8 streamId, UINT16 groupId);
extern bool func_l3bfdf_hopper_insert_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId, UINT16 hopperNewId);
extern bool func_l3bfdf_hopper_del_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId);
extern int  func_l3bfdf_hopper_dual_chain_audit(void);
extern bool func_l3bfdf_group_auto_alloc_init_range_in_average(UINT8 streamId, UINT16 nbrGroup, UINT32 wgtMin, UINT32 wgtMax);
extern bool func_l3bfdf_group_auto_alloc_init_target_with_uplimit(UINT8 streamId, UINT32 targetWgt, double ulRatio);
extern bool func_l3bfdf_print_all_hopper_status_by_id(UINT8 streamId);
extern bool func_l3bfdf_print_all_hopper_status_by_chain(UINT8 streamId);
extern bool func_l3bfdf_print_all_hopper_ratio_by_weight(UINT8 streamId);

//核心搜索算法
UINT16 func_l3bfdf_new_ws_search_group(UINT8 streamId, double weight);
UINT16 func_l3bfdf_new_ws_search_hopper_double_full(UINT8 streamId);
UINT16 func_l3bfdf_new_ws_search_hopper_basket_full(UINT8 streamId);
UINT16 func_l3bfdf_new_ws_search_hopper_buffer_lack_one(UINT8 streamId, UINT16 gid, UINT32 weight);
UINT16 func_l3bfdf_new_ws_search_hopper_buffer_normal(UINT8 streamId, UINT16 gid, UINT32 weight);
UINT16 func_l3bfdf_new_ws_search_hopper_basket_lack_one(UINT8 streamId, UINT16 gid, UINT32 weight);
UINT16 func_l3bfdf_new_ws_search_hopper_valid_normal(UINT8 sid, UINT16 gid, UINT32 weight);
bool   func_l3bfdf_hopper_judge_cur_mat_is_in_right_space(UINT8 streamId, UINT16 hid, UINT32 weight);
bool   func_l3bfdf_hopper_judge_cur_mat_is_in_buffer_space(UINT8 streamId, UINT16 hid, UINT32 weight);
bool   func_l3bfdf_hopper_judge_buffer_is_lack_one_full(UINT8 sid, UINT16 hid, UINT32 weight);
bool   func_l3bfdf_new_ws_send_out_comb_out_message_by_pullin(UINT8 streamId, UINT16 hopperId);
bool   func_l3bfdf_new_ws_send_out_comb_out_message_w_basket_full(UINT8 streamId, UINT16 hopperId);
bool   func_l3bfdf_new_ws_send_out_comb_out_message_w_double_full(UINT8 streamId, UINT16 hopperId);

//基础函数
double gaussian(double u, double n);

//External APIs
extern OPSTAT hcu_sps232_qr_printer_init(void);
extern void   hcu_sps232_send_char_to_ext_printer(char *s, int len);
extern OPSTAT dbi_HcuBfdf_callcell_save(HcuSysMsgIeL3bfdfCallcellElement_t *input);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFDF(...)				do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFDF_RECOVERY(...) 	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++; func_l3bfdf_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3BFDF_MSGSEND 	   	 	do{HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANALPHA].taskName);}while(0)

//通知刷新界面的复合宏定义
#define HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(bid, par) \
	do{\
		StrHlcIe_cui_hcu2uir_status_report_t status;\
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));\
		status.boardStatus = par;\
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(bid, &status);\
	}while(0)

//为了各个消息的发送，填入比较复杂的bitmap过程
#define HCU_L3BFDF_FILL_ALL_BOARD_BITMAP(status) \
	do{\
		total=0;\
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){\
			for (i = 1; i< HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){\
				boardId = (j<<3)+i;\
				if (gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus > HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MIN)\
				{\
					gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus = status;\
					gTaskL3bfdfContext.nodeDyn[j][i].cfgRcvFlag = FALSE;\
					total++;\
					snd.boardBitmap[boardId] = TRUE;\
				}\
				else snd.boardBitmap[boardId] = FALSE;\
			}\
		}\
	}while(0)

//为了各个消息的发送，填入比较复杂的bitmap过程
#define HCU_L3BFDF_FORCE_FILL_ALL_BOARD_BITMAP(status) \
	do{\
		total=0;\
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){\
			for (i = 1; i< HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){\
				boardId = (j<<3)+i;\
				gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus = status;\
				gTaskL3bfdfContext.nodeDyn[j][i].cfgRcvFlag = FALSE;\
				total++;\
				snd.boardBitmap[boardId] = TRUE;\
			}\
		}\
	}while(0)

//FIll bitmap for for calibration request message send
#define HCU_L3BFDF_FILL_WGT_BOARD_BITMAP() \
	do{\
		int j=0, boardId=0;\
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){\
			boardId = (j<<3)+1;\
			snd.boardBitmap[boardId] = TRUE;\
		}\
	}while(0)


//填入各个bitmap后，为了打印的方便，制作宏
#define HCU_L3BFDF_PRINT_ALL_BOARD_BITMAP() \
	do{\
		memset(s, 0, sizeof(s));\
		sprintf(s, "L3BFDF: Sending command out, Total action sensor number = %d, bitmap = ", total);\
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){\
			for (i=0; i<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){\
				boardId = (j<<3)+i;\
				memset(tmp, 0, sizeof(tmp));\
				sprintf(tmp, "%d/", snd.boardBitmap[boardId]);\
				if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);\
			}\
		}\
		strcat(s, "\n");\
		HCU_DEBUG_PRINT_CRT(s);\
	}while(0)

//反馈差错控制消息
#define HCU_L3BFDF_FEEDBACK_CTRL_RESP_MESSAGE(status) \
	do{\
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;\
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));\
		snd.validFlag = FALSE;\
		snd.errCode = HCU_SYSMSG_BFDF_ERR_CODE_INVALIID;\
		snd.cmdid = status;\
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);\
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);\
	}while(0)

//接收消息关键内容检查
#define HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK() \
	do{\
		line = rcv.snrId>>3;\
		if ((line <0) || (line >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX))\
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Received line id [=%d] out of range!\n", line);\
		boardId = rcv.snrId & 0x07;\
		if ((boardId <= 0) || (boardId >= HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX))\
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Received board id [=%d] out of range!\n", boardId);\
	}while(0)



#endif /* L3APP_BFDF_H_ */
