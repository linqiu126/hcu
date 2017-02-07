/*
 * gps.c
 *
 *  Created on: 2015年11月23日
 *      Author: test
 */

#include "../l1hwopt/gps.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*
** FSM of the GPS
*/
HcuFsmStateItem_t FsmGps[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_gps_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_gps_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_gps_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_GPS_RECEIVED,            		fsm_gps_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_GPS_RECEIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_GPS_RECEIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_GPS_RECEIVED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
GpsPosInfo_t zHcuGpsPosInfo;
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

//For Serial Port Init
//SerialPort_t gSerialPortForGPS = {zHcuSysEngPar.serialport.SeriesPortForGPS, zHcuSysEngPar.serialport.BautRateForMODBUSPort, 8, 'N', 1, HCU_INVALID_U16, 0, 1, 0};//initial config date for serial port
SerialPortCom_t gSerialPortForGPS;

//For GPS receive data
CHAR GPS_BUF[GPS_REC_BUFF_SIZE];

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_gps_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_GPS, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("GPS: Error Set FSM State at fsm_gps_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_gps_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_GPS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("GPS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_GPS], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_GPS, FSM_STATE_GPS_INITED) == FAILURE){
		HcuErrorPrint("GPS: Error Set FSM State!\n");
		return FAILURE;
	}


	//初始化硬件接口
	/*
	if (func_gps_int_init() == FAILURE){
		HcuErrorPrint("GPS: Error initialize interface!\n");
		return FAILURE;
	}
	*/

	//初始化全局变量
	memset(&zHcuGpsPosInfo, 0, sizeof(GpsPosInfo_t));
	zHcuGpsPosInfo.EW = 'E';
	zHcuGpsPosInfo.NS = 'N';

	gSerialPortForGPS.id = zHcuSysEngPar.serialport.SeriesPortForGPS;
	gSerialPortForGPS.nSpeed = HCU_SERIESPORT_BAUTRATE_DEFAULT;
	gSerialPortForGPS.nBits = 8;
	gSerialPortForGPS.nEvent = 'N';
	gSerialPortForGPS.nStop = 1;
	gSerialPortForGPS.fd = HCU_INVALID_U16;
	gSerialPortForGPS.vTime = HCU_INVALID_U8;
	gSerialPortForGPS.vMin = HCU_INVALID_U8;
	gSerialPortForGPS.c_lflag = 0;

	ret = hcu_sps485_serial_init(&gSerialPortForGPS);
	if (FAILURE == ret)
	{
		HcuErrorPrint("GPS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("GPS: Init Serial Port Success ...\n");
	}

	hcu_spsapi_SerialPortSetVtimeVmin(&gSerialPortForGPS, 10, 5);
	//HcuDebugPrint("GPS: SeriesPortForGPS = %d\n",zHcuSysEngPar.serialport.SeriesPortForGPS);
	HcuDebugPrint("GPS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPortForGPS.vTime, gSerialPortForGPS.vMin);


	zHcuSysStaPm.taskRunErrCnt[TASK_ID_GPS] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_GPS, FSM_STATE_GPS_RECEIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_GPS]++;
		HcuErrorPrint("GPS: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("GPS: Enter FSM_STATE_GPS_ACTIVED status, Keeping refresh here!\n");
	}

	UINT32 Gpsfd = gSerialPortForGPS.fd;

	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("GPS: fd = %d !\n", Gpsfd);
	}

	func_gps_receive(Gpsfd);



/*
			UINT32 i=0;
		    CHAR c;
		    CHAR buf[GPS_REC_BUFF_SIZE];
			UINT32 t = FAILURE;
		    memset(GPS_BUF,0,sizeof(GPS_BUF));

		    while (t)
		    {
		        if(read(Gpsfd,&c,1) < 0 )
		        {
		        	HcuDebugPrint("GPS: read Gpsdev error!\n");
		            return FAILURE;
		        }
				//HcuDebugPrint("GPS: Received GPS data c = %d\n", c);
		        buf[i++] = c;
		        if(c == '\n')
		        {
		            strncpy(GPS_BUF,buf,i);
		            i=0;
		        	//if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		        		//HcuDebugPrint("GPS: Received GPS data %S !\n", GPS_BUF);
		        	//}

		            func_gps_parse();

		        	//if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		        		//func_gps_print_info();
		        	//}
		        }
		        //hcu_sleep(5);
		     }
*/
	close(Gpsfd);

	return SUCCESS;
}

