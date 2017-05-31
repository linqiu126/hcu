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
	HcuUsbCanHandleTypeDef_t can1;  //Added by MYC 2017/05/15
}gTaskCanitfleoContext_t;

//API
extern OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//extern OPSTAT fsm_canitfleo_l3bfsc_ws_read_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//extern OPSTAT fsm_canitfleo_l3bfsc_general_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_canitfleo_l3bfsc_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_sys_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_sys_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//extern OPSTAT fsm_canitfleo_can_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_usbcan_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_canitfleo_int_init(void);
OPSTAT func_canitfleo_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe);
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval);
OPSTAT func_canitfleo_working_scan_process(void);
OPSTAT func_canitfleo_bfsc_simulation_data_process(void);

//Socket CAN APIs
OPSTAT func_canitfleo_socketcan_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char);
OPSTAT func_canitfleo_socketcan_send(int socket, char *canid_canframe);
OPSTAT func_canitfleo_socketcan_init(char *canitfname, int *sock);
int    func_canitfleo_socketcan_test_main(int argc, char **argv);

//USBCAN BSP function mapping
OPSTAT hcu_canitfleo_usbcan_interface_init(void);
OPSTAT hcu_canitfleo_usbcan_l2frame_send(UINT8 *buffer, UINT32 length, UINT32 wmc_id_bitmap);

//USB CAN test functions
void func_canitfleo_usbcan_TestSetConfigReq(uint8_t wmc_id);
void func_canitfleo_usbcan_TestCommandReq(void);
void func_canitfleo_usbcan_TestStartReq(uint8_t wmc_id);

//MYC CAN message processing procedure
void     func_canitfleo_BigSmallEndianMapping(uint8_t *In, uint8_t *Out, uint32_t len);
uint16_t func_canitfleo_HuitpMsgIdMapToInternalMsgId(uint16_t huitp_msgid);
uint16_t func_canitfleo_InternalMsgIdMapToHuitpMsgId(uint16_t internal_msgid);
OPSTAT   func_canitfleo_usbcan_l2frame_receive_process(uint8_t *p_l2_frame, uint32_t l2_frame_len);
void     func_canitfleo_message_process_bfsc_wmc_start_ind(uint8_t *ptr);
void     func_canitfleo_message_process_bfsc_wmc_set_config_resp(uint8_t *ptr);
void     func_canitfleo_message_process_bfsc_wmc_new_ws_event(uint8_t *ptr);

