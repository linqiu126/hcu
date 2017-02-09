/*
 * apiwifi.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L1HWOPT_WIFI_H_
#define L1HWOPT_WIFI_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_WIFI
{
	FSM_STATE_WIFI_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_WIFI_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_WIFI_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmWifi[];

//API
extern OPSTAT fsm_wifi_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_wifi_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_wifi_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_wifi_phy_link_setup(void);
extern OPSTAT hcu_wifi_phy_link_disconnect(void);

//Local API
OPSTAT func_wifi_int_init(void);

//对外引用的API
extern OPSTAT hcu_wifi_data_send(CloudDataSendBuf_t *buf);

#endif /* L1HWOPT_WIFI_H_ */