OPSTAT fsm_gps_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("GPS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_gps_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_gps_int_init(void)
{
	return SUCCESS;
}




/*

常用的GPS模块大多采用NMEA-0183 协议。NMEA-0183 是美国国家海洋电子协会(National Marine Electronics Association)所指定的标准规格，这一标准制订所有航海电子仪器间的通讯标准，其中包含传输资料的格式以及传输资料的通讯协议。

以下是一组正常的GPS 数据

$GPGGA,082006.000,3852.9276,N,11527.4283,E,1,08,1.0,20.6,M,,,,0000*35

$GPRMC,082006.000,A,3852.9276,N,11527.4283,E,0.00,0.0,261009,,*38

$GPVTG,0.0,T,,M,0.00,N,0.0,K*50

下面分别对每组数据的含义进行分析。

GPS 固定数据输出语句($GPGGA)，这是一帧GPS 定位的主要数据，也是使用最广的数据。为了便于理解，下面举例说明$GPGGA语句各部分的含义。

例：$GPGGA,082006.000,3852.9276,N,11527.4283,E,1,08,1.0,20.6,M,,,,0000*35

其标准格式为：

$GPGGA，(1)，(2)，(3)，(4)，(5)，(6)，(7)，(8)，(9)，M，(10)，M，(11)，(12)＊hh(CR)(LF)

各部分所对应的含义为：

(1) 定位UTC 时间：08 时20 分06 秒

(2) 纬度(格式ddmm.mmmm:即dd 度，mm.mmmm 分)；

(3) N/S(北纬或南纬)：北纬38 度52.9276 分；

(4) 经度(格式dddmm.mmmm：即ddd 度，mm.mmmm 分)；

(5) E/W(东经或西经)：东经115 度27.4283 分；

(6) 质量因子(0=没有定位，1=实时GPS，2=差分GPS)：1=实时GPS；

(7) 可使用的卫星数(0～8)：可使用的卫星数=08；

(8) 水平精度因子(1.0～99.9)；水平精度因子=1.0；

(9) 天线高程(海平面，－9999.9～99999.9，单位：m)；天线高程=20.6m);

(10) 大地椭球面相对海平面的高度(－999.9～9999.9，单位：m):无;

(11) 差分GPS 数据年龄，实时GPS 时无:无;

(12) 差分基准站号(0000～1023)，实时GPS 时无:无;

＊总和校验域；hh 总和校验数:35(CR)(LF)回车，换行。

GPRMC（建议使用最小GPS 数据格式）

$GPRMC,082006.000,A,3852.9276,N,11527.4283,E,0.00,0.0,261009,,*38

$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11><CR><LF>

(1) 标准定位时间（UTC time）格式：时时分分秒秒.秒秒秒（hhmmss.sss）。

(2) 定位状态，A = 数据可用，V = 数据不可用。

(3) 纬度，格式：度度分分.分分分分（ddmm.mmmm）。

(4) 纬度区分，北半球（N）或南半球（S）。

(5) 经度，格式：度度分分.分分分分。

(6) 经度区分，东（E）半球或西（W）半球。

(7) 相对位移速度， 0.0 至1851.8 knots

(8) 相对位移方向，000.0 至359.9 度。实际值。

(9) 日期，格式：日日月月年年（ddmmyy）。

(10) 磁极变量，000.0 至180.0。

(11) 度数。

(12) Checksum.(检查位)

$GPVTG 地面速度信息

例：$GPVTG,0.0,T,,M,0.00,N,0.0,K*50

字段0：$GPVTG，语句ID，表明该语句为Track Made Good and Ground Speed（VTG）地

面速度信息

字段1：运动角度，000 - 359，（前导位数不足则补0）

字段2：T=真北参照系

字段3：运动角度，000 - 359，（前导位数不足则补0）

字段4：M=磁北参照系

字段5：水平运动速度（0.00）（前导位数不足则补0）

字段6：N=节，Knots

字段7：水平运动速度（0.00）（前导位数不足则补0）

字段8：K=公里/时，km/h

字段9：校验值
*/

////////////////////////////////////////////////////////////////////////////////
//解释GPS发出的数据
//0      7  0   4 6   0     6 8 0        90         0  3      0        9
//$GPRMC,091400,A,3958.9870,N,11620.3278,E,000.0,000.0,120302,005.6,W*62
//$GPGGA,091400,3958.9870,N,11620.3278,E,1,03,1.9,114.2,M,-8.3,M,,*5E
void func_gps_parse()
////////////////////////////////////////////////////////////////////////////////
{
	UINT32 tmp;
    CHAR c;
    double latitude, longitude, speed, high;

    c = GPS_BUF[5];
    if(c=='C')
    {
        //"GPRMC"
    	zHcuGpsPosInfo.D.hour   =(GPS_BUF[ 7]-'0')*10+(GPS_BUF[ 8]-'0');
    	zHcuGpsPosInfo.D.minute =(GPS_BUF[ 9]-'0')*10+(GPS_BUF[10]-'0');
    	zHcuGpsPosInfo.D.second =(GPS_BUF[11]-'0')*10+(GPS_BUF[12]-'0');
        tmp = GetComma(9,GPS_BUF);
        zHcuGpsPosInfo.D.day    =(GPS_BUF[tmp+0]-'0')*10+(GPS_BUF[tmp+1]-'0');
        zHcuGpsPosInfo.D.month  =(GPS_BUF[tmp+2]-'0')*10+(GPS_BUF[tmp+3]-'0');
        zHcuGpsPosInfo.D.year   =(GPS_BUF[tmp+4]-'0')*10+(GPS_BUF[tmp+5]-'0')+2000;
        UTC2BTC(zHcuGpsPosInfo.D);
        //zHcuGpsPosInfo.D.hour = zHcuGpsPosInfo.D.hour +8;
        zHcuGpsPosInfo.D.second++;
        if(zHcuGpsPosInfo.D.second>59){
        	zHcuGpsPosInfo.D.second=0;
        	zHcuGpsPosInfo.D.minute++;
            if(zHcuGpsPosInfo.D.minute>59){
            	zHcuGpsPosInfo.D.minute=0;
            	zHcuGpsPosInfo.D.hour++;
            }
        }

        zHcuGpsPosInfo.D.hour += 8;

        if(zHcuGpsPosInfo.D.hour>23)
        {
        	zHcuGpsPosInfo.D.hour-=24;
        	zHcuGpsPosInfo.D.day+=1;
            if(zHcuGpsPosInfo.D.month==2 ||
            		zHcuGpsPosInfo.D.month==4 ||
					zHcuGpsPosInfo.D.month==6 ||
					zHcuGpsPosInfo.D.month==9 ||
					zHcuGpsPosInfo.D.month==11 ){
                if(zHcuGpsPosInfo.D.day>30){
                	zHcuGpsPosInfo.D.day=1;
                	zHcuGpsPosInfo.D.month++;
                }
            }
            else{
                if(zHcuGpsPosInfo.D.day>31){
                	zHcuGpsPosInfo.D.day=1;
                	zHcuGpsPosInfo.D.month++;
                }
            }
            if(zHcuGpsPosInfo.D.year % 4 == 0 ){
                if(zHcuGpsPosInfo.D.day > 29 && zHcuGpsPosInfo.D.month ==2){
                	zHcuGpsPosInfo.D.day=1;
                	zHcuGpsPosInfo.D.month++;
                }
            }
            else{
                if(zHcuGpsPosInfo.D.day>28 && zHcuGpsPosInfo.D.month ==2){
                	zHcuGpsPosInfo.D.day=1;
                	zHcuGpsPosInfo.D.month++;
                }
            }
            if(zHcuGpsPosInfo.D.month>12){
            	zHcuGpsPosInfo.D.month-=12;
            	zHcuGpsPosInfo.D.year++;
            }
        }

        zHcuGpsPosInfo.status   = GPS_BUF[GetComma(2,GPS_BUF)];
        latitude = get_locate(get_double_number(&GPS_BUF[GetComma(3,GPS_BUF)]));
        zHcuGpsPosInfo.gpsY = (UINT32)(latitude*100000);
        zHcuGpsPosInfo.NS       = GPS_BUF[GetComma(4,GPS_BUF)];
        longitude = get_locate(get_double_number(&GPS_BUF[GetComma(5,GPS_BUF)]));
        zHcuGpsPosInfo.gpsX = (UINT32)(longitude*100000);
        zHcuGpsPosInfo.EW       = GPS_BUF[GetComma(6,GPS_BUF)];
        speed = get_double_number(&GPS_BUF[GetComma(7,GPS_BUF)]);
        zHcuGpsPosInfo.speed = (UINT32)speed;

    }

    if(c=='A')
    {
        //"$GPGGA"
    	high = get_double_number(&GPS_BUF[GetComma(9,GPS_BUF)]);
    	zHcuGpsPosInfo.gpsZ = (UINT32)high;
    }
 }


//将获取文本信息转换为double型
double get_double_number(CHAR *s)
{
    CHAR buf[128];
    UINT32 i;
    double rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);

    return rev;
}

