/*
 * avorion.c
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

//SDL2.0 LXH =>http://blog.csdn.net/leixiaohua1020/article/details/38868499
//SDL2.0 New =>http://blog.csdn.net/yue7603835/article/details/41976175
//SDL2.0 深度分析=> http://www.cnblogs.com/xkfz007/p/4524511.html

#include "avorion.h"

#include "../l0service/trace.h"
#ifdef AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT
#include <SDL2/SDL.h>
#endif //AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT

/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
  *Add SPS,PPS in front of IDR frame
  *Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter
#define USE_H264BSF 0

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter
#define USE_AACBSF 0



/*
** FSM of the AVORION
*/
HcuFsmStateItem_t HcuFsmAvorion[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_avorion_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_avorion_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_avorion_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_HSMMP_AVORION_DATA_READ,    FSM_STATE_AVORION_RECEIVED,      fsm_avorion_data_read},
    {MSG_ID_HSMMP_AVORION_STOP,     	FSM_STATE_AVORION_RECEIVED,      fsm_avorion_stop_work},
	{MSG_ID_LCD_AVORION_DATA_RX,       	FSM_STATE_AVORION_RECEIVED,      fsm_avorion_lcd_data_rx},
	{MSG_ID_CAMERA_AVORION_DATA_RX,     FSM_STATE_AVORION_RECEIVED,      fsm_avorion_camera_data_rx},
	{MSG_ID_MICROPHONE_AVORION_DATA_RX, FSM_STATE_AVORION_RECEIVED,      fsm_avorion_microphone_data_rx},

    //精细控制AV外设时，可以使用等待状态机，从而确保解码的单入性，防止相互之间的串扰和影响
    {MSG_ID_COM_INIT_FEEDBACK,			FSM_STATE_AVORION_RECEIVED_WFFB,    fsm_com_do_nothing},
    {MSG_ID_COM_HEART_BEAT,     		FSM_STATE_AVORION_RECEIVED_WFFB,   	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  		FSM_STATE_AVORION_RECEIVED_WFFB,    fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//extern global variables

//视频全局控制变量，采用
UINT32 zHcuAvorionFrameTotalNum = 0;
UINT32 zHcuAvorionRefreshRate = 0;

//实时计数变量，采用
INT32  zHcuAvorionRefreshThreadExitFlag = 0; //全局控制标识
UINT32 zHcuAvorionFrameCnt = 0; //计数摄像头的读取帧数
UINT32 zHcuAvorionFrameRefreshCnt = 0; //使用该计数器给刷新线程计数使用
UINT32 zHcuEncodeFrameCnt = 0; //计数存入文件编码的帧数
UINT32 zHcuSdlPushOrNot = 0; //防止重复push

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_avorion_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_AVORION, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_avorion_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_AVORION, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("AVORION: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_AVORION].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	FsmSetState(TASK_ID_AVORION, FSM_STATE_AVORION_INITED);

	//初始化硬件接口
	if (func_avorion_int_init() == FAILURE){
		HcuErrorPrint("AVORION: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION] = 0;
	zHcuAvorionFrameTotalNum = 0;
	zHcuAvorionRefreshRate = 0;
	zHcuAvorionRefreshThreadExitFlag = 0;
	zHcuAvorionFrameCnt = 0;
	zHcuAvorionFrameRefreshCnt = 0;
	zHcuEncodeFrameCnt = 0;

	//设置状态机到目标状态
	FsmSetState(TASK_ID_AVORION, FSM_STATE_AVORION_RECEIVED);
	HCU_DEBUG_PRINT_FAT("AVORION: Enter FSM_STATE_AVORION_ACTIVED status, Keeping refresh here!\n");

	//进入阻塞式接收数据状态，然后继续发送
	// if (func_avorion_static_frame_capture() == FAILURE){

	//干一次活，纯粹为了测试，后面会禁止在这里干活
	/*int i=0;
	for (i = 0; i<10; i++){
		zHcuAvorionFrameTotalNum = AVORION_STREAM_DURATION_DEFAULT;
		zHcuAvorionRefreshRate = AVORION_STREAM_FRAME_RATE_DEFAULT;
		if (func_avorion_ffmpeg_capture_and_save(FILE_OPERATION_TYPE_AVORION_H264, "./test01.h264", "output.yuv") == FAILURE){
			zHcuRunErrCnt[TASK_ID_AVORION]++;
			HcuErrorPrint("AVORION: Error capture frame main loop function!\n");
			return FAILURE;
		}
	}*/


	return SUCCESS;
}

OPSTAT fsm_avorion_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("AVORION: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_avorion_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_avorion_int_init(void)
{
	return SUCCESS;
}

//由HSMMP控制，在该状态下干活，其它情形下干活的，都是测试过程
OPSTAT fsm_avorion_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret = 0;
	//char fileNameBack[HCU_FILE_NAME_LENGTH_MAX] ="";
	//char fileDirName[HCU_FILE_NAME_LENGTH_MAX] ="";

	//收消息
	msg_struct_hsmmp_avorion_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_hsmmp_avorion_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_hsmmp_avorion_data_read_t))){
		HcuErrorPrint("AVORION: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查参数
	if ((rcv.fileType != FILE_OPERATION_TYPE_AVORION_H264) && (rcv.fileType != FILE_OPERATION_TYPE_AVORION_AVI) && (rcv.fileType != FILE_OPERATION_TYPE_AVORION_MKV)){
		HcuErrorPrint("AVORION: Receive error file type!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	if ((rcv.captureDur > AVORION_WOKRING_DURATION_MAX) || (rcv.captureDur <=0)){
		HcuErrorPrint("AVORION: Receive error working duration!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	if ((rcv.refreshRate > AVORION_WORKING_REFRSH_RATE_MAX) || (rcv.refreshRate < AVORION_WORKING_REFRSH_RATE_MIN)){
		HcuErrorPrint("AVORION: Receive error working refresh rate!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	if ((rcv.boolBackCloud != TRUE) && (rcv.boolBackCloud != FALSE)){
		HcuErrorPrint("AVORION: Receive error working back type!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	if ((rcv.fDirName ==NULL) || (rcv.fName == NULL) || (rcv.tmpFname == NULL)){
		HcuErrorPrint("AVORION: Receive error file dir or name!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}

	//设置参数
	zHcuAvorionFrameTotalNum = rcv.captureDur * rcv.refreshRate;
	if (zHcuAvorionFrameTotalNum > AVORION_STREAM_NB_FRAMES_MAX) zHcuAvorionFrameTotalNum = AVORION_STREAM_NB_FRAMES_MAX;
	zHcuAvorionRefreshRate = rcv.refreshRate;

	//启动干活
	printf("AVORION: TotalFrame=%d, RefreshRate=%d, FileType=%d, Fdir=[%s], Fname=[%s], tmpFile=[%s]\n", zHcuAvorionFrameTotalNum, zHcuAvorionRefreshRate,
			rcv.fileType, rcv.fDirName, rcv.fName, rcv.tmpFname);
	/*
	if (func_avorion_ffmpeg_capture_and_save(rcv.fileType, rcv.fDirName, rcv.tmpFname) == FAILURE){
		zHcuRunErrCnt[TASK_ID_AVORION]++;
		HcuErrorPrint("AVORION: Error capture frame main loop function!\n");
		return FAILURE;
	}
	*/
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: fileNameBack=[%s]\n", rcv.fName);
	}

	//发送结果给HSMMP
	msg_struct_avorion_hsmmp_data_read_fb_t snd;
	memset(&snd, 0, sizeof(msg_struct_avorion_hsmmp_data_read_fb_t));
	snd.length = sizeof(msg_struct_avorion_hsmmp_data_read_fb_t);
	snd.hsmmp.equipid = rcv.equipid;
	snd.boolBackCloud = rcv.boolBackCloud;
	char outfiletype[5] = ".mp4";
	strcpy(snd.hsmmp.hsmmpFdir, rcv.fDirName);
	//add by shanchun for change file name from .264 to .mp4 due to the captured files have been converted into .mp4 files via func_avorion_ffmpeg_capture_and_save
	strcat(snd.hsmmp.hsmmpFdir, outfiletype);
	strcpy(snd.hsmmp.hsmmpFname, rcv.fName);
	strcat(snd.hsmmp.hsmmpFname, outfiletype);
	strcpy(snd.hsmmp.hsmmpLink, rcv.tmpFname); //使用linkname暂时代替TmpName，并带给上层模块
	//考虑到AVORION将放到外部单独进程中，GPS时钟无法共享，故而GPS数据的采集直接放到HSMMP模块中，而不放到传感器中
	snd.hsmmp.timeStamp = rcv.timeStampStart;

	ret = hcu_message_send(MSG_ID_AVORION_HSMMP_DATA_READ_FB, TASK_ID_HSMMP, TASK_ID_AVORION, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		HcuErrorPrint("AVORION: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_AVORION].taskName, zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName);
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_avorion_stop_work(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//检查参数

	//将工作的总帧数直接赋值为0，干活既停止了
	zHcuAvorionFrameTotalNum = 0;
	return SUCCESS;
}

//音视频是个大海洋，linux支持的音视频格式，已经具备编解码库的，达到数百种
//这部分程序由三个样例拼装起来，未来可以继续优化
//第一部分是如何读取摄像头的数据，第二部分是存入H264格式，第三部分是如何进行格式转化并存入文件
//其中格式读取摄像头的数据，还使用到另外一个技巧，就是单启刷新时钟线程，未来可以优化为高精度时钟
//格式转化中使用了样例的函数，未来可以跟第一部分进行进一步融合

//fileType: 指明文件的类型，比如AVI/H264等
//fdir: 带目录的完整文件名字
//ftout: 不带目录的本地干净文件名字
#ifdef AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT
OPSTAT func_avorion_ffmpeg_capture_and_save(UINT8 fileType, char *fdir, char *ftout)
{
	//输入视频控制参数（包括潜在的输入数据文件）
	AVFormatContext *pFormatCtxAvorion; 	//输入输出的容器，但不能同时作为输入和输出的容器
	AVCodecContext *pCodecCtxAvorion;   	//输入视频解码器上下文
	AVCodec *pCodecAvorion;    			//输入视频解码器，是由链表组成的，链表的类型是AVCodec
	AVFrame *pFrameAvorion;    			//输入视频帧，用来保存数据缓存的对象
	AVFrame *pFrameYUVmid; 				//输入视频采集得到的中间帧，视频缓存对象
	AVPacket *packetAvorion;    			//输入视频数据包，主要记录音视频数据帧，时钟信息和压缩数据首地址，大小等信息
	AVInputFormat *ifmt; 				//输入音视频输入格式
	//SDL_Overlay *bmpSdlOutput;  		//SDL1.2视频输出界面
    SDL_Window *screen;					//SDL2监控窗口容器
    SDL_Renderer* sdlRenderer;			//SDL2监控窗口渲染器
    SDL_Texture* sdlTexture;			//SDL2监控窗口的纹理
    SDL_Rect sdlRect;					//SDL2监控窗口在屏幕上的位置
    SDL_Event event;
	struct SwsContext *pSwsCtxAvorion;  	//视频分辨率，色彩空间变换时所需要的上下文句柄
	//char VIDEO_INPUT[]="/dev/video0";	//测试时固定输入摄像头的物理标识，直接应用外部定义的摄像头
	INT32 videoStreamAvorion=0;			//视频输入
	INT32 frameFinishedAvorion=0;		//视频输入
	INT32 numBytesAvorion=0;				//视频输入
	UINT8 *bufferAvorion;				//视频输入

	//中间临时输出文件之一的控制参数，以没有经过编码的YUV为格式
	//目前留着这个纯粹是为了测试和对比目的，未来可以去掉
	//为了进程的相互独立，该参数移到L3了
	//const char* fileNameOutputYUVmid = "output.yuv";
	FILE *filePtrOutputYUVmid;

	//输出视频流控制参数（包括主输出文件）
    AVFormatContext* pFormatCtxOutput;  //输出视频的容器
    AVOutputFormat* ofmt;				//输出视频的数据格式
    AVStream* video_st_output;			//输出视频数据输出流
    AVCodecContext* pCodecCtxOutput;	//输出视频编码器上下文
    AVCodec* pCodecOutput;				//输出视频编码器
    AVPacket* packetOutput;				//输出视频编码包
    AVFrame* pFrameOutput;				//输出视频帧
	UINT8* bufferOutput;				//输出视频缓冲存储区
    INT32 picture_size = 0;				//输出视频的包缓冲区申请长度

    //来源于输入文件，直接读取并转换为中间数据
    int y_size;
	FILE *in_file = fopen("src01_480x272.yuv", "rb");   //Input raw YUV data
    //int in_w=640, in_h=480;                              //Input data's width and height
    //char out_file[]="./test01.h264";              	//Output Filepath

	//临时控制参数
	INT32 i=0, ret=0;

	/*
	 * STEP1: 初始化全体对象
	 *
	 */
	//Register all devices
	/*//输出文件名字
    if ((fileType == FILE_OPERATION_TYPE_AVORION_H264) && (strlen(zCurTimeDate.curAvorionFdH264) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(fnb, zCurTimeDate.curAvorionFnameH264);
    	strcpy(fdir, zCurTimeDate.curAvorionFdH264);
    }else if((fileType == FILE_OPERATION_TYPE_AVORION_AVI) && (strlen(zCurTimeDate.curAvorionFdAvi) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(fdir, zCurTimeDate.curAvorionFdAvi);
    	strcpy(fnb, zCurTimeDate.curAvorionFnameAvi);
    }else if((fileType == FILE_OPERATION_TYPE_AVORION_MKV) && (strlen(zCurTimeDate.curAvorionFdMkv) < HCU_FILE_NAME_LENGTH_MAX)){
    	strcpy(fdir, zCurTimeDate.curAvorionFdMkv);
    	strcpy(fnb, zCurTimeDate.curAvorionFnameMkv);
    }else{
        HcuErrorPrint("AVORION: Not set save file type correctly.\n");
        zHcuRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
	*/
	//初始化计数器
    zHcuAvorionFrameCnt = 0; //Incoming source frame
    zHcuAvorionFrameRefreshCnt = 0; //Refresh frame
    zHcuEncodeFrameCnt = 0; //Output frame
    zHcuAvorionRefreshThreadExitFlag = 0; //Thread control counter
    zHcuSdlPushOrNot = FALSE; //防止SDL_QUIT重复PUSH，目前还未找到FLUSH SDL队列的方式

    //初始化系统
	av_register_all();
	avformat_network_init();
    avdevice_register_all();

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step1 Global init success!\n");
	}

	/*
	 * STEP2: 打开输入设备或者文件或者内存 =>pFormatCtxAvorion
	 *
	 */
    //输入文件格式
	//const char *filename = "/home/hitpony/workspace/HelloWorld/Debug/test.avi";
	//读取输入源的格式，并且把信息保存到pFormatCtx
    pFormatCtxAvorion=avformat_alloc_context();
	/*INT32 result=0;
	char s[100];
	result = avformat_open_input(&pFormatCtx, filename, NULL, NULL);  //双指针
	strcpy(s, strerror(errno));
	if (result!= 0)	return FAILURE;*/
	ifmt=av_find_input_format("video4linux2");
    if(avformat_open_input(&pFormatCtxAvorion, HCU_DEFAULT_DEVICE_USB_AVORION0, ifmt, NULL)!=0){
        HcuErrorPrint("AVORION: Couldn't open input stream.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
	//负责为pFormatCtx->streams填上正确的信息,pFormatCtx->streams仅仅是一组大小为pFormatCtx->nb_streams的指针
	//av_dump_format(pFormatCtxAvorion, 0, filename, 0);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step2 input avorion source context creation, pFormatCtxAvorion=0x%x\n", pFormatCtxAvorion);
	}

	/*
	 * STEP3: 找到输入流 =>videoStreamAvorion
	 *
	 */
	//从文件的头部，检查文件中的流的信息
	if (avformat_find_stream_info(pFormatCtxAvorion, NULL) < 0){
		HcuErrorPrint("AVORION: Couldn't find stream information.\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Couldn't find stream information
	}
	// Find the first video stream
	videoStreamAvorion = -1;
	for (i = 0; i < pFormatCtxAvorion->nb_streams; i++)
	{
		if (pFormatCtxAvorion->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamAvorion = i;
			break;
		}
	}
	if (videoStreamAvorion == -1){
        HcuErrorPrint("AVORION: Couldn't find a video stream.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Didn't find a video stream
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step3 input avorion source stream finding, videoStreamAvorion=%d\n", videoStreamAvorion);
	}

	/*
	 * STEP4: 找到解码器 =>pCodecCtxAvorion
	 *
	 */
	// 得到解码器的上下文信息
	pCodecCtxAvorion = pFormatCtxAvorion->streams[videoStreamAvorion]->codec;
	// Find the decoder for the video stream
	pCodecAvorion = avcodec_find_decoder(pCodecCtxAvorion->codec_id);
	if (pCodecAvorion == NULL) {
		HcuErrorPrint(strerror(errno), "AVORION: Unsupported codec!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Codec not found
	}
	// Open codec
	if (avcodec_open2(pCodecCtxAvorion, pCodecAvorion, NULL) < 0){
        HcuErrorPrint("AVORION: Could not open codec.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Could not open codec
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step4 input avorion source decoder pCodecCtxAvorion=0x%x， pCodecAvorion=0x%x\n", pCodecCtxAvorion, pCodecAvorion);
	}

	/*
	 * STEP5: 为解码器以及中间YUV帧申请FRAME对象
	 *
	 */
	// Allocate video frame 该函数负责申请内存, 注意:初始化的时候AVFrame中的元素data, linesize均为空。未指向任何内存数据
	pFrameAvorion = av_frame_alloc(); //avcodec_alloc_frame();
	// Allocate an AVFrame structure
	pFrameYUVmid = av_frame_alloc(); //avcodec_alloc_frame();
	if ((pFrameYUVmid == NULL) || (pFrameAvorion == NULL)){
		HcuErrorPrint("AVORION: Not support frame allocation!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	// 计算AVCodeContext缓冲区的大小和申请空间
	numBytesAvorion = avpicture_get_size(AVORION_STREAM_PIX_FMT_DEFAULT, pCodecCtxAvorion->width, pCodecCtxAvorion->height);  //PIX_FMT_YUV420P
	//申请内存对齐, 但不保证内存泄漏
	bufferAvorion = (UINT8 *) av_malloc(numBytesAvorion * sizeof(UINT8));
	//给pFrameYUVmid分配内存, 把帧和pFrameYUVmid关联起来
	avpicture_fill((AVPicture *)pFrameYUVmid, bufferAvorion, AVORION_STREAM_PIX_FMT_DEFAULT, pCodecCtxAvorion->width, pCodecCtxAvorion->height); //PIX_FMT_YUV420P
	//准备按照包为步长进行读取和处理
	packetAvorion = (AVPacket *)av_malloc(sizeof(AVPacket));
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step5 Avorion/YUV malloc size=%d, bufferAvorion=0x%x， packetAvorion=0x%x\n", numBytesAvorion, bufferAvorion, packetAvorion);
	}

	/*
	 * STEP6: SDL1.2输出屏幕初始化
	 * 视频库已经换成SDL2.0了，这部分不再有用了，放在这里只是作为对比参考，可以看出，SDL1.2和SDL2.0对视频的操作函数及流程差异极大
	 *
	 */
	//初始化输出屏幕的播放器
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		HcuErrorPrint(stderr, "AVORION: Could not initialize SDL - %s/n", SDL_GetError());
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	//初始化输出屏幕并设置视频模式
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(pCodecCtxAvorion->width, pCodecCtxAvorion->height, 0,0);
	if (!screen)
	{
		HcuErrorPrint(stderr, "AVORION: could not set video mode - exiting/n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	//初始化输出视频格式
	bmpSdlOutput = SDL_CreateYUVOverlay(pCodecCtxAvorion->width, pCodecCtxAvorion->height, SDL_YV12_OVERLAY, screen);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtxAvorion->width;
	rect.h = pCodecCtxAvorion->height;
	SDL_WM_SetCaption("BXXH FFMPEG MONITOR AVORION", NULL);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step6.0 SDL1.2 display generation, bmpSdlOutput=0x%x, screen width=%d, height=%d\n", bmpSdlOutput, rect.w, rect.h);
	}

	/*
	 * STEP6.5: SDL2.0输出屏幕初始化
	 *
	 */
	if (AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT == TRUE)
	{
		//初始化输出屏幕的播放器
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
		{
			HcuErrorPrint("AVORION: Could not initialize SDL - %s/n", SDL_GetError());
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			return FAILURE;
		}
		//初始化输出屏幕并设置视频模式
		//SDL_Surface *screen;
		screen = SDL_CreateWindow("BXXH SDL2.0 MONITOR VIDEO", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pCodecCtxAvorion->width, pCodecCtxAvorion->height, SDL_WINDOW_OPENGL);
		if (!screen)
		{
			HcuErrorPrint("AVORION: could not set video mode - exiting: %s/n", SDL_GetError());
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			return FAILURE;
		}
		//初始化输出视频格式
		sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	    //IYUV: Y + U + V  (3 planes)
	    //YV12: Y + V + U  (3 planes)
		sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtxAvorion->width, pCodecCtxAvorion->height);
	    sdlRect.x = 0;
	    sdlRect.y = 0;
	    sdlRect.w = pCodecCtxAvorion->width;
	    sdlRect.h = pCodecCtxAvorion->height;
	    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
	    	HcuDebugPrint("AVORION: Step6.5 SDL2.0 display generation, sdlTexture=0x%x, screen width=%d, height=%d\n", sdlTexture, sdlRect.w, sdlRect.h);
	    }
	}

	/*
	 * STEP7: 初始化输出文件之一和视频取景框
	 *
	 */
	//初始化文件
	filePtrOutputYUVmid = fopen(ftout, "wb+");
    if (filePtrOutputYUVmid == NULL){
    	HcuErrorPrint("AVORION: Cann't open temp-mid output file!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //视频取景框的设定：负责得到视频分辨率，色彩控件变换时的上下文句柄, 获取渲染的句柄
    pSwsCtxAvorion = sws_getContext(pCodecCtxAvorion->width,
    		pCodecCtxAvorion->height,
			pCodecCtxAvorion->pix_fmt,
    		pCodecCtxAvorion->width,
			pCodecCtxAvorion->height,
			AVORION_STREAM_PIX_FMT_DEFAULT,
			SWS_BICUBIC, NULL, NULL, NULL);
    SDL_Thread *video_tid = SDL_CreateThread((SDL_ThreadFunction)func_avorion_refresh_thread, NULL, NULL);
    if (video_tid == NULL){
    	HcuErrorPrint("AVORION: Cann't create thread!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step7 avorion capture scale set, pSwsCtxAvorion=0x%x\n", pSwsCtxAvorion);
    }

	/*
	 * STEP8: 初始化输出流和输出文件
	 *
	 */
    //Method1.
    pFormatCtxOutput = avformat_alloc_context();
    //Guess Format
    ofmt = av_guess_format(NULL, fdir, NULL);
    if (ofmt == NULL){
        HcuErrorPrint("AVORION: Failed to guess file format! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    pFormatCtxOutput->oformat = ofmt;
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.1 File output encoder FORMAT context generation , pFormatCtxOutput=0x%x, ofmt=0x%x\n", pFormatCtxOutput, ofmt);
    }

    //Method 2.
    //avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, fdir);
    //fmt = pFormatCtx->oformat;
    //Open output file, 并赋予给输出流容量
    if (avio_open(&pFormatCtxOutput->pb, fdir, AVIO_FLAG_READ_WRITE) < 0){
        HcuErrorPrint("AVORION: Failed to open output file! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //视频输出流
    video_st_output = avformat_new_stream(pFormatCtxOutput, 0);
    if (video_st_output==NULL){
    	HcuErrorPrint("AVORION: Failed to create video output stream! \n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    video_st_output->time_base.num = 1;
    video_st_output->time_base.den = zHcuAvorionRefreshRate;

    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.2 File output encoder stream creation , video_st_output=0x%x\n", video_st_output);
    }

    //Parameter that must set
    pCodecCtxOutput = video_st_output->codec;
    //pCodecCtxOutput->codec_id =AV_CODEC_ID_HEVC;
    pCodecCtxOutput->codec_id = ofmt->video_codec;
    pCodecCtxOutput->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtxOutput->pix_fmt = AVORION_STREAM_PIX_FMT_DEFAULT;
    //从输入源得到解析度，而不是固定设死
    pCodecCtxOutput->width = pCodecCtxAvorion->width;//in_w;
    pCodecCtxOutput->height = pCodecCtxAvorion->height;//in_h;
    pCodecCtxOutput->time_base.num = 1;
    pCodecCtxOutput->time_base.den = zHcuAvorionRefreshRate;
    pCodecCtxOutput->bit_rate = 400000;
    pCodecCtxOutput->gop_size=250; // emit one intra frame every gop_size frames at most
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.3 File output encoder context , pCodecCtxOutput=0x%x, width=%d, height=%d, fresh rate=%d\n", pCodecCtxOutput, pCodecCtxOutput->width, pCodecCtxOutput->height, pCodecCtxOutput->time_base.den);
    }

    //H264
    //pCodecCtxOutput->me_range = 16;
    //pCodecCtxOutput->max_qdiff = 4;
    //pCodecCtxOutput->qcompress = 0.6;
    pCodecCtxOutput->qmin = 10;
    pCodecCtxOutput->qmax = 51;
    //Optional Param
    pCodecCtxOutput->max_b_frames=3;
    // Set Option
    AVDictionary *param = 0;
    //H.264
    if(pCodecCtxOutput->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
        //av_dict_set(&param, "profile", "main", 0);
    }
    //H.265
    else if(pCodecCtxOutput->codec_id == AV_CODEC_ID_H265){
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }
    //avi format
    else{
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }
    //Show some Information
    av_dump_format(pFormatCtxOutput, 0, fdir, 1);
    //Context create
    pCodecOutput = avcodec_find_encoder(pCodecCtxOutput->codec_id);
    if (!pCodecOutput){
        HcuErrorPrint("AVORION: Can not find encoder! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    if (avcodec_open2(pCodecCtxOutput, pCodecOutput, &param) < 0){
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        HcuErrorPrint("AVORION: Failed to open encoder! \n");
        return FAILURE;
    }
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.4 File output encoder create , pCodecOutput=0x%x\n", pCodecOutput);
    }

    //Fill pFrame
    pFrameOutput = av_frame_alloc();
    if (pFrameOutput == NULL){
        HcuErrorPrint("AVORION: Failed to allocate frame! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    pFrameOutput->width = pCodecCtxOutput->width;
    pFrameOutput->height = pCodecCtxOutput->height;
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.5 File output encoder frame pFrameOutput=0x%x, width=%d, height=%d\n", pFrameOutput, pFrameOutput->width, pFrameOutput->height);
    }

    picture_size = avpicture_get_size(pCodecCtxOutput->pix_fmt, pCodecCtxOutput->width, pCodecCtxOutput->height);
    bufferOutput = (UINT8 *)av_malloc(picture_size);
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.6 File output encoder output malloc size=%d, bufferOutput=0x%x\n", picture_size, bufferOutput);
    }

    avpicture_fill((AVPicture *)pFrameOutput, bufferOutput, pCodecCtxOutput->pix_fmt, pCodecCtxOutput->width, pCodecCtxOutput->height);
    pFrameOutput->format = AVORION_STREAM_PIX_FMT_DEFAULT;
    //Write File Header
    avformat_write_header(pFormatCtxOutput, NULL);

    /*
     *
     * 本来是以整个帧为单位，试试单包
     * 下面的申请内存函数，本来是以av_new_packet为基础，但一直在x86下处于崩溃的状态，换成av_malloc，而且内存少多了，好些，但还会造成avcodec_encode_video2崩溃
     * 当将该内存换成5倍时，不再崩溃，这里放10倍，也就是720BYTE，暂时确保不崩溃，待观察和研究
     *
     */
    //av_new_packet(packetOutput, picture_size);
    packetOutput = (AVPacket *)av_malloc(sizeof(AVPacket));
    //printf("Picture_size=%d, Avpackage = %d\n", picture_size, sizeof(AVPacket)*5);
    y_size = pCodecCtxAvorion->width * pCodecCtxAvorion->height;  //为了从文件中读取一帧一帧，所以计算出该大小
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.7 File output encoder package packetOutput=0x%x\n", packetOutput);
    }

	/*
	 * STEP9: 读取循环处理
	 *
	 */
    //Event Loop
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step9, TS measurement starting, EncodeCnt=%5d\n", zHcuEncodeFrameCnt);
    }
    while(1)
    {
    	SDL_WaitEvent(&event);
        if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			HcuDebugPrint("AVORION: Step9.0 starting to work, WaitEven coms!\n");
        }
    	if(event.type==AVORION_SFM_REFRESH_EVENT)
    	{
    		if(av_read_frame(pFormatCtxAvorion, packetAvorion) >= 0)
    		{
    			if (packetAvorion->stream_index == videoStreamAvorion)
    			{
    				zHcuAvorionFrameCnt++; //帧号从1开始计数

    				// Decode video frame, 把包转化为帧
    				ret = avcodec_decode_video2(pCodecCtxAvorion, pFrameAvorion, &frameFinishedAvorion, packetAvorion);
    				if(ret < 0)
                    {
                        HcuErrorPrint("AVORION: Decode Error.\n");
                        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
                        return FAILURE;
                    }
    				if (frameFinishedAvorion)
        			{
        				//设置取景框大小
                        sws_scale(pSwsCtxAvorion,
                        		(const uint8_t* const*)pFrameAvorion->data,
								pFrameAvorion->linesize, 0,
								pCodecCtxAvorion->height,
								pFrameYUVmid->data,
								pFrameYUVmid->linesize);
                        if (AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT == TRUE)
                        {
                            //SDL---------------------------
                            //更新纹理
                            SDL_UpdateTexture(sdlTexture, NULL, pFrameYUVmid->data[0], pFrameYUVmid->linesize[0] );
                            //清空渲染器
                            SDL_RenderClear(sdlRenderer );
                            //纹理设置到渲染器中，选择NULL和sldRect，使用效果上没看出差别，缺省选择NULL
                            //SDL_RenderCopy(sdlRenderer, sdlTexture, &sdlRect, &sdlRect );
                            //将纹理设置到渲染器中
                            SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
                            //显示数据到输出屏幕上
                            SDL_RenderPresent(sdlRenderer );
                            //SDL End-----------------------
                        }
						/*
						 * 存入文件
						 * 读取文件的访问处理，这里先不用，但它是极好的参考，暂时留在这里
						 * 在视频处理中，该存储会极大的拖累处理速度，反正是为了测试目的，当测试完成后，这部分就不用了，留着未来的测试用途吧
						 *
						 */
                        //RGB
                        if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_RGB24){
                        	fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)*3, 1 ,filePtrOutputYUVmid);
                        }
                        //UYVY
                        else if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_UYVY422){
                        	fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height), 2, filePtrOutputYUVmid);
                        }
                        //YUV420P
                        else if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_YUV420P){
                            fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height), 1, filePtrOutputYUVmid);    //Y
                            fwrite(pFrameYUVmid->data[1],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)/4, 1, filePtrOutputYUVmid);  //U
                            fwrite(pFrameYUVmid->data[2],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)/4, 1, filePtrOutputYUVmid);  //V
                        }
                        else{
                        	//Do nothing
                        }
                        //此处的打印，帮助调试文件写入的进度
                        if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
							HcuDebugPrint("AVORION: Step9.1 Success write to file, packetAvorion=0x%x\t,  AvorionFrameCnt=%5d\n", packetAvorion, zHcuAvorionFrameCnt);
                        }

						/*
						 * 进行存入第二种视频流的处理
						 * 读取文件的访问处理，这里先不用，但它是极好的参考，暂时留在这里
						 *
						 */
                        //Read raw YUV data, 架设数据是以YUV的格式存在文件中
                        if (AVORION_INPUT_FROM_DEVICE_OR_FILE == TRUE)
                        {
    						//如果获取的中间格式YUV跟输出数据不一致，则需要做数据变换，并确认是好使的
                            pFrameOutput->data[0] = pFrameYUVmid->data[0];    // Y
                            pFrameOutput->data[1] = pFrameYUVmid->data[1];    // U
                            pFrameOutput->data[2] = pFrameYUVmid->data[2];    // V
                        }
                        else
                        {
                            if (fread(bufferOutput, 1, y_size*3/2, in_file) <= 0){
                                HcuErrorPrint("Failed to read raw data! \n");
                                zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
                                return FAILURE;
                            }else if(feof(in_file)){
                                break;
                            }
                            pFrameOutput->data[0] = bufferOutput;              // Y
                            pFrameOutput->data[1] = bufferOutput+ y_size;      // U
                            pFrameOutput->data[2] = bufferOutput+ y_size*5/4;  // V
                        }

                        //PTS => 帧号
                        pFrameOutput->pts = zHcuEncodeFrameCnt;
                        int got_package=0;

                        /*
                         * Encode
                         * 通过前后打印得知，avcodec_encode_video2编码函数是造成程序时不时崩溃的根源，是packageOutput的内存越界，申请的长度不够
                         * 现在暂时将其设为10倍，规避该问题，由于av_new_packet(packetOutput, picture_size)一直失败，原因不得而知，未来待查阅
                         * 最终发现，还是Package没有正常的被初始化，导致崩溃
                         *
                         */
                        //入参只有pFrameOutput
                        //printf("AVORION: Start: pCodecCtxOutput=0x%x, packetOutput=0x%x, pFrameOutput=0x%x, got_package=0x%x\n", (unsigned int)pCodecCtxOutput, (unsigned int)packetOutput, (unsigned int)pFrameOutput, (unsigned int)&got_package);

                        av_init_packet(packetOutput);
                        packetOutput->data = NULL;    // packet data will be allocated by the encoder
                        packetOutput->size = 0;

                        ret = avcodec_encode_video2(pCodecCtxOutput, packetOutput, pFrameOutput, &got_package);
                        if(ret < 0){
                            HcuErrorPrint("AVORION: Failed to encode! \n");
                            zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
                            return FAILURE;
                        }
                        //HcuDebugPrint("AVORION: avcodec_encode_video2 done.\n");

                        if (got_package==1){
                        	//这里的打印留着，对视频性能的影响极小，经过时间戳对比测试。反而是SDLDISPLAY部分，对系统性能影响较大
                        	zHcuEncodeFrameCnt++;
                            if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
								HcuDebugPrint("AVORION: Step9.2 Success to encode frame, packageOutput=0x%x\t, EncodeCnt=%5d\t, size=%5d\t, gotPackFlag=%d\n", packetOutput, zHcuEncodeFrameCnt, packetOutput->size, got_package);
                            }
                        	packetOutput->stream_index = video_st_output->index;
                            ret = av_write_frame(pFormatCtxOutput, packetOutput);
                            if (ret == FAILURE){
                            	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
                            	HcuErrorPrint("AVORION: Failed to write into file!\n");
                            	return FAILURE;
                            }
                            if (packetOutput !=NULL) av_free_packet(packetOutput);
                        }

        			}//FrameFinished
    			}//==videoStream
    			else{
                    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
    					HcuDebugPrint("AVORION: Step9.3 packetAvorion->stream_index != videoStreamAvorion\n");
                    }
    			}
    			//释放帧
    			if (packetAvorion !=NULL) av_free_packet(packetAvorion);
    			//if (packetOutput !=NULL) av_free_packet(packetOutput);  //第二个文件写入的释放，放在这里比较好，暂时放在上面，等待以后再优化
    		}//av_read_frame
    		else{
                //Exit Thread
                if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
                	HcuDebugPrint("AVORION: Step9.4 av_read_frame(pFormatCtxAvorion, packetAvorion) < 0\n");
                }
    			zHcuAvorionRefreshThreadExitFlag=1;
                break;
    		}
    	} //End of SFM_REFRESH_EVENT
    	else if(event.type==SDL_QUIT){
            if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
            	HcuDebugPrint("AVORION: Step9.5 event.type==SDL_QUIT, event.type = %d\n", event.type);
            }
    		zHcuAvorionRefreshThreadExitFlag = 1;
    	    break;
    	}
    }//End of while(1）

	/*
	 * STEP10: 处理输出流的尾巴
	 *
	 */
    //Flush Encoder
    ret = func_avorion_flush_encoder(pFormatCtxOutput,0);
    if (ret < 0) {
        HcuErrorPrint("AVORION: Flushing encoder failed\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //Write file trailer
    av_write_trailer(pFormatCtxOutput);

    //通过前后两边的时间戳掐精度，100秒/2500帧的测试颗粒度上，大约在2.2%的水平，已经很不错了
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
		HcuDebugPrint("AVORION: Step10, TS measurement End, EncodeCnt=%5d\n", zHcuEncodeFrameCnt);
    }

	/*
	 * STEP11: 清理环境和各个指针
	 *
	 */
	//Clean
    if (pSwsCtxAvorion !=NULL) sws_freeContext(pSwsCtxAvorion);
    fclose(filePtrOutputYUVmid);
    //if (AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT == TRUE){
        SDL_Quit();
    //}
    if (bufferAvorion !=NULL) av_free(bufferAvorion);
    if (pFrameYUVmid !=NULL) av_free(pFrameYUVmid);
    if (pFrameAvorion !=NULL) av_free(pFrameAvorion);
	avcodec_close(pCodecCtxAvorion);
    // Close the video file
	avformat_close_input(&pFormatCtxAvorion); //双指针

	//clean output file and stream
    if (video_st_output){
        avcodec_close(video_st_output->codec);
        if (pFrameOutput !=NULL) av_free(pFrameOutput);
        if (bufferOutput !=NULL) av_free(bufferOutput);
    }
    avio_close(pFormatCtxOutput->pb);
    if (pFormatCtxOutput !=NULL) avformat_free_context(pFormatCtxOutput);

    //fclose(in_file);

	return SUCCESS;
}

//独立的线程
INT32 func_avorion_refresh_thread(void *opaque)
{
	INT32 delay_second = 0;
	//保护性参数，使得刷新率具备实际意义
	delay_second = (INT32)1000/zHcuAvorionRefreshRate;
	if (delay_second < 15) delay_second = 10;
	if (delay_second > 200) delay_second = 200;

	//保护连续工作的时间长度和帧数量，防止打爆硬盘和存储设备
	if (zHcuAvorionFrameTotalNum > AVORION_STREAM_NB_FRAMES_MAX)
	{
		zHcuAvorionFrameTotalNum = AVORION_STREAM_NB_FRAMES_MAX;
	}

	//条件工作
    while (zHcuAvorionRefreshThreadExitFlag==0)
    {
        SDL_Event event;
        zHcuAvorionFrameRefreshCnt++;
        if (zHcuAvorionFrameRefreshCnt > zHcuAvorionFrameTotalNum)
        {
        	event.type = SDL_QUIT;
        }
        else
        {
        	event.type = AVORION_SFM_REFRESH_EVENT;
        }
        if (event.type != SDL_QUIT){
            SDL_PushEvent(&event);
        }else if ((event.type == SDL_QUIT) && (zHcuSdlPushOrNot == FALSE)){
            SDL_PushEvent(&event);
            zHcuSdlPushOrNot = TRUE;
        }
        SDL_Delay(delay_second);
    }
    return SUCCESS;
}

#else
OPSTAT func_avorion_ffmpeg_capture_and_save(UINT8 fileType, char *fdir, char *ftout)
{
	//输入视频控制参数（包括潜在的输入数据文件）
	AVFormatContext *pFormatCtxAvorion; 	//输入输出的容器，但不能同时作为输入和输出的容器
	AVCodecContext *pCodecCtxAvorion;   	//输入视频解码器上下文
	AVCodec *pCodecAvorion;    			//输入视频解码器，是由链表组成的，链表的类型是AVCodec
	AVFrame *pFrameAvorion;    			//输入视频帧，用来保存数据缓存的对象
	AVFrame *pFrameYUVmid; 				//输入视频采集得到的中间帧，视频缓存对象
	AVPacket *packetAvorion;    			//输入视频数据包，主要记录音视频数据帧，时钟信息和压缩数据首地址，大小等信息
	AVInputFormat *ifmt; 				//输入音视频输入格式
	struct SwsContext *pSwsCtxAvorion;  	//视频分辨率，色彩空间变换时所需要的上下文句柄
	//char VIDEO_INPUT[]="/dev/video0";	//测试时固定输入摄像头的物理标识，直接应用外部定义的摄像头
	INT32 videoStreamAvorion=0;			//视频输入
	INT32 frameFinishedAvorion=0;		//视频输入
	INT32 numBytesAvorion=0;				//视频输入
	UINT8 *bufferAvorion;				//视频输入

	//中间临时输出文件之一的控制参数，以没有经过编码的YUV为格式
	//目前留着这个纯粹是为了测试和对比目的，未来可以去掉
	//为了进程的相互独立，该参数移到L3了
	//const char* fileNameOutputYUVmid = "output.yuv";
	FILE *filePtrOutputYUVmid;

	//输出视频流控制参数（包括主输出文件）
    AVFormatContext* pFormatCtxOutput;  //输出视频的容器
    AVOutputFormat* ofmt;				//输出视频的数据格式
    AVStream* video_st_output;			//输出视频数据输出流
    AVCodecContext* pCodecCtxOutput;	//输出视频编码器上下文
    AVCodec* pCodecOutput;				//输出视频编码器
    AVPacket* packetOutput;				//输出视频编码包
    AVFrame* pFrameOutput;				//输出视频帧
	UINT8* bufferOutput;				//输出视频缓冲存储区
    INT32 picture_size = 0;				//输出视频的包缓冲区申请长度

    //来源于输入文件，直接读取并转换为中间数据
    int y_size;
	FILE *in_file = fopen("src01_480x272.yuv", "rb");   //Input raw YUV data
    //int in_w=640, in_h=480;                              //Input data's width and height
    //char out_file[]="./test01.h264";              	//Output Filepath

	//临时控制参数
	INT32 i=0, ret=0;

	/*
	 * STEP1: 初始化全体对象
	 *
	 */
	//Register all devices

	//初始化计数器
    zHcuAvorionFrameCnt = 0; //Incoming source frame
    zHcuAvorionFrameRefreshCnt = 0; //Refresh frame
    zHcuEncodeFrameCnt = 0; //Output frame
    zHcuAvorionRefreshThreadExitFlag = 0; //Thread control counter
    zHcuSdlPushOrNot = FALSE; //防止SDL_QUIT重复PUSH，目前还未找到FLUSH SDL队列的方式

    //初始化系统
	av_register_all();
	avformat_network_init();
    avdevice_register_all();

    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step1 Global init success!\n");
    }

	/*
	 * STEP2: 打开输入设备或者文件或者内存 =>pFormatCtxAvorion
	 *
	 */
    //输入文件格式
	//const char *filename = "/home/hitpony/workspace/HelloWorld/Debug/test.avi";
	//读取输入源的格式，并且把信息保存到pFormatCtx
    pFormatCtxAvorion=avformat_alloc_context();
	/*INT32 result=0;
	char s[100];
	result = avformat_open_input(&pFormatCtx, filename, NULL, NULL);  //双指针
	strcpy(s, strerror(errno));
	if (result!= 0)	return FAILURE;*/
	ifmt=av_find_input_format("video4linux2");
    if(avformat_open_input(&pFormatCtxAvorion, HCU_DEFAULT_DEVICE_USB_CAMERA0, ifmt, NULL)!=0){
        HcuErrorPrint("AVORION: Couldn't open input stream.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
	//负责为pFormatCtx->streams填上正确的信息,pFormatCtx->streams仅仅是一组大小为pFormatCtx->nb_streams的指针
	//av_dump_format(pFormatCtxAvorion, 0, filename, 0);
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step2 input avorion source context creation, pFormatCtxAvorion=0x%x\n", pFormatCtxAvorion);
    }

	/*
	 * STEP3: 找到输入流 =>videoStreamAvorion
	 *
	 */
	//从文件的头部，检查文件中的流的信息
	if (avformat_find_stream_info(pFormatCtxAvorion, NULL) < 0){
		HcuErrorPrint("AVORION: Couldn't find stream information.\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Couldn't find stream information
	}
	// Find the first video stream
	videoStreamAvorion = -1;
	for (i = 0; i < pFormatCtxAvorion->nb_streams; i++)
	{
		if (pFormatCtxAvorion->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamAvorion = i;
			break;
		}
	}
	if (videoStreamAvorion == -1){
        HcuErrorPrint("AVORION: Couldn't find a video stream.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Didn't find a video stream
	}
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step3 input avorion source stream finding, videoStreamAvorion=%d\n", videoStreamAvorion);
    }

	/*
	 * STEP4: 找到解码器 =>pCodecCtxAvorion
	 *
	 */
	// 得到解码器的上下文信息
	pCodecCtxAvorion = pFormatCtxAvorion->streams[videoStreamAvorion]->codec;
	// Find the decoder for the video stream
	pCodecAvorion = avcodec_find_decoder(pCodecCtxAvorion->codec_id);
	if (pCodecAvorion == NULL) {
		HcuErrorPrint(strerror(errno), "AVORION: Unsupported codec!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Codec not found
	}
	// Open codec
	if (avcodec_open2(pCodecCtxAvorion, pCodecAvorion, NULL) < 0){
        HcuErrorPrint("AVORION: Could not open codec.\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE; // Could not open codec
	}
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step4 input avorion source decoder pCodecCtxAvorion=0x%x， pCodecAvorion=0x%x\n", pCodecCtxAvorion, pCodecAvorion);
    }

	/*
	 * STEP5: 为解码器以及中间YUV帧申请FRAME对象
	 *
	 */
	// Allocate video frame 该函数负责申请内存, 注意:初始化的时候AVFrame中的元素data, linesize均为空。未指向任何内存数据
	pFrameAvorion = av_frame_alloc(); //avcodec_alloc_frame();
	// Allocate an AVFrame structure
	pFrameYUVmid = av_frame_alloc(); //avcodec_alloc_frame();
	if ((pFrameYUVmid == NULL) || (pFrameAvorion == NULL)){
		HcuErrorPrint("AVORION: Not support frame allocation!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}
	// 计算AVCodeContext缓冲区的大小和申请空间
	numBytesAvorion = avpicture_get_size(AVORION_STREAM_PIX_FMT_DEFAULT, pCodecCtxAvorion->width, pCodecCtxAvorion->height);  //PIX_FMT_YUV420P
	//申请内存对齐, 但不保证内存泄漏
	bufferAvorion = (UINT8 *) av_malloc(numBytesAvorion * sizeof(UINT8));
	//给pFrameYUVmid分配内存, 把帧和pFrameYUVmid关联起来
	avpicture_fill((AVPicture *)pFrameYUVmid, bufferAvorion, AVORION_STREAM_PIX_FMT_DEFAULT, pCodecCtxAvorion->width, pCodecCtxAvorion->height); //PIX_FMT_YUV420P
	//准备按照包为步长进行读取和处理
	packetAvorion = (AVPacket *)av_malloc(sizeof(AVPacket));
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step5 Avorion/YUV malloc size=%d, bufferAvorion=0x%x， packetAvorion=0x%x\n", numBytesAvorion, bufferAvorion, packetAvorion);
    }

	/*
	 * STEP6: SDL1.2输出屏幕初始化
	 * 视频库已经换成SDL2.0了，这部分不再有用了，放在这里只是作为对比参考，可以看出，SDL1.2和SDL2.0对视频的操作函数及流程差异极大
	 *
	 */


	/*
	 * STEP6.5: SDL2.0输出屏幕初始化
	 *
	 */


	/*
	 * STEP7: 初始化输出文件之一和视频取景框
	 *
	 */
	//初始化文件
	filePtrOutputYUVmid = fopen(ftout, "wb+");
    if (filePtrOutputYUVmid == NULL){
    	HcuErrorPrint("AVORION: Cann't open temp-mid output file!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //视频取景框的设定：负责得到视频分辨率，色彩控件变换时的上下文句柄, 获取渲染的句柄
    pSwsCtxAvorion = sws_getContext(pCodecCtxAvorion->width,
    		pCodecCtxAvorion->height,
			pCodecCtxAvorion->pix_fmt,
    		pCodecCtxAvorion->width,
			pCodecCtxAvorion->height,
			AVORION_STREAM_PIX_FMT_DEFAULT,
			SWS_BICUBIC, NULL, NULL, NULL);
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step7 avorion capture scale set, pSwsCtxAvorion=0x%x\n", pSwsCtxAvorion);
    }

	/*
	 * STEP8: 初始化输出流和输出文件
	 *
	 */
    //Method1.
    pFormatCtxOutput = avformat_alloc_context();
    //Guess Format
    ofmt = av_guess_format(NULL, fdir, NULL);
    if (ofmt == NULL){
        HcuErrorPrint("AVORION: Failed to guess file format! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    pFormatCtxOutput->oformat = ofmt;
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.1 File output encoder FORMAT context generation , pFormatCtxOutput=0x%x, ofmt=0x%x\n", pFormatCtxOutput, ofmt);
    }

    //Method 2.
    //avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, fdir);
    //fmt = pFormatCtx->oformat;
    //Open output file, 并赋予给输出流容量
    if (avio_open(&pFormatCtxOutput->pb, fdir, AVIO_FLAG_READ_WRITE) < 0){
        HcuErrorPrint("AVORION: Failed to open output file! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //视频输出流
    video_st_output = avformat_new_stream(pFormatCtxOutput, 0);
    if (video_st_output==NULL){
    	HcuErrorPrint("AVORION: Failed to create video output stream! \n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    video_st_output->time_base.num = 1;
    video_st_output->time_base.den = zHcuAvorionRefreshRate;

    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.2 File output encoder stream creation , video_st_output=0x%x\n", video_st_output);
    }

    //Parameter that must set
    pCodecCtxOutput = video_st_output->codec;
    //pCodecCtxOutput->codec_id =AV_CODEC_ID_HEVC;
    pCodecCtxOutput->codec_id = ofmt->video_codec;
    pCodecCtxOutput->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtxOutput->pix_fmt = AVORION_STREAM_PIX_FMT_DEFAULT;
    //从输入源得到解析度，而不是固定设死
    pCodecCtxOutput->width = pCodecCtxAvorion->width;//in_w;
    pCodecCtxOutput->height = pCodecCtxAvorion->height;//in_h;
    pCodecCtxOutput->time_base.num = 1;
    pCodecCtxOutput->time_base.den = zHcuAvorionRefreshRate;
    pCodecCtxOutput->bit_rate = 400000;
    pCodecCtxOutput->gop_size=250; // emit one intra frame every gop_size frames at most
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.3 File output encoder context , pCodecCtxOutput=0x%x, width=%d, height=%d, fresh rate=%d\n", pCodecCtxOutput, pCodecCtxOutput->width, pCodecCtxOutput->height, pCodecCtxOutput->time_base.den);
    }

    //H264
    //pCodecCtxOutput->me_range = 16;
    //pCodecCtxOutput->max_qdiff = 4;
    //pCodecCtxOutput->qcompress = 0.6;
    pCodecCtxOutput->qmin = 10;
    pCodecCtxOutput->qmax = 51;
    //Optional Param
    pCodecCtxOutput->max_b_frames=3;
    // Set Option
    AVDictionary *param = 0;
    //H.264
    if(pCodecCtxOutput->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
        //av_dict_set(&param, "profile", "main", 0);
    }
    //H.265
    else if(pCodecCtxOutput->codec_id == AV_CODEC_ID_H265){
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }
    //avi format
    else{
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }
    //Show some Information
    av_dump_format(pFormatCtxOutput, 0, fdir, 1);
    //Context create
    pCodecOutput = avcodec_find_encoder(pCodecCtxOutput->codec_id);
    if (!pCodecOutput){
        HcuErrorPrint("AVORION: Can not find encoder! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    if (avcodec_open2(pCodecCtxOutput, pCodecOutput, &param) < 0){
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        HcuErrorPrint("AVORION: Failed to open encoder! \n");
        return FAILURE;
    }
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.4 File output encoder create , pCodecOutput=0x%x\n", pCodecOutput);
    }

    //Fill pFrame
    pFrameOutput = av_frame_alloc();
    if (pFrameOutput == NULL){
        HcuErrorPrint("AVORION: Failed to allocate frame! \n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    pFrameOutput->width = pCodecCtxOutput->width;
    pFrameOutput->height = pCodecCtxOutput->height;
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.5 File output encoder frame pFrameOutput=0x%x, width=%d, height=%d\n", pFrameOutput, pFrameOutput->width, pFrameOutput->height);
    }

    picture_size = avpicture_get_size(pCodecCtxOutput->pix_fmt, pCodecCtxOutput->width, pCodecCtxOutput->height);
    bufferOutput = (UINT8 *)av_malloc(picture_size);
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.6 File output encoder output malloc size=%d, bufferOutput=0x%x\n", picture_size, bufferOutput);
    }

    avpicture_fill((AVPicture *)pFrameOutput, bufferOutput, pCodecCtxOutput->pix_fmt, pCodecCtxOutput->width, pCodecCtxOutput->height);
    pFrameOutput->format = AVORION_STREAM_PIX_FMT_DEFAULT;
    //Write File Header
    avformat_write_header(pFormatCtxOutput, NULL);

    /*
     *
     * 本来是以整个帧为单位，试试单包
     * 下面的申请内存函数，本来是以av_new_packet为基础，但一直在x86下处于崩溃的状态，换成av_malloc，而且内存少多了，好些，但还会造成avcodec_encode_video2崩溃
     * 当将该内存换成5倍时，不再崩溃，这里放10倍，也就是720BYTE，暂时确保不崩溃，待观察和研究
     *
     */
    //av_new_packet(packetOutput, picture_size);
    packetOutput = (AVPacket *)av_malloc(sizeof(AVPacket));
    //printf("Picture_size=%d, Avpackage = %d\n", picture_size, sizeof(AVPacket)*5);
    y_size = pCodecCtxAvorion->width * pCodecCtxAvorion->height;  //为了从文件中读取一帧一帧，所以计算出该大小
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("AVORION: Step8.7 File output encoder package packetOutput=0x%x\n", packetOutput);
    }

	/*
	 * STEP9: 读取循环处理
	 *
	 */
    //Event Loop
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step9, TS measurement starting, EncodeCnt=%5d\n", zHcuEncodeFrameCnt);
    }

	INT32 delay_second = 0;
	//保护性参数，使得刷新率具备实际意义
	delay_second = (INT32)1000/zHcuAvorionRefreshRate;
	if (delay_second < 15) delay_second = 10;
	if (delay_second > 200) delay_second = 200;

	//保护连续工作的时间长度和帧数量，防止打爆硬盘和存储设备
	if (zHcuAvorionFrameTotalNum > AVORION_STREAM_NB_FRAMES_MAX)
	{
		zHcuAvorionFrameTotalNum = AVORION_STREAM_NB_FRAMES_MAX;
	}
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step9.0 starting to work, WaitEven comes!\n");
    }

	for(zHcuAvorionFrameCnt = 0; zHcuAvorionFrameCnt < zHcuAvorionFrameTotalNum; zHcuAvorionFrameCnt++)
	{
		hcu_usleep(delay_second);
		if(av_read_frame(pFormatCtxAvorion, packetAvorion) >= 0)
		{
			if (packetAvorion->stream_index == videoStreamAvorion)
			{
				//zHcuAvorionFrameCnt++; //帧号从1开始计数

				// Decode video frame, 把包转化为帧
				ret = avcodec_decode_video2(pCodecCtxAvorion, pFrameAvorion, &frameFinishedAvorion, packetAvorion);
				if(ret < 0)
				{
					HcuErrorPrint("AVORION: Decode Error.\n");
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
					return FAILURE;
				}
				if (frameFinishedAvorion)
				{
					//设置取景框大小
					sws_scale(pSwsCtxAvorion,
							(const uint8_t* const*)pFrameAvorion->data,
							pFrameAvorion->linesize, 0,
							pCodecCtxAvorion->height,
							pFrameYUVmid->data,
							pFrameYUVmid->linesize);

					/*
					 * 存入文件
					 * 读取文件的访问处理，这里先不用，但它是极好的参考，暂时留在这里
					 * 在视频处理中，该存储会极大的拖累处理速度，反正是为了测试目的，当测试完成后，这部分就不用了，留着未来的测试用途吧
					 *
					 */
					//RGB
					if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_RGB24){
						fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)*3, 1 ,filePtrOutputYUVmid);
					}
					//UYVY
					else if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_UYVY422){
						fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height), 2, filePtrOutputYUVmid);
					}
					//YUV420P
					else if (AVORION_OUTPUT_FILE_FORMAT == AVORION_OUTPUT_FILE_FORMAT_YUV420P){
						fwrite(pFrameYUVmid->data[0],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height), 1, filePtrOutputYUVmid);    //Y
						fwrite(pFrameYUVmid->data[1],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)/4, 1, filePtrOutputYUVmid);  //U
						fwrite(pFrameYUVmid->data[2],(pCodecCtxAvorion->width)*(pCodecCtxAvorion->height)/4, 1, filePtrOutputYUVmid);  //V
					}
					else{
						//Do nothing
					}
					//此处的打印，帮助调试文件写入的进度
				    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
						//HcuDebugPrint("AVORION: Step9.1 Success write to file, packetAvorion=0x%x\t,  AvorionFrameCnt=%5d\n", packetAvorion, zHcuAvorionFrameCnt);
				    }

					/*
					 * 进行存入第二种视频流的处理
					 * 读取文件的访问处理，这里先不用，但它是极好的参考，暂时留在这里
					 *
					 */
					//Read raw YUV data, 架设数据是以YUV的格式存在文件中
					if (AVORION_INPUT_FROM_DEVICE_OR_FILE == TRUE)
					{
						//如果获取的中间格式YUV跟输出数据不一致，则需要做数据变换，并确认是好使的
						pFrameOutput->data[0] = pFrameYUVmid->data[0];    // Y
						pFrameOutput->data[1] = pFrameYUVmid->data[1];    // U
						pFrameOutput->data[2] = pFrameYUVmid->data[2];    // V
					}
					else //在FOR循环中以下的逻辑还有些问题，未来待完善
					{
						if (fread(bufferOutput, 1, y_size*3/2, in_file) <= 0){
							HcuErrorPrint("Failed to read raw data! \n");
							zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
							return FAILURE;
						}else if(feof(in_file)){
							break;
						}
						pFrameOutput->data[0] = bufferOutput;              // Y
						pFrameOutput->data[1] = bufferOutput+ y_size;      // U
						pFrameOutput->data[2] = bufferOutput+ y_size*5/4;  // V
					}

					//PTS => 帧号
					pFrameOutput->pts = zHcuEncodeFrameCnt;
					int got_package=0;

					/*
					 * Encode
					 * 通过前后打印得知，avcodec_encode_video2编码函数是造成程序时不时崩溃的根源，是packageOutput的内存越界，申请的长度不够
					 * 现在暂时将其设为10倍，规避该问题，由于av_new_packet(packetOutput, picture_size)一直失败，原因不得而知，未来待查阅
					 * 最终发现，还是Package没有正常的被初始化，导致崩溃
					 *
					 */
					//入参只有pFrameOutput
					//printf("AVORION: Start: pCodecCtxOutput=0x%x, packetOutput=0x%x, pFrameOutput=0x%x, got_package=0x%x\n", (unsigned int)pCodecCtxOutput, (unsigned int)packetOutput, (unsigned int)pFrameOutput, (unsigned int)&got_package);

					av_init_packet(packetOutput);
					packetOutput->data = NULL;    // packet data will be allocated by the encoder
					packetOutput->size = 0;

					ret = avcodec_encode_video2(pCodecCtxOutput, packetOutput, pFrameOutput, &got_package);
					if(ret < 0){
						HcuErrorPrint("AVORION: Failed to encode! \n");
						zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
						return FAILURE;
					}
					//HcuDebugPrint("AVORION: avcodec_encode_video2 done.\n");

					if (got_package==1){
						//这里的打印留着，对视频性能的影响极小，经过时间戳对比测试。反而是SDLDISPLAY部分，对系统性能影响较大
						zHcuEncodeFrameCnt++;
					    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
							//HcuDebugPrint("AVORION: Step9.2 Success to encode frame, packageOutput=0x%x\t, EncodeCnt=%5d\t, size=%5d\t, gotPackFlag=%d\n", packetOutput, zHcuEncodeFrameCnt, packetOutput->size, got_package);
					    }
						packetOutput->stream_index = video_st_output->index;
						ret = av_write_frame(pFormatCtxOutput, packetOutput);
						if (ret == FAILURE){
							zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
							HcuErrorPrint("AVORION: Failed to write into file!\n");
							return FAILURE;
						}
						if (packetOutput !=NULL) av_free_packet(packetOutput);
					}

				}//FrameFinished
			}//==videoStream
			else{
				 if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
					 HcuDebugPrint("AVORION: Step9.3 packetAvorion->stream_index != videoStreamAvorion\n");
				 }
			}
			//释放帧
			if (packetAvorion !=NULL) av_free_packet(packetAvorion);
			//if (packetOutput !=NULL) av_free_packet(packetOutput);  //第二个文件写入的释放，放在这里比较好，暂时放在上面，等待以后再优化
		}//av_read_frame
		else{
			//Exit Thread
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
				HcuDebugPrint("AVORION: Step9.4 av_read_frame(pFormatCtxAvorion, packetAvorion) < 0\n");
			}
			zHcuAvorionRefreshThreadExitFlag=1;
			break;
		}
	} //End of FOR

	/*
	 * STEP10: 处理输出流的尾巴
	 *
	 */
    //Flush Encoder
    ret = func_avorion_flush_encoder(pFormatCtxOutput,0);
    if (ret < 0) {
        HcuErrorPrint("AVORION: Flushing encoder failed\n");
        zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
        return FAILURE;
    }
    //Write file trailer
    av_write_trailer(pFormatCtxOutput);

    //通过前后两边的时间戳掐精度，100秒/2500帧的测试颗粒度上，大约在2.2%的水平，已经很不错了
    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
    	HcuDebugPrint("AVORION: Step10, TS measurement End, EncodeCnt=%5d\n", zHcuEncodeFrameCnt);
    }


	/*
	 * STEP11: 清理环境和各个指针
	 *
	 */
	//Clean
    if (pSwsCtxAvorion !=NULL) sws_freeContext(pSwsCtxAvorion);
    fclose(filePtrOutputYUVmid);
    if (bufferAvorion !=NULL) av_free(bufferAvorion);
    if (pFrameYUVmid !=NULL) av_free(pFrameYUVmid);
    if (pFrameAvorion !=NULL) av_free(pFrameAvorion);
	avcodec_close(pCodecCtxAvorion);
    // Close the video file
	avformat_close_input(&pFormatCtxAvorion); //双指针

	//clean output file and stream
    if (video_st_output){
        avcodec_close(video_st_output->codec);
        if (pFrameOutput !=NULL) av_free(pFrameOutput);
        if (bufferOutput !=NULL) av_free(bufferOutput);
    }
    avio_close(pFormatCtxOutput->pb);
    if (pFormatCtxOutput !=NULL) avformat_free_context(pFormatCtxOutput);

    //fclose(in_file);


	/*
	 * STEP12: convert h264 to mp4
	 */
	AVOutputFormat *Ofmt = NULL;
	AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL,*ofmt_ctx = NULL;
	AVPacket pkt;
	//int ret, i;
	int videoindex_v=-1,videoindex_out=-1;
	int frame_index=0;
	//int64_t cur_pts_v=0;

	//const char *fdir = "/home/pi/workspace/hcu/RasberryPi/log/201607/avorion201606261823.h264";

	char out_filename[MAX_AVORION_FILE_LENGTH];
	memset(out_filename, 0, sizeof(out_filename));
	strcpy(out_filename, fdir);
	char outfiletype[5] = ".mp4";
	strcat(out_filename,outfiletype);

	//const char *out_filename = "/home/pi/workspace/hcu/RasberryPi/log/201607/avorion201606261823.h264.mp4";//Output file URL
	av_register_all();
	//Input
	if ((ret = avformat_open_input(&ifmt_ctx_v, fdir, 0, 0)) < 0) {
    	HcuErrorPrint("AVORION: Could not open input file!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {
    	HcuErrorPrint("AVORION: Failed to retrieve input stream information!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		goto end;
	}

	HcuDebugPrint("AVORION: ===========Input Information==========\n");
	av_dump_format(ifmt_ctx_v, 0, fdir, 0);

	HcuDebugPrint("AVORION: ======================================\n");
	//Output
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
	if (!ofmt_ctx) {
    	HcuErrorPrint("AVORION: Could not create output context!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	Ofmt = ofmt_ctx->oformat;

	for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
		//Create output AVStream according to input AVStream
		if(ifmt_ctx_v->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
		AVStream *in_stream = ifmt_ctx_v->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		videoindex_v=i;
		if (!out_stream) {
	    	HcuErrorPrint("AVORION: Failed allocating output stream!\n");
	    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		videoindex_out = out_stream->index;
		//Copy the settings of AVCodecContext
		if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
	    	HcuErrorPrint("AVORION: Failed to copy context from input to output stream codec context!\n");
	    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		break;
		}
	}

	HcuDebugPrint("AVORION: ===========Output Information==========\n");
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	HcuDebugPrint("AVORION: ======================================\n");
	//Open output file
	if (!(Ofmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
	    	HcuErrorPrint("AVORION: Could not open output file '%s'\n", out_filename);
	    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			goto end;
		}
	}
	//Write file header
	if (avformat_write_header(ofmt_ctx, NULL) < 0) {
    	HcuErrorPrint("AVORION: Error occurred when opening output file!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		goto end;
	}


#if USE_H264BSF
	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
	AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

	while (1) {
		AVFormatContext *ifmt_ctx;
		int stream_index=0;
		AVStream *in_stream, *out_stream;

		//Get an AVPacket
			ifmt_ctx=ifmt_ctx_v;
			stream_index=videoindex_out;

			if(av_read_frame(ifmt_ctx, &pkt) >= 0){
				do{
					in_stream  = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];

					if(pkt.stream_index==videoindex_v){
						//FIX No PTS (Example: Raw H.264)
						//Simple Write PTS
						if(pkt.pts==AV_NOPTS_VALUE){
							//Write PTS
							AVRational time_base1=in_stream->time_base;
							//Duration between 2 frames (us)
							int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
							//Parameters
							pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
							pkt.dts=pkt.pts;
							pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
							frame_index++;
						}
//						double cur_pts_v=0;
//						cur_pts_v=pkt.pts;
						break;
					}
				}while(av_read_frame(ifmt_ctx, &pkt) >= 0);
			}else{
				break;
			}
		//FIX:Bitstream Filter
#if USE_H264BSF
		av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
		av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif

		//Convert PTS/DTS
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		pkt.stream_index=stream_index;

	    if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_IPT_ON) != FALSE){
	    	HcuDebugPrint("AVORION: Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size,pkt.pts);
	    }
		//Write
		if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
	    	HcuErrorPrint("AVORION: Error muxing packet!\n");
	    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
			break;
		}
		av_free_packet(&pkt);

	}
	//Write file trailer
	av_write_trailer(ofmt_ctx);

#if USE_H264BSF
	av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
	av_bitstream_filter_close(aacbsfc);
#endif

end:
	avformat_close_input(&ifmt_ctx_v);
	avformat_close_input(&ifmt_ctx_a);
	/* close output */
	if (ofmt_ctx && !(Ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
    	HcuErrorPrint("AVORION: Error occurred!\n");
    	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AVORION]++;
		return FAILURE;
	}


	return SUCCESS;
}

#endif //AVORION_MONITOR_RECT_SDL2_OPEN_OR_NOT


INT32 func_avorion_flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
        CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt, NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame){
            ret=0;
            break;
        }
        zHcuEncodeFrameCnt++;
        if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
			HcuDebugPrint("Flush Encoder: Succeed to encode 1 frame!\t EncodeFrameCnt=%5d\t size=%5d\n", zHcuEncodeFrameCnt, enc_pkt.size);
        }
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}

OPSTAT fsm_avorion_lcd_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_avorion_camera_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_avorion_microphone_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}
