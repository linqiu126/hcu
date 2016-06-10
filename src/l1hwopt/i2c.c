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
	int ret=0, conCounter=0;

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
	zHcuI2cPm25Bmpd300 = HCU_SENSOR_VALUE_NULL;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_I2C, FSM_STATE_I2C_RECEIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_I2C]++;
		HcuErrorPrint("I2C: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("I2C: Enter FSM_STATE_I2C_ACTIVED status, Keeping refresh here!\n");
	}

	int workingCycle = 4;
	//进入循环工作模式
	while(1){
		conCounter = 0;
		if (HCU_SENSOR_PRESENT_BMP180 == HCU_SENSOR_PRESENT_YES){
			func_i2c_read_data_bmp180();
			hcu_sleep(RPI_I2C_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_SHT20 == HCU_SENSOR_PRESENT_YES){
			func_i2c_read_data_sht20();
			hcu_sleep(RPI_I2C_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_BH1750 == HCU_SENSOR_PRESENT_YES){
			func_i2c_read_data_bh1750();
			hcu_sleep(RPI_I2C_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		if (HCU_SENSOR_PRESENT_BMPD300 == HCU_SENSOR_PRESENT_YES){
			func_i2c_read_data_bmpd300();
			hcu_sleep(RPI_I2C_SENSOR_READ_GAP/workingCycle);
			conCounter++;
		}
		conCounter = workingCycle-conCounter;
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/workingCycle * conCounter);
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
	INT32 fd, i;
	INT32 temp, humid;
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
	INT32 fd;
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
	INT32 fd, i;
	INT64 airprs, temp, x1, x2, x3, b5, b6, b7, p, tmp;
	UINT64 b4;
	INT64 airprsSum, tempSum;
	float coef;
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_BMP180))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}

	//读取校准数据 Read calibration data
	INT16 ac1, ac2, ac3, b1, b2, b3, mb, mc, md;
	UINT16 ac4, ac5, ac6;
	delay(2); ac1 = wiringPiI2CReadReg16(fd, 0xAA); ac1 = ((ac1&0xFF)<<8) + ((ac1&0xFF00)>>8);
	delay(2); ac2 = wiringPiI2CReadReg16(fd, 0xAC); ac2 = ((ac2&0xFF)<<8) + ((ac2&0xFF00)>>8);
	delay(2); ac3 = wiringPiI2CReadReg16(fd, 0xAE); ac3 = ((ac3&0xFF)<<8) + ((ac3&0xFF00)>>8);
	delay(2); ac4 = wiringPiI2CReadReg16(fd, 0xB0); ac4 = ((ac4&0xFF)<<8) + ((ac4&0xFF00)>>8);
	delay(2); ac5 = wiringPiI2CReadReg16(fd, 0xB2); ac5 = ((ac5&0xFF)<<8) + ((ac5&0xFF00)>>8);
	delay(2); ac6 = wiringPiI2CReadReg16(fd, 0xB4); ac6 = ((ac6&0xFF)<<8) + ((ac6&0xFF00)>>8);
	delay(2); b1 = wiringPiI2CReadReg16(fd, 0xB6); b1 = ((b1&0xFF)<<8) + ((b1&0xFF00)>>8);
	delay(2); b2 = wiringPiI2CReadReg16(fd, 0xB8); b2 = ((b2&0xFF)<<8) + ((b2&0xFF00)>>8);
	delay(2); mb = wiringPiI2CReadReg16(fd, 0xBA); mb = ((mb&0xFF)<<8) + ((mb&0xFF00)>>8);
	delay(2); mc = wiringPiI2CReadReg16(fd, 0xBC); mc = ((mc&0xFF)<<8) + ((mc&0xFF00)>>8);
	delay(2); md = wiringPiI2CReadReg16(fd, 0xBE); md = ((md&0xFF)<<8) + ((md&0xFF00)>>8);

	//循环读取
	tempSum = 0;
	airprsSum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++){
		//读取温度temp
		delay (10); wiringPiI2CWriteReg8(fd, 0xF4, 0x2E);
		delay(5); temp = wiringPiI2CReadReg16(fd, 0xF6);
		temp = ((temp&0xFF)<<8) + ((temp&0xFF00)>>8);
		tempSum += temp;
		//读取气压airprs
		delay (10); wiringPiI2CWriteReg8(fd, 0xF4, (0x34+(RPI_I2C_SENSOR_BMP180_OVER_SAMPLE_SET<<6)));
		delay(5); airprs = wiringPiI2CReadReg16(fd, 0xF6);
		delay(5); tmp = wiringPiI2CReadReg8(fd, 0xF8);
		airprs = ((((airprs&0xFF)<<16) + ((airprs&0xFF00)>>8) + (tmp&0xFF)) >> (8-RPI_I2C_SENSOR_BMP180_OVER_SAMPLE_SET));
		airprsSum += airprs;
//		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
//			HcuDebugPrint("I2C: Sensor BMP180 Original read result Airprs=0x%xPa, Temp=0x%xC, DATA_I2C_SDA#=%d\n", airprs, temp, RPI_I2C_PIN_SDA);
//		}
	}

	//求平均
	tempSum = tempSum / RPI_I2C_READ_REPEAT_TIMES;
	airprsSum = airprsSum / RPI_I2C_READ_REPEAT_TIMES;

	//计算真实温度
	x1 = ((tempSum - ac6) * ac5 ) >> 15;
	x2 = (mc << 11) / (x1 + md);
	b5 = x1 + x2;
	tempSum = ((b5 + 8) >> 4); //in 0.1degree
	zHcuI2cTempBmp180 = tempSum / 10; //in 1 degree

	//计算真实气压
	b6 = b5 - 4000;
	x1 = (b2 * ((b6 * b6)>>12)) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((ac1*4 + x3) << RPI_I2C_SENSOR_BMP180_OVER_SAMPLE_SET) + 2) >> 2;
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = ac4 * (UINT64)(x3 + 32768) >> 15;
	b7 = ((UINT64)airprsSum - b3) *(50000 >> RPI_I2C_SENSOR_BMP180_OVER_SAMPLE_SET);
	if (b7 < 0x80000000){
		p = (b7*2)/b4;
	}else{
		p = (b7/b4)*2;
	}
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p = p + ((x1 + x2 + 3791) << 4);

	zHcuI2cAirprsBmp180 = p; //in Parsca

	//计算出海拔高度数据
	coef = powf(((float)p/(float)RPI_I2C_SENSOR_BMP180_SEA_LEVEL_AIRPRESS), (float)(1.0/5.255));
	zHcuI2cAltitudeBmp180 = 44330 * (1- coef);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BMP180 Transformed average float result Airprs=%6.2fPa, Temp=%6.2fC, Altitude = %6.2fm, DATA_I2C_SDA#=%d\n", zHcuI2cAirprsBmp180, zHcuI2cTempBmp180, zHcuI2cAltitudeBmp180, RPI_I2C_PIN_SDA);
	}

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做, 以下only for test
	//float t = 44330 * (1- pow(69965/RPI_I2C_SENSOR_BMP180_SEA_LEVEL_AIRPRESS, 1/5.255));
    return SUCCESS;
#endif
}

//BM PD300 PM25读取
OPSTAT func_i2c_read_data_bmpd300(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	INT32 fd, i;
	INT32 pm25;
	float pm25Sum;
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_BMPD300))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}

	pm25Sum = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++){
		delay (200);
		//数据格式以及计算方法
		//读数格式为0x0B，0xXX, 0xXX，后两个
		if (wiringPiI2CReadReg8(fd, 0x00) == 0x0B){
			delay(200);
			pm25 = wiringPiI2CReadReg16(fd, 0x01);
			pm25 = ((pm25&0xFF)<<8) + ((pm25&0xFF00)>>8);
			pm25Sum += pm25;
		}
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
