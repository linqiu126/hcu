/*
 * sensorhcho.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORHCHO_H_
#define L3APP_SENSORHCHO_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_HCHO
{
	FSM_STATE_HCHO_INITED = 0x02,
	FSM_STATE_HCHO_INITIED,
	FSM_STATE_HCHO_ACTIVED,
	FSM_STATE_HCHO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_HCHO_RPI_PRESENT_TRUE 1
#define SENSOR_HCHO_RPI_PRESENT_FALSE 0
#define SENSOR_HCHO_RPI_ZE08CH2O_PRESENT SENSOR_HCHO_RPI_PRESENT_TRUE
extern OPSTAT func_hcho_time_out_read_data_from_ze08ch2o(void);

//Global variables
extern HcuFsmStateItem_t FsmHcho[];

//API
extern OPSTAT fsm_hcho_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hcho_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hcho_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hcho_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_hcho_int_init(void);

//引用外部API
extern OPSTAT dbi_HcuHchoDataInfo_save(sensor_hcho_data_element_t *hchoData);
extern OPSTAT dbi_HcuHchoZe08ch2oDataInfo_save(sensor_hcho_ze08ch2o_data_element_t *hchoData);

#endif /* L3APP_SENSORHCHO_H_ */
