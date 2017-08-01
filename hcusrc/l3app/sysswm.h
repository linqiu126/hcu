/*
 * sysswm.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SYSSWM_H_
#define L3APP_SYSSWM_H_

#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_SYSSWM
{
	FSM_STATE_SYSSWM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_SYSSWM_ACTIVED,
	FSM_STATE_SYSSWM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmSysswm[];

//软件系在的上下文
typedef struct gTaskSysswmContext
{
	HcuSysMsgIeL3SysSwmSwPkgElement_t  	cloudSwPkg;
	HcuSysMsgIeL3SysSwmSwDlElement_t	cloudSwDl;
	UINT8	swDlSession;
	UINT8	reTransTimes;
}gTaskSysswmContext_t;
#define HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT 	0
#define HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE 	1
#define HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL 	2
#define HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH 	3
#define HCU_SYSSWM_SW_DOWNLOAD_SESSION_MAX_NBR	 	4

#define HCU_SYSSWM_SW_PACKAGE_RETRANS_MAX_TIMES  3

//文件操控的数据结构表单
typedef struct strTaskSysswmSwpkgLable
{
	char fPathName[200];
	UINT16 fileNameLen;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
}strTaskSysswmSwpkgLable_t;

typedef struct strTaskSysswmSwpkgSegment
{
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	UINT16 segTotal;
	UINT16 segIndex;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8* buf;
}strTaskSysswmSwpkgSegment_t;

//API
extern OPSTAT fsm_sysswm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//CLOUDVELA
extern OPSTAT fsm_sysswm_cloudvela_inventory_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_sw_package_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//CANITFLEO
extern OPSTAT fsm_sysswm_canitfleo_inventory_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_canitfleo_sw_package_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);


//Local API
OPSTAT func_sysswm_int_init(void);
OPSTAT func_sysswm_time_out_period_working_scan_hcu_client(void);
OPSTAT func_sysswm_time_out_period_working_scan_ihu_stable(void);
OPSTAT func_sysswm_time_out_period_working_scan_ihu_trial(void);
OPSTAT func_sysswm_time_out_period_working_scan_ihu_patch(void);
OPSTAT func_sysswm_time_out_segment_download_wait_process(void);
OPSTAT func_sysswm_analysis_ihu_sw_package(UINT16 hwType, UINT16 hwId, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, strTaskSysswmSwpkgLable_t* input);
OPSTAT func_sysswm_delete_ihu_redundance_sw_package(UINT16 hwType, UINT8 upgradeFlag);
OPSTAT func_sysswm_read_ihu_sw_package_segment(strTaskSysswmSwpkgSegment_t *input);
UINT16 func_sysswm_caculate_file_whole_checksum(char *fname);
UINT32 func_sysswm_caculate_file_length_in_bytes(char *fname);
OPSTAT func_sysswm_send_cloudvela_sw_package_report(void);
void   func_sysswm_copy_exe_to_target_dir_and_restart(void);



//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_SYSSWM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L3APP_SYSSWM_H_ */
