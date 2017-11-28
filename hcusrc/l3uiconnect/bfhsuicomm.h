/*
 * bfhsuicomm.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L3UI_BFHSUICOMM_H_
#define L3UI_BFHSUICOMM_H_

#include "../l1com/l1comdef.h"
#include "../l2codec/huitp.h"
#include "../l2codec/huicobus.h"
#include "../l2codec/huicobuscodec.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BFHSUICOMM
{
	FSM_STATE_BFHSUICOMM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BFHSUICOMM_ACTIVED,
	FSM_STATE_BFHSUICOMM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

typedef struct gTaskL3bfhsuicommContext{
	UINT8	bfhsuiState; // 1 - UI START; 2 - UI STOP
	//本地全局变量，分别用于标识开启命令，校准命令，和配置命令的变化，系统启动初始化为0, UI界面修改后自动累加，HCU对比Flag变化判断用户修改了那个Json文件
	UINT32  cmdStartStopFlag;
	UINT32  cmdCalibrationFlag;
	UINT32  cmdResumeFlag;
	UINT32  cmdTestFlag;
}gTaskL3bfhsuicommContext_t;


typedef struct L3BfhsuiStartStopCmd
{
	UINT32  cmdFlag;
	UINT32  cmdValue;
	UINT16  confindex;
}L3BfhsuiStartStopCmd_t;

typedef struct L3BfhsuiCalibrationCmd
{
	UINT32  cmdFlag;
	UINT32  cmdValue;
	UINT8	 sensorid;
	UINT32	weight;
}L3BfhsuiCalibrationCmd_t;

typedef struct L3BfhsuiResumeCmd
{
	UINT32  cmdFlag;
	UINT32 	cmdValue;
}L3BfhsuiResumeCmd_t;

typedef struct L3BfhsuiTestCmd
{
	UINT32  cmdFlag;
	UINT32 cmdValue;
	UINT8	sensorid;
	UINT32	testCmd;
	UINT32	testPara;
} L3BfhsuiTestCmd_t;

typedef struct L3BfhsuiJsonCmdParseResult
{
	L3BfhsuiStartStopCmd_t  cmdStartStop;
	L3BfhsuiCalibrationCmd_t  cmdCalibration;
	L3BfhsuiResumeCmd_t  cmdResume;
	L3BfhsuiTestCmd_t  cmdTest;
} L3BfhsuiJsonCmdParseResult_t;

//本地常量定义
#define HCU_BFHSCOMM_JASON_CMD_START 		(1)
#define HCU_BFHSCOMM_JASON_CMD_STOP 		(2)
#define HCU_BFHSCOMM_JASON_CMD_CONFIG 		(3)
#define HCU_BFHSCOMM_JASON_CMD_TEST 		(4)
#define HCU_BFHSCOMM_JASON_CMD_CALZERO		(5)
#define HCU_BFHSCOMM_JASON_CMD_CALFULL 		(6)
#define HCU_BFHSCOMM_JASON_CMD_RESUME 		(7)
#define HCU_BFHSCOMM_JASON_CMD_SUSPEND 		(8)

//Global variables
extern HcuFsmStateItem_t HcuFsmBfhsuicomm[];

//API
extern OPSTAT fsm_bfhsuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_l3bfhs_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_cali_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_cali_full_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_study_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_study_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_one_key_clean_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT  func_bfhsuicomm_read_cfg_file_into_ctrl_table(UINT16 config_index);
OPSTAT  func_bfhsuicomm_time_out_period_read_process(void);


//External APIs
extern OPSTAT dbi_HcuBfhs_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_BFHSUICOMM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_BFHSUICOMM_H_ */
