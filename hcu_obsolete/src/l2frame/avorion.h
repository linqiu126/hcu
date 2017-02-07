/*
 * avorion.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L2FRAME_AVORION_H_
#define L2FRAME_AVORION_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_AVORION
{
	FSM_STATE_AVORION_INITED = 0x02,
	FSM_STATE_AVORION_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_AVORION_RECEIVED_WFFB,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_AVORION_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t FsmAvorion[];


//决定输出视频格式的选择
#define AVORION_OUTPUT_FILE_FORMAT 4 //1=RGB24, 2=UYVY422, 3=YUV420P, 4=DO NOTHING/Not Save
#define AVORION_OUTPUT_FILE_FORMAT_RGB24 1
#define AVORION_OUTPUT_FILE_FORMAT_UYVY422 2
#define AVORION_OUTPUT_FILE_FORMAT_YUV420P 3

//是否打开视频采集监控框SDL2
//#define AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT 0  //1=TRUE, OTHERS=FLASE
#undef AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT

//是否从文件读入视频文件并进行转换
#define AVORION_INPUT_FROM_DEVICE_OR_FILE TRUE //TRUE=AVORION_DEVICE, OTHERS=FILES

//采集视频的缺省格式
#define AVORION_STREAM_PIX_FMT_DEFAULT    AV_PIX_FMT_YUV420P /* default pix_fmt */

/* 5 seconds stream duration */
#define AVORION_STREAM_DURATION_DEFAULT   25  //多少帧数量
#define AVORION_STREAM_FRAME_RATE_DEFAULT  25 /* 25 images/s */
#define AVORION_STREAM_NB_FRAMES_DEFAULT  ((int)(AVORION_STREAM_DURATION_DEFAULT * AVORION_STREAM_FRAME_RATE_DEFAULT))

//保护性工作参数
#define AVORION_WOKRING_DURATION_MAX 	3600 //1H
#define AVORION_WORKING_REFRSH_RATE_MIN 	5
#define AVORION_WORKING_REFRSH_RATE_MAX 	60
#define AVORION_STREAM_NB_FRAMES_MAX      90000 //1H = 3600S => 90000 frames
//Refresh Event
#define AVORION_SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
#define AVORION_SFM_INVALID  (SDL_USEREVENT + 2)

//The max file length of the Video file
#define MAX_AVORION_FILE_LENGTH 255

//API
extern OPSTAT fsm_avorion_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_stop_work(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_lcd_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_camera_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_avorion_microphone_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_avorion_int_init(void);

//第一种FFMPEG完整样例形成的程序，采用, 基于《雷霄骅》样例
INT32 func_avorion_flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
OPSTAT func_avorion_ffmpeg_capture_and_save(UINT8 fileType, char *fdir, char *ftout);
#ifdef AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT
INT32 func_avorion_refresh_thread(void *opaque);
#endif //AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT


#endif /* L2FRAME_AVORION_H_ */
