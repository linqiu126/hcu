/*
 * spi.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L1HWOPT_SPI_H_
#define L1HWOPT_SPI_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPI
{
	FSM_STATE_SPI_INITED = 0x02,
	FSM_STATE_SPI_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_SPI_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmSpi[];
extern float zHcuSpiTempRht03;
extern float zHcuSpiHumidRht03;
extern float zHcuSpiTempMth01;
extern float zHcuSpiHumidMth01;

//Local definition
#define RPI_SPI_PIN_VCC 19
#define RPI_SPI_PIN_GND 25
#define RPI_SPI_PIN_MOSI 19
#define RPI_SPI_PIN_MISO 21
#define RPI_SPI_PIN_CE0 10  //片选 Pin#24
#define RPI_SPI_PIN_CE1 11  //片选 Pin#26


#define RPI_SPI_ADDR_MTH01 0 //选择第一个地址，CE0
#define RPI_SPI_ADDR_RHT03 1 //选择第二个地址，CE1
#define RPI_SPI_SPEED 4000000 //另外一种情况是直接采用官方所说的2K的速率，这里采用4Mbps
#define RPI_SPI_SENSOR_READ_GAP 60 //in second
#define RPI_SPI_READ_REPEAT_TIMES 10

//API
extern OPSTAT fsm_spi_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spi_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spi_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT func_spi_read_data_rht03(void);
OPSTAT func_spi_read_data_mth01(void);

//Local API
OPSTAT func_spi_int_init(void);

#endif /* L1HWOPT_SPI_H_ */
