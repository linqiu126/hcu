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
SerialPort_t gSerialPortForSPS232;

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

	int workingCycle = 2;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SENSOR_PRESENT_SHARP == HCU_SENSOR_PRESENT_YES){
			func_sps232_read_data_pm25sharp();
			hcu_sleep(RPI_SPS232_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_ZE08CH2O == HCU_SENSOR_PRESENT_YES){
			func_sps232_read_data_ze08ch2o();
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
	//可能要根据每一种传感器，分别进行单次操作的初始化，待定待完善
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

	ret = hcu_sps485_serial_init(&gSerialPortForSPS232);
	if (FAILURE == ret)
	{
		HcuErrorPrint("SPS232: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("SPS232: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPortForSPS232, 10, 5);
	HcuDebugPrint("SPS232: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPortForSPS232.vTime, gSerialPortForSPS232.vMin);


	return SUCCESS;
}


OPSTAT func_sps232_read_data_pm25sharp(void)
{
#ifdef TARGET_RASPBERRY_PI3B

	int ret = 0;
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
	float average_pm25, sum_2s = 0;
	//sensor_pm25_sharp_data_element_t pm25Data;

	//起始时间
	start_time = time((time_t*)NULL);

	//读取数据

	nread = hcu_sps485_serial_port_get(&gSerialPortForSPS232, &received_single_byte, 1);
	if (nread > 0)
	{
		//起始位是0xAA
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
	}
	else
	{
		HcuDebugPrint("SPS232: Sensor PM25Sharp Read(fd, &received_single_byte, 1) error!\n");
	}


	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

OPSTAT func_sps232_read_data_ze08ch2o(void)
{
#ifdef TARGET_RASPBERRY_PI3B


	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}


