///*
// ============================================================================
// Name        : TestHellopWorld.c
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
// ============================================================================
// */
//
//#include <stdio.h>
//#include <stdlib.h>
//
//int main(void) {
//
//
//}

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//#ifndef WIN32
//#include <errno.h>
//#include <unistd.h>
//#include <net/if.h>
//#include <sys/ioctl.h>
//#include <sys/shm.h>
//#include <sys/socket.h>
//#include <sys/stat.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <termios.h>
//#else
//#include <io.h>
//#include <time.h>
//#include "windows.h"
//#endif
//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"
//#include "MQTTClient.h"
//
//#define ADDRESS     "tcp://localhost:1883"
//#define CLIENTID    "ExampleClientPub"
//#define TOPIC       "MQTT Examples"
//#define PAYLOAD     "Hello World!"
//#define QOS         1
//#define TIMEOUT     10000L
//
//int main(int argc, char* argv[])
//{
//    MQTTClient client;
//    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//    MQTTClient_message pubmsg = MQTTClient_message_initializer;
//    MQTTClient_deliveryToken token;
//    int rc;
//
//    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
//    conn_opts.keepAliveInterval = 20;
//    conn_opts.cleansession = 1;
//
//    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
//    {
//        printf("Failed to connect, return code %d\n", rc);
//        exit(-1);
//    }
//    pubmsg.payload = PAYLOAD;
//    pubmsg.payloadlen = strlen(PAYLOAD);
//    pubmsg.qos = QOS;
//    pubmsg.retained = 0;
//    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
//    printf("Waiting for up to %d seconds for publication of %s\n"
//            "on topic %s for client with ClientID: %s\n",
//            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
//    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
//    printf("Message with delivery token %d delivered\n", token);
//    MQTTClient_disconnect(client, 10000);
//    MQTTClient_destroy(&client);
//    return rc;
//}



/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}*/

/** @endcode
*
* @page pubasync Asynchronous publication example
@code{.c}*/
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payloadptr;

  printf("Message arrived\n");
  printf("     topic: %s\n", topicName);
  printf("   message: ");

  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++)
  {
      putchar(*payloadptr++);
  }
  putchar('\n');
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void connlost(void *context, char *cause)
{
  printf("\nConnection lost\n");
  printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  int rc;

  MQTTClient_create(&client, ADDRESS, CLIENTID,
      MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
      printf("Failed to connect, return code %d\n", rc);
      exit(EXIT_FAILURE);
  }
  pubmsg.payload = PAYLOAD;
  pubmsg.payloadlen = strlen(PAYLOAD);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;
  deliveredtoken = 0;
  MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
  printf("Waiting for publication of %s\n"
          "on topic %s for client with ClientID: %s\n",
          PAYLOAD, TOPIC, CLIENTID);
  while(deliveredtoken != token);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}*/

/*** @endcode
* @page subasync Asynchronous subscription example
@code*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payloadptr;

  printf("Message arrived\n");
  printf("     topic: %s\n", topicName);
  printf("   message: ");

  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++)
  {
      putchar(*payloadptr++);
  }
  putchar('\n');
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void connlost(void *context, char *cause)
{
  printf("\nConnection lost\n");
  printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;

  MQTTClient_create(&client, ADDRESS, CLIENTID,
      MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
      printf("Failed to connect, return code %d\n", rc);
      exit(EXIT_FAILURE);
  }
  printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
         "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC, QOS);

  do
  {
      ch = getchar();
  } while(ch!='Q' && ch != 'q');

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}












//int main(int argc, char **argv)
//{
//
//
//	return 1;
//}
//



//#define PDR_PIC_X_WIDTH 960
//#define PDR_PIC_Y_HEIGHT 540
//typedef struct strPdrPixel{
//	unsigned char R;
//	unsigned char G;
//	unsigned char B;
//}strPdrPIxel_t;
//strPdrPIxel_t zStg1PicSource[PDR_PIC_Y_HEIGHT][PDR_PIC_X_WIDTH];
//unsigned char zStg2GrayPic[PDR_PIC_Y_HEIGHT][PDR_PIC_X_WIDTH];
//unsigned char zStg3BinPicture[PDR_PIC_Y_HEIGHT][PDR_PIC_X_WIDTH];
//char zInputFile[256];
//#define PDR_FILE_EXTENTION_1 ".bmp"
//#define PDR_FILE_EXTENTION_2 ".BMP"
//#define PDR_STATISTIC_DIM  20
//float zStgStatisticFigure[PDR_STATISTIC_DIM];
//
//void picture_pre_distribution_process(unsigned int width, unsigned int height);


//	unsigned int i=0, j=0;
//	char strTemp[10];
//	FILE *fp;
//
//	//初始化
//	memset(zStg1PicSource, 0, sizeof(zStg1PicSource));
//	memset(zStg2GrayPic, 0, sizeof(zStg2GrayPic));
//	memset(zStg3BinPicture, 0, sizeof(zStg3BinPicture));
//	memset(zInputFile, 0, sizeof(zInputFile));
//
//	//输出测试开始
//	puts("Test starting...");
//
//	//扫描得到输入的字符串
//	//scanf("%s%s", &s1, &s2);
//
//	//得到输入文件，否则退出
//	if (argc <2) {printf("No input file, exit!\n"); exit(1); }
//	else {printf("Your input parameter = [%s]\n", argv[1]); }
//
//	//分析文件名，如果不是BMP文件，则退出
//	strncpy(zInputFile,  argv[1], sizeof(zInputFile)-1);
//	memset(strTemp, 0, sizeof(strTemp));
//	strncpy(strTemp, zInputFile + strlen(zInputFile)-4, 4);
//	printf("Your input file extension = [%s]\n",strTemp);
//
//	int res1=0, res2=0;
//	res1 = strcmp(strTemp, PDR_FILE_EXTENTION_1);
//	res2 = strcmp(strTemp, PDR_FILE_EXTENTION_2);
//	if ((res1 !=0) && (res2 !=0 ))
//	{printf("Not %s or %s files, exit!\n", PDR_FILE_EXTENTION_1, PDR_FILE_EXTENTION_2); exit(1); }
//	if ((fp = fopen(zInputFile, "rb")) == NULL){ printf("Open file error, exit!\n"); exit(1); }
//
//	//拷贝到目标字符串上来
//	printf("Your input file handler = [%d]\n", (int)fp);
//
//	//获取文件的格式：假设文件的宽高各为多少
//	unsigned int IncomingWidth = PDR_PIC_X_WIDTH;
//	unsigned int IncomingHeight = PDR_PIC_Y_HEIGHT;
//	//feof(fp)
//	for(i=0; i<IncomingHeight; i++)
//	 {
//		for (j=0; j<IncomingWidth; j++){
//			if(fread(&zStg1PicSource[i][j], sizeof(strPdrPIxel_t), 1, fp) != 1){
//				printf("File read error\n");
//				fclose(fp);
//				exit(1);
//			}
//		}
//	 }
//	fclose(fp);
//	printf("Original Pic Data => \n");
//	//检查并打印
///*    for(j = 0; j < IncomingHeight; j++){
//    	printf("Picture read column [%d],", j);
//    	for (i=0; i<IncomingWidth; i++) {printf(" %02x %02x %02x", zStg1PicSource[j][i].R, zStg1PicSource[j][i].G, zStg1PicSource[j][i].B);}
//    	printf(", Size = %d\n", sizeof(zStg1PicSource[j]));
// 	 }*/
//
//    //图像预处理
//    picture_pre_distribution_process(IncomingWidth, IncomingHeight);
//
//    //算法处理