// WILL BE REPLACED BY HUITP, ADDED 2017/05/15 */
//=================================================================================================================//
// THIS iS FOR TEST, TO BE REMOVE BY ZHANG JIanlin when all internal messages defined, this is only for TEST ==== START ============//
// ================================================================================================================//
///*
//** =============================================================================
//** ============================ MYC START FOR BFSC =============================
//** =============================================================================
//*/
////MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
////MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
////MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,      //       = 0x3B91,
////MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
////MSG_ID_L3BFSC_WMC_START_RESP,           //       = 0x3B92,
////MSG_ID_L3BFSC_WMC_STOP_REQ,             //       = 0x3B13,
////MSG_ID_L3BFSC_WMC_STOP_RESP,            //       = 0x3B93,
////MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
////MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,	  //	   = 0x3B95,
////MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
////MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP,     //       = 0x3B95,
////MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
////MSG_ID_L3BFSC_WMC_COMMAND_RESP,         //       = 0x3B97,
////MSG_ID_L3BFSC_WMC_FAULT_IND,            //       = 0x3B98,
////MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,      //       = 0x3B19,
////MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP,     //       = 0x3B99,
//
//// =====================================================================
//// ========== FOLLOWING SHOULD BE SAME AS IN WMC, COPY START ===========
//// =====================================================================
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
typedef struct msg_struct_l3bfsc_wmc_resp
{
	UINT16	msgid;
	UINT16  length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WmcErrorCode_t result;
}msg_struct_l3bfsc_wmc_resp_t;
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
////==========================================================
///*
//**	MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
//*/
typedef struct msg_struct_l3bfsc_wmc_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcInventory_t wmc_inventory;
}msg_struct_l3bfsc_wmc_startup_ind_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
//*/
//typedef struct CombinationAlgorithmParamaters
//{
//	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
//	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
//	UINT32	MinScaleNumberStartCombination;		//开始查找的最小个数，就是说大于这个个数就开始搜索
//	UINT32	TargetCombinationWeight;				  //组合目标重量
//	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
//	UINT32	IsPriorityScaleEnabled;					  // 1: Enable, 0: Disable
//	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
//	UINT32	CombinationBias;						      //每个Scale要求放几个物品
//	UINT32	IsRemainDetectionEnable;				  //Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
//	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
//	UINT32	RemainScaleTreatment;					    // 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
//	UINT32	CombinationSpeedMode;					    // 0：SpeedPriority，1: PrecisePriority
//	UINT32	CombinationAutoMode;					    // 0: Auto, 1: Manual
//	UINT32	MovingAvrageSpeedCount;					  //计算平均速度的时候使用最近多少个组合做统计
//	UINT32	spare1;
//	UINT32	spare2;
//	UINT32	spare3;
//	UINT32	spare4;
//}CombinationAlgorithmParamaters_t;
//
//typedef struct WeightSensorParamaters
//{
//	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
//	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
//	UINT32	WeightSensorEmptyThread;
//	UINT32	WeightSensorEmptyDetectionTimeMs;
//	UINT32	WeightSensorPickupThread;						// NOT for GUI
//	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
//	UINT32	StardardReadyTimeMs;								//???
//	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
//	//UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
//
//	UINT32	WeightSensorInitOrNot;							// NOT for GUI
//	UINT32	WeightSensorAdcSampleFreq;
//	UINT32	WeightSensorAdcGain;
//	UINT32	WeightSensorAdcBitwidth;						// NOT for GUI
//	UINT32  WeightSensorAdcValue;								// NOT for GUI
//	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
//	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
//	UINT32	WeightSensorCalibrationFullWeight;
//	UINT32	WeightSensorStaticZeroValue;
//	UINT32	WeightSensorTailorValue;
//	UINT32	WeightSensorDynamicZeroThreadValue;
//	UINT32	WeightSensorDynamicZeroHysteresisMs;
//	//UINT32  WeightSensorFilterCoeff[32];				// NOT for GUI
//	//UINT32  WeightSensorOutputValue[32];				// NOT for GUI
//}WeightSensorParamaters_t;
//
//typedef struct MotorControlParamaters
//{
//	UINT32	MotorSpeed;
//	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
//	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
//	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
//	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
//	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
//	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
//	UINT32	spare1;
//	UINT32	spare2;
//	UINT32	spare3;
//	UINT32	spare4;
//}MotorControlParamaters_t;
//
typedef struct msg_struct_l3bfsc_wmc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}msg_struct_l3bfsc_wmc_set_config_req_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
//*/
typedef struct msg_struct_l3bfsc_wmc_start_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_start_req_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_STOP_REQ,             //       = 0x3B13,
//*/
typedef struct msg_struct_l3bfsc_wmc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_stop_req_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
//**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,	  	//	  	 = 0x3B95,
//**
//**	NOTE: These two are used for report Weight event, used same struct
//**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT -> EMPTY -> LOAD
//**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT -> LOAD -> LOAD. with repeat time, used for AWS to
//**	**
//**
//*/
//#define 	WEIGHT_EVENT_ID_LOAD						(0)
//#define 	WEIGHT_EVENT_ID_EMPTY						(1)
//#define 	WEIGHT_EVENT_ID_PICKUP						(2)
//
//typedef struct WeightIndication
//{
//	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
//	UINT32 average_weight;		//average value in the detect window  // <--- MUST
//	//UINT32 repeat_times;		// always = 0 for MSG_ID_L3BFSC_WMC_NEW_WS_EVENT, = n for MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT
//}WeightIndication_t;
//
typedef struct msg_struct_l3bfsc_wmc_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;             /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
}msg_struct_l3bfsc_wmc_ws_event_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
//*/
////#define 	LED_COMMNAD_ID_IGNORE					(0)//MUSR BE 0
////#define 	LED_COMMNAD_ID_ON						(1)
////#define 	LED_COMMNAD_ID_OFF						(2)
////#define 	LED_COMMNAD_ID_BINKING_HIGHSPEED		(3)
////#define 	LED_COMMNAD_ID_BINKING_LOWSPEED			(4)
//
////#define 	MOTOR_COMMAND_ID_IGORE					(0) //MUSR BE 0
////#define 	MOTOR_COMMAND_ID_START					(1)
////#define 	MOTOR_COMMAND_ID_STOP					(2)
////#define 	MOTOR_COMMAND_ID_ROLLONCE				(3)
////#define 	MOTOR_COMMAND_ID_SPEED_READ				(4)
//
//// CombineType Definition
//// COMNINETPYE_ROOLOUT_START
//// COMNINETPYE_ROOLOUT_COMPLETE
//// COMNINETPYE_DROP_START
//// COMNINETPYE_DROP_COMPLETE
//// COMNINETPYE_WARNING_START
//// COMNINETPYE_WARNING_COMPLETE
//// COMNINETPYE_ERROR_START
//// COMNINETPYE_ERROR_COMPLETE
//
//typedef struct CombineType
//{
//	UINT32	WeightCombineType;
//	UINT32	ActionDelayMs;
//}CombineType_t;
//
//typedef struct msg_struct_l3bfsc_wmc_comb_out_req
//{
//	UINT16 msgid;
//	UINT16 length;
//	CombineType_t weight_combin_type;
//}msg_struct_l3bfsc_wmc_comb_out_req_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_FAULT_IND
//*/
typedef struct msg_struct_l3bfsc_wmc_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}msg_struct_l3bfsc_wmc_fault_ind_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
//*/
//#define SENSOR_COMMAND_ID_WEITGH_READ (0x0001)
//#define MOTOR_COMMAND_ID (0x0002)
//#define LED1_COMMAND_ID (0x0004)
//#define LED2_COMMAND_ID (0x0008)
//#define LED3_COMMAND_ID (0x0010)
//#define LED4_COMMAND_ID (0x0020)
//
//#define 	LED_COMMNAD_ON						(1)
//#define 	LED_COMMNAD_OFF						(2)
//#define 	LED_COMMNAD_BINKING_HIGHSPEED		(3)
//#define 	LED_COMMNAD_BINKING_LOWSPEED		(4)
//
//#define 	SESOR_COMMAND_ID_IGORE						(0) //MUSR BE 0
//#define 	SESOR_COMMAND_ID_WEITGH_READ				(3)
//#define 	SESOR_COMMAND_ID_CALIBRATION_ZERO			(4)
//#define 	SESOR_COMMAND_ID_CALIBRATION_FULL			(5)
//
//typedef struct msg_struct_l3bfsc_wmc_command_req
//{
//	UINT32 msgid;
//	UINT16 length;
//	UINT16 spare;
//	UINT32 wmc_state;
//	UINT8 led1_command;
//	UINT8 led2_command;
//	UINT8 led3_command;
//	UINT8 led4_command;
//	UINT32 motor_command;
//	UINT32 sensor_command;
//}msg_struct_l3bfsc_wmc_command_req_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_COMMAND_RESP,         //       = 0x3B97,
//*/
typedef struct msg_struct_l3bfsc_wmc_command_resp
{
	UINT16	msgid;
	UINT16	result;
	UINT16	error_code;
	UINT32 	motor_speed;
	UINT32 	sensor_weight;
}msg_struct_l3bfsc_wmc_command_resp_t;
//
///*
//**	MSG_ID_L3BFSC_WMC_MSH_HEADER
//*/
//typedef struct msg_struct_l3bfsc_wmc_msg_header
//{
//	UINT16 msgid;
//	UINT16 length;
//}msg_struct_l3bfsc_wmc_msg_header_t;
//
////MSG_ID_CAN_L3BFSC_WMC_ERR_INQ_CMD_REQ,      //       = 0x3B19,
////TBD
//
////MSG_ID_CAN_L3BFSC_WMC_ERR_INQ_CMD_RESP,     //       = 0x3B99,
////TBD
//
///* Message Length definition */
#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND				(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_REQ				(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ				(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT			(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))
#define 	MSG_SIZE_L3BFSC_WMC_REPEAT_WS_EVENT			(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))
#define 	MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_REQ			(sizeof(msg_struct_l3bfsc_wmc_comb_out_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_RESP		(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND				(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ				(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP			(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_REQ			(sizeof(msg_struct_l3bfsc_wmc_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_RESP		(sizeof(msg_struct_l3bfsc_wmc_resp_t))
//
///* Can ID for communication between AWS and WMC */
///* 1: AWS to WMC: 1 to n, n = 0 ... 15       */
///* Node ID bitmap: CAN ID = 0x0010 XXXX    */
///* CAN ID          direction => ^  ~~~~ <= 1111 1111 1111 1111, */
//#define AWS_TO_WMC_CAN_ID_PREFIX		(0x00100000U)
////#define AWS_TO_WMC_CAN_ID_SUFFIX		(mwc_id_bitmap)
//
///* 2: AWS to WMC-S: 1 to 1  */
///* CAN ID: 0x0010 0000 */
//#define AWS_TO_WMCS_CAN_ID				(0x00110000U)
//
///* 3: WMC to AWS: */
///* CAN ID: 0x0030 0000 (WMC Node ID 0) to 0x0030 000F (WMC Node ID 15) */
//#define WMC_TO_AWS_CAN_ID_PREFIX		(0x00300000U)
////#define WMC_TO_AWS_CAN_ID_SUFFIX		(mwc_id)
//
///* 4: WMC-S to AWS: */
///* CAN ID: 0x0030 0010 (Node ID 0) */
//#define WMCS_TO_AWS_CAN_ID				(0x00300010U)
//
///* CAN Msg Length */
//#define		MAX_WMC_CONTROL_MSG_LEN				(256U)
//#define		MAX_WMC_CONTROL_MSG_HEADER_LEN		(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
//#define		MAX_WMC_CONTROL_MSG_BODY_LEN		(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)
//
///* ERROR CODE */
//typedef enum IHU_ERROR_CODE
//{
//	//ERROR CODE ID
//	ERROR_CODE_NO_ERROR = 0, //Starting point
//
//	//COMMON ERROR CODE
//	ERROR_CODE_CALLING_ERROR,
//	ERROR_CODE_INPUT_PARAMETER_KO,
//	ERROR_CODE_WRONG_WMC_STATE,
//	ERROR_CODE_UNKNOWN,
//
//	/* TO BE ADDED FOR EACH OF THE MESSAGE */
//	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,
//
//	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,
//
//	//**	MSG_ID_L3BFSC_WMC_START_REQ,
//
//	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,
//
//	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,
//
//	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,
//
//	ERROR_CODE_MAX, //Ending point
//
//}error_code_t; //end of IHU_INTER_TASK_MSG_ID
//
//// =====================================================================
//// ============== ABOVE SHOULD BE SAME AS IN WMC, COPY END =============
//// =====================================================================
//
///*
//** =============================================================================
//** ============================= MYC END FOR BFSC ==============================
//** =============================================================================
//*/

//=================================================================================================================//
// THIS iS FOR TEST, TO BE REMOVE BY ZHANG JIanlin when all internal messages defined, this is only for TEST ===== END ============//
// ================================================================================================================//

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CANITFLEO(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L2FRAME_CANITFLEO_H_ */
