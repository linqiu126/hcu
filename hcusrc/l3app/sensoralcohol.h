/*
 * sensoralcohol.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORALCOHOL_H_
#define L3APP_SENSORALCOHOL_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_ALCOHOL
{
	FSM_STATE_ALCOHOL_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_ALCOHOL_ACTIVED,
	FSM_STATE_ALCOHOL_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_ALCOHOL_RPI_PRESENT_TRUE 1
#define SENSOR_ALCOHOL_RPI_PRESENT_FALSE 0
#define SENSOR_ALCOHOL_RPI_MQ3ALCO_PRESENT SENSOR_ALCOHOL_RPI_PRESENT_TRUE
extern OPSTAT func_alcohol_time_out_read_data_from_mq3alco(void);

//Global variables
extern HcuFsmStateItem_t HcuFsmAlcohol[];

typedef struct gTaskAlcoholContext
{
	UINT32 sendCloudCnt;  //用于描述发送到后台，多少次才发送一次
}gTaskAlcoholContext_t;



//API
extern OPSTAT fsm_alcohol_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_alcohol_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_alcohol_int_init(void);

//引用外部API
extern OPSTAT dbi_HcuAlcoholDataInfo_save(sensor_alcohol_data_element_t *alcoholData);
extern OPSTAT dbi_HcuAlcoholMq3alcoDataInfo_save(sensor_alcohol_mq3alco_data_element_t *alcoholData);

#endif /* L3APP_SENSORALCOHOL_H_ */
