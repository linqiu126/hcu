/*
 * microphone.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L1HWOPT_MICROPHONE_H_
#define L1HWOPT_MICROPHONE_H_

#include "../l0comvm/vmlayer.h"

//主要还是API机制，状态机只是考虑接收的持续性，让该模块一直处于接收状态，从而解放上层AVORION模块，以便它可以控制音视频等多种外设
//未来可以考虑，该摄像头模块处于一直工作状态，将数据片段循环写到BUFFER，上层AVORION来读取时，直接读取缓存的结果，从而避免长时间等待
//如果采用这种方式，还可以让麦克风有可能取代噪声仪，从而更加节省费用，当然，麦克风的硬件性能将决定噪声仪的指标，能否达到规范要求，待研究
//这种方式稍微多耗电，但系统的反应将非常敏捷
//摄像头可以采用类似设计方式

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_MICROPHONE
{
	FSM_STATE_MICROPHONE_INITED = 0x02,
	FSM_STATE_MICROPHONE_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_MICROPHONE_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t FsmMicrophone[];

//API
extern OPSTAT fsm_microphone_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_microphone_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_microphone_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT func_microphone_noise_apower_value_read(UINT32 noiseValue);

//Local API
OPSTAT func_microphone_int_init(void);

#endif /* L1HWOPT_MICROPHONE_H_ */
