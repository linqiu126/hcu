/*
 * ethernet.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L1HWOPT_ETHERNET_H_
#define L1HWOPT_ETHERNET_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_ETHERNET
{
	FSM_STATE_ETHERNET_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_ETHERNET_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_ETHERNET_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmEthernet[];

//Local variables
#define HCU_ETHERNET_INSTANCE_DATA_SEND_TIME_OUT_IN_MS 5000
#define HCU_ETHERNET_SOCKET_DURATION_PERIOD_RECV 200000
#define HCU_ETHERNET_SOCKET_CON_ERR_PRINT_FREQUENCY 20

//API
extern OPSTAT fsm_ethernet_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ethernet_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ethernet_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_ethernet_phy_link_setup(void);
extern OPSTAT hcu_ethernet_phy_link_disconnect(void);
extern OPSTAT hcu_ethernet_curl_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_ethernet_hate_data_send(CloudDataSendBuf_t *buf);

//API for NBIOT CJ188/QG376
extern OPSTAT hcu_ethernet_socket_link_setup(void);
extern OPSTAT hcu_ethernet_socket_link_disconnect(void);
extern OPSTAT hcu_ethernet_socket_data_send(CloudDataSendBuf_t *buf);

//Local API
OPSTAT func_ethernet_int_init(void);

//引用外部函数
extern size_t func_cloudvela_http_curl_write_callback(void *buffer, size_t size, size_t nmemb, void *userp);

#endif /* L1HWOPT_ETHERNET_H_ */