//void picture_pre_distribution_process(unsigned int width, unsigned int height)
//{
//	int i=0, j=0;
//    double tResultAvg = 0;
//    double tResultVar = 0;
//    double tt;
//    unsigned int iRes = 0, iVar = 0;
//    //求统计均值
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			tResultAvg += (((float)zStg1PicSource[j][i].R + (float)zStg1PicSource[j][i].G + (float)zStg1PicSource[j][i].B))/3;
//		}
//    }
//    tResultAvg = tResultAvg/width/height;
//    iRes = (unsigned long)(tResultAvg+0.5) & 0xFFFFFFFF;
//    //求绝对方差
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			tt = ((float)zStg1PicSource[j][i].R + (float)zStg1PicSource[j][i].G +(float)zStg1PicSource[j][i].B)/3 - tResultAvg;
//			if (tt >=0) tResultVar += tt;
//			else tResultVar -=tt;
//		}
//    }
//    tResultVar = tResultVar/width/height;
//    iVar = (unsigned long)(tResultVar+0.5) & 0xFFFFFFFF;
//    //求统计方差分布情况
//    float difBase = tResultVar;
//    memset(zStgStatisticFigure, 0, sizeof(zStgStatisticFigure));
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			int t2 = 0;
//			t2 = (int)(((float)(zStg1PicSource[j][i].R + zStg1PicSource[j][i].G + zStg1PicSource[j][i].B) - tResultAvg) / difBase +0.5);
//			t2 = t2 + (int)PDR_STATISTIC_DIM/2;
//			if (t2 < 0) zStgStatisticFigure[0]++;
//			else if (t2 >= PDR_STATISTIC_DIM) zStgStatisticFigure[PDR_STATISTIC_DIM-1]++;
//			else zStgStatisticFigure[t2]++;
//		}
//    }
//    //计算百分比
//    for (i=0; i<PDR_STATISTIC_DIM; i++) {
//    	zStgStatisticFigure[i] = zStgStatisticFigure[i]/width/height*100;
//    }
//    printf("Color Picture Statistic Result, Average = [%d], Var = [%d], \nDistribution: ", iRes, iVar);
//    for (i=0; i<PDR_STATISTIC_DIM; i++) {
//    	printf("[%d-%5.2f\%] ", i, zStgStatisticFigure[i]);
//    }
//    printf("\n");
//
//    //灰度化处理
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			zStg2GrayPic[j][i] = (unsigned char)(0.3*(float)zStg1PicSource[j][i].R + 0.51*(float)zStg1PicSource[j][i].G + 0.19*(float)zStg1PicSource[j][i].B);
//		}
//    }
//    //求统计均值
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			tResultAvg += (float)zStg2GrayPic[j][i];
//		}
//    }
//    tResultAvg = tResultAvg/width/height;
//    iRes = (unsigned int)(tResultAvg+0.5) & 0xFF;
//    //求绝对方差
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			tt = (float)zStg2GrayPic[j][i] - tResultAvg;
//			if (tt >=0) tResultVar += tt;
//			else tResultVar -=tt;
//		}
//    }
//    tResultVar = tResultVar/width/height;
//    iVar = (unsigned long)(tResultVar+0.5) & 0xFFFFFFFF;
//    //求统计方差分布情况
//    memset(zStgStatisticFigure, 0, sizeof(zStgStatisticFigure));
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			int t2 = 0;
//			t2 = (int)((float)zStg2GrayPic[j][i] +0.5);
//			t2 = t2 + (int)PDR_STATISTIC_DIM/2;
//			if (t2 < 0) zStgStatisticFigure[0]++;
//			else if (t2 >= PDR_STATISTIC_DIM) zStgStatisticFigure[PDR_STATISTIC_DIM-1]++;
//			else zStgStatisticFigure[t2]++;
//		}
//    }
//    //计算百分比
//    for (i=0; i<PDR_STATISTIC_DIM; i++) {
//    	zStgStatisticFigure[i] = zStgStatisticFigure[i]/width/height*100;
//    }
//    printf("Color Picture Statistic Result, Average = [%d], Var = [%d], \nDistribution: ", iRes, iVar);
//    for (i=0; i<PDR_STATISTIC_DIM; i++) {
//    	printf("[%d-%5.2f\%] ", i, zStgStatisticFigure[i]);
//    }
//    printf("\n");
//
//
//
//
//    //二值处理
//    for (j=0; j<height; j++){
//		for (i=0; i<width; i++){
//			if (abs((zStg1PicSource[j][i].R + zStg1PicSource[j][i].G + zStg1PicSource[j][i].B) - iRes) <= iVar){
//				zStg3BinPicture[j][i] = 0;
//			}
//			else{
//				zStg3BinPicture[j][i] = 1;
//			}
//		}
//    }
//
//
//    //检查并打印
//    printf("Convert result => \n");
///*    for(j = 0; j < height; j++){
//    	printf("Picture read column [%d],", j);
//    	for (i=0; i<width; i++){
//            printf(" %02x", zStg2BinPicture[j][i]);
//    	}
//    	printf("\n");
// 	 }*/
//}



