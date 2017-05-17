/*
 * canitfleo.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_CANITFLEO_H_
#define L2FRAME_CANITFLEO_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "l2usbcan.h"   		//Added by MYC 2017/05/15
#include "../l2codec/huitp.h"	//Added by MYC 2017/05/15

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_CANITFLEO
{
	FSM_STATE_CANITFLEO_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CANITFLEO_ACTIVED,
	FSM_STATE_CANITFLEO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmCanitfleo[];

/* ************ CAN ************** */
#define MAXSOCK 16    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */
#define SILENT_INI 42 /* detect user setting on commandline */
#define SILENT_OFF 0  /* no silent mode */
#define SILENT_ANI 1  /* silent mode with animation */
#define SILENT_ON  2  /* silent mode (completely silent) */
#define MAXANI 4
#define MAX_CANFRAME      "12345678#01.23.45.67.89.AB.CD.EF"
/* ************ CAN ************** */

typedef struct gTaskCanitfleoContext
{
	UINT32 sensorIdRoundBing;
	UINT32 can_socket_id;
	//Added by MYC 2017/05/15
	USB_CAN_HandleTypeDef can1;
}gTaskCanitfleoContext_t;

//API
extern OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_read_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_general_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//Local API
OPSTAT func_canitfleo_int_init(void);
OPSTAT func_canitfleo_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe);
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval);
OPSTAT func_canitfleo_can_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char);
OPSTAT func_canitfleo_can_send(int socket, char *canid_canframe);
OPSTAT func_canitfleo_can_init(char *canitfname, int *sock);
OPSTAT func_canitfleo_working_scan_process(void);
OPSTAT func_canitfleo_bfsc_simulation_data_process(void);

// WILL BE REPLACED BY HUITP, ADDED 2017/05/15 */

/*
** ====================================================================
** ============================ MYC START =============================
** ====================================================================
*/

//WMC <-> AWS //MYC
#define MSG_ID_L3BFSC_WMC_STARTUP_IND			(36)
#define MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ		(37)
#define MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP		(38)
#define MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ		(39)
#define MSG_ID_L3BFSC_WMC_GET_CONFIG_RESP		(40)
#define MSG_ID_L3BFSC_WMC_START_REQ				(41)
#define MSG_ID_L3BFSC_WMC_START_RESP			(42)
#define MSG_ID_L3BFSC_WMC_STOP_REQ				(43)
#define MSG_ID_L3BFSC_WMC_STOP_RESP				(44)
#define MSG_ID_L3BFSC_WMC_WEIGHT_IND			(45)
#define MSG_ID_L3BFSC_WMC_COMBIN_REQ			(46)
#define MSG_ID_L3BFSC_WMC_COMBIN_RESP			(47)
#define MSG_ID_L3BFSC_WMC_FAULT_IND				(48)
#define MSG_ID_L3BFSC_WMC_COMMAND_REQ			(49)
#define MSG_ID_L3BFSC_WMC_COMMAND_RESP			(50)

//	//HCU-IHU SUI新增内容
//上电过程
#define	HUITP_MSGID_sui_bfsc_startup_ind                 (0x3B90)
//配置过程
#define	HUITP_MSGID_sui_bfsc_set_config_req              (0x3B11)
#define	HUITP_MSGID_sui_bfsc_set_config_resp             (0x3B91)
//启动过程
#define	HUITP_MSGID_sui_bfsc_start_req                   (0x3B12)
#define	HUITP_MSGID_sui_bfsc_start_resp                  (0x3B92)
#define	HUITP_MSGID_sui_bfsc_stop_req                    (0x3B13)
#define	HUITP_MSGID_sui_bfsc_stop_resp                   (0x3B93)
//重量汇报过程
#define	HUITP_MSGID_sui_bfsc_new_ws_event                (0x3B94)
#define HUITP_MSGID_sui_bfsc_repeat_ws_event             (0x3B94)  //TO BE REMOVE
//组合出料过程
#define	HUITP_MSGID_sui_bfsc_ws_comb_out_req             (0x3B15)
#define	HUITP_MSGID_sui_bfsc_ws_comb_out_resp            (0x3B95)

