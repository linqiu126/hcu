/*
 ============================================================================
 Name        : PrjHcwd.c
 Author      : ZHANG
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "hcwd.h"

//Main Entry
int main(int argc, char* argv[])
{
	if(wiringPiSetup()==-1)
		exit(1);

	//if(wiringPiSetupGpio()==-1)
		//exit(1);

	//循环工作模式
	while(1){
		sleep(3);
		func_hcwd_feed_ext_watchdog();
		func_hcwd_read_ext_trigger_and_soft_shut_down();
	}

	return EXIT_SUCCESS;
}

void func_hcwd_feed_ext_watchdog(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int i = 0;
	pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, OUTPUT);
	for (i=0; i<RPI_GPIO_READ_REPEAT_TIMES; i++){
		delayMicroseconds(100000);
		//sleep(1);
	    //pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, OUTPUT);
	    digitalWrite(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, 1);

		delayMicroseconds(100000);
	    //sleep(1);
	    //printf("test \n");
	    //pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, OUTPUT);
	    digitalWrite(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, 0);
	}
#endif
	return;
}

void func_hcwd_read_ext_trigger_and_soft_shut_down(void)
{
	if (func_hcwd_read_ext_soft_shut_trigger_value() == TRUE){
		time_t timer;  //time_t就是long int 类型
		struct tm *tblock;
		timer = time(NULL);
		tblock = localtime(&timer);
		printf("HCWD: Shutdown one time at local time = %s\n", asctime(tblock));

		while(1){
			system("shutdown -h now");
			sleep(1);
		}
	}
	return;
}


//TRUE表示低电平
bool func_hcwd_read_ext_soft_shut_trigger_value(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int i = 0;
	UINT8 readRes[RPI_GPIO_READ_REPEAT_TIMES];
	UINT8 total = 0;

    pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_SHUT_DOWN_TRIGGER, INPUT);
    pullUpDnControl(RPI_GPIO_PIN_EXT_WATCHDOG_SHUT_DOWN_TRIGGER, PUD_UP);
    delayMicroseconds(20);

    memset(readRes, 0, RPI_GPIO_READ_REPEAT_TIMES*sizeof(UINT8));
	for (i=0; i<RPI_GPIO_READ_REPEAT_TIMES; i++){
		delayMicroseconds(10);
		readRes[i] = digitalRead(RPI_GPIO_PIN_EXT_WATCHDOG_SHUT_DOWN_TRIGGER);
		total+=readRes[i];
	}

	//总和计算：低电平表示掉电，需要执行重启动
	if (total < (RPI_GPIO_READ_REPEAT_TIMES/2)) return TRUE;
	else
		return FALSE;
#endif
	return FALSE;
}


//以下是DHT11读取和存储的例子，供参考
UINT32 databuf;
OPSTAT func_gpio_read_data_ext_watchdog(void)
{
#ifdef TARGET_RASPBERRY_PI3B
	int i;
	float tmp1, tmp2, tempSum, humidSum;
    pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, OUTPUT);
    digitalWrite(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, 1);

	tempSum = 0;
	humidSum = 0;
	databuf = 0;
	for (i=0; i<RPI_GPIO_READ_REPEAT_TIMES; i++){
		delay(200);
        pinMode(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, OUTPUT);
        digitalWrite(RPI_GPIO_PIN_EXT_WATCHDOG_FEED, 1);
        delay(200); //wiringPi functions
        if(func_gpio_read_pin_data_from_ext_watchdog(RPI_GPIO_PIN_EXT_WATCHDOG_FEED))
        {
        	tmp1 =  (databuf>>8)&0xFF;
        	tmp2 = databuf&0xFF;
        	tempSum += tmp1 + tmp2/256;
        	tmp1 =  (databuf>>24)&0xFF;
        	tmp2 = (databuf>>16)&0xFF;
        	humidSum += tmp1 + tmp2/256;
            databuf=0;
        }
        else
        {
            databuf=0;
        }
    }

	//求平均
//	zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].fVal = tempSum / RPI_GPIO_READ_REPEAT_TIMES;
//	zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].updateTimeStamp = time(0);
//	zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOHUMIDDHT11].fVal = humidSum / RPI_GPIO_READ_REPEAT_TIMES;
//	zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOHUMIDDHT11].updateTimeStamp = time(0);
//	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
//    	HcuDebugPrint("GPIO: Sensor DHT11 Transformed float result Temp=%6.2fC, Humid=%6.2f\%, DATA_GPIO#=%d\n", zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOTEMPDHT11].fVal, zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOHUMIDDHT11].fVal, RPI_GPIO_PIN_EXT_WATCHDOG_FEED);
//	}

    return SUCCESS;
#else
    //对于其他平台, 暂时啥都不做
    return SUCCESS;
#endif
}

//读取温湿度DHT11的数据
#ifdef TARGET_RASPBERRY_PI3B
UINT8 func_gpio_read_pin_data_from_ext_watchdog(UINT32 pin)
{
	UINT8 crc;
	UINT8 i;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
    delay(25);
    digitalWrite(pin, 1);
    pinMode(pin, INPUT);
    pullUpDnControl(pin, PUD_UP);

    delayMicroseconds(27);
    if(digitalRead(pin)==0)
    {
        while(!digitalRead(pin));

        for(i=0;i<32;i++)
        {
            while(digitalRead(pin));
            while(!digitalRead(pin));
            delayMicroseconds(RPI_GPIO_HIGH_TIME);
            databuf*=2;
            if(digitalRead(pin)==1)
            {
                databuf++;
            }
        }

        for(i=0;i<8;i++)
        {
            while(digitalRead(pin));
            while(!digitalRead(pin));
            delayMicroseconds(RPI_GPIO_HIGH_TIME);
            crc*=2;
            if(digitalRead(pin)==1)
            {
                crc++;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif


