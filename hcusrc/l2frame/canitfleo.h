/*
 * canitfleo.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_CANITFLEO_H_
#define L2FRAME_CANITFLEO_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CANITFLEO
{
	FSM_STATE_CANITFLEO_INITED = 0x02,
	FSM_STATE_CANITFLEO_ACTIVED,
	FSM_STATE_CANITFLEO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmCanitfleo[];

typedef struct strHcuCanitfleoCmdFrame
{
	UINT8 bfscCmdPrefixH;
	UINT8 bfscCmdPrefixL;
	UINT8 bfscCmdId;
	UINT8 bfscOptId;
	UINT8 bfscOptPar;
	UINT8 bfscPar1;
	UINT8 bfscPar2;
	UINT8 bfscPar3;
}strHcuCanitfleoCmdFrame_t;
#define HCU_CANITFLEO_CMD_PREFIXH 0x23
#define HCU_CANITFLEO_CMD_PREFIXL 0x00
#define HCU_CANITFLEO_CMD_BFSC_ID 0x65
//不同的控制命令
typedef enum
{
	HCU_CANITFLEO_OPTID_none = 0,
	HCU_CANITFLEO_OPTID_min = 0x01,
	HCU_CANITFLEO_OPTID_wegith_read = 0x02,
	HCU_CANITFLEO_OPTID_auto_zero_track = 0x09,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_min_sensitivity = 0x0A,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_manual_set_zero = 0x0C,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_static_detect_range = 0x0D, //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_static_detect_duration = 0x0E, //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_weight_scale_calibration = 0x10,
	HCU_CANITFLEO_OPTID_motor_turn_around = 0x20,
	HCU_CANITFLEO_OPTID_motor_speed = 0x21,
	HCU_CANITFLEO_OPTID_scale_range = 0x25,
	HCU_CANITFLEO_OPTID_max,
	HCU_CANITFLEO_OPT_invalid = 0xFF,
}HcuCanitfleoOptidEnmu;
typedef enum
{
	HCU_CANITFLEO_OPTPAR_none = 0,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_normal = 0x01,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_reverse = 0x02,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_stop = 0x03,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_0 = 0x01,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_1kg = 0x02,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_exit = 0x03,
	HCU_CANITFLEO_OPTPAR_invalid = 0xFF,
}HcuCanitfleoOptParEnmu;
//如果是32为量值数据，则采用little_endian_windows的风格，低位在前

/*
叶工，测试指令和之前你的测试平台使用一致，没有改变：

重量读取：23 00 65 02 00 00 00 00
自动0点跟踪：23 00 65 09 xx xx xx xx （值设定及含义同modbus协议）
最小灵敏度：23 00 65 0a xx xx xx xx （值设定及含义同modbus协议）
手动清零：23 00 65 0c xx xx xx xx (值设定及含义同modbus协议)
静止检测范围：23 00 65 0d xx xx xx xx （值设定及含义同modbus协议）
静止检测时间：23 00 65 0e xx xx xx xx （值设定及含义同modbus协议）
称量0校准：23 00 65 10 01 00 00 00
称量1kg校准：23 00 65 10 02 00 00 00
称量退出校准：23 00 65 10 00 00 00 00
退出校正的指令要改成：23 00 65 10 03 00 00 00
电机正转：23 00 65 20 01 00 00 00
电机反转：23 00 65 20 02 00 00 00
电机停止：23 00 65 20 03 00 00 00
电机速度：23 00 65 21 xx xx xx xx
量程：23 00 65 25 xx xx xx xx

校准顺序是，首先空称台，然后点零载，
放砝码，然后点满载。然后点退出校准保存。
32位的数据低位在前，例如，23 00 65 0d 01 00 00 00
 */


//API
extern OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_canitfleo_int_init(void);
void func_canitfleo_working_scan_process(void);
OPSTAT func_canitfleo_frame_encode(UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe);
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 optid, UINT8 optpar, UINT32 modbusval);

#endif /* L2FRAME_CANITFLEO_H_ */