#define HUITP_MSGID_sui_bfsc_command_req                 (0x3B17)
#define HUITP_MSGID_sui_bfsc_command_resp                (0x3B97)
//差错过程
#define HUITP_MSGID_sui_bfsc_fault_ind                   (0x3B98)
#define HUITP_MSGID_sui_bfsc_err_inq_cmd_req             (0x3B19)
#define HUITP_MSGID_sui_bfsc_err_inq_cmd_resp            (0x3B99)

///*
//**	HUITP_MSGID_sui_bfsc_startup_ind
//*/
//typedef struct WmcErrorCode
//{
//	UINT16 error_code;
//	UINT16 spare1;
//}WmcErrorCode_t;
//
//typedef struct WmcId
//{
//	UINT8 wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//	UINT8 spare1;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//	UINT8 spare2;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//	UINT8 spare3;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//}WmcId_t;
//
//typedef struct msg_struct_l3bfsc_wmc_resp
//{
//	UINT16	msgid;
//	UINT16  length;
//	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//}msg_struct_l3bfsc_wmc_resp_t;
//
//
//typedef struct WmcInventory
//{
//	UINT32 hw_inventory_id;
//	UINT32 sw_inventory_id;
//	UINT32 stm32_cpu_id;
//	UINT32 weight_sensor_type;
//	UINT32 motor_type;
//	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
//	UINT32 spare1;
//	UINT32 spare2;
//}WmcInventory_t;
//
//
//typedef struct msg_struct_l3bfsc_wmc_startup_ind
//{
//	UINT16 msgid;
//	UINT16 length;
//	WmcInventory_t wmc_inventory;
//}msg_struct_l3bfsc_wmc_startup_ind_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ
//*/
//typedef struct CombinationAlgorithmParamaters
//{
//	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
//	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
//	UINT32	MinScaleNumberStartCombination;			//开始查找的最小个数，就是说大于这个个数就开始搜索
//	UINT32	TargetCombinationWeight;				//组合目标重量
//	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
//	UINT32	IsPriorityScaleEnabled;					// 1: Enable, 0: Disable
//	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
//	UINT32	CombinationBias;						//每个Scale要求放几个物品
//	UINT32	IsRemainDetectionEnable;				//Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
//	UINT32	RemainDetectionTimeSec;					// RemainDetionTime in Seconds
//	UINT32	RemainScaleTreatment;					// 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
//	UINT32	CombinationSpeedMode;					// 0：SpeedPriority，1: PrecisePriority
//	UINT32	CombinationAutoMode;					// 0: Auto, 1: Manual
//	UINT32	MovingAvrageSpeedCount;					//计算平均速度的时候使用最近多少个组合做统计
//	UINT32	spare1;
//	UINT32	spare2;
//	UINT32	spare3;
//	UINT32	spare4;
//}CombinationAlgorithmParamaters_t;
//
//typedef struct WeightSensorParamaters
//{
//	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
//	UINT32	WeightSensorLoadThread;					//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
//	UINT32	WeightSensorEmptyThread;
//	UINT32	WeightSensorEmptyDetectionTimeMs;
//	UINT32	WeightSensorPickupThread;				// NOT for GUI
//	UINT32	WeightSensorPickupDetectionTimeMs;		// NOT for GUI
//	UINT32	StardardReadyTimeMs;					//???
//	UINT32	MaxAllowedWeight;						//如果发现超过这个最大值，说明Sensor出错
//
//	UINT32	WeightSensorInitOrNot;					// NOT for GUI
//	UINT32	WeightSensorAdcSampleFreq;
//	UINT32	WeightSensorAdcGain;
//	UINT32	WeightSensorAdcBitwidth;				// NOT for GUI
//	UINT32  WeightSensorAdcValue;					// NOT for GUI
//	UINT32	WeightSensorCalibrationZeroAdcValue;	// NOT for GUI
//	UINT32	WeightSensorCalibrationFullAdcValue;	// NOT for GUI
//	UINT32	WeightSensorCalibrationFullWeight;
//	double	WeightSensorCalibrationK;				// NOT for GUI
//	UINT32	WeightSensorCalibrationB;				// NOT for GUI
//	UINT32	WeightSensorStaticZeroValue;
//	UINT32	WeightSensorTailorValue;
//	UINT32	WeightSensorDynamicZeroThreadValue;
//	UINT32	WeightSensorDynamicZeroHysteresisMs;
//	UINT32  WeightSensorFilterCoeff[32];			// NOT for GUI
//	UINT32  WeightSensorOutputValue[32];			// NOT for GUI
//}WeightSensorParamaters_t;
//
//typedef struct MotorControlParamaters
//{
//	UINT32	MotorSpeed;
//	UINT32	MotorDirection;							//0: Clockwise; 1: Counter-Clockwise
//	UINT32	MotorRollingStartMs;					//how long do the motor rolling for start action
//	UINT32	MotorRollingStopMs;						//how long do the motor rolling for stop action
//	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
//	UINT32	MotorFailureDetectionVaration;			// % of the MotorSpeed
//	UINT32	MotorFailureDetectionTimeMs;			// within TimeMs, 如果速度都在外面，认为故障
//	UINT32	spare1;
//	UINT32	spare2;
//	UINT32	spare3;
//	UINT32	spare4;
//}MotorControlParamaters_t;

