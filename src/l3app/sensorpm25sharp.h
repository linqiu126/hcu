/*
 * sensorpm25sharp.h
 *
 *  Created on: 12 May 2016
 *      Author: pi
 */

#ifndef L3APP_SENSORPM25SHARP_H_
#define L3APP_SENSORPM25SHARP_H_


#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/sps485.h"
#include "../l1com/l1comdef.h"



//State definition

enum FSM_STATE_PM25SHARP
{
	FSM_STATE_PM25SHARP_INITED = 0x02,
	FSM_STATE_PM25SHARP_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_PM25SHARP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_PM25_SHARP_RPI_PRESENT_TRUE 1
#define SENSOR_PM25_SHARP_RPI_PRESENT_FALSE 0
#define SENSOR_PM25_RPI_SHARP_PRESENT SENSOR_PM25_SHARP_RPI_PRESENT_TRUE

//Global variables
extern FsmStateItem_t FsmPm25Sharp[];
//extern GpsPosInfo_t zHcuGpsPosInfo;

//API
extern OPSTAT fsm_pm25sharp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_pm25sharp_int_init(void);


//For sharp pm25
void func_pm25_sharp_read_data(UINT32 fd);
extern OPSTAT dbi_HcuPm25SharpDataInfo_save(sensor_pm25_sharp_data_element_t *pm25Data);



#endif /* L3APP_SENSORPM25SHARP_H_ */
