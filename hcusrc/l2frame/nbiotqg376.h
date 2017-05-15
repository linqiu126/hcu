/*
 * nbiotqg376.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L2FRAME_NBIOTQG376_H_
#define L2FRAME_NBIOTQG376_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_NBIOTQG376
{
	FSM_STATE_NBIOTQG376_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_NBIOTQG376_OFFLINE,
	FSM_STATE_NBIOTQG376_ONLINE,
	FSM_STATE_NBIOTQG376_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmNbiotqg376[];

//API
extern OPSTAT fsm_nbiotqg376_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_ipm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_ipm_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_nbiotqg376_int_init(void);
OPSTAT func_nbiotqg376_time_out_period(void);
OPSTAT func_nbiotqg376_time_out_sendback_offline_data(void);
OPSTAT func_nbiotqg376_heart_beat_check(void);
OPSTAT func_nbiotqg376_heart_beat_msg_pack(CloudDataSendBuf_t *buf);
OPSTAT func_nbiotqg376_send_data_to_cloud(CloudDataSendBuf_t *buf);

//外部引用
extern OPSTAT hcu_ethernet_socket_link_setup(void);
extern OPSTAT hcu_ethernet_socket_link_disconnect(void);
extern OPSTAT hcu_ethernet_socket_date_send(CloudDataSendBuf_t *buf);


#endif /* L2FRAME_NBIOTQG376_H_ */
