/*
 * i2c.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L1HWOPT_I2C_H_
#define L1HWOPT_I2C_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_I2C
{
	FSM_STATE_I2C_INITED = 0x02,
	FSM_STATE_I2C_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_I2C_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmI2c[];
extern float zHcuI2cTempSht20;
extern float zHcuI2cHumidSht20;
extern float zHcuI2cLightstrBh1750;
extern float zHcuI2cAirprsBmp180;
extern float zHcuI2cTempBmp180;
extern float zHcuI2cAltitudeBmp180; //通过气压计算出的海拔高度数据
extern float zHcuI2cPm25Bmpd300;

//Local definition
#define RPI_I2C_PIN_VCC 17
#define RPI_I2C_PIN_GND 25
#define RPI_I2C_PIN_SDA 3
#define RPI_I2C_PIN_SCL 5

#define RPI_I2C_ADDR_SHT20 0x40
#define RPI_I2C_ADDR_BH1750 0x23
#define RPI_I2C_ADDR_BMP180 0x77
#define RPI_I2C_ADDR_BMPD300 0x11  //待确定
#define RPI_I2C_READ_REPEAT_TIMES 10
#define RPI_I2C_SENSOR_READ_GAP 60 //in second

//API
extern OPSTAT fsm_i2c_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_i2c_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_i2c_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT func_i2c_read_data_sht20(void);  //温湿度
OPSTAT func_i2c_read_data_bh1750(void); //光强GY30
OPSTAT func_i2c_read_data_bmp180(void); //气压GY68
OPSTAT func_i2c_read_data_bmpd300(void); //PM25蓝月公司

//Local API
OPSTAT func_i2c_int_init(void);


#endif /* L1HWOPT_I2C_H_ */
