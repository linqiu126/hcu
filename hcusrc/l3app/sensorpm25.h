/*
 * sensor_pm25.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L3APP_SENSORPM25_H_
#define L3APP_SENSORPM25_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//如果将所有的操作都做成可重入的，则状态机很简单，但控制表就太复杂了，所以综合取舍，状态机做成单操作不可重入的，从而控制表将简单多了
//本状态机还可以支持多个从设备一起工作，但所有设备操作相互之间均为不可重入型，意味着任何时候只有一个设备在操作，完成之后才能做下一个
//这将大大简化所有的状态机设计以及程序控制，对于轻量级的外设控制足够了
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_PM25
{
	FSM_STATE_PM25_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_PM25_ACTIVED,
	FSM_STATE_PM25_OPT_WFFB,  //Operational and wait for feedback from MODBUS，这个状态是单入的，不能重入
	FSM_STATE_PM25_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define SENSOR_PM25_RPI_PRESENT_TRUE 1
#define SENSOR_PM25_RPI_PRESENT_FALSE 0
#define SENSOR_PM25_RPI_BMPD300_PRESENT SENSOR_PM25_RPI_PRESENT_TRUE
#define SENSOR_PM25_RPI_SHARP_PRESENT SENSOR_PM25_RPI_PRESENT_TRUE

//Global variables
extern HcuFsmStateItem_t HcuFsmPm25[];

extern sensor_modbus_opertion_general_t zPM25ConfigData; // by Shanchun for sensor config data

typedef struct SensorPm25Info
{
	UINT8 sensorId;
	UINT32 equId;
	UINT8 hwStatus;
	UINT8 hwAccess;
	UINT8 busyCount;  //被重复访问，但状态一直处于忙的次数
}SensorPm25Info_t;
#define SENSOR_PM25_HW_STATUS_INVALID 0xFF
#define SENSOR_PM25_HW_STATUS_ACTIVE 1
#define SENSOR_PM25_HW_STATUS_DEACTIVE 2
#define SENSOR_PM25_HW_ACCESS_IDLE 1
#define SENSOR_PM25_HW_ACCESS_BUSY 2
#define SENSOR_PM25_HW_ACCESS_INVALID 0xFF
#define SENSOR_PM25_HW_ACCESS_BUSY_COUNT_NUM_MAX 3

#define MAX_NUM_OF_SENSOR_PM25_INSTALLED 1

//#define PM25_TIMER_DURATION_PERIOD_READ 180 //should be 60 second, in second
//#define PM25_TIMER_DURATION_MODBUS_FB 3    //通过MODBUS访问硬件，回应的时间，不给硬件太多的时间考虑问题


//API
extern OPSTAT fsm_pm25_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
void func_pm25_time_out_read_data_from_modbus(void);
void func_pm25_time_out_processing_no_rsponse(void);
extern OPSTAT fsm_pm25_cloudvela_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT func_pm25_time_out_read_data_from_bmpd300(void);
OPSTAT func_pm25_time_out_read_data_from_sharp(void);

//引用外部API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuPm25DataInfo_save(sensor_pm25_data_element_t *pm25Data);
extern OPSTAT dbi_HcuPm25ConfigData_save(sensor_modbus_opertion_general_t *PM25ConfigData);
extern OPSTAT dbi_HcuPm25ConfigData_update(UINT8 optId, sensor_modbus_opertion_general_t *PM25ConfigData);
extern OPSTAT dbi_HcuPm25Bmpd300DataInfo_save(sensor_pm25_bmpd300_data_element_t *pm25Data);
extern OPSTAT dbi_HcuPm25SharpDataInfo_save(sensor_pm25_sharp_data_element_t *pm25Data);

#endif /* L3APP_SENSORPM25_H_ */
