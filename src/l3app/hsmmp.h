/*
 * hsmmp.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_HSMMP_H_
#define L3APP_HSMMP_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_HSMMP
{
	FSM_STATE_HSMMP_INITED = 0x02,
	FSM_STATE_HSMMP_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_HSMMP_ACTIVED_WFFB,  //Operational and wait for feedback from MODBUS，这个状态是单入的，不能重入
	FSM_STATE_HSMMP_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmHsmmp[];
extern GpsPosInfo_t zHcuGpsPosInfo;

//定时器，控制摄像头工作周期及时长
//#define HSMMP_TIMER_DURATION_PERIOD_AVORION_READ 60 //should be 600second = 10分钟, in second
//#define HSMMP_AVORION_CAPTURE_DURATION_DEFAULT 2 //10 second
//下面的时间设置，必须是2.5%精度以上，不能设置为固定TIMEOUT时长，否则会出现逻辑错误
//比如，采样时间为1000秒，则超时 >= 1000*1.025 = 1025
//#define HSMMP_TIMER_DURATION_AVORION_FB HSMMP_AVORION_CAPTURE_DURATION_DEFAULT + 10    //通过AVORION的访问，得到结果的时长。这个时间必须大于摄像头干活的时长
//#define HSMMP_AVORION_REFRESH_RATE_DEFAULT 25 //10 second

//多少次回传后台一次，如果不想频繁回传，可以设置为10次
#define HSMMP_SEND_BACK_MOD_BASE  1

//API
extern OPSTAT fsm_hsmmp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_audio_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_avorion_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_cloudvela_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hsmmp_avorion_data_read_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_hsmmp_time_out_period(void);
OPSTAT func_hsmmp_time_out_wait_for_cammera_fb(void);

//引用外部函数
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT dbi_HcuHsmmpDataInfo_save(sensor_hsmmp_data_element_t *hsmmpData);

#endif /* L3APP_HSMMP_H_ */
