/*
 * sensorlightstr.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORLIGHTSTR_H_
#define L3APP_SENSORLIGHTSTR_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_LIGHTSTR
{
	FSM_STATE_LIGHTSTR_INITED = 0x02,
	FSM_STATE_LIGHTSTR_INITIED,
	FSM_STATE_LIGHTSTR_ACTIVED,
	FSM_STATE_LIGHTSTR_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_LIGHTSTR_RPI_PRESENT_TRUE 1
#define SENSOR_LIGHTSTR_RPI_PRESENT_FALSE 0
#define SENSOR_LIGHTSTR_RPI_BH1750_PRESENT SENSOR_LIGHTSTR_RPI_PRESENT_TRUE

//Global variables
extern FsmStateItem_t FsmLightstr[];

//API
extern OPSTAT fsm_lightstr_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_lightstr_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_lightstr_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_lightstr_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT func_lightstr_time_out_read_data_from_bh1750(void);

//Local API
OPSTAT func_lightstr_int_init(void);

//引用外部API
extern OPSTAT dbi_HcuLightstrDataInfo_save(sensor_lightstr_data_element_t *lightstrData);
extern OPSTAT dbi_HcuLightstrBh1750DataInfo_save(sensor_lightstr_bh1750_data_element_t *lightstrData);

#endif /* L3APP_SENSORLIGHTSTR_H_ */
