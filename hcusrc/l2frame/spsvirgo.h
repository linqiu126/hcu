/*
 * spsvirgo.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L2FRAME_SPSVIRGO_H_
#define L2FRAME_SPSVIRGO_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPSVIRGO
{
	FSM_STATE_SPSVIRGO_INITED = 0x02,
	FSM_STATE_SPSVIRGO_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_SPSVIRGO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t HcuFsmSpsvirgo[];
extern GpsPosInfo_t zHcuGpsPosInfo;


#define SPSVIRGO_ACTIVE_CHOICE_NOISE_MICROPHONE 1
#define SPSVIRGO_ACTIVE_CHOICE_NOISE_ZSY 2  //自行购买的噪声仪，还挺贵的，700大元呢，奈何格式搞不清楚。。。
#define SPSVIRGO_ACTIVE_CHOICE_NOISE_FINAL  SPSVIRGO_ACTIVE_CHOICE_NOISE_ZSY

typedef struct SerialSpsMsgBuf
{
	UINT32 curLen;
	UINT8  curBuf[MAX_HCU_MSG_BODY_LENGTH];
}SerialSpsMsgBuf_t;


//API
extern OPSTAT fsm_spsvirgo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spsvirgo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spsvirgo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spsvirgo_stop_work(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spsvirgo_noise_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spsvirgo_noise_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_spsvirgo_int_init(void);
unsigned char HexToAsc(unsigned char aChar);

//外部引用API
extern OPSTAT func_microphone_noise_apower_value_read(UINT32 noiseValue);



#endif /* L2FRAME_SPSVIRGO_H_ */
