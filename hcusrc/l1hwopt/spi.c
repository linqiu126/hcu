/*
 * spi.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "../l1hwopt/spi.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the SPI
*/
FsmStateItem_t HcuFsmSpi[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_spi_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_spi_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_spi_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_SPI_ACTIVIED,            		fsm_spi_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SPI_ACTIVIED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SPI_ACTIVIED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SPI_ACTIVIED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
float zHcuSpiTempRht03;
float zHcuSpiHumidRht03;
float zHcuSpiTempMth01;
float zHcuSpiHumidMth01;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spi_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPI, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SPI: Error Set FSM State at fsm_spi_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_spi_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, conCounter=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SPI, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SPI: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPI], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPI, FSM_STATE_SPI_INITED) == FAILURE){
		HcuErrorPrint("SPI: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_spi_int_init() == FAILURE){
		HcuErrorPrint("SPI: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_SPI] = 0;
	zHcuSpiTempRht03 = HCU_SENSOR_VALUE_NULL;
	zHcuSpiHumidRht03 = HCU_SENSOR_VALUE_NULL;
	zHcuSpiTempMth01 = HCU_SENSOR_VALUE_NULL;
	zHcuSpiHumidMth01 = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPI, FSM_STATE_SPI_ACTIVIED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_SPI]++;
		HcuErrorPrint("SPI: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SPI: Enter FSM_STATE_SPI_ACTIVED status, Keeping refresh here!\n");
	}

	int workingCycle = 2;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SENSOR_PRESENT_MTH01 == HCU_SENSOR_PRESENT_YES){
			func_spi_read_data_mth01();
			hcu_sleep(RPI_SPI_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_RHT03 == HCU_SENSOR_PRESENT_YES){
			func_spi_read_data_rht03();
			hcu_sleep(RPI_SPI_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		conCounter = workingCycle-conCounter;
		hcu_sleep(RPI_SPI_SENSOR_READ_GAP/workingCycle * conCounter);
	}

	return SUCCESS;
}

OPSTAT fsm_spi_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SPI: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_spi_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_spi_int_init(void)
{
	return SUCCESS;
}

//Starting point for Raspberry-Pi function definition
OPSTAT func_spi_read_data_mth01(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd, i;
	int temp, humid;
	unsigned char read[256];
	float tempSum, humidSum;

	if((fd=wiringPiSPISetup(RPI_SPI_ADDR_MTH01, RPI_SPI_SPEED))<0){
		HcuDebugPrint("SPI: can't find spi!\n");
		zHcuRunErrCnt[TASK_ID_SPI]++;
		return fd;
	}

	tempSum = 0;
	humidSum = 0;
	for (i=0; i<RPI_SPI_READ_REPEAT_TIMES; i++){
		delay (1000);
		//数据存在的位置是前两个字节是温度，第三个字节是湿度，第四个是CRC
		//CRC8暂时不检查，未来需要检查
//		for (j=0;j<256;j++) read[j]=j;
		wiringPiSPIDataRW(RPI_SPI_ADDR_MTH01, read, 4);
//		sprintf(str, "SPI: Sensor MTH01 read result: Index=%d, Return = %d, Read result = 0x ", i, tmp);
//		for(j=0;j<256;j++){
//			sprintf(strtmp, "%02x ", read[j]);
//			strcat(str, strtmp);
//		}
//		strcat(str, "\n");
//		HcuDebugPrint(str);
		//HcuDebugPrint("SPI: Sensor MTH01 read result: Index=%d, Read[4] = 0x%x %x %x %x, Return=%d\n", i, read[0], read[1], read[2], read[3], tmp);
		temp = ((read[0]<<8)&0xFF00) + (read[1]&0xFF) - 400;
		tempSum += temp/10;
		humid = read[2]&0xFF;
		humidSum += humid;
//		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("SPI: Sensor MTH01 Original read result Temp=0x%xC, Temp=0x%x\%, index = %d, DATA_MOSI#=%d\n", temp, humid, i, RPI_SPI_PIN_MOSI);
//		}
	}

	//求平均
	zHcuSpiTempMth01 = tempSum / RPI_SPI_READ_REPEAT_TIMES;
	zHcuSpiHumidMth01 = humidSum / RPI_SPI_READ_REPEAT_TIMES;

	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPI: Sensor MTH01 Transformed average float result Temp=%6.2fC, Humid=%6.2f\%, DATA_SPI_MOSI#=%d\n", zHcuSpiTempMth01, zHcuSpiHumidMth01, RPI_SPI_PIN_MOSI);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}


OPSTAT func_spi_read_data_rht03(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd, i;
	int temp, humid;
	unsigned char read[4] = {0, 0, 0, 0};
	float tempSum, humidSum;

	if((fd=wiringPiSPISetup(RPI_SPI_ADDR_RHT03, RPI_SPI_SPEED))<0){
		HcuDebugPrint("SPI: can't find spi!\n");
		zHcuRunErrCnt[TASK_ID_SPI]++;
		return fd;
	}

	tempSum = 0;
	humidSum = 0;
	for (i=0; i<RPI_SPI_READ_REPEAT_TIMES; i++){
		delay (200);
		//数据存在的位置是前两个字节是温度，第三个字节是湿度，第四个是CRC
		//CRC16暂时不检查，未来需要检查
		wiringPiSPIDataRW(RPI_SPI_ADDR_RHT03, read, 4);
		//HcuDebugPrint("SPI: Sensor RHT03 read result: Index=%d, Read[4] = 0x%x %x %x %x, Return=%d\n", i, read[0], read[1], read[2], read[3], tmp);
		temp = ((read[0]<<8)&0xFF00) + (read[1]&0xFF) - 400;
		tempSum += temp/10;
		humid = read[2]&0xFF;
		humidSum += humid;
//		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("SPI: Sensor RHT03 Original read result Temp=0x%xC, Temp=0x%x\%, index = %d, DATA_MOSI#=%d\n", temp, humid, i, RPI_SPI_PIN_MOSI);
//		}
	}

	//求平均
	zHcuSpiTempRht03 = tempSum / RPI_SPI_READ_REPEAT_TIMES;
	zHcuSpiHumidRht03 = humidSum / RPI_SPI_READ_REPEAT_TIMES;

	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPI: Sensor RHT03 Transformed average float result Temp=%6.2fC, Humid=%6.2f\%, DATA_SPI_MOSI#=%d\n", zHcuSpiTempRht03, zHcuSpiHumidRht03, RPI_SPI_PIN_MOSI);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}


