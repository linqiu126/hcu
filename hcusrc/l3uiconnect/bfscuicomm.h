/*
 * bfscuicomm.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L3UI_BFSCUICOMM_H_
#define L3UI_BFSCUICOMM_H_

#include "../l1com/l1comdef.h"
#include "../l2codec/huitp.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BFSCUICOMM
{
	FSM_STATE_BFSCUICOMM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BFSCUICOMM_ACTIVED,
	FSM_STATE_BFSCUICOMM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

typedef struct gTaskL3bfscuicommContext{
	UINT8	bfscuiState; // 1 - UI START; 2 - UI STOP
	//本地全局变量，分别用于标识开启命令，校准命令，和配置命令的变化，系统启动初始化为0, UI界面修改后自动累加，HCU对比Flag变化判断用户修改了那个Json文件
	UINT32  cmdStartStopFlag;
	UINT32  cmdCalibrationFlag;
	UINT32  cmdResumeFlag;
	UINT32  cmdTestFlag;
}gTaskL3bfscuicommContext_t;


typedef struct L3BfscuiStartStopCmd
{
	UINT32  cmdFlag;
	UINT32 	cmdValue;
	UINT32	confindex;
}L3BfscuiStartStopCmd_t;

typedef struct L3BfscuiCalibrationCmd
{
	UINT32  cmdFlag;
	UINT32 	cmdValue;
	UINT8	sensorid;
	UINT32	weight;
}L3BfscuiCalibrationCmd_t;

typedef struct L3BfscuiResumeCmd
{
	UINT32  cmdFlag;
	UINT32 	cmdValue;
}L3BfscuiResumeCmd_t;

typedef struct L3BfscuiTestCmd
{
	UINT32  cmdFlag;
	UINT32 	cmdValue;
	UINT8	sensorid;
	UINT32	testCmd;
	UINT32	testPara;
} L3BfscuiTestCmd_t;

typedef struct L3BfscuiJsonCmdParseResult
{
	L3BfscuiStartStopCmd_t  cmdStartStop;
	L3BfscuiCalibrationCmd_t  cmdCalibration;
	L3BfscuiResumeCmd_t  cmdResume;
	L3BfscuiTestCmd_t  cmdTest;
} L3BfscuiJsonCmdParseResult_t;

//本地常量定义
#define HCU_BFSCCOMM_JASON_CMD_START 		(1)
#define HCU_BFSCCOMM_JASON_CMD_STOP 		(2)
#define HCU_BFSCCOMM_JASON_CMD_CONFIG 		(3)
#define HCU_BFSCCOMM_JASON_CMD_TEST 		(4)
#define HCU_BFSCCOMM_JASON_CMD_CALZERO		(5)
#define HCU_BFSCCOMM_JASON_CMD_CALFULL 		(6)
#define HCU_BFSCCOMM_JASON_CMD_RESUME 		(7)
#define HCU_BFSCCOMM_JASON_CMD_SUSPEND 		(8)

//Global variables
extern HcuFsmStateItem_t HcuFsmBfscuicomm[];

//API
extern OPSTAT fsm_bfscuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_l3bfsc_cfg_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_can_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_scan_jason_callback(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT  func_bfscuicomm_read_cfg_file_into_ctrl_table(UINT32 config_index);
OPSTAT  func_bfscuicomm_time_out_period_read_process(void);
OPSTAT  func_bfscuicomm_cmdfile_json_parse(char *monitorJsonFile, L3BfscuiJsonCmdParseResult_t *parseResult );



//External APIs
extern OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_BFSCUICOMM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_BFSCUICOMM_H_ */
