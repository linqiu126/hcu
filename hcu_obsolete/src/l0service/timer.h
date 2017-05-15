/*
 * timer.h
 *
 *  Created on: 2015年11月15日
 *      Author: test
 */

#ifndef L0SERVICE_TIMER_H_
#define L0SERVICE_TIMER_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_TIMER
{
	FSM_STATE_TIMER_INITED = 0x02,
	FSM_STATE_TIMER_ACTIVED,
	FSM_STATE_TIMER_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Granularity of 1second and 10ms timer set
#define HCU_TIMER_TICK_1_SEC 1      //in second
#define HCU_TIMER_TICK_10_MS 10000  //in u second
#define HCU_TIMER_TICK_1_MS 1000  //in u second
#define HCU_TIMER_TICK_10_MS_IN_NS 10000000  //in n second
#define HCU_TIMER_TICK_1_MS_IN_NS 1000000  //in n second
#define HCU_TIMER_TICK_100_MS_IN_NS 100000000  //in n second

//Timer Id definition
//#define MAX_TIMER_NUM_IN_ONE_HCU_1S 40  //Normal situation
#define MAX_TIMER_NUM_IN_ONE_HCU_10MS 4 //Frame usage
#define MAX_TIMER_NUM_IN_ONE_HCU_1MS 4  //Real-time usage
#define MAX_TIMER_SET_DURATION 100000
enum HCU_TIMER_ID_1S_ALL
{
	TIMER_ID_1S_MIN = 0,
	TIMER_ID_1S_SVRCON_INIT_FB,
	TIMER_ID_1S_EMC_PERIOD_READ,
	TIMER_ID_1S_EMC_MODBUS_FB,
	TIMER_ID_1S_PM25_PERIOD_READ,
	TIMER_ID_1S_PM25_MODBUS_FB,
	TIMER_ID_1S_WINDDIR_PERIOD_READ,
	TIMER_ID_1S_WINDDIR_MODBUS_FB,
	TIMER_ID_1S_WINDSPD_PERIOD_READ,
	TIMER_ID_1S_WINDSPD_MODBUS_FB,
	TIMER_ID_1S_TEMP_PERIOD_READ,
	TIMER_ID_1S_TEMP_FB,//update for SPIBUSARIES
	TIMER_ID_1S_HUMID_PERIOD_READ,
	TIMER_ID_1S_HUMID_MODBUS_FB,
	TIMER_ID_1S_NOISE_PERIOD_READ,
	TIMER_ID_1S_NOISE_MODBUS_FB,
	TIMER_ID_1S_NOISE_SPSVIRGO_FB,
	TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ,
	TIMER_ID_1S_HSMMP_AVORION_FB,
	//TIMER_ID_1S_HSMMP_FILESIZE_READ,//for disk size checking
	TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT,
	TIMER_ID_1S_CLOUDVELA_SEND_DATA_BACK,
	TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_LONG,//for cmd control long timer by Shanchun
	TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_SHORT,//for cmd control short timer by Shanchun
	TIMER_ID_1S_NBIOTCJ188_PERIOD_LINK_HEART_BEAT,
	TIMER_ID_1S_NBIOTCJ188_SEND_DATA_BACK,
	TIMER_ID_1S_NBIOTQG376_PERIOD_LINK_HEART_BEAT,
	TIMER_ID_1S_NBIOTQG376_SEND_DATA_BACK,
	TIMER_ID_1S_AIRPRS_PERIOD_READ,
	TIMER_ID_1S_CO1_PERIOD_READ,
	TIMER_ID_1S_LIGHTSTR_PERIOD_READ,
	TIMER_ID_1S_ALCOHOL_PERIOD_READ,
	TIMER_ID_1S_HCHO_PERIOD_READ,
	TIMER_ID_1S_TOXICGAS_PERIOD_READ,
	TIMER_ID_1S_PM25SHARP_PERIOD_READ,
	TIMER_ID_1S_IWM_PERIOD_READ,
	TIMER_ID_1S_IHM_PERIOD_READ,
	TIMER_ID_1S_IGM_PERIOD_READ,
	TIMER_ID_1S_IPM_PERIOD_READ,
	TIMER_ID_1S_SYSPM_PERIOD_WORKING,
	TIMER_ID_1S_MAX,
};
#define MAX_TIMER_NUM_IN_ONE_HCU_1S TIMER_ID_1S_MAX + 1  //Normal situation

enum HCU_TIMER_ID_10MS_ALL
{
	TIMER_ID_10MS_MIN = 0,
	TIMER_ID_10MS_SVRCON_TEST,
	TIMER_ID_10MS_MAX,
};
enum HCU_TIMER_ID_1MS_ALL
{
	TIMER_ID_1MS_MIN = 0,
	TIMER_ID_1MS_SVRCON_TEST,
	TIMER_ID_1MS_MAX,
};

//System level timer global variables
typedef struct HcuTimerElement
{
	UINT32 timerId;
	UINT32 taskId;
	UINT32 tDuration;
	UINT8  timerType;
	UINT8  timerRes; //Timer Resolution
	UINT32 elapse;
	UINT8  status;
}HcuTimerElement_t;
typedef struct HcuTimerTable
{
	HcuTimerElement_t timer1s[MAX_TIMER_NUM_IN_ONE_HCU_1S];
	HcuTimerElement_t timer10ms[MAX_TIMER_NUM_IN_ONE_HCU_10MS];
	HcuTimerElement_t timer1ms[MAX_TIMER_NUM_IN_ONE_HCU_1MS];
}HcuTimerTable_t;

#define TIMER_STATUS_DEACTIVE 1
#define TIMER_STATUS_ACTIVE 2
#define TIMER_STATUS_START 0xFD
#define TIMER_STATUS_STOP 0xFE
#define TIMER_STATUS_INVALID 0xFF

#define TIMER_TYPE_ONE_TIME 1
#define TIMER_TYPE_PERIOD 2
#define TIMER_TYPE_INVALID 0xFF

#define TIMER_RESOLUTION_1S 1
#define TIMER_RESOLUTION_10MS 2
#define TIMER_RESOLUTION_1MS 3
#define TIMER_RESOLUTION_INVALID 0xFF

#define HCU_TIMER_CONFIG_START_RESOLUTION_1S TRUE
#define HCU_TIMER_CONFIG_START_RESOLUTION_10MS FALSE
#define HCU_TIMER_CONFIG_START_RESOLUTION_1MS FALSE

//Global variables
extern HcuFsmStateItem_t FsmTimer[];

//API
extern OPSTAT fsm_timer_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_timer_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_timer_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_timer_start(UINT32 task_id, UINT32 timer_id, UINT32 t_dur, UINT8 t_type, UINT8 t_res);
extern OPSTAT hcu_timer_stop(UINT32 task_id, UINT32 timer_id, UINT8 t_res);

//local API
void func_timer_routine(int signo);
void func_timer_routine_handler_1s(union sigval v);
void func_timer_routine_handler_10ms(union sigval v);
void func_timer_routine_handler_1ms(union sigval v);


#endif /* L0SERVICE_TIMER_H_ */
