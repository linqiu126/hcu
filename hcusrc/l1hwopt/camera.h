/*
 * camera.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L1HWOPT_CAMERA_H_
#define L1HWOPT_CAMERA_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//主要还是API机制，状态机只是考虑接收的持续性，让该模块一直处于接收状态，从而解放上层AVORION模块，以便它可以控制音视频等多种外设
//未来可以考虑，该摄像头模块处于一直工作状态，将数据片段循环写到BUFFER，上层AVORION来读取时，直接读取缓存的结果，从而避免长时间等待
//这种方式稍微多耗电，但系统的反应将非常敏捷
//麦克风可以采用类似设计方式

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CAMERA
{
	FSM_STATE_CAMERA_INITED = 0x02,
	FSM_STATE_CAMERA_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_CAMERA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

#define JPG "./image%d.jpg"

typedef struct HcuBufferType
{
	void *start;
	UINT32 length;
}HcuBufferType_t;

//采集视频的缺省格式
#define AVORION_STREAM_PIX_FMT_DEFAULT    AV_PIX_FMT_YUV420P /* default pix_fmt */


//Global variables
extern FsmStateItem_t HcuFsmCamera[];

//API
extern OPSTAT fsm_camera_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_camera_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_camera_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_camera_int_init(void);

//第二种V4L2库形成的样例函数体系，未采用
INT32 func_camera_open_device(void);
OPSTAT func_camera_static_frame_capture(void);
OPSTAT func_camera_check_scale(UINT32 fd);
OPSTAT func_camera_check_input_output(UINT32 fd);
OPSTAT func_camera_check_standard(UINT32 fd);
OPSTAT func_camera_init_mmap(UINT32 fd);
OPSTAT func_camera_init_device(UINT32 fd);
OPSTAT func_camera_start_capture(UINT32 fd);
OPSTAT func_camera_process_image(void *addr, UINT32 length);
UINT32 func_camera_read_frame(UINT32 fd);
OPSTAT func_camera_mainloop(UINT32 fd);
OPSTAT func_camera_stop_capture(UINT32 fd);
OPSTAT func_camera_close_device(UINT32 fd);

//第三种FFMPEG官方函数例子形成的处理过程，未采用
INT32 func_camera_write_output_file(INT32 argc, char **argv, char *fileInput, char *fileOutput);
AVStream *func_camera_add_stream(AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
void func_camera_open_audio(AVFormatContext *oc, AVCodec *codec, AVStream *st);
void func_camera_write_audio_frame(AVFormatContext *oc, AVStream *st);
void func_camera_get_audio_frame(INT16 *samples, INT32 frame_size, INT32 nb_channels);
void func_camera_write_audio_frame(AVFormatContext *oc, AVStream *st);
void func_camera_close_audio(AVFormatContext *oc, AVStream *st);
void func_camera_open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st);
void func_camera_fill_yuv_image(AVPicture *pict, INT32 frame_index, INT32 width, INT32 height);
void func_camera_write_video_frame(AVFormatContext *oc, AVStream *st);
void func_camera_close_video(AVFormatContext *oc, AVStream *st);


#endif /* L1HWOPT_CAMERA_H_ */
