/*
 * sensorairprs.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORAIRPRS_H_
#define L3APP_SENSORAIRPRS_H_

#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_AIRPRS
{
	FSM_STATE_AIRPRS_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_AIRPRS_INITIED,
	FSM_STATE_AIRPRS_ACTIVED,
	FSM_STATE_AIRPRS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_AIRPRS_RPI_PRESENT_TRUE 1
#define SENSOR_AIRPRS_RPI_PRESENT_FALSE 0
#define SENSOR_AIRPRS_RPI_BMP180_PRESENT SENSOR_AIRPRS_RPI_PRESENT_TRUE

//Global variables
extern HcuFsmStateItem_t HcuFsmAirprs[];

typedef struct gTaskAirprsContext
{
	UINT32 sendCloudCnt;  //用于描述发送到后台，多少次才发送一次
}gTaskAirprsContext_t;


//API
extern OPSTAT fsm_airprs_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT func_airprs_time_out_read_data_from_bmp180(void);
extern OPSTAT fsm_airprs_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_airprs_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_airprs_int_init(void);

//引用外部API
extern OPSTAT dbi_HcuAirprsDataInfo_save(sensor_airprs_data_element_t *airprsData);
extern OPSTAT dbi_HcuAirprsBmp180DataInfo_save(sensor_airprs_bmp180_data_element_t *airprsData);
extern OPSTAT dbi_HcuAirprsAltitudeBmp180DataInfo_save(sensor_airprs_altitude_bmp180_data_element_t *altitudeData);

#endif /* L3APP_SENSORAIRPRS_H_ */
