/*
 * gps.h
 *
 *  Created on: 2015年11月23日
 *      Author: test
 */

#ifndef L1HWOPT_GPS_H_
#define L1HWOPT_GPS_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h" //Shanchun: For serial port, to do later: move the serial port operation to a separated file
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_GPS
{
	FSM_STATE_GPS_INITED = 0x02,
	FSM_STATE_GPS_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_GPS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmGps[];
extern GpsPosInfo_t zHcuGpsPosInfo;

//API
extern OPSTAT fsm_gps_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gps_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gps_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_gps_int_init(void);

//For GPS
#define GPS_REC_BUFF_SIZE 1024


void func_gps_parse();
void func_gps_receive(UINT32 Gpsfd);
void func_gps_print_info(void);
double get_double_number(CHAR *s);
double get_locate(double temp);
UINT32 GetComma(UINT32 num,CHAR *str);
void UTC2BTC(hcuStrDateTime_t GPS);

void func_gps_resolve_GPRMC(CHAR data);
void func_gps_receive_data(UINT32 fd);

#endif /* L1HWOPT_GPS_H_ */
