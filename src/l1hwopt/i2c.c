/*
 * i2c.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "../l1hwopt/i2c.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the I2C
*/
FsmStateItem_t FsmI2c[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_i2c_task_entry}, //Starting

	//System level initialisation, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_i2c_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_i2c_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialisation
    {MSG_ID_COM_RESTART,        FSM_STATE_I2C_RECEIVED,            		fsm_i2c_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_I2C_RECEIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_I2C_RECEIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_I2C_RECEIVED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
float zHcuI2cTempSht20;
float zHcuI2cHumidSht20;
float zHcuI2cLightstrBh1750;
float zHcuI2cAirprsBmp180;
float zHcuI2cTempBmp180;
float zHcuI2cAltitudeBmp180;
float zHcuI2cPm25Bmpd300;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_i2c_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_I2C, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("I2C: Error Set FSM State at fsm_i2c_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_i2c_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_I2C, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("I2C: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_I2C], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_I2C, FSM_STATE_I2C_INITED) == FAILURE){
		HcuErrorPrint("I2C: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_i2c_int_init() == FAILURE){
		HcuErrorPrint("I2C: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_I2C] = 0;
	zHcuI2cTempSht20 = HCU_SENSOR_VALUE_NULL;
	zHcuI2cHumidSht20 = HCU_SENSOR_VALUE_NULL;
	zHcuI2cLightstrBh1750 = HCU_SENSOR_VALUE_NULL;
	zHcuI2cAirprsBmp180 = HCU_SENSOR_VALUE_NULL;
	zHcuI2cTempBmp180 = HCU_SENSOR_VALUE_NULL;
	zHcuI2cAltitudeBmp180 = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_I2C, FSM_STATE_I2C_RECEIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_I2C]++;
		HcuErrorPrint("I2C: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("I2C: Enter FSM_STATE_I2C_ACTIVED status, Keeping refresh here!\n");
	}

	//进入循环工作模式
	while(1){
		func_i2c_read_data_sht20();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/4);
		func_i2c_read_data_bh1750();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/4);
		func_i2c_read_data_bmp180();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/4);
		func_i2c_read_data_bmpd300();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/4);
	}

	return SUCCESS;
}

OPSTAT fsm_i2c_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("I2C: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_i2c_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_i2c_int_init(void)
{
	return SUCCESS;
}


//Starting point for Raspberry-Pi function definition

//温湿度
OPSTAT func_i2c_read_data_sht20(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd, i;
	int temp, humid;
	float tmp1, tmp2, tempSum, humidSum;

	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_SHT20))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}

	tempSum = 0;
	humidSum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++){
		delay (200);
		temp = wiringPiI2CReadReg16(fd, 0xE3);
		humid = wiringPiI2CReadReg16(fd, 0xE5);
//		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("I2C: Sensor SHT20 Original read result Temp=0x%xC, Humid=0x%x\%, DATA_I2C_SDA#=%d\n", temp, humid, RPI_I2C_PIN_SDA);
//		}
		tmp1 = (temp>>8)&0xFF;
		tmp2 = ((temp&0xFF)<<8)&0xFF00;
		tempSum += (tmp1 + tmp2) * 175.72 / 1024 / 64 - 46.85;
		tmp1 = (humid>>8)&0xFF;
		tmp2 = ((humid&0xFF)<<8)&0xFF00;
		humidSum += (tmp1 + tmp2) * 125 / 1024 / 64 - 6;
	}

	//求平均
	zHcuI2cTempSht20 = tempSum / RPI_I2C_READ_REPEAT_TIMES;
	zHcuI2cHumidSht20 = humidSum / RPI_I2C_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor SHT20 Transformed average float result Temp=%6.2fC, Humid=%6.2f\%, DATA_I2C_SDA#=%d\n", zHcuI2cTempSht20, zHcuI2cHumidSht20, RPI_I2C_PIN_SDA);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//光强GY30
