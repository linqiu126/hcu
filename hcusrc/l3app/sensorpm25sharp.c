/*
 * sensorpm25sharp.cpp
 *
 *  Created on: 12 May 2016
 *      Author: pi
 */


#include "sensorpm25sharp.h"


#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"


/*
** FSM of the FsmPm25Sharp
*/
HcuFsmStateItem_t HcuFsmPm25sharp[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_pm25sharp_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_pm25sharp_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_pm25sharp_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_pm25sharp_restart},

    //Task level initialization
	{MSG_ID_CLOUDVELA_PM25SP_DATA_REQ,		FSM_STATE_PM25SHARP_ACTIVED,      	  	fsm_pm25sharp_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM,	FSM_STATE_PM25SHARP_ACTIVED,      	  	fsm_pm25sharp_cloudvela_data_confirm},


    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskPm25sharpContext_t gTaskPm25sharpContext;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_pm25sharp_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_PM25SHARP, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("PM25SHARP: Error Set FSM State at fsm_pm25sharp_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_pm25sharp_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_PM25SHARP, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("PM25SHARP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25SHARP].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_PM25SHARP, FSM_STATE_PM25SHARP_INITED) == FAILURE){
		HcuErrorPrint("PM25SHARP: Error Set FSM State!\n");
		return FAILURE;
	}

	/*
	//初始化硬件接口
	gTaskPm25sharpContext.serialPort.id = zHcuSysEngPar.serialport.SeriesPortForPm25Sharp;
	gTaskPm25sharpContext.serialPort.nSpeed = 2400;
	gTaskPm25sharpContext.serialPort.nBits = 8;
	gTaskPm25sharpContext.serialPort.nEvent = 'N';
	gTaskPm25sharpContext.serialPort.nStop = 1;
	gTaskPm25sharpContext.serialPort.fd = HCU_INVALID_U16;
	gTaskPm25sharpContext.serialPort.vTime = HCU_INVALID_U8;
	gTaskPm25sharpContext.serialPort.vMin = HCU_INVALID_U8;
	gTaskPm25sharpContext.serialPort.c_lflag = 0;

	ret = hcu_sps485_serial_init(&gTaskPm25sharpContext.serialPort);
	if (FAILURE == ret)
	{
		HcuErrorPrint("PM25SHARP: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("PM25SHARP: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gTaskPm25sharpContext.serialPort, 10, 5);
	HcuDebugPrint("PM25SHARP: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gTaskPm25sharpContext.serialPort.vTime, gTaskPm25sharpContext.serialPort.vMin);
    */

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP] = 0;
	memset(&gTaskPm25sharpContext, 0, sizeof(gTaskPm25sharpContext_t));

	//启动周期性定时器
	/*
	ret = hcu_timer_start(TASK_ID_PM25SHARP, TIMER_ID_1S_PM25SHARP_PERIOD_READ,
	zHcuSysEngPar.timer.array[TIMER_ID_1S_PM25SHARP_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_PM25SHARP]++;
		HcuErrorPrint("PM25SHARP: Error start period timer!\n");
		return FAILURE;
	}
	*/

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_PM25SHARP, FSM_STATE_PM25SHARP_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP]++;
		HcuErrorPrint("PM25SHARP: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("PM25SHARP: Enter FSM_STATE_PM25SHARP_ACTIVED status, Keeping refresh here!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_pm25sharp_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("PM25SHARP: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_pm25sharp_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_pm25sharp_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_pm25sharp_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("PM25SHARP: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_PM25SHARP]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_PM25SHARP, TASK_ID_PM25SHARP, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP]++;
			HcuErrorPrint("PM25SHARP: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_PM25SHARP].taskName, zHcuVmCtrTab.task[TASK_ID_PM25SHARP].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_PM25SHARP_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_PM25SHARP) != FSM_STATE_PM25SHARP_ACTIVED){
			ret = FsmSetState(TASK_ID_PM25SHARP, FSM_STATE_PM25SHARP_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP]++;
				HcuErrorPrint("PM25SHARP: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
		//干活
		UINT32 pm25sharpfd = gTaskPm25sharpContext.serialPort.fd;
		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
			HcuDebugPrint("PM25SHARP: fd = %d !\n", pm25sharpfd);
		}
		func_pm25sharp_read_data(pm25sharpfd);
		close(pm25sharpfd);
	}

	return SUCCESS;
}

void func_pm25sharp_read_data(UINT32 fd)
{

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
	sensor_pm25_sharp_data_element_t pm25Data;

	//起始时间
	start_time = time((time_t*)NULL);

	//读取数据
	//memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	//ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	//nread = read(fd, &received_single_byte, 1);
	nread = hcu_sps485_serial_port_get(&gTaskPm25sharpContext.serialPort, &received_single_byte, 1);
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
				  HcuDebugPrint("PM25SHARP: Last bytes received: %02x %02x %02x %02x %02x %02x %02x \n", pm25_frame_received_buff[0], pm25_frame_received_buff[1], pm25_frame_received_buff[2], pm25_frame_received_buff[3], pm25_frame_received_buff[4], pm25_frame_received_buff[5], pm25_frame_received_buff[6]);
				  average_pm25 = sum_2s / counter;

				  if ((average_pm25 >= HCU_SENSOR_PM25_VALUE_MIN) && (average_pm25 <= HCU_SENSOR_PM25_VALUE_MAX))
				  {
					  memset(&pm25Data, 0, sizeof(sensor_pm25_sharp_data_element_t));
					  pm25Data.equipid = 0;
					  pm25Data.timeStamp = time(0);
					  pm25Data.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
					  pm25Data.pm2d5Value = (int)(average_pm25*100);
					  ret = dbi_HcuPm25SharpDataInfo_save(&pm25Data);
					  if (ret == FAILURE){
							zHcuSysStaPm.taskRunErrCnt[TASK_ID_PM25SHARP]++;
							HcuErrorPrint("PM25SHARP: Can not save data into database!\n");
					  }
				  }
				  HcuDebugPrint("PM25SHARP: start_time is %d, end_time is %d, counter in 2 seconds is %d, sum_2s is %f, average_pm25 value is:%f \n", start_time, end_time, counter, sum_2s, average_pm25);
				  counter = 0;
				  sum_2s = 0;
				  start_time = time((time_t*)NULL);
				  HcuDebugPrint("PM25SHARP: counter_good_frame is: %d, counter_total_frame is: %d.\n",counter_good_frame, counter_total_frame);
			  }
			  counter_good_frame++;
			  //log_debug(logfile,"counter_good_frame is: %d.",counter_good_frame);
		  }
		  else
		  {
			  counter_bad_frame++;
			  HcuDebugPrint("PM25SHARP: counter_bad_frame is: %d.",counter_bad_frame);
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
		HcuDebugPrint("PM25SHARP: Read(fd, &received_single_byte, 1) error!\n");
	}

}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_pm25sharp_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_pm25sharp_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}