typedef struct msg_struct_l3bfsc_wmc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}msg_struct_l3bfsc_wmc_set_config_req_t;

/*
**	MSG_ID_L3BFSC_WMC_START_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_start_req
{
	UINT16 msgid;
	UINT16 length;

}msg_struct_l3bfsc_wmc_start_req_t;

/*
**	MSG_ID_L3BFSC_WMC_STOP_REQ
*/
typedef struct msg_struct_l3bfsc_wmc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_stop_req_t;

/*
**	MSG_ID_L3BFSC_WMC_WEIGHT_IND
*/
#define 	WEIGHT_EVENT_ID_LOAD						(0)
#define 	WEIGHT_EVENT_ID_EMPTY						(1)
#define 	WEIGHT_EVENT_ID_PICKUP						(2)

typedef struct WeightIndication
{
	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
	UINT32 average_weight;		//average value in the detect window  // <--- MUST
}WeightIndication_t;

typedef struct msg_struct_l3bfsc_wmc_weight_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;             /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
}msg_struct_l3bfsc_wmc_weight_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMBIN_OUT_REQ    //bfsc_ws_comb_out_req
*/
//#define 	LED_COMMNAD_ID_IGNORE					(0)//MUSR BE 0
//#define 	LED_COMMNAD_ID_ON						(1)
//#define 	LED_COMMNAD_ID_OFF						(2)
//#define 	LED_COMMNAD_ID_BINKING_HIGHSPEED		(3)
//#define 	LED_COMMNAD_ID_BINKING_LOWSPEED			(4)

//#define 	MOTOR_COMMAND_ID_IGORE					(0) //MUSR BE 0
//#define 	MOTOR_COMMAND_ID_START					(1)
//#define 	MOTOR_COMMAND_ID_STOP					(2)
//#define 	MOTOR_COMMAND_ID_ROLLONCE				(3)
//#define 	MOTOR_COMMAND_ID_SPEED_READ				(4)