///**
// * cantool.c
// *
// * Socket CAN工具程序
// *
// * version: Ver1.0
// * Date   : 2015.11.16
// * Decrip : 原始版本
// */
//
//
//#ifndef WIN32
//#include "TestHellopWorld.h"
//#include "raw.h" // for CAN_RAW_FILTER
//#endif
//
//#define _VERSION_   "Ver1.0"
//#define _OWNER      "reille @ http://velep.com/"
//
//#define true        1
//#define false       0
//
//typedef __u8        bool;
//typedef __u8        byte;
//
//typedef union u_canframe_data
//{
//    __u8            data[8];
//
//    struct
//    {
//        __u32       dl;
//        __u32       dh;
//    } s;
//} u_canframe_data_t;
//
//static int  send_socket_fd = -1;
//static int  recv_socket_fd = -1;
//
//// -----------------------------------------------------------------------------
//
//static void panic(const char *msg)
//{
//    printf(msg);
//    exit(1);
//}
//
//void delay_ms(const __u32 ms)
//{
//    struct timespec tv;
//
//    tv.tv_sec = ms / 1000;
//    tv.tv_nsec = (long)(ms % 1000) * 1000000;
//
//    nanosleep(&tv, NULL);
//}
//
//// 判断字符串是否为数字
//// h@ex: 是否包含十六进制数据
//// @point: 小数点是否有效
//bool str_isdigit(const char *s, const bool hex, const bool point)
//{
//    int i = 0;
//
//    if (s == NULL || strlen(s) == 0)
//    {
//        return  false;
//    }
//
//    while (s[i] != '\0')
//    {
//        if (s[i] >='0' && s[i] <= '9' || (point && s[i] == '.') ||
//            (hex && (s[i] >= 'a' && s[i] <= 'f')) || (hex && (s[i] >= 'A' && s[i] <= 'F')))
//        {
//            i++;
//        }
//        else
//        {
//            return  false;
//        }
//    }
//
//    return  true;
//}
//
////Function:1-byte ASC char converts to 1-byte unsigned char number
////In:
////	i_cAscNumber:the 1-byte ASC char to be converted ('0'~'9','A'~'F')
////Out:
////	no
////Return:
////	the converting 1-byte unsigned char number(0x0~0x0F)
////note:
////	'8' --> 0x08	'D' --> 0x0D
//unsigned char Char_No1Hex(char i_cAscChar)
//{
//    unsigned char c;
//
//    if (i_cAscChar>='A') {
//        c=i_cAscChar-'A'+10;
//    }else {
//        c=i_cAscChar-48;
//    }
//    c%=16;
//
//    return c;
//}
//
////Function:2-byte ASC string converts to 1-byte unsigned char number
////In:
////	i_cpStr:pointer of the 2-byte ASC number string("00"~"FF")
////Out:
////	no
////Return:
////	the converting 1-byte unsigned char number
////note:
////	"28" --> 0x28	"7D" --> 0x7D	"AB" --> 0xAB
//unsigned char String_Asc2TouChar(char *i_cpStr)
//{
//    unsigned char c;
//
//    c = Char_No1Hex(i_cpStr[0])*0x10+Char_No1Hex(i_cpStr[1]);
//
//    return c;
//
//}
//
////Function:8-byte ASC string converts to 4-byte unsigned long number
////In:
////	i_cpStr:pointer of the 8-byte ASC number string("00000000"~"FFFFFFFF")
////Out:
////	no
////Return:
////	the 4-byte converting unsigned long number
////note:if ASC sring length >8,the get front 8 bytes
////	"00000072" --> 0x72 "789" --> 0x789 "12AC7850" --> 0x12AC7850 "12AC789950" --> 0x12AC7899
//unsigned long String_Asc8TouLong(char *i_cpStr)
//{
//    int i,j;
//    unsigned long lRet,value;
//    unsigned char c;
//    char tmp_cStr[9];
//
//    for(j=0;j<9;j++) tmp_cStr[j]=0x30;
//    j=(int)strlen(i_cpStr);
//    if( j<8 ){
//        for(i=0;i<j;i++)
//            tmp_cStr[8-j+i]=i_cpStr[i];
//    }
//    else{
//        for(i=0;i<8;i++)
//            tmp_cStr[i]=i_cpStr[i];
//
//    }
//    value=1;
//    lRet=0;
//    for (i=7;i>=0;i--)
//    {
//        if (tmp_cStr[i]>=65)
//            c=tmp_cStr[i]-65+10;
//        else
//            c=tmp_cStr[i]-48;
//        lRet+=value*c;
//        value=value*16;
//    }
//
//    return lRet;
//}
//
//// -----------------------------------------------------------------------------
//
//typedef struct sys_error_frame
//{
//    __u32       id;
//    char        *err_info;
//} s_sys_error_frame_t;
//
//static char g_errframe_info_buf[256] = {0};
//static const s_sys_error_frame_t s_sys_error_frame[] = {
//    { CAN_ERR_TX_TIMEOUT, "TX timeout (by netdevice driver)"    },
//    { CAN_ERR_LOSTARB,    "lost arbitration    / data[0]"       },
//    { CAN_ERR_CRTL,       "controller problems / data[1]"       },
//    { CAN_ERR_PROT,       "protocol violations / data[2..3]"    },
//    { CAN_ERR_TRX,        "transceiver status  / data[4]"       },
//    { CAN_ERR_ACK,        "received no ACK on transmission"     },
//    { CAN_ERR_BUSOFF,     "bus off"                             },
//    { CAN_ERR_BUSERROR,   "bus error (may flood!)"              },
//    { CAN_ERR_RESTARTED,  "controller restarted"                },
//
//    { 0,                  NULL},
//};
//
//bool is_error_frame(const __u32 id)
//{
//    int i = 0;
//
//    while (s_sys_error_frame[i].id && s_sys_error_frame[i].err_info != NULL)
//    {
//        if (id == s_sys_error_frame[i].id)
//        {
//            return  true;
//        }
//
//        i++;
//    }
//    return  false;
//}
//
//const char* get_errframe_info(const __u32 id, const char* data, const __u8 dlc)
//{
//    int l = 0;
//    bool unspecified = false;
//    char *p = g_errframe_info_buf;
//
//    switch (id)
//    {
//    case CAN_ERR_TX_TIMEOUT:    sprintf(p, "发送超时");         break;
//    case CAN_ERR_LOSTARB:       sprintf(p, "总线仲裁错误");     break;
//    case CAN_ERR_CRTL:
//        if (data && dlc >= 2)
//        {
//            switch (data[1])
//            {
//            case CAN_ERR_CRTL_RX_OVERFLOW:  l += sprintf(p + l, "接收缓存溢出");    break;
//            case CAN_ERR_CRTL_TX_OVERFLOW:  l += sprintf(p + l, "发送缓存溢出");    break;
//            case CAN_ERR_CRTL_RX_WARNING:   l += sprintf(p + l, "接收报警");        break;
//            case CAN_ERR_CRTL_TX_WARNING:   l += sprintf(p + l, "发送报警");        break;
//            case CAN_ERR_CRTL_RX_PASSIVE:   l += sprintf(p + l, "接收被动错误");    break;
//            case CAN_ERR_CRTL_TX_PASSIVE:   l += sprintf(p + l, "发送被动错误");    break;
//            default:  unspecified = true;   break; /* unspecified */
//            }
//        }
//        else
//        {
//            unspecified = true;
//        }
//
//        if (unspecified)
//        {
//            l += sprintf(p + l, "CAN控制器错误");
//        }
//        break;
//
//    case CAN_ERR_PROT:
//        if (data && dlc >= 4)
//        {
//            if (data[2])
//            {
//                switch (data[2])
//                {
//                case CAN_ERR_PROT_BIT:      l += sprintf(p + l, "位错误");           break;
//                case CAN_ERR_PROT_FORM:     l += sprintf(p + l, "帧格式错误");       break;
//                case CAN_ERR_PROT_STUFF:    l += sprintf(p + l, "位填充错误");       break;
//                case CAN_ERR_PROT_BIT0:     l += sprintf(p + l, "(接收站)不能发送占有位(dominant bit)"); break;
//                case CAN_ERR_PROT_BIT1:     l += sprintf(p + l, "(发送站)不能发送空闲位(recessive bit)"); break;
//                case CAN_ERR_PROT_OVERLOAD: l += sprintf(p + l, "总线超负荷");       break;
//                case CAN_ERR_PROT_ACTIVE:   l += sprintf(p + l, "主动错误");         break;
//                case CAN_ERR_PROT_TX:       l += sprintf(p + l, "传输错误");         break;
//                default: unspecified = true;   break; /* unspecified */
//                }
//            }
//            else
//            {
//                unspecified = true;
//            }
//
//            if (unspecified && data[3])
//            {
//                switch (data[3])
//                {
//                //case CAN_ERR_PROT_LOC_SOF:  l += sprintf(p + l, "");         break;
//                default: unspecified = true;   break; /* unspecified */
//                }
//            }
//        }
//        else
//        {
//            unspecified = true;
//        }
//
//        if (unspecified)
//        {
//            l += sprintf(p + l, "协议违反");
//        }
//        break;
//
//    case CAN_ERR_TRX:
//        break;
//
//    case CAN_ERR_ACK:       sprintf(p, "应答错误");               break;
//    case CAN_ERR_BUSOFF:    sprintf(p, "总线关闭");               break;
//    case CAN_ERR_BUSERROR:  sprintf(p, "总线错误(可能溢出)");     break;
//    case CAN_ERR_RESTARTED: sprintf(p, "CAN控制器重启");          break;
//
//    default: break;
//    }
//
//    return  (const char *)p;
//}
//
//static void usage(void)
//{
//    int i = 0;
//
//    printf("\n");
//    printf("SOCKET CAN工具程序 - %s Build %s, COPYRIGHT (C) 2015 %s\n", _VERSION_, __DATE__, _OWNER);
//    printf("\n");
//
//    printf("介绍:  \n");
//    printf("    本SOCKET CAN程序具备全部CAN功能，包括CAN标准帧/扩展帧接收与发送、CAN总线错误判断、环回等功能\n");
//    printf("    适用基于LINUX SOCKET机制实现的CAN接口，可用于嵌入式LINUX中的CAN测试程序\n");
//    printf("    程序采用标准LINUX命令行参数选项形式，接受用户参数\n");
//    printf("\n");
//
//    printf("用法: ./cantool [选项]...\n\n");
//
//    printf("选项:  \n");
//    printf("    -p, --port=CAN接口号        指定CAN接口号，从1开始，     默认为 1（即CAN1接口）\n");
//    printf("    -b, --baud=波特率           指定CAN通讯波特率，单位Kbps，默认为 250 Kbps\n");
//    printf("                                可用波特率：5，10，20，40，50，80，100，125，200，250，400，500，666，800，1000\n");
//    printf("\n");
//
//    printf("    -i, --frame-id=帧ID         指定CAN发送帧ID（Hex格式）， 默认为1801F456\n");
//    printf("    -d, --data=数据             指定CAN发送帧数据，          默认为：00 01 FF FF FF FF FF FF，字节数据间以空格隔开\n");
//    printf("    -f, --freq=间隔             指定CAN帧发送间隔，单位ms，  默认为250ms, 最小值为1ms\n");
//    printf("    -t, --times=次数            指定CAN帧发送次数，          默认为0次\n");
//    printf("    -s,                         指定CAN发送帧为标准帧，      默认为发送扩展帧\n");
//    printf("    -I,                         帧ID每发送一帧递增，         默认不递增\n");
//    printf("    -g,                         发送数据每发送一帧递增，     默认不递增\n");
//    printf("    -l,                         发送数据时本地环回，         默认不环回\n");
//    printf("\n");
//
//    printf("        --help                  显示此帮助信息并退出\n");
//    printf("\n");
//
//    printf("注意，以下CAN帧ID作为系统使用:\n");
//    while (s_sys_error_frame[i].id && strlen(s_sys_error_frame[i].err_info))
//    {
//    printf("    0x%08X - %s\n", s_sys_error_frame[i].id, s_sys_error_frame[i].err_info);
//    i++;
//    }
//    printf("\n");
//
//    printf("    使用 Ctrl^C 组合键结束程序运行\n");
//    printf("\n");
//}
//
//// 检查选项是否需要用户输入值
//bool option_no_value(const char *option)
//{
//    int i = 0;
//    const char *options[] = {"-s", "-I", "-g", "-l", ""};
//
//    while (strlen(options[i]))
//    {
//        if (!strcmp(option, options[i]))
//        {
//            return  true;
//        }
//
//        i++;
//    }
//
//    return  false;
//}
//
//// -----------------------------------------------------------------------------
//
//bool find_can(const int port)
//{
//    char buf[128] = {0};
//    sprintf(buf, "/sys/class/net/can%d/can_bittiming/bitrate", port);
//    return  ((access(buf, 0) == 0));
//}
//
///*
// *	bitrate: 250000 or 125000
// */
//void set_bitrate(const int port, const int bitrate)
//{
//#define TX_QUEUE_LEN		4096 // 使用足够多的发送缓存
//
//#ifndef WIN32
//	char	l_c8Command[128] = {0};
//
//	sprintf(l_c8Command, "echo %d > /sys/class/net/can%d/can_bittiming/bitrate", bitrate, port);
//	system(l_c8Command);
//
//	// 设置tx_queue_len
//	memset(l_c8Command, 0, sizeof(l_c8Command));
//	sprintf(l_c8Command, "echo %d > /sys/class/net/can%d/tx_queue_len", TX_QUEUE_LEN, port);
//	system(l_c8Command);
//#endif
//}
//
//void open_can(const int port, const int bitrate)
//{
//#ifndef WIN32
//    char	l_c8Command[64] = {0};
//
//    sprintf(l_c8Command, "ifconfig can%d up", port);
//    system(l_c8Command);
//#endif
//}
//
//void close_can(const int port)
//{
//#ifndef WIN32
//    char	l_c8Command[64] = {0};
//
//    sprintf(l_c8Command, "ifconfig can%d down", port);
//    system(l_c8Command);
//#endif
//}
//
//void close_socket(const int sockfd)
//{
//#ifndef WIN32
//    if (sockfd != -1)
//    {
//        close(sockfd);
//    }
//#endif
//}
//
//// 绑定sock，然后监听端口
//// 返回监听 套接字 文件描述符
//int socket_listen(const int port)
//{
//    int sockfd = -1;
//
//#ifndef WIN32
//    struct sockaddr_can _addr;
//    struct ifreq _ifreq;
//    char buf[256];
//    int ret = 0;
//
//     /* 建立套接字，设置为原始套接字，原始CAN协议 */
//    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
//    if (sockfd < 0)
//    {
//        sprintf(buf, "\n\t打开 socket can%d 错误\n\n", port + 1);
//        panic(buf);
//        return  -1;
//    }
//
//     /* 以下是对CAN接口进行初始化，如设置CAN接口名，即当我们用ifconfig命令时显示的名字 */
//    sprintf(buf, "can%d", port);
//    strcpy(_ifreq.ifr_name, buf);
//    ret = ioctl(sockfd, SIOCGIFINDEX, &_ifreq);
//    if (ret < 0)
//    {
//        sprintf(buf, "\n\t匹配 socket can%d 错误\n\n", port + 1);
//        panic(buf);
//        return  -1;
//    }
//
//    /* 设置CAN协议 */
//    _addr.can_family = AF_CAN;
//    _addr.can_ifindex = _ifreq.ifr_ifindex;
//
//
//    /* disable default receive filter on this RAW socket */
//    /* This is obsolete as we do not read from the socket at all, but for */
//    /* this reason we can remove the receive list in the Kernel to save a */
//    /* little (really a very little!) CPU usage.                          */
//    //	setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
//
//    /* 将刚生成的套接字与CAN套接字地址进行绑定 */
//    ret = bind(sockfd, (struct sockaddr *)&_addr, sizeof(_addr));
//    if ( ret < 0)
//    {
//        close_socket(sockfd);
//        sprintf(buf, "\n\t绑定 socket can%d 错误\n\n", port + 1);
//        panic(buf);
//        return  -1;
//    }
//#endif
//
//    return  sockfd;
//}
//
//int set_can_filter(void)
//{
///**
// * struct can_filter - CAN ID based filter in can_register().
// * @can_id:   relevant bits of CAN ID which are not masked out.
// * @can_mask: CAN mask (see description)
// *
// * Description:
// * A filter matches, when
// *
// *          <received_can_id> & mask == can_id & mask
// *
// */
//    const int n = 1;
//    struct can_filter rfilter[n];
//
//    // 过滤规则：当<received_can_id> & mask == can_id & mask时，接收报文，否则过滤掉.
//    // 可以同时添加多条过滤规则
//
//    // 在用来发送CAN帧的CAN_RAW套接字上不接收任何CAN帧
//    rfilter[0].can_id   = 0x00000000;
//    rfilter[0].can_mask = CAN_EFF_MASK;
//    (void)setsockopt(send_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, n * sizeof(struct can_filter));
//
//    // 在用来接收CAN帧的CAN_RAW套接字上禁用接收过滤规则
//    //(void)setsockopt(recv_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
//
//    return  0;
//}
//
//int set_can_loopback(const int sockfd, const bool lp)
//{
//    // 在默认情况下，本地回环功能是开启的，可以使用下面的方法关闭回环/开启功能：
//    int loopback = lp;  // 0表示关闭, 1表示开启(默认)
//    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
//
//#if 0
//    // 在本地回环功能开启的情况下，所有的发送帧都会被回环到与CAN总线接口对应的套接字上。
//    // 默认情况下，发送CAN报文的套接字不想接收自己发送的报文，因此发送套接字上的回环功能是关闭的。
//    // 可以在需要的时候改变这一默认行为：
//    int ro = 1; // 0表示关闭(默认), 1表示开启
//    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &ro, sizeof(ro));
//#endif
//
//    return  0;
//}
//
//int socket_connect(const int port)
//{
//    return  socket_listen(port);
//}
//
//void disconnect(int *sockfd)
//{
//    if (!sockfd || *sockfd == -1)
//    {
//        return ;
//    }
//
//    close_socket(*sockfd);
//    *sockfd = -1;
//}
//
//int send_frame(const int sockfd, const byte* data, const int count)
//{
//#ifndef WIN32
//    int ret = write(sockfd, (const char*)data, count);
//    if (ret > 0)
//    {
//        tcdrain(sockfd);//不做出错处理，因为网络不支持tcdrain
//    }
//    return  ret;
//#endif
//}
//
//int recv_frame(const int sockfd, byte* buf, const int count, const int timeout_ms)
//{
//    struct timeval tv_timeout;
//    tv_timeout.tv_sec  = timeout_ms  / 1000;
//    tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;
//    fd_set fs_read;
//
//    FD_ZERO(&fs_read);
//    FD_SET(sockfd, &fs_read);	//if fd == -1, FD_SET will block here
//
//    int ret = select((int)sockfd + 1, &fs_read, NULL, NULL, &tv_timeout);
//    if (ret == 0) // recv timeout
//    {
//        return  0;
//    }
//    if (ret < 0) // select error
//    {
//        return  ret;
//    }
//
//#ifndef WIN32
//    ret = read(sockfd, (char*)buf, count);
//#else
//    ret = recv(sockfd, (char*)buf, count, 0);
//#endif
//
//    if (ret <= 0)
//    {
//        return  -1;
//    }
//    return  ret;
//}
//
//// -----------------------------------------------------------------------------
//
//static int   *pframeno = NULL;
//static int   port = 0;
//static int   bitrate = 250 * 1000;
//static __u32 send_frame_id  = 0x1801F456;
//static __u32 send_frame_freq_ms = 250;
//static int   send_frame_times   = 0;
//static __u8  send_frame_data[8] = {0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//static __u8  dlc = 8;
//static bool  extended_frame         = true;
//static bool  send_frame_id_inc_en   = false;// 帧ID每发送一帧递增
//static bool  send_frame_data_inc_en = false;// 发送数据每发送一帧递增
//static bool  lp = false;// 本地环回使能开关
//static bool  run = true;
//
//
//// return 0: normal; return 1: 终止程序运行
//int parse_options(int argc, char **argv)
//{
//    int  i = 0, j = 0, m = 0, n = 0;
//    int  v = 0;
//    char *p = NULL;
//    bool long_option = false;
//    bool ok = true;
//
//
//    for (i = 1; i < argc; i++)
//    {
//        if (!argv[i])
//        {
//            continue;
//        }
//
//        long_option = false;
//        p = NULL;
//
//        //printf("--- i = %d, argc = %d, argv[%d]: %s ---\n", i, argc, i, argv[i]);
//
//        if (strstr(argv[i], "--")) // 长选项
//        {
//            long_option = true;
//            p = strstr(argv[i], "=");
//            if (p == NULL)
//            {
//                return  -1;
//            }
//
//            p += 1;
//        }
//        else if (strstr(argv[i], "-")) // 短选项
//        {
//            if (option_no_value(argv[i]))
//            {
//                p = argv[i];
//            }
//            else
//            {
//                if (argc <= (i + 1))
//                {
//                    return  -1;
//                }
//
//                p = argv[i + 1];
//            }
//        }
//        else
//        {
//            return  -1;
//        }
//
//
//        ok = false;
//        if (!strcmp(argv[i], "--help"))
//        {
//            return  1;
//        }
//        else if (!strcmp(argv[i], "-p") || strstr(argv[i], "--port") != NULL)
//        {
//            v = atoi(p);
//            if (v >= 1 && v < 256)
//            {
//                port = v - 1;
//                ok = true;
//            }
//        }
//        else if (!strcmp(argv[i], "-b") || strstr(argv[i], "--baud") != NULL)
//        {
//            v = atoi(p);
//            if (v == 5 || v == 10 || v == 20 || v == 40 || v == 50 || v == 80 || v == 100 || v == 125 ||
//                v == 200 || v == 250 || v == 400 || v == 500 || v == 666 || v == 800 || v == 1000)
//            {
//                bitrate = v * 1000;
//                ok = true;
//            }
//        }
//        else if (!strcmp(argv[i], "-i") || strstr(argv[i], "--frame-id") != NULL)
//        {
//            send_frame_id = (__u32)String_Asc8TouLong(p);
//            if ((send_frame_id & CAN_EFF_MASK))
//            {
//                return  -1;
//            }
//            send_frame_id &= CAN_EFF_MASK;
//            if (is_error_frame(send_frame_id))
//            {
//                return  -1;
//            }
//            ok = true;
//        }
//        else if (!strcmp(argv[i], "-d") || strstr(argv[i], "--data") != NULL)
//        {
//            dlc = 0;
//
//            if (long_option)
//            {
//                if (!str_isdigit(p, true, false))
//                {
//                    return  -1;
//                }
//
//                v = atoi(p);
//                if (v < 0 || v > 256)
//                {
//                    return  -1;
//                }
//
//                send_frame_data[dlc] = (__u8)String_Asc2TouChar(p);
//                dlc++;
//            }
//
//            for (j = i + 1; j < argc && dlc < 8; j++)
//            {
//                //printf("111 i = %d, j = %d, dlc = %d, argc = %d, argv[%d]: %s \n", i, j, dlc, argc, j, argv[j]);
//
//                if (strstr(argv[j], "-") || strstr(argv[j], "--"))
//                {
//                    j--;
//                    break;
//                }
//                if (!str_isdigit(argv[j], true, false))
//                {
//                    return  -1;
//                }
//
//                v = atoi(argv[j]);
//                if (v < 0 || v > 256)
//                {
//                    return  -1;
//                }
//                send_frame_data[dlc] = (__u8)String_Asc2TouChar(argv[j]);
//                dlc++;
//            }
//
//            i = j;
//            ok = true;
//
//            if (i >= argc)
//            {
//                break;
//            }
//
//            //printf("222 i = %d, j = %d, dlc = %d, argc = %d, argv[%d]: %s \n", i, j, dlc, argc, j, argv[j]);
//
//            continue;
//        }
//        else if (!strcmp(argv[i], "-f") || strstr(argv[i], "--freq") != NULL)
//        {
//            v = atoi(p);
//            if (v >= 1)
//            {
//                send_frame_freq_ms = v;
//                ok = true;
//            }
//        }
//        else if (!strcmp(argv[i], "-t") || strstr(argv[i], "--times") != NULL)
//        {
//            v = atoi(p);
//            if (v >= 0)
//            {
//                send_frame_times = v;
//                ok = true;
//            }
//        }
//        else if (!strcmp(argv[i], "-s"))
//        {
//            extended_frame = false;
//            ok = true;
//        }
//        else if (!strcmp(argv[i], "-I"))
//        {
//            send_frame_id_inc_en = true;
//            ok = true;
//        }
//        else if (!strcmp(argv[i], "-g"))
//        {
//            send_frame_data_inc_en = true;
//            ok = true;
//        }
//        else if (!strcmp(argv[i], "-l"))
//        {
//            lp = true;
//            ok = true;
//        }
//
//
//
//        if (!ok) // 用户输入了不能解析的选项或参数
//        {
//            return  -1;
//        }
//
//        if (!long_option && !option_no_value(argv[i]))
//        {
//            i++;
//        }
//    }
//
//    return  0;
//}
//
//void printf_head(void)
//{
//    printf("\n");
//    printf("SOCKET CAN工具程序，COPYRIGHT (C) 2015 %s \n", _OWNER);
//    printf("CAN接口号 ：CAN%d        通讯速率 ：%d Kbps\n\n", port + 1, bitrate / 1000);
//
//
//    // head
////            CAN1 ==> |  000001  |  18:09:00.356  |  发送成功  | 0x1801F456 |  扩展帧  |  8  |  00 01 FF FF FF FF FF FF
//    printf("+----------+----------+----------------+------------+------------+----------+-----+-------------------------+\n");
//    printf("|   接口   |   序号   |  帧间隔时间ms  |    名称    |    帧ID    |  帧格式  | DLC |  数据                   |\n");
//    printf("+----------+----------+----------------+------------+------------+----------+-----+-------------------------+\n");
//}
//
//// 每调用一次本函数，帧序号自加1
//void printf_frame(const __u32 frame_id, const char *data, const __u8 len,
//    const bool extended,
//    const bool ok_flag,
//    const bool sendflag)
//{
//    int i = 0, l = 0;
//    char buf[128] = {0};
//    char timestamp[128] = {0};
//    struct timeval tv;
//    __u32 no = *pframeno;
//
//    l = 0;
//    for (i = 0; data && i < len && i < 8; i++)
//    {
//        l += sprintf(buf + l, "%02X ", data[i]);
//    }
//
//    /* 时间字符串 */
//    l = 0;
//    (void)gettimeofday(&tv, NULL);
//    (void)strftime(timestamp, 128, "%X", localtime(&tv.tv_sec));
//    l = strlen(timestamp);
//    (void)sprintf(timestamp + l, ".%03ld", tv.tv_usec/1000);
//
//    printf(   "  CAN%d %s | %7u  |  %s  |",
//        port + 1,
//        sendflag ? "==>" : "<==",
//        no,
//        timestamp);
//
//    if (1 && is_error_frame(frame_id))
//    {
//    printf(   "  %s  | 0x%08X |  %s  |  %u  |  %s x %s\n",
//        "总线错误",
//        frame_id,
//        "错误帧",
//        len,
//        buf,
//        get_errframe_info(frame_id, data, len));
//    }
//    else
//    {
//    printf(   "  %s  | 0x%08X |  %s  |  %u  |  %s\n",
//        ok_flag ? (sendflag ? "发送成功" : "接收成功") : (sendflag ? "发送失败" : "接收失败"),
//        frame_id,
//        extended ? "扩展帧" : "标准帧",
//        len,
//        buf);
//    }
//
//
//    no++;
//    *pframeno = no;
//}
//
//void sig_handler(int signo)
//{
//#ifndef WIN32
//    switch(signo)
//    {
//    case SIGINT:
//        run = false;
//        printf(".Exit\n");
//        break;
//
//    default: break;
//    }
//#endif
//}
//
//void install_sig(void)
//{
//#ifndef WIN32
//    signal(SIGINT, sig_handler);
//#endif
//}
//
//int main(int argc, char **argv)
//{
//    S_CanFrame sendframe, recvframe;
//    byte *psendframe = (byte *)&sendframe;
//    byte *precvframe = (byte *)&recvframe;
//    u_canframe_data_t *psend_data = (u_canframe_data_t *)sendframe.data;
//    const int can_frame_len = sizeof(S_CanFrame);
//
//    pid_t pid = -1;
//    int   status;
//
//    int  ret = 0;
//    char buf[128] = {0};
//    bool carry_bit = false;// 进位标志
//
//    int segment_id;//id for shared memo
//
//
//    if (parse_options(argc, argv))
//    {
//        usage();    return  0;
//    }
//
//    if (!find_can(port))
//    {
//        sprintf(buf, "\n\t错误：CAN%d设备不存在\n\n", port + 1);
//        panic(buf);
//        return  -1;
//    }
//
//    close_can(port);// 必须先关闭CAN，才能成功设置CAN波特率
//    set_bitrate(port, bitrate);// 操作CAN之前，先要设置波特率
//    open_can(port, bitrate);
//
//    send_socket_fd = socket_connect(port);
//    recv_socket_fd = socket_connect(port);
//    //printf("send_socket_fd = %d, recv_socket_fd = %d\n", send_socket_fd, recv_socket_fd);
//    if (send_socket_fd < 0 || send_socket_fd < 0)
//    {
//        disconnect(&send_socket_fd);
//        disconnect(&recv_socket_fd);
//        panic("\n\t打开socket can错误\n\n");
//        return  -1;
//    }
//    set_can_filter();
//    set_can_loopback(send_socket_fd, lp);
//
//    printf_head();
//
//    memset(&sendframe, 0x00, sizeof(sendframe));
//    memset(&recvframe, 0x00, sizeof(recvframe));
//
//    if (extended_frame) // 指定发送帧类型：扩展帧或标准帧
//    {
//        sendframe.can_id = (send_frame_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
//    }
//    else
//    {
//        sendframe.can_id = (send_frame_id & CAN_SFF_MASK);
//    }
//    sendframe.can_dlc = dlc;
//    memcpy(sendframe.data, send_frame_data, dlc);
//
//
//    segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);// allocate memo
//    pframeno = (int *)shmat(segment_id, NULL, 0);// attach the memo
//    if (pframeno == NULL)
//    {
//        panic("\n\t创建共享内存失败\n\n");
//        return  -1;
//    }
//    *pframeno = 1;
//
//    run = true;
//
//    pid = fork();
//    if(pid == -1)
//    {
//        panic("\n\t创建进程失败\n\n");
//        return  -1;
//    }
//    else if(pid == 0) // 子进程，用于发送CAN帧
//    {
//        while (run && (send_frame_times > 0))
//        {
//            ret = send_frame(send_socket_fd, (char *)&sendframe, sizeof(sendframe));
//            printf_frame(sendframe.can_id & CAN_EFF_MASK, sendframe.data, sendframe.can_dlc,
//                ((sendframe.can_id & CAN_EFF_FLAG) ? true : false),
//                ret > 0 ? true : false,
//                true);
//            delay_ms(send_frame_freq_ms);
//
//            if (send_frame_id_inc_en)
//            {
//                sendframe.can_id++;
//                if (extended_frame)
//                {
//                    sendframe.can_id = (sendframe.can_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
//                }
//                else
//                {
//                    sendframe.can_id = (sendframe.can_id & CAN_SFF_MASK);
//                }
//            }
//
//            if (send_frame_data_inc_en && dlc > 0)
//            {
//                if (dlc > 4 && psend_data->s.dl == ((__u32)0xFFFFFFFF))
//                {
//                    carry_bit = true;// 发生进位
//                }
//                psend_data->s.dl++;
//
//                if (dlc <= 4)
//                {
//                    if (psend_data->s.dl >= (1 << (dlc * 8)))
//                    {
//                        psend_data->s.dl = 0;
//                    }
//                }
//                else if (dlc <= 8)
//                {
//                    if (carry_bit)
//                    {
//                        psend_data->s.dh++;
//                        if (psend_data->s.dh >= (1 << ((dlc - 4) * 8)))
//                        {
//                            psend_data->s.dh = 0;
//                        }
//
//                        carry_bit = false;
//                    }
//                }
//            }
//
//            send_frame_times--;
//        }
//
//        exit(0);
//    }
//    else // 父进程，接收CAN帧
//    {
//        install_sig();
//
//        while (run)
//        {
//            memset(precvframe, 0x00, can_frame_len);
//            ret = recv_frame(recv_socket_fd, precvframe, can_frame_len, 5 * 1000);
//            if (ret > 0)
//            {
//                printf_frame(recvframe.can_id & CAN_EFF_MASK, recvframe.data, recvframe.can_dlc,
//                    ((recvframe.can_id & CAN_EFF_FLAG) ? true : false),
//                    true,
//                    false);
//            }
//        }
//
//        while(((pid = wait(&status)) == -1) && (errno == EINTR))
//        {
//            delay_ms(10);
//        }
//    }
//
//    disconnect(&send_socket_fd);
//    disconnect(&recv_socket_fd);
//
//    shmdt(pframeno);// detach memo
//    shmctl(segment_id, IPC_RMID, NULL);// remove
//
//    return  0;
//}
//
//// -----------------------------------------------------------------------------
//// cantool.c




