/*
 * led.h
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#ifndef L1HWOPT_LED_H_
#define L1HWOPT_LED_H_


#include "../l0comvm/vmlayer.h"
#include "../l0service/timer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_LED
{
	FSM_STATE_LED_INITED = 0x02,
	FSM_STATE_LED_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_LED_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmLed[];

//GPIO Based LED and Output Wave Gennerator, 简称GALOWAG / Galowag
//总控表结构 //暂时只能支持秒级计数器
typedef struct GlobalGalowagCtrlTable
{
	UINT8 galId;
	UINT8 galState;
	UINT16 galPattenOn;
	UINT16 galPattenOff;
	UINT16 galPattenCycle;
	UINT32 galTotalDuration;
	UINT16 galPeriodWorkCounter;
	UINT32 galTotalWorkCounter;
	void (*galFuncOn)(void);
	void (*galFuncOff)(void);
}GlobalGalowagCtrlTable_t;
enum GLOBAL_GALOWAG_CTRL_TABLE_ID
{
	GALOWAG_CTRL_ID_MIN = 0x00,
	GALOWAG_CTRL_ID_GLOBAL_POWER,
	GALOWAG_CTRL_ID_GLOBAL_COMMU,
	GALOWAG_CTRL_ID_GLOBAL_WORK_STATE,
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	GALOWAG_CTRL_ID_TEST1,
#endif
	GALOWAG_CTRL_ID_MAX,
};
enum GLOBAL_LED_LIGHT_CTRL_TABLE_STATE
{
	GALOWAG_CTRL_STATE_MIN = 0x00,
	GALOWAG_CTRL_STATE_ON,
	GALOWAG_CTRL_STATE_OFF,
	GALOWAG_CTRL_STATE_MAX,
};

//控制LED输出频率闪烁功能是否启动
#define HCU_LED_GALOWAG_FUNC_ACTIVE 1
#define HCU_LED_GALOWAG_FUNC_DEACTIVE 2
#define HCU_LED_GALOWAG_FUNC_SET HCU_LED_GALOWAG_FUNC_ACTIVE

#define HCU_LED_BLINK_OLD_MODE_YES 1
#define HCU_LED_BLINK_OLD_MODE_NO  2
#define HCU_LED_BLINK_OLD_MODE_SET HCU_LED_BLINK_OLD_MODE_NO

//Local variables
#define RPI_LED_PIN_VCC 2  //5V
#define RPI_LED_PIN_GND 6
#define RPI_LED_PIN_OUTPUT_LED2PIN 25  //Pin#37  //用于LED的测试

//这种LED闪烁方式将被放弃，待删去
#define RPI_LED_SENSOR_WRITE_GAP 60 //in second
#define RPI_LED_WRITE_REPEAT_TIMES 10

//API
extern OPSTAT fsm_led_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_led_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_led_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_led_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_led_galowag_start(UINT8 galId, UINT32 galDur);
extern OPSTAT hcu_led_galowag_stop(UINT8 galId);

//Local API
OPSTAT func_led_int_init(void);
OPSTAT func_led_write_data_led2pin(void);
void func_led_time_out_galowag_scan(void);
void func_led_work_state_on(void);
void func_led_work_state_off(void);
void func_led_test1_on(void);
void func_led_test1_off(void);

//高级定义，简化程序的可读性
#define IHU_ERROR_PRINT_LED zIhuRunErrCnt[TASK_ID_LED]++; IhuErrorPrint

#endif /* L1HWOPT_LED_H_ */