double get_locate(double temp)
{
	UINT32 m;
    double  n;
    m=(UINT32)temp/100;
    n=(temp-m*100)/60;
    n=n+m;
    return n;

}


//得到指定序号的逗号位置
UINT32 GetComma(UINT32 num,CHAR *str)
{
	UINT32 i,j=0;
    UINT32 len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')
        {
             j++;
        }

        if(j==num)
            return i+1;
    }
    return 0;
}



//将世界时转换为北京时
void UTC2BTC(hcuStrDateTime_t GPS)
{

//如果秒号先出,再出时间数据,则将时间数据+1秒
        GPS.second++;
        if(GPS.second>59){
            GPS.second=0;
            GPS.minute++;
            if(GPS.minute>59){
                GPS.minute=0;
                GPS.hour++;
            }
        }

        GPS.hour = GPS.hour + 8;

        if(GPS.hour>23)
        {
            GPS.hour-=24;
            GPS.day+=1;
            if(GPS.month==2 ||
                    GPS.month==4 ||
                    GPS.month==6 ||
                    GPS.month==9 ||
                    GPS.month==11 ){
                if(GPS.day>30){
                    GPS.day=1;
                    GPS.month++;
                }
            }
            else{
                if(GPS.day>31){
                    GPS.day=1;
                    GPS.month++;
                }
            }
            if(GPS.year % 4 == 0 ){
                if(GPS.day > 29 && GPS.month ==2){
                    GPS.day=1;
                    GPS.month++;
                }
            }
            else{
                if(GPS.day>28 && GPS.month ==2){
                    GPS.day=1;
                    GPS.month++;
                }
            }
            if(GPS.month>12){
                GPS.month-=12;
                GPS.year++;
            }

        }
}