// CombineType Defination
// COMNINETPYE_ROOLOUT_START
// COMNINETPYE_ROOLOUT_COMPLETE
// COMNINETPYE_DROP_START
// COMNINETPYE_DROP_COMPLETE
// COMNINETPYE_WARNING_START
// COMNINETPYE_WARNING_COMPLETE
// COMNINETPYE_ERROR_START
// COMNINETPYE_ERROR_COMPLETE

typedef struct CombineType
{
	UINT32	WeightCombineType;
	UINT32	ActionDelayMs;
}CombineType_t;

typedef struct msg_struct_l3bfsc_wmc_combin_out_req
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_combin_out_req_t;

/*
**	MSG_ID_L3BFSC_WMC_FAULT_IND
*/
typedef struct msg_struct_l3bfsc_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}msg_struct_l3bfsc_wmc_fault_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_REQ
*/
#define 	SESOR_COMMAND_ID_IGORE						(0) //MUSR BE 0
#define 	SESOR_COMMAND_ID_WEITGH_READ				(3)

typedef struct msg_struct_l3bfsc_wmc_command_req
{
	UINT32 msgid;
	UINT16 length;
	UINT16 spare;
	UINT32 wmc_state;
	UINT8 led1_command;
	UINT8 led2_command;
	UINT8 led3_command;
	UINT8 led4_command;
	UINT32 motor_command;
	UINT32 sensor_command;
}msg_struct_l3bfsc_wmc_command_req_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_RESP
*/
typedef struct msg_struct_l3bfsc_wmc_command_resp
{
	UINT16	msgid;
	UINT16	result;
	UINT16	error_code;
	UINT32 	motor_speed;
	UINT32 	sensor_weight;
}msg_struct_l3bfsc_wmc_command_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_MSH_HEADER
*/
typedef struct msg_struct_l3bfsc_wmc_msg_header
{
	UINT32 msgid;
	UINT16 length;
	UINT16 spare;
	UINT32 wmc_state;
}msg_struct_l3bfsc_wmc_msg_header_t;

/* Message Length definition */
#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND				(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_REQ				(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ				(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_WEIGHT_IND				(sizeof(msg_struct_l3bfsc_wmc_weight_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_REQ				(sizeof(msg_struct_l3bfsc_wmc_combin_out_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMBIN_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND				(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ				(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP			(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))

/* Can ID for communication between AWS and WMC */
#define		AWS_CAN_ID_PREFIX							(0x600U)
#define		WMC_CAN_ID_PREFIX							(0x400U)
#define		AWS_CAN_ID_SUFFIX							(0x001U)
#define		WMC_CAN_ID_SUFFIX							((zWmcInvenory.wmc_id&0x2F))
#define		AWS_CAN_ID									((AWS_CAN_ID_PREFIX)|(AWS_CAN_ID_SUFFIX))
#define		WMC_CAN_ID									((AWS_CAN_ID_PREFIX)|(AWS_CAN_ID_SUFFIX))

/* CAN Msg Lenth */
#define		MAX_WMC_CONTROL_MSG_LEN						(256U)
#define		MAX_WMC_CONTROL_MSG_HEADER_LEN				(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
#define		MAX_WMC_CONTROL_MSG_BODY_LEN				(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)

/* ERROR CODE */
typedef enum IHU_ERROR_CODE
{
	//ERROR CODE ID
	ERROR_CODE_NO_ERROR = 0, //Starting point

	//COMMON ERROR CODE
	ERROR_CODE_CALLING_ERROR,
	ERROR_CODE_INPUT_PARAMETER_KO,
	ERROR_CODE_WRONG_WMC_STATE,
	ERROR_CODE_UNKNOWN,

	/* TO BE ADDED FOR EACH OF THE MESSAGE */
	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_START_REQ,

	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,

	ERROR_CODE_MAX, //Ending point

}error_code_t; //end of IHU_INTER_TASK_MSG_ID

/*
** ====================================================================
** ============================= MYC END ==============================
** ====================================================================
*/

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CANITFLEO(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L2FRAME_CANITFLEO_H_ */
