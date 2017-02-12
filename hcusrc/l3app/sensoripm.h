/*
 * sensoripm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORIPM_H_
#define L3APP_SENSORIPM_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_IPM
{
	FSM_STATE_IPM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_IPM_ACTIVED,
	FSM_STATE_IPM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmIpm[];

//API
extern OPSTAT fsm_ipm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_nbiotcj188_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_nbiotcj188_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_nbiotqg376_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_nbiotqg376_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_ipm_int_init(void);

//引用外部API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuIpmCj188DataInfo_save(sensor_ipm_cj188_data_element_t *ipmData);
extern OPSTAT dbi_HcuIpmQg376DataInfo_save(sensor_ipm_qg376_data_element_t *iwmData);

#endif /* L3APP_SENSORIPM_H_ */
