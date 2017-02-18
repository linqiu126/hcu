/*
 * sensorpm25sharp.h
 *
 *  Created on: 12 May 2016
 *      Author: pi
 */

#ifndef L3APP_SENSORPM25SHARP_H_
#define L3APP_SENSORPM25SHARP_H_


#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_PM25SHARP
{
	FSM_STATE_PM25SHARP_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_PM25SHARP_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_PM25SHARP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

/*
#define SENSOR_PM25_SHARP_RPI_PRESENT_TRUE 1
#define SENSOR_PM25_SHARP_RPI_PRESENT_FALSE 0
#define SENSOR_PM25_RPI_SHARP_PRESENT SENSOR_PM25_SHARP_RPI_PRESENT_TRUE
*/
//Global variables
extern HcuFsmStateItem_t HcuFsmPm25sharp[];

typedef struct gTaskPm25sharpContext
{
	UINT32 sendCloudCnt;  //用于描述发送到后台，多少次才发送一次
	SerialPortCom_t 	serialPort;
}gTaskPm25sharpContext_t;



//API
extern OPSTAT fsm_pm25sharp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pm25sharp_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_pm25sharp_int_init(void);


//For sharp pm25
void func_pm25sharp_read_data(UINT32 fd);
extern OPSTAT dbi_HcuPm25SharpDataInfo_save(sensor_pm25_sharp_data_element_t *pm25Data);



#endif /* L3APP_SENSORPM25SHARP_H_ */