void func_gps_receive(UINT32 Gpsfd)
{
	UINT32 i=0;
    CHAR c;
    CHAR buf[GPS_REC_BUFF_SIZE];
	UINT32 t = FAILURE;
    memset(GPS_BUF,0,sizeof(GPS_BUF));

    while (t)
    {
        if(read(Gpsfd,&c,1) < 0 )
        {
        	HcuDebugPrint("GPS: read Gpsdev error!\n");
            return ;
        }

        buf[i++] = c;
        if(c == '\n')
        {
            strncpy(GPS_BUF,buf,i);
            i=0;

        	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
        		//HcuDebugPrint("GPS: Received GPS data %S !\n", GPS_BUF);
        	}
            //HcuDebugPrint("%s",GPS_BUF);
            func_gps_parse();

        	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
        		//func_gps_print_info();
        	}
        }
        //hcu_sleep(5);
     }
}


void func_gps_print_info(void)
{

	HcuDebugPrint("GPS: NS is %c.\n", zHcuGpsPosInfo.NS);
	HcuDebugPrint("GPS: latitude is %d.\n", zHcuGpsPosInfo.gpsY);
	HcuDebugPrint("GPS: EW is %c.\n", zHcuGpsPosInfo.EW);
	HcuDebugPrint("GPS: longitude is %d.\n", zHcuGpsPosInfo.gpsX);
	HcuDebugPrint("GPS: Status is %c.\n", zHcuGpsPosInfo.status);
	HcuDebugPrint("GPS: speed is %d.\n", zHcuGpsPosInfo.speed);
	HcuDebugPrint("GPS: high is %d.\n\n\n", zHcuGpsPosInfo.gpsZ);
	HcuDebugPrint("GPS: year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d\n", zHcuGpsPosInfo.D.year, zHcuGpsPosInfo.D.month, zHcuGpsPosInfo.D.day, zHcuGpsPosInfo.D.hour, zHcuGpsPosInfo.D.minute, zHcuGpsPosInfo.D.second);

}