//for (j=0; j<height; j++){
//	for (i=0; i<width; i++){
//		//float t3 = 0;
//		int t2 = 0;
//		//t3 = (float)((float)zStg1PicSource[j][i] - tResultAvg) * 20;
//		//t3 = (float)(zStg1PicSource[j][i].R + zStg1PicSource[j][i].G + zStg1PicSource[j][i].B) - tResultAvg;
//		//t3 = t3 / difBase;
//		//t3 = t3 + 0.5;
//		//t3 = ((float)(zStg1PicSource[j][i].R + zStg1PicSource[j][i].G + zStg1PicSource[j][i].B) - tResultAvg) / difBase +0.5;
//		t2 = (int)(((float)(zStg1PicSource[j][i].R + zStg1PicSource[j][i].G + zStg1PicSource[j][i].B) - tResultAvg) / difBase +0.5);
//		//t2 = (int)(((float)zStg1PicSource[j][i] - tResultAvg))/difBase + 0.5);
//		if (t2 < ((-1)*PDR_STATISTIC_DIM/2)) zStgStatisticFigure[0]++;
//		else if (t2 > PDR_STATISTIC_DIM/2) zStgStatisticFigure[PDR_STATISTIC_DIM-1]++;
//		else zStgStatisticFigure[t2+PDR_STATISTIC_DIM/2]++;
//	}
//}
