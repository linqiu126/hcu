/**
 ****************************************************************************************
 *
 * @file huicobuscodec.h
 *
 * @brief HUICOBUSCODEC
 *
 * BXXH team
 * Created by ZJL, 20171125
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_HUICOBUSCODEC_H_
#define L2FRAME_HUICOBUSCODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <json-c/json.h>
#include <json-c/json_object.h>
#include "../l2codec/huicobus.h"
#include "../l1com/l1comdef.h"
#include "../l0service/mqtt.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_HUICOBUSCODEC
{
	FSM_STATE_HUICOBUSCODEC_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_HUICOBUSCODEC_ACTIVED,
	FSM_STATE_HUICOBUSCODEC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmHuicobuscodec[];

//Global APIs
extern OPSTAT fsm_huicobuscodec_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huicobuscodec_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huicobuscodec_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huicobuscodec_mqtt_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//发送API函数
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_init_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_start_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_stop_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_resume_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_suspend_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_finish(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_test_cmd_resp_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_version_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_version_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_status_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_alarm_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_alarm_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_error_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_error_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_restart_report(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_logout_report(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfsc_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfsc_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_one_key_clean_zero_resp(INT32 cmdValue);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfsc_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfhs_report_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_pullin_confirm(INT32 cmdValue, StrHlcIe_cui_hcu2uir_pullin_confirm_t *buf);


//LOCAL APIs
OPSTAT func_huicobuscodec_HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req_received_handle(msg_struct_huicobus_uir_test_cmd_req_t *snd, msg_struct_com_mqtt_rcv_t *rcv);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_HUICOBUSCODEC(...)	           	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_HUICOBUSCODEC]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//FIX DEFINATION
#define HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE() \
	do{\
		memset(&pMsgProc, 0, sizeof(msg_struct_com_mqtt_send_t));\
		pMsgProc.srcNode = HUICOBUS_MQTT_NBID_LOCALHOST;\
		pMsgProc.destNode = HUICOBUS_MQTT_NBID_LOCALHOST;\
		pMsgProc.srcId = HUICOBUS_MQTT_CLID_HCUENTRY;\
		pMsgProc.destId = HUICOBUS_MQTT_CLID_UIROUTER;\
		pMsgProc.topicId = HUICOBUS_MQTT_TPID_HCU2UIR;\
		pMsgProc.cmdValue = cmdValue;\
		pMsgProc.length = sizeof(msg_struct_com_mqtt_send_t);\
	}while(0)

#define HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE() \
	do{\
		if (hcu_mqtt_msg_send_syn_mode(&pMsgProc) > 0)\
			return SUCCESS;\
		else\
			return FAILURE;\
	}while(0)

//处理多个项目的过程
#define HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(msgid)\
	do{\
		if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)\
			HCU_MSG_SEND_GENERNAL_PROCESS(msgid, TASK_ID_BFDFUICOMM, TASK_ID_HUICOBUSCODEC);\
		else if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)\
			HCU_MSG_SEND_GENERNAL_PROCESS(msgid, TASK_ID_BFHSUICOMM, TASK_ID_HUICOBUSCODEC);\
		else\
			HCU_ERROR_PRINT_HUICOBUSCODEC("HUICOBUSCODEC: Error set project name!\n");\
	}while(0)

#endif /* L2FRAME_HUICOBUSCODEC_H_ */
