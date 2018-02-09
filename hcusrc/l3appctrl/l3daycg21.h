/*
 * l3daycg21.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_DAYC_G21_H_
#define L3APP_DAYC_G21_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l1hwopt/spsapi.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3DAYCG21
{
	FSM_STATE_L3DAYCG21_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3DAYCG21_ACTIVED,
	FSM_STATE_L3DAYCG21_WFFB_CTRL,
	FSM_STATE_L3DAYCG21_WFFB_DATA,
	FSM_STATE_L3DAYCG21_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

typedef struct LedMsgBuf
{
	UINT32 curLen;
	UINT8  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
}LedMsgBuf_t;

//Global variables
extern HcuFsmStateItem_t HcuFsmL3daycg21[];



//API
extern OPSTAT fsm_l3daycg21_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3daycg21_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3daycg21_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3daycg21_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//LOCAL APIS
OPSTAT func_l3daycg21_int_init(void);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3DAYCG21(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L3APP_DAYC_G21_H_ */
