/*
 * nbiotcj188.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L2FRAME_NBIOTCJ188_H_
#define L2FRAME_NBIOTCJ188_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l1hwopt/ethernet.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_NBIOTCJ188
{
	FSM_STATE_NBIOTCJ188_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_NBIOTCJ188_OFFLINE,
	FSM_STATE_NBIOTCJ188_ONLINE,
	FSM_STATE_NBIOTCJ188_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmNbiotcj188[];

#define HCU_NBIOT_CJ188_REAL_TIME_IN_REALITY 2016071301152001


//API
extern OPSTAT fsm_nbiotcj188_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_iwm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_iwm_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_ihm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_ihm_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_igm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_igm_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_ipm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_ipm_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotcj188_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_nbiotcj188_int_init(void);
OPSTAT func_nbiotcj188_time_out_period(void);
OPSTAT func_nbiotcj188_time_out_sendback_offline_data(void);
OPSTAT func_nbiotcj188_heart_beat_check(void);
OPSTAT func_nbiotcj188_heart_beat_msg_pack(CloudDataSendBuf_t *buf);
OPSTAT func_nbiotcj188_send_data_to_cloud(CloudDataSendBuf_t *buf);
OPSTAT func_nbiotcj188_ul_msg_pack(NbiotCj188BhItfComElement_t *input, CloudDataSendBuf_t *buf);
OPSTAT func_nbiotcj188_dl_msg_unpack(CloudDataSendBuf_t *buf);
OPSTAT func_nbiotcj188_checksum_caculate(char *s, INT8 output);

//外部引用
extern OPSTAT hcu_ethernet_socket_link_setup(void);
extern OPSTAT hcu_ethernet_socket_link_disconnect(void);
extern OPSTAT hcu_ethernet_socket_date_send(CloudDataSendBuf_t *buf);



#endif /* L2FRAME_NBIOTCJ188_H_ */
