/*
 * sps232.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L1HWOPT_SPS232_H_
#define L1HWOPT_SPS232_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"

#include "../l3appctrl/l3aqycg20.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_SPS232
{
	FSM_STATE_SPS232_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_SPS232_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_SPS232_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmSps232[];


#define RPI_SPS232_READ_REPEAT_TIMES 3
#define RPI_SPS232_SENSOR_READ_GAP 60 //in second

#define RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD1 0xff
#define RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD2 0xFF
#define RPI_SPS232_SENSOR_ZE08CH2O_REPORT_CMMID 0x17
#define RPI_SPS232_SENSOR_ZE08CH2O_REPORT_UNIT 0x04
#define RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN 9


#define RPI_SPS232_SENSOR_PM25SHARP_FRAME_HEAD1 0xaa
#define RPI_SPS232_SENSOR_PM25SHARP_FRAME_HEAD2 0xAA
#define RPI_SPS232_SENSOR_PM25SHARP_FRAME_TAIL1 0xff
#define RPI_SPS232_SENSOR_PM25SHARP_FRAME_TAIL2 0xFF
#define RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN 7
#define RPI_SPS232_SENSOR_PM25SHARP_COFF 200  //按照SHARP模块的定标，这个数值在150-250之间，具体需要采用其他更高精度的激光传感器定标后确定该斜率系数，这里取平均值200

//用来配置串口是否只配置了一个串口传感器，如果是的，则不需要无限次初始化，因为在这种模式下，传感器工作的效率是非常低下的，但至少可以相互共同工作的，总比不能共同工作强
//这个开关如果打开为YES的话，需要确认SYSCONFIG中，串口传感器的确至少不多于1个是打开YES的，不然其不到作用
#define RPI_SPS232_SENSOR_SINGLE_PRESENT_TRUE 1
#define RPI_SPS232_SENSOR_SINGLE_PRESENT_NO 2
#define RPI_SPS232_SENSOR_SINGLE_PRESENT RPI_SPS232_SENSOR_SINGLE_PRESENT_TRUE

//API
extern OPSTAT fsm_sps232_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sps232_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sps232_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_sps232_int_init(void);

OPSTAT func_sps232_read_data_pm25sharp(void); //PM25 sharp
OPSTAT func_sps232_read_data_ze08ch2o(void); //甲醛
UINT8 func_sps232_check_sum_ze08ch2o(UINT8 *s, UINT8 len);
UINT8 func_sps232_check_sum_pm25sharp(UINT8 *s, UINT8 len);

#endif /* L1HWOPT_SPS232_H_ */
