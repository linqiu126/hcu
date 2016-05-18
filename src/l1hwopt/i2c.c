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
	zHcuI2cTempSht20 = 0;
	zHcuI2cHumidSht20 = 0;

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
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/3);
		func_i2c_read_data_bh1750();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/3);
		func_i2c_read_data_bmp180();
		hcu_sleep(RPI_I2C_SENSOR_READ_GAP/3);
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
	int fd;
	int temp, humid;
	float tmp1, tmp2;
	//short int ax,ay,az,tmp,gx,gy,gz;
//	pinMode(8,I2C_PIN);
//	pinMode(9,I2C_PIN);
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_SHT20))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}
	//wiringPiI2CWriteReg8(fd, RPI_I2C_ADDR_SHT20, 00);
	delay (1000);
	temp = wiringPiI2CReadReg16(fd, 0xE3);
	humid = wiringPiI2CReadReg16(fd, 0xE5);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor SHT20 Original read result Temp=0x%xC, Humid=0x%x\%, DATA_GPIO#=%d\n", temp, humid, RPI_I2C_PIN_SHT20_SDA);
	}
	tmp1 = (temp>>8)&0xFF;
	tmp2 = ((temp&0xFF)<<8)&0xFF00;
	zHcuI2cTempSht20 = (tmp1 + tmp2) * 175.72 / 1024 / 64 - 46.85;
	tmp1 = (humid>>8)&0xFF;
	tmp2 = ((humid&0xFF)<<8)&0xFF00;
	zHcuI2cHumidSht20 = (tmp1 + tmp2) * 125 / 1024 / 64 - 6;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor SHT20 Transformed float result Temp=%6.2fC, Humid=%6.2f\%, DATA_I2C#=%d\n", zHcuI2cTempSht20, zHcuI2cHumidSht20, RPI_I2C_PIN_SHT20_SDA);
	}

	/*while(1){
		ax=wiringPiI2CReadReg8(fd,0x3B)<<8|wiringPiI2CReadReg8(fd,0x3C);
		ay=wiringPiI2CReadReg8(fd,0x3D)<<8|wiringPiI2CReadReg8(fd,0x3E);
		az=wiringPiI2CReadReg8(fd,0x3F)<<8|wiringPiI2CReadReg8(fd,0x40);
		gx=wiringPiI2CReadReg8(fd,0x43)<<8|wiringPiI2CReadReg8(fd,0x44);
		gy=wiringPiI2CReadReg8(fd,0x45)<<8|wiringPiI2CReadReg8(fd,0x46);
		gz=wiringPiI2CReadReg8(fd,0x47)<<8|wiringPiI2CReadReg8(fd,0x48);
		HcuDebugPrint("I2C: ax=%d  ay=%d  az=%d\n",ax,ay,az);
		HcuDebugPrint("I2C: gx=%d  gy=%d  gz=%d\n",gx,gy,gz);
		delay(100);
	}*/
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
	char buf[RPI_I2C_READ_REPEAT_TIMES][3];
	char val;
	float flight[RPI_I2C_READ_REPEAT_TIMES], tmp;

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
		HcuErrorPrint("I2C: write 0x01 err\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
	}

	val=0x10;
	if(write(fd, &val,1)<0)
	{
		HcuErrorPrint("I2C: write 0x10 err\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
	}

	int i=0;
	tmp = 0;
	for (i=0; i<RPI_I2C_READ_REPEAT_TIMES; i++)
	{
		flight[i]=0;
		if(read(fd, &buf[i][0], 3))
		{
			flight[i]=(buf[i][0]*256+buf[i][1])/1.2;
			if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
				HcuDebugPrint("I2C: Sensor BH1750 Original float result light = %6.2f lx\n", flight[i]);
			}
			tmp+= flight[i];
		}
		else
		{
			HcuErrorPrint("I2C: read light error\n");
			zHcuRunErrCnt[TASK_ID_I2C]++;
			close(fd);
			return FAILURE;
		}
		hcu_usleep(100000);//sleep 0.1s
	}
	//输出结果到目标变量中
	zHcuI2cLightstrBh1750 = tmp/RPI_I2C_READ_REPEAT_TIMES;
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BH1750 Transformed float result light = %6.2f lx\n", zHcuI2cLightstrBh1750);
	}
	close(fd);

	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//气压GY68
OPSTAT func_i2c_read_data_bmp180(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int fd;
	int airprs;
	if((fd=wiringPiI2CSetup(RPI_I2C_ADDR_BMP180))<0){
		HcuDebugPrint("I2C: can't find i2c!!\n");
		zHcuRunErrCnt[TASK_ID_I2C]++;
		return FAILURE;
	}
	delay (1000);
	airprs = wiringPiI2CReadReg16(fd, 0x00);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("I2C: Sensor BMP180 Original read result Airprs=0x%xPa\n", airprs);
	}
	zHcuI2cAirprsBmp180 = airprs;
	return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

