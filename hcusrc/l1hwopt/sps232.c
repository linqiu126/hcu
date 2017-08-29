/*
 * sps232.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#include "../l1hwopt/sps232.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the SPS232
*/
HcuFsmStateItem_t HcuFsmSps232[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_sps232_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_sps232_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,		FSM_STATE_COMMON,            			fsm_sps232_restart},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SPS232_RECEIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SPS232_RECEIVED,   			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SPS232_RECEIVED,   			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
SerialPortCom_t gSerialPortForQrPrinter;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_sps232_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPS232, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SPS232: Error Set FSM State at fsm_sps232_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_sps232_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	int conCounter=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SPS232, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SPS232: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SPS232].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPS232, FSM_STATE_SPS232_INITED) == FAILURE){
		HcuErrorPrint("SPS232: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_sps232_int_init() == FAILURE){
		HcuErrorPrint("SPS232: Error initialize interface!\n");
		return FAILURE;
	}

	//Global variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232] = 0;
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].fVal = HCU_SENSOR_VALUE_NULL;
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].fVal = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPS232, FSM_STATE_SPS232_RECEIVED) == FAILURE){
		HcuErrorPrint("SPS232: Error Set FSM State!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SPS232: Enter FSM_STATE_SPS232_ACTIVED status, Keeping refresh here!\n");
	}

	//串口同时支持两种速率，没有成功，意味着不同的外设具备不同速度时，必须设置树莓派的串口环境速率，然后这里u一次性初始化，而不能单次重复初始化
	//多个串口外设同时打开时，互相斥需要将不工作的传感器关掉，不然它会无谓的占据很多的时间，导致其它正常工作的传感器得不到快速响应

	//这里等待，是为了保证外设在串口初始化后有足够的时间稳定，不然其读数不太正常
	hcu_sleep(60);
	int workingCycle = 2;
	//进入循环工作模式：或者采用定期器启动模式
	while(1){
		conCounter = 0;
		if (zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].present == HCU_SYSCFG_SENSOR_PRESENT_YES){
			func_sps232_read_data_ze08ch2o();
			hcu_sleep(RPI_SPS232_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].present == HCU_SYSCFG_SENSOR_PRESENT_YES){
			func_sps232_read_data_pm25sharp();
			hcu_sleep(RPI_SPS232_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		conCounter = workingCycle-conCounter;
		hcu_sleep(RPI_SPS232_SENSOR_READ_GAP/workingCycle * conCounter);
	}

	return SUCCESS;
}

OPSTAT fsm_sps232_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SPS232: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_sps232_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_sps232_int_init(void)
{
	//初始化硬件接口
	zHcuVmCtrTab.hwinv.sps232.sp.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
	if (zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].present == HCU_SYSCFG_SENSOR_PRESENT_YES) zHcuVmCtrTab.hwinv.sps232.sp.nSpeed = 2400;
	else zHcuVmCtrTab.hwinv.sps232.sp.nSpeed = 9600;
	zHcuVmCtrTab.hwinv.sps232.sp.nBits = 8;
	zHcuVmCtrTab.hwinv.sps232.sp.nEvent = 'N';
	zHcuVmCtrTab.hwinv.sps232.sp.nStop = 1;
	zHcuVmCtrTab.hwinv.sps232.sp.fd = HCU_INVALID_U16;
	zHcuVmCtrTab.hwinv.sps232.sp.vTime = HCU_INVALID_U8;
	zHcuVmCtrTab.hwinv.sps232.sp.vMin = HCU_INVALID_U8;
	zHcuVmCtrTab.hwinv.sps232.sp.c_lflag = 0;

	int ret=0;

	ret = hcu_spsapi_serial_init(&zHcuVmCtrTab.hwinv.sps232.sp);
	if (FAILURE == ret)
	{
		gTaskL3aqycq20Context.eqtStatus.a50001_RS = OFF;
		HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
		return ret;
	}
	else
	{
		HcuDebugPrint("SPS232: Init Serial Port Success ...\n");
	}

	hcu_spsapi_SerialPortSetVtimeVmin(&zHcuVmCtrTab.hwinv.sps232.sp, 10, 5);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  zHcuVmCtrTab.hwinv.sps232.sp.vTime, zHcuVmCtrTab.hwinv.sps232.sp.vMin);
	}

	return SUCCESS;
}

