/*
 * sensornoise.h
 *
 *  Created on: 2015年12月27日
 *      Author: test
 */

#ifndef L3APP_SENSORNOISE_H_
#define L3APP_SENSORNOISE_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//如果将所有的操作都做成可重入的，则状态机很简单，但控制表就太复杂了，所以综合取舍，状态机做成单操作不可重入的，从而控制表将简单多了
//本状态机还可以支持多个从设备一起工作，但所有设备操作相互之间均为不可重入型，意味着任何时候只有一个设备在操作，完成之后才能做下一个
//这将大大简化所有的状态机设计以及程序控制，对于轻量级的外设控制足够了
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_NOISE
{
	FSM_STATE_NOISE_INITED = 0x02,
	FSM_STATE_NOISE_ACTIVED,
	FSM_STATE_NOISE_MODBUS_WFFB,  //Operational and wait for feedback from MODBUS，这个状态是单入的，不能重入
	FSM_STATE_NOISE_SPSVIRGO_WFFB,  //Operational and wait for feedback from SPSVIRGO，这个状态是单入的，不能重入
	FSM_STATE_NOISE_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t HcuFsmNoise[];

typedef struct SensorNoiseInfo
{
	UINT8 sensorId;
	UINT32 equId;
	UINT8 hwStatus;
	UINT8 hwAccess;
	UINT8 busyCount;  //被重复访问，但状态一直处于忙的次数
}SensorNoiseInfo_t;
#define SENSOR_NOISE_HW_STATUS_INVALID 0xFF
#define SENSOR_NOISE_HW_STATUS_ACTIVE 1
#define SENSOR_NOISE_HW_STATUS_DEACTIVE 2
#define SENSOR_NOISE_HW_ACCESS_IDLE 1
#define SENSOR_NOISE_HW_ACCESS_BUSY 2
#define SENSOR_NOISE_HW_ACCESS_INVALID 0xFF
#define SENSOR_NOISE_HW_ACCESS_BUSY_COUNT_NUM_MAX 3

#define MAX_NUM_OF_SENSOR_NOISE_INSTALLED 1

//#define NOISE_TIMER_DURATION_PERIOD_READ 180 //should be 60 second, in second
//#define NOISE_TIMER_DURATION_MODBUS_FB 2    //通过MODBUS访问硬件，回应的时间，不给硬件太多的时间考虑问题
#define NOISE_TIMER_DURATION_SPSVIRGO_FB 2    //通过SPSVIRGO访问硬件，回应的时间，不给硬件太多的时间考虑问题


#define NOISE_ACTIVE_CHOICE_MODBUS 1
#define NOISE_ACTIVE_CHOICE_SPSVIRGO 2
#define NOISE_ACTIVE_CHOICE_FINAL  NOISE_ACTIVE_CHOICE_SPSVIRGO

//API
extern OPSTAT fsm_noise_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_noise_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_data_report_from_spsvirgo(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_noise_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_spsvirgo_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_noise_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_noise_cloudvela_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

void func_noise_time_out_read_data_from_modbus(void);
void func_noise_time_out_read_data_from_spsvirgo(void);

void func_noise_time_out_processing_no_rsponse(void);

//引用外部API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuNoiseDataInfo_save(sensor_noise_data_element_t *noiseData);

#endif /* L3APP_SENSORNOISE_H_ */
