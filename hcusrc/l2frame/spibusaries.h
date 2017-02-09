/*
 * spibusaries.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_SPIBUSARIES_H_
#define L2FRAME_SPIBUSARIES_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
//#include "../l0comvm/commsg.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_SPIBUSARIES
{
	FSM_STATE_SPIBUSARIES_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_SPIBUSARIES_ACTIVED,
	FSM_STATE_SPIBUSARIES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


typedef struct SPIBusariesMsgBuf
{
	/*
	UINT32 curLen;
	UINT8  curBuf[MAX_HCU_MSG_BODY_LENGTH];
	*/

	UINT8 start; //0xAA
	UINT16 len; //0-1500
	UINT8 checksum1;//加总头的结果
	UINT8 msgBody[MAX_HCU_MSG_BODY_LENGTH];
	UINT8 checksum2;//加总消息体的结果
}SPIBusariesMsgBuf_t; // to update for SPIBUSARIES later

//Global variables
extern HcuFsmStateItem_t HcuFsmSpibusaries[];



//FSM API
extern OPSTAT fsm_spibusaries_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spibusaries_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spibusaries_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT fsm_spibusaries_temp_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

extern OPSTAT func_spibusaries_temp_msg_pack(msg_struct_temp_spibusaries_data_read_t *inMsg, SPIBusariesMsgBuf_t *outMsg);
//Local API
OPSTAT func_spibusaries_int_init(void);


#endif /* L2FRAME_SPIBUSARIES_H_ */
