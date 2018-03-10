/*
 * sensor_humidity.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L3APP_SENSORHUMID_H_
#define L3APP_SENSORHUMID_H_

#include "../l1com/l1comdef.h"

//如果将所有的操作都做成可重入的，则状态机很简单，但控制表就太复杂了，所以综合取舍，状态机做成单操作不可重入的，从而控制表将简单多了
//本状态机还可以支持多个从设备一起工作，但所有设备操作相互之间均为不可重入型，意味着任何时候只有一个设备在操作，完成之后才能做下一个
//这将大大简化所有的状态机设计以及程序控制，对于轻量级的外设控制足够了
//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_HUMID
{
	FSM_STATE_HUMID_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_HUMID_ACTIVED,
	FSM_STATE_HUMID_OPT_WFFB,  //Operational and wait for feedback from MODBUS，这个状态是单入的，不能重入
	FSM_STATE_HUMID_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmHumid[];

typedef struct gTaskHumidContextElement
{
	UINT8 sensorId;
	UINT32 equId;
	UINT8 hwStatus;
	UINT8 hwAccess;
	UINT8 busyCount;  //被重复访问，但状态一直处于忙的次数
}gTaskHumidContextElement_t;
#define MAX_NUM_OF_SENSOR_HUMID_INSTALLED 1
typedef struct gTaskHumidContext
{
	gTaskHumidContextElement_t humid[MAX_NUM_OF_SENSOR_HUMID_INSTALLED];
	UINT8 currentSensorId;
	UINT32 humidValue;
}gTaskHumidContext_t;
#define SENSOR_HUMID_HW_STATUS_INVALID 0xFF
#define SENSOR_HUMID_HW_STATUS_ACTIVE 1
#define SENSOR_HUMID_HW_STATUS_DEACTIVE 2
#define SENSOR_HUMID_HW_ACCESS_IDLE 1
#define SENSOR_HUMID_HW_ACCESS_BUSY 2
#define SENSOR_HUMID_HW_ACCESS_INVALID 0xFF
#define SENSOR_HUMID_HW_ACCESS_BUSY_COUNT_NUM_MAX 3



#define SENSOR_HUMID_RPI_PRESENT_TRUE 1
#define SENSOR_HUMID_RPI_PRESENT_FALSE 0
#define SENSOR_HUMID_RPI_DHT11_PRESENT SENSOR_HUMID_RPI_PRESENT_TRUE
#define SENSOR_HUMID_RPI_SHT20_PRESENT SENSOR_HUMID_RPI_PRESENT_TRUE
#define SENSOR_HUMID_RPI_RHT03_PRESENT SENSOR_HUMID_RPI_PRESENT_TRUE
#define SENSOR_HUMID_RPI_MTH01_PRESENT SENSOR_HUMID_RPI_PRESENT_TRUE

//#define HUMID_TIMER_DURATION_PERIOD_READ 180 //should be 60 second, in second
//#define HUMID_TIMER_DURATION_MODBUS_FB 3    //通过MODBUS访问硬件，回应的时间，不给硬件太多的时间考虑问题


extern gTaskHumidContext_t		gTaskHumidContext;

//API
extern OPSTAT fsm_humid_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_data_report_from_modbus(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_modbus_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_humid_zhbl3mod_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

OPSTAT func_humid_time_out_read_data_from_dht11(void);
OPSTAT func_humid_time_out_read_data_from_sht20(void);
OPSTAT func_humid_time_out_read_data_from_rht03(void);
OPSTAT func_humid_time_out_read_data_from_mth01(void);
void func_humid_time_out_read_data_from_modbus(void);
void func_humid_time_out_processing_no_rsponse(void);

//引用外部API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuHumidDataInfo_save(sensor_humid_data_element_t *humidData);
extern OPSTAT dbi_HcuHumidDht11DataInfo_save(sensor_humid_dht11_data_element_t *humidData);
extern OPSTAT dbi_HcuHumidSht20DataInfo_save(sensor_humid_sht20_data_element_t *humidData);
extern OPSTAT dbi_HcuHumidRht03DataInfo_save(sensor_humid_rht03_data_element_t *humidData);
extern OPSTAT dbi_HcuHumidMth01DataInfo_save(sensor_humid_mth01_data_element_t *humidData);

#endif /* L3APP_SENSORHUMID_H_ */
