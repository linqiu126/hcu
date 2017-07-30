/*
 * jsoninotify.h
 *
 *  Created on: 2017年06月23日
 *      Author: LZH
 */

#ifndef L3UI_JSONINOTIFY_H_
#define L3UI_JSONINOTIFY_H_

#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_JSONINOTIFY
{
	FSM_STATE_JSONINOTIFY_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_JSONINOTIFY_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_JSONINOTIFY_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmJsoninotify[];
extern   char zHcuCmdflagJsonFile[] ;

//API
extern OPSTAT fsm_jsoninotify_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_jsoninotify_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_jsoninotify_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_jsoninotify_add_watch(char *monitorJsonFile);

#endif /* L1HWOPT_JSONINOTIFY_H_ */
