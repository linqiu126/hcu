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
FsmStateItem_t FsmSps232[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_sps232_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_sps232_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_sps232_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_SPS232_RECEIVED,            	fsm_sps232_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SPS232_RECEIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SPS232_RECEIVED,   			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SPS232_RECEIVED,   			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
//For Serial Port Init
SerialPortCom_t gSerialPortForSPS232;

float zHcuSps232Pm25Sharp;
float zHcuSps232HchoZe08ch2o;

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
	int ret=0, conCounter=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SPS232, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SPS232: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPS232], zHcuTaskNameList[src_id]);
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
	zHcuRunErrCnt[TASK_ID_SPS232] = 0;
	zHcuSps232Pm25Sharp = HCU_SENSOR_VALUE_NULL;
	zHcuSps232HchoZe08ch2o = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPS232, FSM_STATE_SPS232_RECEIVED) == FAILURE){
		HcuErrorPrint("SPS232: Error Set FSM State!\n");
		zHcuRunErrCnt[TASK_ID_SPS232]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SPS232: Enter FSM_STATE_SPS232_ACTIVED status, Keeping refresh here!\n");
	}

	//串口同时支持两种速率，没有成功，意味着不同的外设具备不同速度时，必须设置树莓派的串口环境速率，然后这里u一次性初始化，而不能单次重复初始化
	//多个串口外设同时打开时，互相斥需要将不工作的传感器关掉，不然它会无谓的占据很多的时间，导致其它正常工作的传感器得不到快速响应

	//这里等待，是为了保证外设在串口初始化后有足够的时间稳定，不然其读数不太正常
	hcu_sleep(60);
	int workingCycle = 2;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SENSOR_PRESENT_ZE08CH2O == HCU_SENSOR_PRESENT_YES){
			func_sps232_read_data_ze08ch2o();
			hcu_sleep(RPI_SPS232_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_SHARP == HCU_SENSOR_PRESENT_YES){
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
	zHcuGlobalCounter.restartCnt++;
	fsm_sps232_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_sps232_int_init(void)
{
	//初始化硬件接口
	gSerialPortForSPS232.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
	if (HCU_SENSOR_PRESENT_SHARP == HCU_SENSOR_PRESENT_YES) gSerialPortForSPS232.nSpeed = 2400;
	else gSerialPortForSPS232.nSpeed = 9600;
	gSerialPortForSPS232.nBits = 8;
	gSerialPortForSPS232.nEvent = 'N';
	gSerialPortForSPS232.nStop = 1;
	gSerialPortForSPS232.fd = HCU_INVALID_U16;
	gSerialPortForSPS232.vTime = HCU_INVALID_U8;
	gSerialPortForSPS232.vMin = HCU_INVALID_U8;
	gSerialPortForSPS232.c_lflag = 0;

	int ret=0;

	ret = hcu_spsapi_serial_init(&gSerialPortForSPS232);
	if (FAILURE == ret)
	{
		HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
		zHcuRunErrCnt[TASK_ID_SPS232]++;
		return ret;
	}
	else
	{
		HcuDebugPrint("SPS232: Init Serial Port Success ...\n");
	}

	spsapi_SerialPortSetVtimeVmin(&gSerialPortForSPS232, 10, 5);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPortForSPS232.vTime, gSerialPortForSPS232.vMin);
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
		spsapi_SerialPortClose(gSerialPortForSPS232.fd);
		//初始化硬件接口
		gSerialPortForSPS232.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
		gSerialPortForSPS232.nSpeed = 9600;
		gSerialPortForSPS232.nBits = 8;
		gSerialPortForSPS232.nEvent = 'N';
		gSerialPortForSPS232.nStop = 1;
		gSerialPortForSPS232.fd = HCU_INVALID_U16;
		gSerialPortForSPS232.vTime = HCU_INVALID_U8;
		gSerialPortForSPS232.vMin = HCU_INVALID_U8;
		gSerialPortForSPS232.c_lflag = 0;
		if (hcu_spsapi_serial_init(&gSerialPortForSPS232) == FAILURE)
		{
			HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		spsapi_SerialPortSetVtimeVmin(&gSerialPortForSPS232, 10, 5);
		delay(60000);
	}

	//循环读取多次
	hchoSum = 0;
	for (readCount=0; readCount<RPI_SPS232_READ_REPEAT_TIMES; readCount++){
		delay(100);
		//读取数据
		nread = hcu_spsapi_serial_port_get(&gSerialPortForSPS232, rb, (2*RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN));
		if (nread <=0)
		{
			HcuErrorPrint("SPS232: Sensor ZE08CH2O Read data error. nread=%d!\n", nread);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		//HcuDebugPrint("SPS232: Fd = %d, ZE08CH2O receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10], rb[11], rb[12], rb[13], rb[14], rb[15], rb[16], rb[17]);

		//example: ff 17	//先关掉串口句柄 04 00 01 20 13 88 29 ff
		//寻找帧头
		for (i=0; i<RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN; i++){
			if ((rb[i] == RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD1) || (rb[i] == RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD2))
			break;
		}
		if ((i==RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN) || ((rb[i+RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN]!= RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD1) && (rb[i+RPI_SPS232_SENSOR_ZE08CH2O_FRAME_LEN]!= RPI_SPS232_SENSOR_ZE08CH2O_FRAME_HEAD2))){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame decode error, can not find frame header. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//计算校验和，判断是否是正确
		checksum = func_sps232_check_sum_ze08ch2o(&rb[i+1], 7);
		if (checksum != rb[i+8]){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame checksum error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//Header of CMMID / UNIT
		if(rb[i+1] != RPI_SPS232_SENSOR_ZE08CH2O_REPORT_CMMID){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame cmdid error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		if(rb[i+2] != RPI_SPS232_SENSOR_ZE08CH2O_REPORT_UNIT){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame UNIT error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//得到最终浓度
		hcho = ((rb[i+4]<<8)&0xFF00) + (rb[i+5]&0xFF);
		hchoMax = ((rb[i+6]<<8)&0xFF00) + (rb[i+7]&0xFF);
		if (hcho > hchoMax){
			HcuErrorPrint("SPS232: Sensor ZE08CH2O frame received too big data error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6], rb[7], rb[8], rb[9], rb[10]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//加总到目标SUMMARY
		hchoSum += hcho;
	}

	//求平均
	zHcuSps232HchoZe08ch2o = hchoSum / RPI_SPS232_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: Sensor ZE08CH2O Transformed float average read result HCHO = %6.2fppb, around %6.2fug/m3\n", zHcuSps232HchoZe08ch2o, zHcuSps232HchoZe08ch2o*1.295);
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
		spsapi_SerialPortClose(gSerialPortForSPS232.fd);
		//初始化硬件接口
		gSerialPortForSPS232.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
		gSerialPortForSPS232.nSpeed = 2400;
		gSerialPortForSPS232.nBits = 8;
		gSerialPortForSPS232.nEvent = 'N';
		gSerialPortForSPS232.nStop = 1;
		gSerialPortForSPS232.fd = HCU_INVALID_U16;
		gSerialPortForSPS232.vTime = HCU_INVALID_U8;
		gSerialPortForSPS232.vMin = HCU_INVALID_U8;
		gSerialPortForSPS232.c_lflag = 0;
		if (hcu_spsapi_serial_init(&gSerialPortForSPS232) == FAILURE)
		{
			HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}
		spsapi_SerialPortSetVtimeVmin(&gSerialPortForSPS232, 10, 5);
		delay(60000);
	}

	//循环读取多次
	pm25sharpSum = 0;
	for (readCount=0; readCount<RPI_SPS232_READ_REPEAT_TIMES; readCount++){
		delay(100);
		//读取数据
		nread = hcu_spsapi_serial_port_get(&gSerialPortForSPS232, rb, (2*RPI_SPS232_SENSOR_PM25SHARP_FRAME_LEN));
		if (nread <=0)
		{
			HcuErrorPrint("SPS232: Sensor PM25Sharp Read data error!\n");
			zHcuRunErrCnt[TASK_ID_SPS232]++;
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
			HcuErrorPrint("SPS232: Sensor PM25Sharp frame decode error, can not find frame header. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
			return FAILURE;
		}

		//计算校验和，判断是否是正确
		checksum = func_sps232_check_sum_pm25sharp(&rb[i+1], 4);
		if (checksum != rb[i+8]){
			HcuErrorPrint("SPS232: Sensor PM25Sharp frame checksum error. Fd = %d, Receive buffer = %02x %02x %02x %02x %02x %02x %02x!\n", gSerialPortForSPS232.fd, rb[0], rb[1], rb[2], rb[3], rb[4], rb[5], rb[6]);
			zHcuRunErrCnt[TASK_ID_SPS232]++;
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
	zHcuSps232Pm25Sharp = pm25sharpSum / RPI_SPS232_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPS232: Sensor PM25SHARP Transformed float average read result PM25= %6.2fug/m3\n", zHcuSps232Pm25Sharp);
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

/*
//读取PM25SHARP传感器的串口数据
OPSTAT func_sps232_read_data_pm25sharp(void)
{
#ifdef TARGET_RASPBERRY_PI3B

	int nread;
	int i=0,j=0,sum=0,counter=0;
	int counter_good_frame = 0;
	int counter_bad_frame = 0;
	int counter_total_frame = 0;
	float vo;
	float pm25value;
	unsigned char received_single_byte;
	unsigned char pm25_frame_received_buff[7];
	int start_time, end_time;
	float average_pm25=0, sum_2s = 0;

	//起始时间
	start_time = time((time_t*)NULL);

	//读取数据
	nread = hcu_spsapi_serial_port_get(&gSerialPortForSPS232, &received_single_byte, 1);
	if (nread <=0)
	{
		HcuErrorPrint("SPS232: Sensor PM25Sharp Read data error!\n");
		zHcuRunErrCnt[TASK_ID_SPS232]++;
		return FAILURE;
	}

	//帧解码，起始位是0xAA
	if(received_single_byte == 0xaa){
	  j=0;
	  pm25_frame_received_buff[j] = received_single_byte;
	}
	else{
	  j++;
	  pm25_frame_received_buff[j] = received_single_byte;
	}

	if(j==6)
	{
	  sum = pm25_frame_received_buff[1]+ pm25_frame_received_buff[2]+ pm25_frame_received_buff[3] + pm25_frame_received_buff[4];
	  //log_debug(logfile,"Bytes received: %02x %02x %02x %02x %02x %02x %02x ", pm25_frame_received_buff[0], pm25_frame_received_buff[1], pm25_frame_received_buff[2], pm25_frame_received_buff[3], pm25_frame_received_buff[4], pm25_frame_received_buff[5], pm25_frame_received_buff[6]);
	  if(pm25_frame_received_buff[5]==sum && pm25_frame_received_buff[6]== 0xff ) //终止位是0xFF
	  {
		  vo=(pm25_frame_received_buff[1]*256.0+pm25_frame_received_buff[2])/1024.00*5.000;
		  pm25value = vo*700;
		  //log_debug(logfile,"pm25 value is:%f",pm25value);
		  //2秒内的平均值求取
		  counter++;
		  sum_2s = sum_2s + pm25value;
		  end_time = time((time_t*)NULL);
		  if((end_time - start_time) > 2)
		  {
			  //log_debug(logfile,"Last bytes received: %02x %02x %02x %02x %02x %02x %02x ", pm25_frame_received_buff[0], pm25_frame_received_buff[1], pm25_frame_received_buff[2], pm25_frame_received_buff[3], pm25_frame_received_buff[4], pm25_frame_received_buff[5], pm25_frame_received_buff[6]);
			  HcuDebugPrint("SPS232: Sensor PM25Sharp sast bytes received: %02x %02x %02x %02x %02x %02x %02x \n", pm25_frame_received_buff[0], pm25_frame_received_buff[1], pm25_frame_received_buff[2], pm25_frame_received_buff[3], pm25_frame_received_buff[4], pm25_frame_received_buff[5], pm25_frame_received_buff[6]);
			  zHcuSps232Pm25Sharp = sum_2s / counter;
			  HcuDebugPrint("SPS232: Sensor PM25Sharp start_time is %d, end_time is %d, counter in 2 seconds is %d, sum_2s is %f, average_pm25 value is:%f \n", start_time, end_time, counter, sum_2s, average_pm25);
			  counter = 0;
			  sum_2s = 0;
			  start_time = time((time_t*)NULL);
			  HcuDebugPrint("SPS232: Sensor PM25Sharp counter_good_frame is: %d, counter_total_frame is: %d.\n",counter_good_frame, counter_total_frame);
		  }
		  counter_good_frame++;
		  //log_debug(logfile,"counter_good_frame is: %d.",counter_good_frame);
	  }
	  else
	  {
		  counter_bad_frame++;
		  HcuDebugPrint("SPS232: Sensor PM25Sharp counter_bad_frame is: %d.",counter_bad_frame);
	  }
	  counter_total_frame++;
	  //log_debug(logfile,"counter_total_frame is: %d.\n",counter_total_frame);
	  //PM25计算和输出完成之后相关变量清零
	  j=0;
	  received_single_byte='\0';
	  for(i=0;i<7;i++){
		  pm25_frame_received_buff[i]=0;
	  }

	}
	else
	{

	}



	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}
*/


