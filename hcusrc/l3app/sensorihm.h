/*
 * sensorihm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORIHM_H_
#define L3APP_SENSORIHM_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_IHM
{
	FSM_STATE_IHM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_IHM_ACTIVED,
	FSM_STATE_IHM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmIhm[];

typedef struct gTaskIhmContext
{
	UINT32 sendCloudCnt;  //用于描述发送到后台，多少次才发送一次
}gTaskIhmContext_t;



//API
extern OPSTAT fsm_ihm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ihm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ihm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ihm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ihm_nbiotcj188_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ihm_nbiotcj188_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_ihm_int_init(void);

//引用外部API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuIhmCj188DataInfo_save(sensor_ihm_cj188_data_element_t *ihmData);

#endif /* L3APP_SENSORIHM_H_ */
