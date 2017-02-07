/*
 * sensortoxicgas.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORTOXICGAS_H_
#define L3APP_SENSORTOXICGAS_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_TOXICGAS
{
	FSM_STATE_TOXICGAS_INITED = 0x02,
	FSM_STATE_TOXICGAS_INITIED,
	FSM_STATE_TOXICGAS_ACTIVED,
	FSM_STATE_TOXICGAS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_TOXICGAS_RPI_PRESENT_TRUE 1
#define SENSOR_TOXICGAS_RPI_PRESENT_FALSE 0
#define SENSOR_TOXICGAS_RPI_MQ135_PRESENT SENSOR_TOXICGAS_RPI_PRESENT_TRUE
#define SENSOR_TOXICGAS_RPI_ZP01VOC_PRESENT SENSOR_TOXICGAS_RPI_PRESENT_TRUE
extern OPSTAT func_toxicgas_time_out_read_data_from_mq135(void);
extern OPSTAT func_toxicgas_time_out_read_data_from_zp01voc(void);

//Global variables
extern HcuFsmStateItem_t HcuFsmToxicgas[];

//API
extern OPSTAT fsm_toxicgas_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_toxicgas_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_toxicgas_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_toxicgas_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_toxicgas_int_init(void);

//引用外部API
extern OPSTAT dbi_HcuToxicgasDataInfo_save(sensor_toxicgas_data_element_t *toxicgasData);
extern OPSTAT dbi_HcuToxicgasMq135DataInfo_save(sensor_toxicgas_mq135_data_element_t *toxicgasData);
extern OPSTAT dbi_HcuToxicgasZp01vocDataInfo_save(sensor_toxicgas_zp01voc_data_element_t *toxicgasData);

#endif /* L3APP_SENSORTOXICGAS_H_ */