//读取甲醛的数据
OPSTAT func_sps232_read_data_ze08ch2o(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int readCount, hcho, hchoMax, i;
	float hchoSum = 0;
	int nread = 0;
	UINT8 rb[2*RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN+1] = {0}; //2*9+1=19
	UINT8 checksum=0;

	//无限次工作模式打开，效率是比较低下的，但可以多个传感器可以一起兼容工作
	if(RPI_SPS232_SENSOR_SINGLE_PRESENT != RPI_SPS232_SENSOR_SINGLE_PRESENT_TRUE){
		//先关掉串口句柄
		spsapi_SerialPortClose(zHcuVmCtrTab.hwinv.sps232.sp.fd);
		//初始化硬件接口
		zHcuVmCtrTab.hwinv.sps232.sp.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
		zHcuVmCtrTab.hwinv.sps232.sp.nSpeed = 9600;
		zHcuVmCtrTab.hwinv.sps232.sp.nBits = 8;
		zHcuVmCtrTab.hwinv.sps232.sp.nEvent = 'N';
		zHcuVmCtrTab.hwinv.sps232.sp.nStop = 1;
		zHcuVmCtrTab.hwinv.sps232.sp.fd = HCU_INVALID_U16;
		zHcuVmCtrTab.hwinv.sps232.sp.vTime = HCU_INVALID_U8;
		zHcuVmCtrTab.hwinv.sps232.sp.vMin = HCU_INVALID_U8;
		zHcuVmCtrTab.hwinv.sps232.sp.c_lflag = 0;
		if (hcu_spsapi_serial_init(&zHcuVmCtrTab.hwinv.sps232.sp) == FAILURE)
		{
			HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		spsapi_SerialPortSetVtimeVmin(&zHcuVmCtrTab.hwinv.sps232.sp, 10, 5);
		delay(60000);
	}

	//循环读取多次
	hchoSum = 0;
	for (readCount=0; readCount<RPI_SPS232_READ_REPEAT_TIMES; readCount++){
		delay(100);
		//读取数据
		nread = hcu_spsapi_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, rb, (2*RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN));
		if (nread <=0)
		{
			HcuErrorPrint("SPS232: Sensor ZE08CH2O Read data error. nread=%d!\n", nread);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//example: ff 17	//先关掉串口句柄 04 00 01 20 13 88 29 ff
		//寻找帧头
		for (i=0; i<RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN; i++){
			if ((rb[i] == RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD1) || (rb[i] == RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD2))
			break;
		}
		if ((i==RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN) || ((rb[i+RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN]!= RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD1) && (rb[i+RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN]!= RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD2))){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame decode error, can not find frame header. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//计算校验和，判断是否是正确
		checksum = func_sps232_check_sum_ze08ch2o(&rb[i+1], 7);
		if (checksum != rb[i+8]){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame checksum error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//Header of CMMID / UNIT
		if(rb[i+1] != RPI_SPS232_SENSOR_ZE08CH2O_REPORT_CMMID){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame cmdid error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		if(rb[i+2] != RPI_SPS232_SENSOR_ZE08CH2O_REPORT_UNIT){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame UNIT error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//得到最终浓度
		hcho = ((rb[i+4]<<8)&0xFF00) + (rb[i+5]&0xFF);
		hchoMax = ((rb[i+6]<<8)&0xFF00) + (rb[i+7]&0xFF);
		if (hcho > hchoMax){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame received too big data error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//加总到目标SUMMARY
		hchoSum += hcho;
	}

	//求平均
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].fVal = hchoSum / RPI_SPS232_READ_REPEAT_TIMES;
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].updateTimeStamp = time(0);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: Sensor ZE08CH2O Transformed float average read result HCHO = %6.2fppb, around %6.2fug/m3\n", zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].fVal, zHcuVmCtrTab.codab.si[SENSOR_ID_SPSHCHOZE08CH2O].fVal*1.295);
	} //ppb转化为ug/m3，使用了1.295g/L的空气密度，只能算是近似。

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//读取PM25SHARP传感器的串口数据
//使用更好的方式，重写该函数的解码
OPSTAT func_sps232_read_data_pm25sharp(void)
{
#ifdef TARGET_RASPBERRY_PI3B

	int nread;
	int readCount, i, pm25sharpVout, pm25sharpRef;
	float pm25sharp, pm25sharpSum;
	UINT8 rb[2*RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN+1] = {0}; //2*7+1=15
	UINT8 checksum=0;

	//无限次工作模式打开，效率是比较低下的，但可以多个传感器可以一起兼容工作
	if(RPI_SPS232_SENSOR_SINGLE_PRESENT != RPI_SPS232_SENSOR_SINGLE_PRESENT_TRUE){
		//先关掉串口句柄
		spsapi_SerialPortClose(zHcuVmCtrTab.hwinv.sps232.sp.fd);
		//初始化硬件接口
		zHcuVmCtrTab.hwinv.sps232.sp.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
		zHcuVmCtrTab.hwinv.sps232.sp.nSpeed = 2400;
		zHcuVmCtrTab.hwinv.sps232.sp.nBits = 8;
		zHcuVmCtrTab.hwinv.sps232.sp.nEvent = 'N';
		zHcuVmCtrTab.hwinv.sps232.sp.nStop = 1;
		zHcuVmCtrTab.hwinv.sps232.sp.fd = HCU_INVALID_U16;
		zHcuVmCtrTab.hwinv.sps232.sp.vTime = HCU_INVALID_U8;
		zHcuVmCtrTab.hwinv.sps232.sp.vMin = HCU_INVALID_U8;
		zHcuVmCtrTab.hwinv.sps232.sp.c_lflag = 0;
		if (hcu_spsapi_serial_init(&zHcuVmCtrTab.hwinv.sps232.sp) == FAILURE)
		{
			HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		spsapi_SerialPortSetVtimeVmin(&zHcuVmCtrTab.hwinv.sps232.sp, 10, 5);
		delay(60000);
	}

	//循环读取多次
	pm25sharpSum = 0;
	for (readCount=0; readCount<RPI_SPS232_READ_REPEAT_TIMES; readCount++){
		delay(100);
		//读取数据
		nread = hcu_spsapi_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, rb, (2*RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN));
		if (nread <=0)
		{
			HcuErrorPrint("SPS232: Sensor PM25Sharp Read data error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		//HcuDebugPrint("SPS232: Fd = %d, PM25Sharp receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10], rb[11], rb[12], rb[13]);

		//example: aa 01 3a 00 7a d0 ff
		//寻找帧头
		for (i=0; i<RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN; i++){
			if ((rb[i] == RPI_SPS232_SENSOR_PM25SHARP_FRAME_HEAD1) || (rb[i] == RPI_SPS232_SENSOR_PM25SHARP_FRAME_HEAD2))
			break;
		}
		if ((i==RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN) || ((rb[i+RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN]!= RPI_SPS232_SENSOR_PM25SHARP_FRAME_TAIL1) && (rb[i+RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN]!= RPI_SPS232_SENSOR_PM25SHARP_FRAME_TAIL2))){
			HcuErrorPrint("SPS232: Sensor PM25Sharp frame decode error, can not find frame header. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//计算校验和，判断是否是正确
		checksum = func_sps232_check_sum_pm25sharp(&rb[i+1], 4);
		if (checksum != rb[i+8]){
			HcuErrorPrint("SPS232: Sensor PM25Sharp frame checksum error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x!\n", zHcuVmCtrTab.hwinv.sps232.sp.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//得到最终浓度
		pm25sharpVout = ((rb[i+1]<<8)&0xFF00) + (rb[i+2]&0xFF);
		pm25sharpRef = ((rb[i+3]<<8)&0xFF00) + (rb[i+4]&0xFF);
		pm25sharp = pm25sharpVout*5/1024 + pm25sharpRef * 0;  //转换为电压, vRef暂时没有排上用场，SHARP模块说明书中也没有说清楚
		pm25sharp = pm25sharp * RPI_SPS232_SENSOR_PM25SHARP_COFF; //转换为最总浓度ug/M3

		//加总到目标SUMMARY
		pm25sharpSum += pm25sharp;
	}

	//求平均
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].fVal = pm25sharpSum / RPI_SPS232_READ_REPEAT_TIMES;
	zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].updateTimeStamp = time(0);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: Sensor PM25SHARP Transformed float average read result PM25= %6.2fug/m3\n", zHcuVmCtrTab.codab.si[SENSOR_ID_SPSPM25SHARP].fVal);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//校验和：取反求和
UINT8 func_sps232_check_sum_ze08ch2o(UINT8 *s, UINT8 len)
{
	UINT8 j=0, tmp=0;
	for(j=0;j<len;j++){
		tmp+=*s;
		s++;
	}
	tmp=(~tmp)+1;
	return tmp;
}

//校验和：简单的求和
UINT8 func_sps232_check_sum_pm25sharp(UINT8 *s, UINT8 len)
{
	UINT8 j=0, tmp=0;
	for(j=0;j<len;j++){
		tmp+=*s;
		s++;
	}
	return tmp;
}


//初始化二维码打印机
OPSTAT hcu_sps232_qr_printer_init(void)
{
	OPSTAT ret = 0;

	//初始化全局变量
	gSerialPortForQrPrinter.id = zHcuSysEngPar.serialport.SeriesPortForQrPrinter;
	gSerialPortForQrPrinter.nSpeed = HCU_SYSCFG_SERIESPORT_BAUTRATE_DEFAULT;
	gSerialPortForQrPrinter.nBits = 8;
	gSerialPortForQrPrinter.nEvent = 'N';
	gSerialPortForQrPrinter.nStop = 1;
	gSerialPortForQrPrinter.fd = HCU_INVALID_U16;
	gSerialPortForQrPrinter.vTime = HCU_INVALID_U8;
	gSerialPortForQrPrinter.vMin = HCU_INVALID_U8;
	gSerialPortForQrPrinter.c_lflag = 0;

	ret = hcu_spsapi_serial_init(&gSerialPortForQrPrinter);
	if (FAILURE == ret)
	{
		HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
		return ret;
	}

	hcu_spsapi_SerialPortSetVtimeVmin(&gSerialPortForQrPrinter, 10, 5);
	HCU_DEBUG_PRINT_NOR("SPS232: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPortForQrPrinter.vTime, gSerialPortForQrPrinter.vMin);

	return SUCCESS;
}

//	PMJ_M[0] = 0x20;	//英展，耐撕
//	PMJ_M[1] = weightdata/10000+0x30;
//	PMJ_M[2] = (weightdata/1000)%10+0x30;
//	PMJ_M[3] = 0X2E;	//.
//	PMJ_M[4] = (weightdata/100)%10+0x30;
//	PMJ_M[5] = (weightdata/10)%10+0x30;
//	PMJ_M[6] = 0X20;	//space
//	PMJ_M[7] = 0X6B;  //K
//	PMJ_M[8] = 0X67;  //G
//	PMJ_M[9] = 0x0D;  //
//	PMJ_M[10] = 0x0A;  //
//
//	if(PMJ_M[1] == 0X30)
//	{
//		PMJ_M[1] = 0X20;
//	}
//	for(i=0;i<11;i++)	//10
//	{
//			UART1_SendByte(PMJ_M[i]);
////            OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_DLY, &os_err);
//	}
void hcu_sps232_send_char_to_ext_printer(char *s, int len)
{
	char output[130];

	//入参检查
	if ((s==NULL) || (strlen(s) != len) || (len > (sizeof(output)-30))){
		HcuErrorPrint("SPS232: Function hcu_sps232_send_char_to_ext_printer input parameter error!\n");
		return;
	}

	//固定格式，待完善
	memset(output, 0, sizeof(output));
	sprintf(output, " %s\r\n", s);

	//选择打印机串口
	if (hcu_spsapi_serial_port_send(&gSerialPortForQrPrinter, (UINT8*)output, strlen(output)) == FAILURE){
        HcuErrorPrint("SPS232: Function hcu_sps232_send_char_to_ext_printer send data error!\n");
        return;
	}
	return;
}