OPSTAT func_i2c_read_data_bh1750(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd;
	char buf[3];
	char val;
	float flight, flightSum;

	//打开I2C设备
	fd=open(RPI_DEV_I2C_ADDRESS, O_RDWR);
	if(fd<0)
	{
		HcuErrorPrint("I2C: err open file:%s\n", strerror(errno));
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}
	//控制寄存器对应的设备
	if(ioctl(fd, I2C_SLAVE, RPI_I2C_ADDR_BH1750)<0 )
	{
		HcuErrorPrint("I2C: ioctl error : %s\n", strerror(errno));
		zHcuRunErrCnt[TASK_ID_I2C]++;
		close(fd);
		return FAILURE;
	}

	val=0x01;
	if(write(fd, &val,1)<0)
	{
		//HcuErrorPrint("I2C: write 0x01 err\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
	}

	val=0x10;
	if(write(fd, &val,1)<0)
	{
		//HcuErrorPrint("I2C: write 0x10 err\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
	}

	int i=0;
	flightSum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++)
	{
		flight=0;
		if(read(fd, &buf, 3))
		{
			flight=(buf[0]*256 + buf[1])/1.2;
//			if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//				HcuDebugPrint("I2C: Sensor BH1750 Original float result light = %6.2f lx, DATA_I2C_SDA#=%d\n", flight, RPI_I2C_PIN_SDA);
//			}
			flightSum+= flight;
		}
		else
		{
			HcuErrorPrint("I2C: read light error\n");
			zHcuRunErrCnt[TASK_ID_I2C]++;
			close(fd);
			return FAILURE;
		}
		hcu_usleep(100000);//sleep 100ms
	}
	//输出结果到目标变量中
	zHcuI2cLightstrBh1750 = flightSum / RPI_I2C_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BH1750 Transformed average float result light = %6.2f lx, DATA_I2C_SDA#=%d\n", zHcuI2cLightstrBh1750, RPI_I2C_PIN_SDA);
	}
	close(fd);

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//气压温度GY68
//BMP180的度数需要校准，比较复杂，待定！
OPSTAT func_i2c_read_data_bmp180(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd, i;
	int airprs, temp;
	float airprsSum, tempSum;
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_BMP180))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}

	airprsSum = 0;
	tempSum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++){
		delay (200);
		airprs = wiringPiI2CReadReg16(fd, 0x00);
		airprsSum += airprs;
		delay (200);
		temp = wiringPiI2CReadReg16(fd, 0x02);
		tempSum += temp;
		//计算算法待定
//		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("I2C: Sensor BMP180 Original read result Airprs=0x%xPa, Temp=0x%xC, DATA_I2C_SDA#=%d\n", airprs, temp, RPI_I2C_PIN_SDA);
//		}
	}

	//求平均
	zHcuI2cAirprsBmp180 = airprsSum / RPI_I2C_READ_REPEAT_TIMES;
	zHcuI2cTempBmp180 = tempSum / RPI_I2C_READ_REPEAT_TIMES;

	//计算出海拔高度数据
	zHcuI2cAltitudeBmp180 = zHcuI2cAirprsBmp180 * 10;

	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BMP180 Transformed average float result Airprs=%6.2fPa, Temp=%6.2fC, Altitude = %6.2fm, DATA_I2C_SDA#=%d\n", zHcuI2cAirprsBmp180, zHcuI2cTempBmp180, zHcuI2cAltitudeBmp180, RPI_I2C_PIN_SDA);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//BM PD300 PM25读取
OPSTAT func_i2c_read_data_bmpd300(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd, i;
	int pm25;
	float pm25Sum;
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_BMPD300))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}

	pm25Sum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++){
		delay (200);
		delay (200);
		pm25 = wiringPiI2CReadReg16(fd, 0x00);
		pm25Sum += pm25;
		//计算算法待定
//		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("I2C: Sensor BMPD300 Original read result Pm25=0x%xmg/m3DATA_I2C_SDA#=%d\n", pm25, RPI_I2C_PIN_SDA);
//		}
	}

	//求平均
	zHcuI2cPm25Bmpd300 = pm25Sum / RPI_I2C_READ_REPEAT_TIMES;

	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BMPD300 Transformed average float result Pm25=%6.2fmg/m3, DATA_I2C_SDA#=%d\n", zHcuI2cPm25Bmpd300, RPI_I2C_PIN_SDA);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}
