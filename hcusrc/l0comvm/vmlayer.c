/*
 * vmlayer.c
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#include "vmlayer.h"
#include "../l0service/trace.h"

/*
 *
 *   全局变量，存储所有任务的状态信息，以便后面后面使用
 *
 */

HcuVmCtrTab_t 		zHcuVmCtrTab;  		//全局系统总控表
HcuSysEngParTab_t 	zHcuSysEngPar; 		//全局工程参数控制表
HcuSysStaPm_t		zHcuSysStaPm;		//全局性能统计表

/*
 *
 *   任务初始化配置参数
 *
 */

//请确保，该全局字符串的定义跟Task_Id的顺序保持完全一致，不然后面的显示内容会出现差错， 请服从最长长度TASK_NAME_MAX_LENGTH的定义，不然Debug/Trace打印出的信息也会出错
//从极致优化内存的角度，这里浪费了2个TASK对应的内存空间（MIN=0/MAX=n+1)，但它却极大的改善了程序编写的效率，值得浪费！！！
//NULL条目保留，是为了初始化TASK NAME这一属性
HcuVmCtrTaskStaticCfg_t zHcuVmCtrTaskStaticCfg[] =
{
	//TASK_ID,              状态控制             状态机入口                 TRACE标志位                   注释
	{TASK_ID_MIN,           "TASKMIN",          NULL,                    1, 1, 1, 1, 1},              //Starting
//基础任务STARTING：由SYSCONFIG.H中的开关决定是否启动
	{TASK_ID_HCUMAIN,       "HCUMAIN",          NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_HCUVM,         "HCUVM",            NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_TRACE,         "TRACE",            NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_CONFIG,        "CONFIG",           NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_TIMER,         "TIMER",            &HcuFsmTimer,            1, 1, 1, 1, 1},
	{TASK_ID_MMC,           "MMC",              NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_GPIO,          "GPIO",             &HcuFsmGpio,             1, 1, 1, 1, 1},
	{TASK_ID_I2C,           "I2C",              &HcuFsmI2c,              1, 1, 1, 1, 1},
	{TASK_ID_SPI,           "SPI",              &HcuFsmSpi,              1, 1, 1, 1, 1},
	{TASK_ID_PWM,           "PWM",              &HcuFsmPwm,              1, 1, 1, 1, 1},
	{TASK_ID_ADC,           "ADC",              &HcuFsmAdc,              1, 1, 1, 1, 1},
	{TASK_ID_SWITCH,        "SWITCH",           &HcuFsmSwitch,           1, 1, 1, 1, 1},
	{TASK_ID_RELAY,         "RELAY",            &HcuFsmRelay,            1, 1, 1, 1, 1},
	{TASK_ID_MOTOR,         "MOTOR",            &HcuFsmMotor,            1, 1, 1, 1, 1},
	{TASK_ID_ZEEGBE,        "ZEEGBE",           &HcuFsmZeegbe,           1, 1, 1, 1, 1},
	{TASK_ID_GPRS,          "GPRS",             NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_SPS232,        "SPS232",           &HcuFsmSps232,           1, 1, 1, 1, 1},
	{TASK_ID_SPS485,        "SPS485",           &HcuFsmSps485,           1, 1, 1, 1, 1},
	{TASK_ID_BLE,           "BLE",              &HcuFsmBle,              1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         1, 1, 1, 1, 1},
	{TASK_ID_WIFI,          "WIFI",             &HcuFsmWifi,             1, 1, 1, 1, 1},
	{TASK_ID_USBNET,        "USBNET",           &HcuFsmUsbnet,           1, 1, 1, 1, 1},
	{TASK_ID_3G4G,          "3G4G",             &HcuFsm3g4g,             1, 1, 1, 1, 1},
	{TASK_ID_HARDDISK,      "HARDDISK",         NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_CAMERA,        "CAMERA",           NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_MICROPHONE,    "MICROPHONE",       &HcuFsmMicrophone,       1, 1, 1, 1, 1},
	{TASK_ID_FLASH,         "FLASH",            NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_GPS,           "GPS",              &HcuFsmGps,              1, 1, 1, 1, 1},
	{TASK_ID_LCD,           "LCD",              &HcuFsmLcd,              1, 1, 1, 1, 1},
	{TASK_ID_LED,           "LED",              &HcuFsmLed,              1, 1, 1, 1, 1},
	{TASK_ID_HWINV,         "HWINV",            &HcuFsmHwinv,            1, 1, 1, 1, 1},
	{TASK_ID_SPSVIRGO,      "SPSVIRGO",         &HcuFsmSpsvirgo,         1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        1, 1, 1, 1, 1},
	{TASK_ID_MODBUS,        "MODBUS",           &HcuFsmModbus,           1, 1, 1, 1, 1},
	{TASK_ID_AVORION,       "AVORION",          &HcuFsmAvorion,          1, 1, 1, 1, 1},
	{TASK_ID_I2CBUSLIBRA,   "I2CBUSLIBRA",      &HcuFsmI2cbuslibra,      1, 1, 1, 1, 1},
	{TASK_ID_SPIBUSARIES,   "SPIBUSARIES",      &HcuFsmSpibusaries,      1, 1, 1, 1, 1},
	{TASK_ID_NBIOTCJ188,    "NBIOTCJ188",       &HcuFsmNbiotcj188,       1, 1, 1, 1, 1},
	{TASK_ID_NBIOTQG376,    "NBIOTQG376",       &HcuFsmNbiotqg376,       1, 1, 1, 1, 1},
	{TASK_ID_HSMMP,         "HSMMP",            &HcuFsmHsmmp,            1, 1, 1, 1, 1},
	{TASK_ID_EMC,           "EMC",              &HcuFsmEmc,              1, 1, 1, 1, 1},
	{TASK_ID_HUMID,         "HUMID",            &HcuFsmHumid,            1, 1, 1, 1, 1},
	{TASK_ID_PM25,          "PM25",             &HcuFsmPm25,             1, 1, 1, 1, 1},
	{TASK_ID_TEMP,          "TEMP",             &HcuFsmTemp,             1, 1, 1, 1, 1},
	{TASK_ID_WINDDIR,       "WINDDIR",          &HcuFsmWinddir,          1, 1, 1, 1, 1},
	{TASK_ID_WINDSPD,       "WINDSPD",          &HcuFsmWindspd,          1, 1, 1, 1, 1},
	{TASK_ID_NOISE,         "NOISE",            &HcuFsmNoise,            1, 1, 1, 1, 1},
	{TASK_ID_AIRPRS,        "AIRPRS",           &HcuFsmAirprs,           1, 1, 1, 1, 1},
	{TASK_ID_CO1,           "CO1",              &HcuFsmCo1,              1, 1, 1, 1, 1},
	{TASK_ID_LIGHTSTR,      "LIGHTSTR",         &HcuFsmLightstr,         1, 1, 1, 1, 1},
	{TASK_ID_ALCOHOL,       "ALCOHOL",          &HcuFsmAlcohol,          1, 1, 1, 1, 1},
	{TASK_ID_HCHO,          "HCHO",             &HcuFsmHcho,             1, 1, 1, 1, 1},
	{TASK_ID_TOXICGAS,      "TOXICGAS",         &HcuFsmToxicgas,         1, 1, 1, 1, 1},
	{TASK_ID_IWM,           "IWM",              &HcuFsmIwm,              1, 1, 1, 1, 1},
	{TASK_ID_IHM,           "IHM",              &HcuFsmIhm,              1, 1, 1, 1, 1},
	{TASK_ID_IGM,           "IGM",              &HcuFsmIgm,              1, 1, 1, 1, 1},
	{TASK_ID_IPM,           "IPM",              &HcuFsmIpm,              1, 1, 1, 1, 1},
	{TASK_ID_SVRCON,        "SVRCON",           &HcuFsmSvrcon,           1, 1, 1, 1, 1},
	{TASK_ID_SYSPM,         "SYSPM",            &HcuFsmSyspm,            1, 1, 1, 1, 1},
	{TASK_ID_PM25SHARP,     "PM25SHARP",        &HcuFsmPm25sharp,        1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        &HcuFsmCanitfleo,        1, 1, 1, 1, 1},
//基础任务END
//分项目任务STARTING：不要动，需要将任务全部初始化好，并且设置入口为NULL。在分项目中再配置是否真启动
	{TASK_ID_L3AQYCG10,     "L3AQYCG10",        NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3AQYCG20,     "L3AQYCG20",        NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3TBSWRG30,    "L3TBSWRG30",       NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3GQYBG40,     "L3GQYBG40",        NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3CXILC,       "L3CXILC",          NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3CXGLACM,     "L3CXGLACM",        NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3NBLPM,       "L3NBLPM",          NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3NBHPM,       "L3NBHPM",          NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3BFSC,        "L3BFSC",           NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_BFSCUICOMM,    "BFSCUICOMM",       NULL,                    1, 1, 1, 1, 1},
	{TASK_ID_L3OPWLOTDR,    "L3OPWLOTDR",       NULL,                    1, 1, 1, 1, 1},
//分项目任务END
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	{TASK_ID_L3AQYCG10,     "L3AQYCG10",        &HcuFsmL3aqycg10,        1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	{TASK_ID_L3AQYCG20,     "L3AQYCG20",        &HcuFsmL3aqycg20,        1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
	{TASK_ID_L3TBSWRG30,    "L3TBSWRG30",       &HcuFsmL3tbswrg30,       1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
	{TASK_ID_L3GQYBG40,     "L3GQYBG40",        &HcuFsmLgqgybg40,        1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
	{TASK_ID_L3CXILC,       "L3CXILC",          &HcuFsmL3cxilc,          1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
	{TASK_ID_L3CXGLACM,     "L3CXGLACM",        &HcuFsmL3cxglacm,        1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
	{TASK_ID_L3NBLPM,       "L3NBLPM",          &HcuFsmL3nblpm,          1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
	{TASK_ID_L3NBHPM,       "L3NBHPM",          &HcuFsmL3nbhpm,          1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{TASK_ID_L3BFSC,        "L3BFSC",           &HcuFsmL3bfsc,           1, 1, 1, 1, 1},
	{TASK_ID_BFSCUICOMM,    "BFSCUICOMM",       &HcuFsmBfscuicomm,       1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
	{TASK_ID_L3OPWLOTDR,    "L3OPWLOTDR",       &HcuFsmL3opwlotdr,       1, 1, 1, 1, 1},
//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif
	{TASK_ID_MAX,       "TASKMAX",              NULL,                    1, 1, 1, 1, 1},                  //Ending
};

//消息ID的定义全局表，方便TRACE函数使用
//请服从MSG_NAME_MAX_LENGTH的最长定义，不然出错
HcuSysEngTrcMsgCtrStaticCfg_t zHcuSysEngTrcMsgCtrStaticCfg[] ={
	//MSG_ID                                      MsgName                                       TRACE_FLAG   注释
	{MSG_ID_COM_MIN,                              "MSG_ID_COM_MIN",                             0, 0, 0},    //STARTING
	{MSG_ID_COM_INIT,                             "MSG_ID_COM_INIT",                            1, 1, 1},
	{MSG_ID_COM_INIT_FEEDBACK,                    "MSG_ID_COM_INIT_FEEDBACK",                   1, 1, 1},
	{MSG_ID_COM_RESTART,                          "MSG_ID_COM_RESTART",                         1, 1, 1},
	{MSG_ID_COM_STOP,                             "MSG_ID_COM_STOP",                            1, 1, 1},
	{MSG_ID_COM_COMPLETE,                         "MSG_ID_COM_COMPLETE",                        1, 1, 1},
	{MSG_ID_COM_TIME_OUT,                         "MSG_ID_COM_TIME_OUT",                        1, 1, 1},
	{MSG_ID_COM_DEBUG_TEST,                       "MSG_ID_COM_DEBUG_TEST",                      1, 1, 1},
	{MSG_ID_COM_HEART_BEAT,                       "MSG_ID_COM_HEART_BEAT",                      1, 1, 1},
	{MSG_ID_COM_HEART_BEAT_FB,                    "MSG_ID_COM_HEART_BEAT_FB",                   1, 1, 1},
	{MSG_ID_COM_PROCESS_REBOOT,                   "MSG_ID_COM_PROCESS_REBOOT",                  1, 1, 1},
	//Service Control message
	//ETHERNET
	{MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,           "MSG_ID_ETHERNET_CLOUDVELA_DATA_RX",          1, 1, 1},
	{MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX,          "MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX",         1, 1, 1},
	{MSG_ID_ETHERNET_NBIOTQG376_DATA_RX,          "MSG_ID_ETHERNET_NBIOTQG376_DATA_RX",         1, 1, 1},
	{MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,    "MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX",   1, 1, 1},
	//WIFI
	{MSG_ID_WIFI_CLOUDVELA_DATA_RX,               "MSG_ID_WIFI_CLOUDVELA_DATA_RX",              1, 1, 1},
	//USBNET
	{MSG_ID_USBNET_CLOUDVELA_DATA_RX,             "MSG_ID_USBNET_CLOUDVELA_DATA_RX",            1, 1, 1},
	//3G4G
	{MSG_ID_3G4G_CLOUDVELA_DATA_RX,               "MSG_ID_3G4G_CLOUDVELA_DATA_RX",              1, 1, 1},
	//RS232
	{MSG_ID_SPS232_MODBUS_DATA_RX,                "MSG_ID_SPS232_MODBUS_DATA_RX",               1, 1, 1},
	//RS485
	{MSG_ID_SPS485_MODBUS_DATA_RX,                "MSG_ID_SPS485_MODBUS_DATA_RX",               1, 1, 1},
	//SPSVIRGO
	{MSG_ID_SPSVIRGO_HSMMP_DATA_RX,               "MSG_ID_SPSVIRGO_HSMMP_DATA_RX",              1, 1, 1},
	{MSG_ID_SPSVIRGO_NOISE_DATA_REPORT,           "MSG_ID_SPSVIRGO_NOISE_DATA_REPORT",          1, 1, 1},
	{MSG_ID_SPSVIRGO_NOISE_CONTROL_FB,            "MSG_ID_SPSVIRGO_NOISE_CONTROL_FB",           1, 1, 1},
	//AVORION
	{MSG_ID_AVORION_HSMMP_DATA_READ_FB,           "MSG_ID_AVORION_HSMMP_DATA_READ_FB",          1, 1, 1},
	{MSG_ID_AVORION_HSMMP_DATA_RX,                "MSG_ID_AVORION_HSMMP_DATA_RX",               1, 1, 1},
	//BLE
	{MSG_ID_BLE_HSMMP_DATA_RX,                    "MSG_ID_BLE_HSMMP_DATA_RX",                   1, 1, 1},
	{MSG_ID_BLE_MODBUS_DATA_RX,                   "MSG_ID_BLE_MODBUS_DATA_RX",                  1, 1, 1},
	//GPS
	//LCD
	{MSG_ID_LCD_AVORION_DATA_RX,                  "MSG_ID_LCD_AVORION_DATA_RX",                 1, 1, 1},
	//CAMERA
	{MSG_ID_CAMERA_AVORION_DATA_RX,               "MSG_ID_CAMERA_AVORION_DATA_RX",              1, 1, 1},
	//MICROPHONE
	{MSG_ID_MICROPHONE_AVORION_DATA_RX,           "MSG_ID_MICROPHONE_AVORION_DATA_RX",          1, 1, 1},
	//HWINV
	{MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,       "MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG",      1, 1, 1},
	//CloudConnection message
	{MSG_ID_CLOUDVELA_EMC_DATA_REQ,               "MSG_ID_CLOUDVELA_EMC_DATA_REQ",              1, 1, 1},
	{MSG_ID_CLOUDVELA_EMC_CONTROL_CMD,            "MSG_ID_CLOUDVELA_EMC_CONTROL_CMD",           1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25_DATA_REQ,              "MSG_ID_CLOUDVELA_PM25_DATA_REQ",             1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25_CONTROL_CMD,           "MSG_ID_CLOUDVELA_PM25_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,           "MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ",          1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDDIR_CONTROL_CMD,        "MSG_ID_CLOUDVELA_WINDDIR_CONTROL_CMD",       1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,           "MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ",          1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDSPD_CONTROL_CMD,        "MSG_ID_CLOUDVELA_WINDSPD_CONTROL_CMD",       1, 1, 1},
	{MSG_ID_CLOUDVELA_TEMP_DATA_REQ,              "MSG_ID_CLOUDVELA_TEMP_DATA_REQ",             1, 1, 1},
	{MSG_ID_CLOUDVELA_TEMP_CONTROL_CMD,           "MSG_ID_CLOUDVELA_TEMP_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_CLOUDVELA_HUMID_DATA_REQ,             "MSG_ID_CLOUDVELA_HUMID_DATA_REQ",            1, 1, 1},
	{MSG_ID_CLOUDVELA_HUMID_CONTROL_CMD,          "MSG_ID_CLOUDVELA_HUMID_CONTROL_CMD",         1, 1, 1},
	{MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD,          "MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD",         1, 1, 1},
	{MSG_ID_CLOUDVELA_NOISE_DATA_REQ,             "MSG_ID_CLOUDVELA_NOISE_DATA_REQ",            1, 1, 1},
	{MSG_ID_CLOUDVELA_NOISE_CONTROL_CMD,          "MSG_ID_CLOUDVELA_NOISE_CONTROL_CMD",         1, 1, 1},
	//NBIOT message
	{MSG_ID_NBIOTCJ188_IWM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IWM_DATA_REQ",             1, 1, 1},
	{MSG_ID_NBIOTCJ188_IWM_CONTROL_CMD,           "MSG_ID_NBIOTCJ188_IWM_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NBIOTCJ188_IHM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IHM_DATA_REQ",             1, 1, 1},
	{MSG_ID_NBIOTCJ188_IHM_CONTROL_CMD,           "MSG_ID_NBIOTCJ188_IHM_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NBIOTCJ188_IGM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IGM_DATA_REQ",             1, 1, 1},
	{MSG_ID_NBIOTCJ188_IGM_CONTROL_CMD,           "MSG_ID_NBIOTCJ188_IGM_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NBIOTCJ188_IPM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IPM_DATA_REQ",             1, 1, 1},
	{MSG_ID_NBIOTCJ188_IPM_CONTROL_CMD,           "MSG_ID_NBIOTCJ188_IPM_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NBIOTQG376_IPM_DATA_REQ,              "MSG_ID_NBIOTQG376_IPM_DATA_REQ",             1, 1, 1},
	{MSG_ID_NBIOTQG376_IPM_CONTROL_CMD,           "MSG_ID_NBIOTQG376_IPM_CONTROL_CMD",          1, 1, 1},
	//Modbus report
	{MSG_ID_MODBUS_EMC_DATA_REPORT,               "MSG_ID_MODBUS_EMC_DATA_REPORT",              1, 1, 1},
	{MSG_ID_MODBUS_EMC_CONTROL_FB,                "MSG_ID_MODBUS_EMC_CONTROL_FB",               1, 1, 1},
	{MSG_ID_MODBUS_PM25_DATA_REPORT,              "MSG_ID_MODBUS_PM25_DATA_REPORT",             1, 1, 1},
	{MSG_ID_MODBUS_PM25_CONTROL_FB,               "MSG_ID_MODBUS_PM25_CONTROL_FB",              1, 1, 1},
	{MSG_ID_MODBUS_WINDDIR_DATA_REPORT,           "MSG_ID_MODBUS_WINDDIR_DATA_REPORT",          1, 1, 1},
	{MSG_ID_MODBUS_WINDDIR_CONTROL_FB,            "MSG_ID_MODBUS_WINDDIR_CONTROL_FB",           1, 1, 1},
	{MSG_ID_MODBUS_WINDSPD_DATA_REPORT,           "MSG_ID_MODBUS_WINDSPD_DATA_REPORT",          1, 1, 1},
	{MSG_ID_MODBUS_WINDSPD_CONTROL_FB,            "MSG_ID_MODBUS_WINDSPD_CONTROL_FB",           1, 1, 1},
	{MSG_ID_MODBUS_TEMP_DATA_REPORT,              "MSG_ID_MODBUS_TEMP_DATA_REPORT",             1, 1, 1},
	{MSG_ID_MODBUS_TEMP_CONTROL_FB,               "MSG_ID_MODBUS_TEMP_CONTROL_FB",              1, 1, 1},
	{MSG_ID_MODBUS_HUMID_DATA_REPORT,             "MSG_ID_MODBUS_HUMID_DATA_REPORT",            1, 1, 1},
	{MSG_ID_MODBUS_HUMID_CONTROL_FB,              "MSG_ID_MODBUS_HUMID_CONTROL_FB",             1, 1, 1},
	{MSG_ID_MODBUS_NOISE_DATA_REPORT,             "MSG_ID_MODBUS_NOISE_DATA_REPORT",            1, 1, 1},
	{MSG_ID_MODBUS_NOISE_CONTROL_FB,              "MSG_ID_MODBUS_NOISE_CONTROL_FB",             1, 1, 1},
	//EMC
	{MSG_ID_EMC_MODBUS_DATA_READ,                 "MSG_ID_EMC_MODBUS_DATA_READ",                1, 1, 1},
	{MSG_ID_EMC_CLOUDVELA_DATA_RESP,              "MSG_ID_EMC_CLOUDVELA_DATA_RESP",             1, 1, 1},
	{MSG_ID_EMC_MODBUS_CONTROL_CMD,               "MSG_ID_EMC_MODBUS_CONTROL_CMD",              1, 1, 1},
	{MSG_ID_EMC_CLOUDVELA_CONTROL_FB,             "MSG_ID_EMC_CLOUDVELA_CONTROL_FB",            1, 1, 1},
	//PM25 message
	{MSG_ID_PM25_MODBUS_DATA_READ,                "MSG_ID_PM25_MODBUS_DATA_READ",               1, 1, 1},
	{MSG_ID_PM25_CLOUDVELA_DATA_RESP,             "MSG_ID_PM25_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_PM25_MODBUS_CONTROL_CMD,              "MSG_ID_PM25_MODBUS_CONTROL_CMD",             1, 1, 1},
	{MSG_ID_PM25_CLOUDVELA_CONTROL_FB,            "MSG_ID_PM25_CLOUDVELA_CONTROL_FB",           1, 1, 1},
	//WIND_DIRECTION message
	{MSG_ID_WINDDIR_MODBUS_DATA_READ,             "MSG_ID_WINDDIR_MODBUS_DATA_READ",            1, 1, 1},
	{MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP,          "MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP",         1, 1, 1},
	{MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,           "MSG_ID_WINDDIR_MODBUS_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_WINDDIR_CLOUDVELA_CONTROL_FB,         "MSG_ID_WINDDIR_CLOUDVELA_CONTROL_FB",        1, 1, 1},
	//WIND_SPEED message
	{MSG_ID_WINDSPD_MODBUS_DATA_READ,             "MSG_ID_WINDSPD_MODBUS_DATA_READ",            1, 1, 1},
	{MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,          "MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP",         1, 1, 1},
	{MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,           "MSG_ID_WINDSPD_MODBUS_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_WINDSPD_CLOUDVELA_CONTROL_FB,         "MSG_ID_WINDSPD_CLOUDVELA_CONTROL_FB",        1, 1, 1},
	//TEMPERATURE message
	{MSG_ID_TEMP_MODBUS_DATA_READ,                "MSG_ID_TEMP_MODBUS_DATA_READ",               1, 1, 1},
	{MSG_ID_TEMP_SPIBUSARIES_DATA_READ,           "MSG_ID_TEMP_SPIBUSARIES_DATA_READ",          1, 1, 1},
	{MSG_ID_TEMP_CLOUDVELA_DATA_RESP,             "MSG_ID_TEMP_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_TEMP_MODBUS_CONTROL_CMD,              "MSG_ID_TEMP_MODBUS_CONTROL_CMD",             1, 1, 1},
	{MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD,         "MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD",        1, 1, 1},
	{MSG_ID_TEMP_CLOUDVELA_CONTROL_FB,            "MSG_ID_TEMP_CLOUDVELA_CONTROL_FB",           1, 1, 1},
	//HUMIDITY message
	{MSG_ID_HUMID_MODBUS_DATA_READ,               "MSG_ID_HUMID_MODBUS_DATA_READ",              1, 1, 1},
	{MSG_ID_HUMID_CLOUDVELA_DATA_RESP,            "MSG_ID_HUMID_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_HUMID_MODBUS_CONTROL_CMD,             "MSG_ID_HUMID_MODBUS_CONTROL_CMD",            1, 1, 1},
	{MSG_ID_HUMID_CLOUDVELA_CONTROL_FB,           "MSG_ID_HUMID_CLOUDVELA_CONTROL_FB",          1, 1, 1},
	//HSMMP
	{MSG_ID_HSMMP_AVORION_DATA_READ,              "MSG_ID_HSMMP_AVORION_DATA_READ",             1, 1, 1},
	{MSG_ID_HSMMP_AVORION_STOP,                   "MSG_ID_HSMMP_AVORION_STOP",                  1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,            "MSG_ID_HSMMP_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_CONTROL_FB,           "MSG_ID_HSMMP_CLOUDVELA_CONTROL_FB",          1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP,       "MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP",      1, 1, 1},
	//NOISE
	{MSG_ID_NOISE_SPSVIRGO_DATA_READ,             "MSG_ID_NOISE_SPSVIRGO_DATA_READ",            1, 1, 1},
	{MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD,           "MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NOISE_SPSVIRGO_STOP,                  "MSG_ID_NOISE_SPSVIRGO_STOP",                 1, 1, 1},
	{MSG_ID_NOISE_CLOUDVELA_DATA_RESP,            "MSG_ID_NOISE_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_NOISE_CLOUDVELA_CONTROL_FB,           "MSG_ID_NOISE_CLOUDVELA_CONTROL_FB",          1, 1, 1},
	{MSG_ID_NOISE_MODBUS_DATA_READ,               "MSG_ID_NOISE_MODBUS_DATA_READ",              1, 1, 1},
	{MSG_ID_NOISE_MODBUS_CONTROL_CMD,             "MSG_ID_NOISE_MODBUS_CONTROL_CMD",            1, 1, 1},
	//IWM
	{MSG_ID_IWM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IWM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_IWM_NBIOTCJ188_CONTROL_FB,            "MSG_ID_IWM_NBIOTCJ188_CONTROL_FB",           1, 1, 1},
	//IHM
	{MSG_ID_IHM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IHM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_IHM_NBIOTCJ188_CONTROL_FB,            "MSG_ID_IHM_NBIOTCJ188_CONTROL_FB",           1, 1, 1},
	//IGM
	{MSG_ID_IGM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IGM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_IGM_NBIOTCJ188_CONTROL_FB,            "MSG_ID_IGM_NBIOTCJ188_CONTROL_FB",           1, 1, 1},
	//IPM
	{MSG_ID_IPM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IPM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_IPM_NBIOTCJ188_CONTROL_FB,            "MSG_ID_IPM_NBIOTCJ188_CONTROL_FB",           1, 1, 1},
	{MSG_ID_IPM_NBIOTQG376_DATA_RESP,             "MSG_ID_IPM_NBIOTQG376_DATA_RESP",            1, 1, 1},
	{MSG_ID_IPM_NBIOTQG376_CONTROL_FB,            "MSG_ID_IPM_NBIOTQG376_CONTROL_FB",           1, 1, 1},
	//PM
	{MSG_ID_COM_ALARM_REPORT,                     "MSG_ID_COM_ALARM_REPORT",                    1, 1, 1},
	{MSG_ID_COM_PM_REPORT,                        "MSG_ID_COM_PM_REPORT",                       1, 1, 1},
	//CANITF
	{MSG_ID_CANITFLEO_DATA_REPORT,                "MSG_ID_CANITFLEO_DATA_REPORT",               1, 1, 1},
	//L3BFSC
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,         "MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ",        1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,               "MSG_ID_L3BFSC_CAN_WS_COMB_OUT",              1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,                "MSG_ID_L3BFSC_CAN_WS_GIVE_UP",               1, 1, 1},
	{MSG_ID_L3BFSC_UICOMM_CMD_RESP,               "MSG_ID_L3BFSC_UICOMM_CMD_RESP",              1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP,            "MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP",           1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT,         "MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT",        1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_INIT_REQ,               "MSG_ID_L3BFSC_CAN_WS_INIT_REQ",              1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_READ_REQ,               "MSG_ID_L3BFSC_CAN_WS_READ_REQ",              1, 1, 1},
	{MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ,           "MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ",          1, 1, 1},
	//CANITFLEO
	{MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,        "MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP",       1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,        "MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT",       1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,            "MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB",           1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,             "MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB",            1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_INIT_FB,                "MSG_ID_CAN_L3BFSC_WS_INIT_FB",               1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_READ_RESP,              "MSG_ID_CAN_L3BFSC_WS_READ_RESP",             1, 1, 1},
	{MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP,          "MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP",         1, 1, 1},
	//BFSCUICOMM
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,                "MSG_ID_UICOMM_L3BFSC_CMD_REQ",               1, 1, 1},
	{MSG_ID_UICOMM_L3BFSC_PARAM_SET_RESULT,       "MSG_ID_UICOMM_L3BFSC_PARAM_SET_RESULT",      1, 1, 1},
	//CLOUDVELA
	{MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ,             "MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ",            1, 1, 1},
	{MSG_ID_COM_MAX,                              "MSG_ID_COM_MAX",                             0, 0, 0},    //Ending
};

//启动区XML关键字定义
HcuSysEngPhyBootCfg_t zHcuSysEngPhyBootCfg[] = {
	{0,   	"<xml>", 				    "</xml>"},
	{20,  	"<equLable>",               "</equLable>"},
	{2,		"<hwType>",                 "</hwType>"},
	{2,		"<hwPemId>",                "</hwPemId>"},
	{2,		"<swRelId>",                "</swRelId>"},
	{2,		"<swVerId>",                "</swVerId>"},
	{1,		"<swUpgradeFlag>",          "</swUpgradeFlag>"},
	{1,		"<swUpgPollId>",            "</swUpgPollId>"},
	{1,		"<bootIndex>",              "</bootIndex>"},
	{1,		"<bootAreaMax>",            "</bootAreaMax>"},
	{4,		"<facLoadAddr>",            "</facLoadAddr>"},
	{2,		"<facLoadSwRel>",           "</facLoadSwRel>"},
	{2,		"<facLoadSwVer>",           "</facLoadSwVer>"},
	{2,		"<facLoadCheckSum>",        "</facLoadCheckSum>"},
	{2,		"<facLoadValid>",           "</facLoadValid>"},
	{4,		"<spare2>",                 "</spare2>"},
	{4,		"<bootLoad1Addr>",          "</bootLoad1Addr>"},
	{2,		"<bootLoad1RelId>",         "</bootLoad1RelId>"},
	{2,		"<bootLoad1VerId>",         "</bootLoad1VerId>"},
	{2,		"<bootLoad1CheckSum>",      "</bootLoad1CheckSum>"},
	{2,		"<bootLoad1Valid>",         "</bootLoad1Valid>"},
	{4,		"<spare3>",                 "</spare3>"},
	{4,		"<bootLoad2Addr>",          "</bootLoad2Addr>"},
	{2,		"<bootLoad2RelId>",         "</bootLoad2RelId>"},
	{2,		"<bootLoad2VerId>",         "</bootLoad2VerId>"},
	{2,		"<bootLoad2CheckSum>",      "</bootLoad2CheckSum>"},
	{2,		"<bootLoad2Valid>",         "</bootLoad2Valid>"},
	{4,		"<spare4>",                 "</spare4>"},
	{4,		"<bootLoad3Addr>",          "</bootLoad3Addr>"},
	{2,		"<bootLoad3RelId>",         "</bootLoad3RelId>"},
	{2,		"<bootLoad3VerId>",         "</bootLoad3VerId>"},
	{2,		"<bootLoad3CheckSum>",      "</bootLoad3CheckSum>"},
	{2,		"<bootLoad3Valid>",         "</bootLoad3Valid>"},
	{4,		"<spare5>",                 "</spare5>"},
	{8,		"<cipherKey>",              "</cipherKey>"},
	{8,		"<rsv>",                    "</rsv>"}
};


/*
 *
 *   核心API函数
 *
 */

//INIT the whole system
OPSTAT hcu_vm_system_ctr_table_init(void)
{
	int i=0;

	//INIT HCU itself
	HcuDebugPrint("HCU-VM: User task starting, compiled load Info: CURRENT_PRJ=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[%d], SW_REL=[%d], SW_DELIVER=[%d].\n", \
			HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE, HCU_HARDWARE_PRODUCT_CAT_TYPE, HCU_CURRENT_HW_TYPE, HCU_CURRENT_SW_RELEASE, HCU_CURRENT_SW_DELIVERY);
	HcuDebugPrint("HCU-VM: BXXH(TM) HCU(c) Application Layer start and initialized, build at %s, %s.\n", __DATE__, __TIME__);

	//初始化全局总控表(1)
	if (TASK_ID_MAX > MAX_TASK_NUM_IN_ONE_HCU){
		HcuErrorPrint("HCU-VM: Initialize HCU-VM failure, configuration of MAX_TASK_NUM_IN_ONE_HCU error!\n");
		return FAILURE;
	}
	memset(&zHcuVmCtrTab, 0, sizeof(HcuVmCtrTab_t));
	for (i=TASK_ID_MIN; i<TASK_ID_MAX; i++){
		zHcuVmCtrTab.task[i].TaskId = i;
		zHcuVmCtrTab.task[i].pnpState = HCU_TASK_PNP_INIT;
	}

	//初始化全局工参表(2)
	memset(&zHcuSysEngPar, 0, sizeof(HcuSysEngParTab_t));

	//初始化全局统计性能表(3)
	memset(&zHcuSysStaPm, 0, sizeof(HcuSysStaPm_t));

	//TrACE INIT
	TraceInit();

	//Init Fsm
	FsmInit();

	return SUCCESS;
}


//HCU local init for whole application
OPSTAT hcu_vm_application_task_env_init(void)
{
	int item = 0;
	UINT32 taskid = 0;

	//先从工程参数中读取配置信息到任务表
	zHcuVmCtrTab.task[TASK_ID_ETHERNET].pnpState = zHcuSysEngPar.comm.commHwBoardEthernet;
	zHcuVmCtrTab.task[TASK_ID_USBNET].pnpState = zHcuSysEngPar.comm.commHwBoardUsbnet;
	zHcuVmCtrTab.task[TASK_ID_WIFI].pnpState = zHcuSysEngPar.comm.commHwBoardWifi;
	zHcuVmCtrTab.task[TASK_ID_3G4G].pnpState = zHcuSysEngPar.comm.commHwBoard3g4g;
	zHcuVmCtrTab.task[TASK_ID_GPS].pnpState = zHcuSysEngPar.comm.commHwBoardGps;
	zHcuVmCtrTab.task[TASK_ID_LCD].pnpState = zHcuSysEngPar.comm.commHwBoardLcd;
	zHcuVmCtrTab.task[TASK_ID_LED].pnpState = zHcuSysEngPar.comm.commHwBoardLed;
	zHcuVmCtrTab.task[TASK_ID_ZEEGBE].pnpState = zHcuSysEngPar.comm.commHwBoardZeegbe;
	zHcuVmCtrTab.task[TASK_ID_FLASH].pnpState = zHcuSysEngPar.comm.commHwBoardFlash;
	zHcuVmCtrTab.task[TASK_ID_MODBUS].pnpState = zHcuSysEngPar.comm.commFrameModbus;
	zHcuVmCtrTab.task[TASK_ID_SPSVIRGO].pnpState = zHcuSysEngPar.comm.commFrameSpsvirgo;
	zHcuVmCtrTab.task[TASK_ID_AVORION].pnpState = zHcuSysEngPar.comm.commFrameAvorion;
	zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].pnpState = zHcuSysEngPar.comm.commFrameCloudvela;
	zHcuVmCtrTab.task[TASK_ID_I2CBUSLIBRA].pnpState = zHcuSysEngPar.comm.commFrameI2cbuslibra;
	zHcuVmCtrTab.task[TASK_ID_SPIBUSARIES].pnpState = zHcuSysEngPar.comm.commFrameSpibusaries;
	zHcuVmCtrTab.task[TASK_ID_NBIOTCJ188].pnpState = zHcuSysEngPar.comm.commFrameNbiotcj188;
	zHcuVmCtrTab.task[TASK_ID_NBIOTQG376].pnpState = zHcuSysEngPar.comm.commFrameNbiotqg376;
	zHcuVmCtrTab.task[TASK_ID_SPS485].pnpState = zHcuSysEngPar.comm.commFrontSps485;
	zHcuVmCtrTab.task[TASK_ID_SPS232].pnpState = zHcuSysEngPar.comm.commFrontSps232;
	zHcuVmCtrTab.task[TASK_ID_MICROPHONE].pnpState = zHcuSysEngPar.comm.commFrontMicrophone;
	zHcuVmCtrTab.task[TASK_ID_CAMERA].pnpState = zHcuSysEngPar.comm.commFrontCamera;
	zHcuVmCtrTab.task[TASK_ID_BLE].pnpState = zHcuSysEngPar.comm.commFrontBle;
	zHcuVmCtrTab.task[TASK_ID_EMC].pnpState = zHcuSysEngPar.comm.commFrontSensorEmc;
	zHcuVmCtrTab.task[TASK_ID_PM25].pnpState = zHcuSysEngPar.comm.commFrontSensorPm25;
	zHcuVmCtrTab.task[TASK_ID_TEMP].pnpState = zHcuSysEngPar.comm.commFrontSensorTemp;
	zHcuVmCtrTab.task[TASK_ID_HUMID].pnpState = zHcuSysEngPar.comm.commFrontSensorHumid;
	zHcuVmCtrTab.task[TASK_ID_WINDDIR].pnpState = zHcuSysEngPar.comm.commFrontSensorWinddir;
	zHcuVmCtrTab.task[TASK_ID_WINDSPD].pnpState = zHcuSysEngPar.comm.commFrontSensorWindspd;
	zHcuVmCtrTab.task[TASK_ID_NOISE].pnpState = zHcuSysEngPar.comm.commFrontSensorNoise;
	zHcuVmCtrTab.task[TASK_ID_HSMMP].pnpState = zHcuSysEngPar.comm.commFrontSensorHsmmp;
	zHcuVmCtrTab.task[TASK_ID_PM25SHARP].pnpState = zHcuSysEngPar.comm.commFrontSensorPm25Sharp;
	zHcuVmCtrTab.task[TASK_ID_IWM].pnpState = zHcuSysEngPar.comm.commFrontSensorIwm;
	zHcuVmCtrTab.task[TASK_ID_IHM].pnpState = zHcuSysEngPar.comm.commFrontSensorIhm;
	zHcuVmCtrTab.task[TASK_ID_IGM].pnpState = zHcuSysEngPar.comm.commFrontSensorIgm;
	zHcuVmCtrTab.task[TASK_ID_IPM].pnpState = zHcuSysEngPar.comm.commFrontSensorIpm;
	zHcuVmCtrTab.task[TASK_ID_CANITFLEO].pnpState = zHcuSysEngPar.comm.commFrontCanitf;

	//扫描任务输入配置表
	//起始必须是TASK_ID_MIN条目
	if (zHcuVmCtrTaskStaticCfg[0].taskInputId != TASK_ID_MIN){
		HcuErrorPrint("HCU-VM: Initialize HCU-VM failure, task input configuration error!\n");
		return FAILURE;
	}
	strcpy(zHcuVmCtrTab.task[TASK_ID_MIN].taskName, zHcuVmCtrTaskStaticCfg[0].taskInputName);

	//以TASK_ID_MAX为终止条目
	for(item=1; item < MAX_TASK_NUM_IN_ONE_HCU; item++){
		if(zHcuVmCtrTaskStaticCfg[item].taskInputId == TASK_ID_MAX){
			break;
		}
		if ((zHcuVmCtrTaskStaticCfg[item].taskInputId <= TASK_ID_MIN) || (zHcuVmCtrTaskStaticCfg[item].taskInputId > TASK_ID_MAX)){
			HcuErrorPrint("HCU-VM: Initialize HCU-VM failure, task input configuration error!\n");
			return FAILURE;
		}
	}

	//从任务配置输入区域读取参数到系统任务表，一旦遇到TASK_ID_MAX就终止
	item = 0;
	while(zHcuVmCtrTaskStaticCfg[item].taskInputId != TASK_ID_MAX){
		taskid = zHcuVmCtrTaskStaticCfg[item].taskInputId;
		//系统工程参数对于任务的启动具备更高的优先级
		if (zHcuVmCtrTab.task[taskid].pnpState == HCU_TASK_PNP_INIT) zHcuVmCtrTab.task[taskid].pnpState = HCU_TASK_PNP_ON;
		strcpy(zHcuVmCtrTab.task[taskid].taskName, zHcuVmCtrTaskStaticCfg[item].taskInputName);
		zHcuVmCtrTab.task[taskid].taskFuncEntry = zHcuVmCtrTaskStaticCfg[item].fsmFuncEntry;
		item++;
	}
	//最后一项必定是TASK_ID_MAX
	strcpy(zHcuVmCtrTab.task[TASK_ID_MAX].taskName, zHcuVmCtrTaskStaticCfg[item].taskInputName);

	//检查所有启动任务的合法性
	for (taskid = TASK_ID_MIN; taskid <= TASK_ID_MAX; taskid++){
		//如果初始化表中是NULL，则不能启动该任务
		if(zHcuVmCtrTab.task[taskid].taskFuncEntry == NULL) zHcuVmCtrTab.task[taskid].pnpState = HCU_TASK_PNP_OFF;
	}

	//返回
	return SUCCESS;
}


/**************************************************************************************
 * 创建任务，使用的是线程模拟，各个任务之间可以共享内存
 * To Create a thread
 * void  hcu_task_create( UINT32 task_id,          		out: pointer to zHcuTaskId[task_id]
 *                   void *(*task_func)(void *),        in: task's entry point
 *                   void *arg,                         in: parameter of the task's function
 *                   int prio )                         in: priority of the task*
 ***************************************************************************************/
UINT32 hcu_task_create(UINT32 task_id, void *(*task_func)(void *), void *arg, int prio)
{
	int                     err;
	pthread_attr_t          attr;
	struct  sched_param     sched;

	// The thread is initialized with attributes object default values
	if(0!= (err = pthread_attr_init(&attr)))
	{
		HcuDebugPrint("HCU-VM: pthread_attr_init() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	// Change the inherit scheduling attribute
	// of the specified thread attribute object,because PTHREAD_INHERIT_SCHED is default value
	if (0!= (err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)))
	{
		HcuDebugPrint("HCU-VM: pthread_attr_setinheritsched() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	// Set the contention scope attribute of
	// the specified thread attributes object, the thread compete with all threads on the station
	if (0 != (err = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM)))
	{
		HcuDebugPrint("HCU-VM: pthread_attr_setscope() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	// Set the scheduling policy,SCHED_OTHER is the default
	if (0 != (err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO)))
	{
		HcuDebugPrint("HCU-VM: pthread_attr_setschedpolicy() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	// Set the priority of the task
	sched.sched_priority = prio;
	if (0 != (err = pthread_attr_setschedparam(&attr, &sched)))
	{
		HcuErrorPrint("HCU-VM: pthread_attr_setschedparam() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	// Checking task_id
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id!! err=%d, errno=%d, %s, task_id=%d\n", err, errno, strerror(err), task_id);
		return FAILURE;
	}

	//As FsmProcessLaunch has to transfer task_id in, we will use global variance of zHcuFsmTable.currentTaskId
	//So task_id has to store into there
	//Wonderful mechanism!!!
	if (zHcuVmCtrTab.fsm.currentTaskId != TASK_ID_INVALID){
		hcu_sleep(1);
	}
	if (zHcuVmCtrTab.fsm.currentTaskId != TASK_ID_INVALID){
		HcuErrorPrint("HCU-VM: Task_id not yet read by running process, new task create failure!\n");
		return FAILURE;
	}
	zHcuVmCtrTab.fsm.currentTaskId = task_id;

	// creation of the task
	err=pthread_create(&(zHcuVmCtrTab.task[task_id].ThrId), &attr, (void *(*)(void*))(task_func), (void*)arg);
	if(err != 0)
	{
		HcuDebugPrint("HCU-VM: pthread_create() nok!! err=%d, errno=%d, %s\n", err, errno, strerror(err));
		return FAILURE;
	}

	//zHcuTaskInfo[task_id].TaskName to be added in another way, TBD
	zHcuVmCtrTab.task[task_id].TaskId = task_id;
	zHcuVmCtrTab.task[task_id].processId = getpid(); //进程号存入
	HcuDebugPrint("HCU-VM: pthread_create() OK ...\n");
	/* ERRORS
	       EAGAIN Insufficient resources to create another thread, or a system-imposed limit on the number of threads was encountered.  The latter case may occur in two ways: the RLIMIT_NPROC soft  resource  limit
	              (set via setrlimit(2)), which limits the number of process for a real user ID, was reached; or the kernel's system-wide limit on the number of threads, /proc/sys/kernel/threads-max, was reached.

	       EINVAL Invalid settings in attr.

	       EPERM  No permission to set the scheduling policy and parameters specified in attr.
	 */
	return SUCCESS;
}

//kill task
//有关进程与线程方面的杀死清理，未来需要再完善
UINT32 hcu_task_delete(UINT32 task_id)
{
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Not exist
	if (zHcuVmCtrTab.task[task_id].ThrId ==0){return FAILURE;}

	//只是清理掉FSM中的信息
	FsmRemove(task_id);

	//以下KILL线程会出错，未来待研究验证
	/*
	//SIGKILL用于杀死整个进程, 返回为ESRCH则表示线程不存在
	if (pthread_kill(zHcuTaskInfo[task_id].ThrId, 0) != (EINVAL || ESRCH)) {
		pthread_kill(zHcuTaskInfo[task_id].ThrId, SIGQUIT);
		return SUCCESS;
	}else{
		return FAILURE;
	}
	*/
	return SUCCESS;
}

/**************************************************************************************
 * 创建任务队列，以便用于任务之间消息通信, 每个任务创建唯一一个队列，所以使用TASKID作为索引
 * void  Hcu_MsgQue_Create( UINT task_id)
 ***************************************************************************************/
UINT32 hcu_msgque_create(UINT32 task_id)
{
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Generate msgKey
	int msgKey=0, msgQid=0;
	msgKey = task_id + HCU_TASK_QUEUE_ID_START;
	if ((msgKey < TASK_QUE_ID_MIN) || (msgKey > TASK_QUE_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, msgKey=%d!!!\n", msgKey);
		return FAILURE;
	}
	//Checking msgQid exiting or not, if YES, just DELETE.
	msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/
	if (msgQid>=0){
		//直接使用已有的QUEID，而不是重新创建, 删除消息队列
		if (msgctl(msgQid, IPC_RMID, NULL) == FAILURE){
			HcuErrorPrint("HCU-VM: Remove Queue error!\n");
			return FAILURE;
		}
		//HcuErrorPrint("HCU-VM: Already remove Queue Id, input Key = %d\n", msgKey);
	}
	//Re-create the relevant message queue Id
	msgQid = msgget(msgKey, IPC_CREAT|0666);/*创建消息队列*/
	if(msgQid <0){
		HcuErrorPrint("HCU-VM: Failed to create msg-queue | errno=%d [%s]\n", errno, strerror(errno));
		return FAILURE;
	}
	zHcuVmCtrTab.task[task_id].QueId = msgQid;
	return SUCCESS;
}

//kill message queue
UINT32 hcu_msgque_delete(UINT32 task_id)
{
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Not exist
	//特殊情况下，msgqid=0也是可能的，这种情形，需要再仔细考虑
	if (zHcuVmCtrTab.task[task_id].QueId == 0) {return FAILURE;}

	//Delete queue
	msgctl(zHcuVmCtrTab.task[task_id].QueId, IPC_RMID, NULL); //删除消息队列

	zHcuVmCtrTab.task[task_id].QueId =0;
	return SUCCESS;
}

//查询消息队列
UINT32 hcu_msgque_inquery(UINT32 task_id)
{
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Generate msgKey
	int msgKey=0, msgQid=0;
	msgKey = task_id + HCU_TASK_QUEUE_ID_START;
	if ((msgKey < TASK_QUE_ID_MIN) || (msgKey > TASK_QUE_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, msgKey=%d!!!\n", msgKey);
		return FAILURE;
	}

	msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/

	return msgQid;
}

//同步不同进程之间的消息队列，确保不同进程之间的任务可以互发消息
UINT32 hcu_msgque_resync(void)
{
	UINT32 task_id = 0;
	int msgKey=0, msgQid=0;
	for (task_id = TASK_ID_MIN; task_id < TASK_ID_MAX; task_id++){
		//Generate msgKey
		msgKey = task_id + HCU_TASK_QUEUE_ID_START;
		if ((msgKey < TASK_QUE_ID_MIN) || (msgKey > TASK_QUE_ID_MAX)){
			HcuErrorPrint("HCU-VM: Error on task_id, msgKey=%d!!!\n", msgKey);
			return FAILURE;
		}
		//Checking msgQid exiting or not
		//这里，不在本进程中的任务模块，只有最为基本的消息队列和Task_ID，其它信息一律不存在
		msgQid=msgget(msgKey, IPC_EXCL);  /*检查消息队列是否存在*/
		if (msgQid>=0){
			zHcuVmCtrTab.task[task_id].QueId = msgQid;
			zHcuVmCtrTab.task[task_id].TaskId = task_id;
		}
	}//For loop

	return SUCCESS;
}

//聚合创建任务，消息队列，并直接让其开始运行
UINT32 hcu_task_create_and_run(UINT32 task_id, HcuFsmStateItem_t* pFsmStateItem)
{
	OPSTAT ret = 0;

	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Input Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Init Fsm table
	ret = FsmAddNew(task_id, pFsmStateItem);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-VM: Init state machine FsmAddNew error, taskid = %d\n", task_id);
		return FAILURE;
	}
	HCU_DEBUG_PRINT_CRT("HCU-VM: FsmAddNew Successful, taskId = 0x%x [%s].\n", task_id, zHcuVmCtrTab.task[task_id].taskName);

    //Create Queid
   	ret = hcu_msgque_create(task_id);
    if (ret == FAILURE)
    {
    	HcuErrorPrint("HCU-VM: Create queue unsuccessfully, taskId = %d\n", task_id);
    	return FAILURE;
    }
    HCU_DEBUG_PRINT_CRT("HCU-VM: hcu_msgque_create Successful, taskId = 0x%x [%s].\n", task_id, zHcuVmCtrTab.task[task_id].taskName);

    //Create task and make it running for ever
    ret = hcu_task_create(task_id, (CALLBACK)FsmProcessingLaunch, (void *)NULL, HCU_THREAD_PRIO);
    if (ret == FAILURE)
    {
    	HcuErrorPrint("HCU-VM: Create task un-successfully, taskid = %d\n", task_id);
    	return FAILURE;
    }
    HCU_DEBUG_PRINT_CRT("HCU-VM: hcu_task_create Successful, taskId = 0x%x [%s].\n", task_id, zHcuVmCtrTab.task[task_id].taskName);
    HCU_DEBUG_PRINT_IPT("HCU-VM: Whole task environment setup successful, taskId = 0x%x [%s].\n", task_id, zHcuVmCtrTab.task[task_id].taskName);

	return SUCCESS;
}


//message send basic processing
//All in parameters
UINT32 hcu_message_send(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len)
{
	//Checking task_id range
	if ((dest_id < TASK_ID_MIN) || (dest_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	if ((src_id < TASK_ID_MIN) || (src_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, src_id=%d!!!\n", src_id);
		return FAILURE;
	}
	if (param_len>MAX_HCU_MSG_BODY_LENGTH){
		HcuErrorPrint("HCU-VM: Too large message length than HCU set capability, param_len=%d!!!\n", param_len);
		return FAILURE;
	}

	//Starting to process message send
	//Not use malloc/free function, as it is a little bit complex to manage, to be checked here the memory conflict
	int ret=0;
	HcuMsgSruct_t *msg;
	msg = malloc(sizeof(HcuMsgSruct_t));
	if (msg == NULL){
		HcuErrorPrint("HCU-VM: Message send allocate memory error, exit!\n");
		return FAILURE;
	}
	//Init to clean this memory area
	//初始化消息内容，是为了稳定可靠安全
	memset(msg, 0, sizeof(HcuMsgSruct_t));
	msg->msgType = msg_id;
	msg->dest_id = dest_id;
	msg->src_id = src_id;
	msg->msgLen = param_len;
	memcpy(&(msg->msgBody[0]), param_ptr, param_len);
	//msg->msgBody是否可以直接当指针？

	/*没有采用精确的长度，也是为了程序简单稳定可靠，所有队列长度都是一致相等的
	msgflg
	0：当消息队列满时，msgsnd将会阻塞，直到消息能写进消息队列
	IPC_NOWAIT：当消息队列已满的时候，msgsnd函数不等待立即返回
	IPC_NOERROR：若发送的消息大于size字节，则把该消息截断，截断部分将被丢弃，且不通知发送进程。*/
	/*MsgSend Return 错误代码
	EAGAIN：参数msgflg设为IPC_NOWAIT，而消息队列已满
	EIDRM：标识符为msqid的消息队列已被删除
	EACCESS：无权限写入消息队列
	EFAULT：参数msgp指向无效的内存地址
	EINTR：队列已满而处于等待情况下被信号中断
	EINVAL：无效的参数msqid、msgsz或参数消息类型type小于0  */
	//ret = msgsnd(zHcuTaskInfo[dest_id].QueId, msg, (sizeof(HcuMsgSruct_t)-sizeof(long)), IPC_NOWAIT);
	ret = msgsnd(hcu_msgque_inquery(dest_id), msg, (sizeof(HcuMsgSruct_t)-sizeof(long)), IPC_NOWAIT);
	free(msg);
	if ( ret < 0 ) {
		HcuErrorPrint("HCU-VM: msgsnd() write msg failed, errno=%d[%s], dest_id = %d [%s]\n",errno,strerror(errno), dest_id, zHcuVmCtrTab.task[dest_id].taskName);
		zHcuVmCtrTab.task[dest_id].QueFullFlag = HCU_TASK_QUEUE_FULL_TRUE;
		return FAILURE;
	}

	/*
	 *  Message Trace processing
	 *  注意字符串长度，太小会出现内存飞掉的情形，MessageTrace的数据配置来源于数据库，层次比这个还要高，虽然有点怪怪的，但不影响总体架构
	 *  最开始采用IFDEF的形式，后期完善后改为更为直接的代码方式
	 *  软件开发中，DEBUG和TRACE是两种最常用/最有用的调试模式，比单步还有用，这不仅是因为熟手不需要单步执行，而且也是因为多线程多进程单步执行环境制造的复杂性
	 *
	 *  有关MESSAGE TRACE的效率，是一个要注意的问题，当系统负载不高时，打开所有的TRACE是合适的，但一旦部署实际系统，TRACE需要减少到最低程度，这是原则，实际需要
	 *  维护人员根据情况灵活把我
	 *
	 *  本TRACE功能，提供了多种工作模式
	 *
	 */
	char s1[TASK_NAME_MAX_LENGTH+2]="", s2[TASK_NAME_MAX_LENGTH+2]="", s3[MSG_NAME_MAX_LENGTH]="";
	switch (zHcuSysEngPar.traceMode)
	{
		case HCU_TRACE_MSG_MODE_OFF:
			break;

		case HCU_TRACE_MSG_MODE_INVALID:
			break;

		case HCU_TRACE_MSG_MODE_ALL:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			break;

		case HCU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if (msg_id != MSG_ID_COM_TIME_OUT){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_ALL_BUT_HEART_BEAT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((msg_id != MSG_ID_COM_TIME_OUT) && (msg_id != MSG_ID_COM_HEART_BEAT) && (msg_id != MSG_ID_COM_HEART_BEAT_FB)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_TO_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_TO_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToRestrict!= TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_FROM_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleToRestrict!= TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleToAllow == TRUE)
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleToRestrict != TRUE)
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MSGID_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_MSGID_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_TO_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_TO_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_FROM_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_FROM_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgAllow == TRUE) && (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE)
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToAllow == TRUE)
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleFromAllow == TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		case HCU_TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT:
			taskid_to_string(dest_id, s1);
			taskid_to_string(src_id, s2);
			msgid_to_string(msg_id, s3);
			if ((zHcuSysEngPar.traceList.msg[msg_id].msgId == msg_id) && (zHcuSysEngPar.traceList.msg[msg_id].msgRestrict != TRUE) \
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleId == dest_id) && (zHcuSysEngPar.traceList.mod[dest_id].moduleToRestrict != TRUE)
					&& (zHcuSysEngPar.traceList.mod[src_id].moduleId == src_id) && (zHcuSysEngPar.traceList.mod[src_id].moduleFromRestrict != TRUE)\
					&& (zHcuSysEngPar.traceList.msg[msg_id].msgCtrFlag == TRUE) && (zHcuSysEngPar.traceList.mod[src_id].moduleCtrFlag == TRUE)\
					&& (zHcuSysEngPar.traceList.mod[dest_id].moduleCtrFlag == TRUE)){
				HcuDebugPrint("MSGTRC: MSGID=0X%04X%s, DID=%02X%s, SID=%02X%s, LEN=%d.\n", msg_id, s3, dest_id, s1, src_id, s2, param_len);
			}
			break;

		default:
			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
				HcuErrorPrint("HCU-VM: System Engineering Parameter Trace Mode setting error! DebugMode=%d\n", zHcuSysEngPar.debugMode);
			}
			break;
	}

	return SUCCESS;
}

/*
Linux的进程间通讯方式有多种：如通过网卡转发的UDP或TCP方式；消息队列；共享内存；具体怎么编写程序我就不写了，大家在网上google一下什么都有了。在这里我主要说一下用linux的消息队列如何实现可靠的进程间通讯。这里的“进程间”指的是同一个PC上的，跨PC的只能用网络方式实时传递消息。
IPC之消息队列的优势：
为什么要用消息队列呢？根据我所见的，大部分人都是用udp或tcp 进行本机进程间通讯，但一旦网卡失效，程序就歇菜了。我在负责开发一个firmware时就要考虑这个问题，从而写了一个可靠的基于消息队列的进程间通讯。
为什么说要可靠的呢？实验表明，简单的一个消息队列通讯是会有一些问题的：
1.     程序异常退出后，队列不会自动删除。
2.     当通讯双方的一方退出后重新链接的问题。
3.     软件升级时，消息体长度发生变化时，新老版本兼容问题。
4.     1对多或多对多通讯问题。

下面以我的程序IPC模块为例介绍一下，本程序实现了可靠的，1对多，自动重连功能的IPC通讯。IPC模块分为server和cline两部分。 Server部分运行在我们的平台上，cline部分已静态库的方式提供API给应用程序。

Server线程的执行流程是：
1.     编写程序时为server及各个cline分配一个ipc id，如：
typedef enum t_fwipc_id{
IPC_SVR_ID = 1,        //ipc server id
APP1_IPC_ID,           //ipc cline id
APP2_IPC_ID,           //ipc cline id
MAX_IPC_ID             //IPC id count, always the last one
}FW_IPC_ID;
2.     初始化消息队列：创建消息队列，如果已存在，要先删除后再创建。
3.     循环接收消息：消息接受buf清0，msgrcv采用阻塞方式接收消息，如果msgrcv返回失败或消息体长度为0，重新创建消息队列，重复step 1。如果接收成功，则处理消息，返回消息处理结果。IPC通道状态标志置位。然后继续等待消息。

Cline初始化流程：
1.     检测server进程是否存在。（如何检测及如果防止多个server同时运行，我会在另外一篇文章介绍），如果不在，退出。
2.     初始化消息队列
3.     发送注册消息给server。把由server事先定义的ipc cline id发送给server。Server接到消息后会核对IPC cline版本信息。置IPC通道状态标志。
4.     cline设置自己的IPC通道状态标志。
5.     APP可以调用IPC cline API与firmware平台交互信息了。

Cline API发送消息的流程：
1.     发生消息不能用阻塞方式，而是IPC_NOWAIT方式。
2.     每个IPC cline的API在被调用时，都应先检查IPC通道标志，如果标志为“0”，则需要重新执行初始化流程。
3.     检查互斥锁。一个IPC cline同一时间只允许一个API被调用，一个API被调用后，收到server的答复后，释放锁并返回后，下一次API调用才能成功。
4.     每个API有一个特定的超时重试值，这个值要根据server在处理这个消息所费时间来决定的。如果在指定的时间（try次数×try间隔延时）里，server没有返回处理结果，则API返回失败，清IPC通道标志。如果消息发送失败或接收server应答失败，则也清除通道标准，并返回。
*/


/*
 * 慢系统调用的问题
Android平台下程序中的一个线程使用setitimer，而主进程则作为套接字接收服务器，但是却经常出现 accept返回Interrupted system call错误，
追寻原因及解决方法：

出自unix网络编程（第三版）第五章 P115页。

我们用术语慢系统调用（slow system call）描述accept函数，该术语也适用于那些可能永远阻塞的系统调用。永远阻塞的系统调用有可能永远无法返回，多数网络支持函数都属于这一类。举例来说，如果没有客户连接到服务器上，那么服务器的accept调用就没有返回的保证，类似的如果客户端从未发送过数据，那么read调用将永不返回。其他慢系统调用的例子是对管道和终端设备的读和写。一个值得注意的例外是磁盘IO，它们一般都会返回到调用者（假设没有灾难性的硬件事故）。
  适用于慢系统调用的基本规则是：当阻塞于某个慢系统调用的一个进程捕获某个信号且相应处理函数返回时，该系统调用可能返回一个EINTR错误。所以，我们必须对慢系统调用返回的EINTR有所准备。
  为了处理被中断的accept,可以包装证：
 for (;;) {
    if ((connfd = accept(listenfd, NULL, NULL)) < 0) {
  if (errno == EINTR)
    continue;
  else
    err_sys("accept error");
    }
        }
  这段代码所做的事情就是自己重启被中断的系统调用。对于accept以及诸如read、write、select和open之类的函数，这是适合的。不过有一个函数我们不能重启:connect。如果该函数返回EINTR，我们就不能再次调用它，否则将立即返回一个错误。当connect被一个捕获信号中断而且不自动重启时，我们必须调用select来等待连接完成。
http://www.cs.moravian.edu/pipermail/cs320/2008-February/000040.html
   在本人的程序中，为了能够使用一种机制，使accept不会被SIGALRM信号中断，尝试了使用pthread_sigmask函数，但pthread_sigmask 总是返回失败。无奈尝试了下再起一个线程，一个线程作为timer，另一个线程进行套接字数据接收处理，主线程通过等待互斥量来进行阻塞（不能使用sleep，Timer和sleep也会存在冲突，timer到时后的SIGALRM信号会导致sleep被唤醒），测试问题没有出现。。。
    多个线程的SIGALRM信号不会影响？ 原因没有想明白，怀疑和Android内核处理有关。
 *
 */
//Reveive message
//dest_id: in
//*msg: out
// ret: 接受消息中的errno=4/EINTR需要单独处理，不然会出现被SIGALRM打断的情形
UINT32 hcu_message_rcv(UINT32 dest_id, HcuMsgSruct_t *msg)
{
	//Checking task_id range
	if ((dest_id < TASK_ID_MIN) || (dest_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	int ret = 0;
	//UINT32 size;
	//没仔细研究过这里msgrcv的参数，需要后期进一步打磨推敲
	//No need use msgget to retrieve new message
    //msgtyp 0 接收第一个消息
    //msgtyp <0 接收类型等于或者小于msgtyp绝对值的第一个消息
    //msgtyp >0 接收类型等于msgtyp的第一个消息
    //msgflg 0 阻塞式接收消息，没有该类型的消息msgrcv函数一直阻塞等待
    //msgflg IPC_NOWAIT 如果没有返回条件的消息调用立即返回，此时错误码为ENOMSG
    //msgflg IPC_EXCEPT 与msgtype配合使用返回队列中第一个类型不为msgtype的消息
    //msgflg IPC_NOERROR 如果队列中满足条件的消息内容大于所请求的size字节，则把该消息截断，截断部分将被丢弃
	/*返回值：
		成功时返回所获取信息的长度，失败返回-1，错误信息存于error
		Number of bytes copied into message buffer -1 on error: errno =
			E2BIG (Message length is greater than msgsz, no MSG_NOERROR)
			EACCES (No read permission)
			EFAULT (Address pointed to by msgp is invalid)
			EIDRM (Queue was removed during retrieval)
			EINTR (Interrupted by arriving signal)
			EINVAL (msgqid invalid, or msgsz less than 0)
			ENOMSG (IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request)
	*/
	//ret = msgrcv(zHcuTaskInfo[dest_id].QueId, msg, (sizeof(HcuMsgSruct_t) - sizeof(long)), 0, 0);
	ret = msgrcv(hcu_msgque_inquery(dest_id), msg, (sizeof(HcuMsgSruct_t) - sizeof(long)), 0, 0);
	if (errno == EINTR) //EINTR, system interrupt call error
		return EINTR;
	if ( ret < 0 ) {
		HcuErrorPrint("HCU-VM: msgrcv() receive msg failed, Qid=%d, msg=%08X, errno=%d[%s]\n",
				zHcuVmCtrTab.task[dest_id].QueId, msg, errno, strerror(errno));
		return FAILURE;
	}
	return SUCCESS;
}

UINT32 hcu_message_rcv_no_wait(UINT32 dest_id, HcuMsgSruct_t *msg)
{
	//Checking task_id range
	if ((dest_id < TASK_ID_MIN) || (dest_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	int ret = 0;
	//没仔细研究过这里msgrcv的参数，需要后期进一步打磨推敲
	//No need use msgget to retrieve new message
    //msgtyp 0 接收第一个消息
    //msgtyp <0 接收类型等于或者小于msgtyp绝对值的第一个消息
    //msgtyp >0 接收类型等于msgtyp的第一个消息
    //msgflg 0 阻塞式接收消息，没有该类型的消息msgrcv函数一直阻塞等待
    //msgflg IPC_NOWAIT 如果没有返回条件的消息调用立即返回，此时错误码为ENOMSG
    //msgflg IPC_EXCEPT 与msgtype配合使用返回队列中第一个类型不为msgtype的消息
    //msgflg IPC_NOERROR 如果队列中满足条件的消息内容大于所请求的size字节，则把该消息截断，截断部分将被丢弃
	/*返回值：
		成功时返回所获取信息的长度，失败返回-1，错误信息存于error
		Number of bytes copied into message buffer -1 on error: errno =
			E2BIG (Message length is greater than msgsz, no MSG_NOERROR)
			EACCES (No read permission)
			EFAULT (Address pointed to by msgp is invalid)
			EIDRM (Queue was removed during retrieval)
			EINTR (Interrupted by arriving signal)
			EINVAL (msgqid invalid, or msgsz less than 0)
			ENOMSG (IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request)
	*/
	//ret = msgrcv(zHcuTaskInfo[dest_id].QueId, msg, (sizeof(HcuMsgSruct_t) - sizeof(long)), 0, IPC_NOWAIT);
	ret = msgrcv(hcu_msgque_inquery(dest_id), msg, (sizeof(HcuMsgSruct_t) - sizeof(long)), 0, IPC_NOWAIT);

	return ret;
}

//TaskId transfer to string
//待调试的函数，是否需要使用动态内存
//正确的做法也许应该使用zHcuTaskInfo[id].TaskName这个信息来表达
UINT32 taskid_to_string(UINT32 id, char *string)
{
	if ((id<TASK_ID_MIN) || (id>=TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error task Id input!\n");
		return FAILURE;
	}
	char tmp[TASK_NAME_MAX_LENGTH-2]="";
	strcpy(string, "[");
	if (strlen(zHcuVmCtrTab.task[id].taskName)>0){
		strncpy(tmp, zHcuVmCtrTab.task[id].taskName, TASK_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "TASK_ID_XXX");
	}
	strcat(string, "]");
	return SUCCESS;
}

//MsgId transfer to string
//输入的string参数，其内存地址空间和长度预留，是否足够
UINT32 msgid_to_string(UINT32 id, char *string)
{
	if (id <0 || id > MAX_MSGID_NUM_IN_ONE_TASK){
		HcuErrorPrint("HCU-VM: Error Message Id input!\n");
		return FAILURE;
	}
	char tmp[MSG_NAME_MAX_LENGTH-2]="";
	strcpy(string, "[");
	if (strlen(zHcuSysEngTrcMsgCtrStaticCfg[id].msgName)>0){
		strncpy(tmp, zHcuSysEngTrcMsgCtrStaticCfg[id].msgName, MSG_NAME_MAX_LENGTH-3);
		strcat(string, tmp);
	}else{
		strcat(string, "MSG_ID_XXX");
	}
	strcat(string, "]");

	return SUCCESS;
}

//API abstract
//通过时钟问题的解决，这个问题终于解决了，原因就是SLEEP和SIGALM公用同一套信号量，导致相互冲突。时钟采用线程方式后，再也没有问题了
void hcu_sleep(UINT32 second)
{
	if (second <0) second =0;
	second = sleep(second);
	while (second>0)
	{
		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
			//太多的错误，未来需要再研究这个错误出现的原因，这里留下一点点报告的可行性
			//if ((rand()%1000) > 998){
				HcuDebugPrint("HCU-VM: Sleep interrupt by other higher level system call, remaining %d second to be executed\n", second);
			//}
		}
		second = sleep(second);
	}
}

void hcu_usleep(UINT32 usecond)
{
	if (usecond <0) usecond =0;
	usecond = usleep(usecond);
	while (usecond>0)
	{
		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
			HcuErrorPrint("HCU-VM: uSleep interrupt by other higher level system call, remaining %d usecond to be executed\n", usecond);
		}
		usecond = usleep(usecond);
	}
}

//API set timer and back to handler function
UINT32 hcu_timer_polling(time_t sec, UINT32 nsec, void *handler())
{
	return SUCCESS;
}


/*******************************************************************************
** METHOD :FsmInit
**------------------------------------------------------------------------------
** Description :
**    Initialize the module of the fsm.
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : SUCCESS OR FAILURE
*******************************************************************************/
UINT32 FsmInit(void)
{
	UINT32 i;
	HcuDebugPrint("HCU-VM: >>Start init FSM.\n");
	zHcuVmCtrTab.fsm.numOfFsmCtrlTable = 0;
	for(i=0; i<MAX_TASK_NUM_IN_ONE_HCU; i++)
	{
		zHcuVmCtrTab.fsm.pFsmCtrlTable[i].taskId = TASK_ID_INVALID;
		zHcuVmCtrTab.fsm.pFsmCtrlTable[i].numOfFsmArrayElement = 0;
	}
	zHcuVmCtrTab.fsm.currentTaskId = TASK_ID_INVALID;

	HCU_DEBUG_PRINT_FAT("HCU-VM: Maxium (%d) process supported.\n", MAX_TASK_NUM_IN_ONE_HCU);

    return SUCCESS;
}

/*******************************************************************************
** METHOD : FsmAddNew
**------------------------------------------------------------------------------
** Description :
**    fill the FSM table information of entity
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 Taskid : entity identification
**			 pFsmStateItem: pointer to message table of entity
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : SUCCESS OR FAILURE
*******************************************************************************/
UINT32 FsmAddNew(UINT32 task_id, HcuFsmStateItem_t* pFsmStateItem )
{
	OPSTAT ret;
	UINT32 state;
	UINT32 msgid;
	UINT32 item, itemNo, i, j;

	HCU_DEBUG_PRINT_NOR("HCU-VM: >>Register new FSM. TaskId:(%d), pFsm(0x%x)\n", task_id, pFsmStateItem);
	/*
	** Check the array of the state machine.
	*/
	if( NULL== pFsmStateItem)
	{
		HcuErrorPrint("HCU-VM: Can not init FSM machine.\n");
		return FAILURE;
	}
	if((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX))
	{
		HcuErrorPrint("HCU-VM: The task_ID is invalid.\n");
		return FAILURE;
	}
	if( zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].taskId != TASK_ID_INVALID )
	{
		HcuErrorPrint("HCU-VM: This task_id has been already inited.\n");
		return FAILURE;
	}

	/*
	** Check the first entry. 最初一个FSM表单必须以FSM_STATE_ENTRY开始
	*/
	if( pFsmStateItem[FSM_STATE_ENTRY].state != FSM_STATE_ENTRY )
	{
		HcuErrorPrint("HCU-VM: The format of the FSM table is invalid.\n");
		return FAILURE;
	}

    /*
    ** Check each item of the state entry. 最后一个FSM表单使用FSM_STATE_END结束
    */
	for(item=1; item<MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK; item++)
	{
		if( FSM_STATE_END == pFsmStateItem[item].state )
		{
			break;
		}
		if( FSM_STATE_ENTRY == pFsmStateItem[item].state )
		{
			HcuErrorPrint("HCU-VM: Invalid FSM machine.\n");
			return FAILURE;
		}
		if( NULL == pFsmStateItem[item].stateFunc)
		{
			HcuErrorPrint("HCU-VM: Invalid state function pointer.\n");
			return FAILURE;
		}
	}

	/*
	** Check the number of the item.
	** If we can not find the ENDING flag of the state machine after more than
	** MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK entry, raise fatal alarm.
	*/
	if( MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK == item)
	{
		HcuErrorPrint("HCU-VM: Invalid FSM machine -- Can not find the end of the FSM.\n");
		return FAILURE;
	}
	zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].numOfFsmArrayElement = item-1; //有效STATE-MSG条目数，不包括START/END两条

	/*
	**  Insert this new fsm item into fsm table.
	**  The position in the fsm table is based on the task_id.
	*/
	zHcuVmCtrTab.fsm.numOfFsmCtrlTable ++;

	HCU_DEBUG_PRINT_NOR("HCU-VM: FsmAddNew: task_id = 0x%x [%s], src_id= %x, dest_id= %X\n", task_id, zHcuVmCtrTab.task[task_id].taskName, 0, 0);
	HCU_DEBUG_PRINT_NOR("HCU-VM: After add this one, Total (%d) FSM in the table.\n", zHcuVmCtrTab.fsm.numOfFsmCtrlTable);

	/*
	** Save the state machine info.
	*/
	zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].taskId = task_id;
	for(i=0; i<MAX_STATE_NUM_IN_ONE_TASK; i++)
	{
		for(j=0; j<MAX_MSGID_NUM_IN_ONE_TASK; j++)
		{
			zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].pFsmArray[i][j].stateFunc = NULL;
		}
	}

	//检查Msg_id的合法性，不能超过FF，这里很有技巧，全靠消息ID不超过FF来设定，小心消息过多的情况
	//内部消息也不能使用任意定义的数值，因为这里将采用MsgId进行索引，以提高效率
	for(itemNo=0; itemNo < item; itemNo ++ )
	{
		state = pFsmStateItem[itemNo].state;
		if(state >= MAX_STATE_NUM_IN_ONE_TASK)
		{
			HcuErrorPrint("HCU-VM: The State number > %d\n", MAX_STATE_NUM_IN_ONE_TASK);
			return FAILURE;
		}
		msgid = pFsmStateItem[itemNo].msg_id & MASK_MSGID_NUM_IN_ONE_TASK;
		zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].pFsmArray[state][msgid].stateFunc = pFsmStateItem[itemNo].stateFunc;
	}

	/*
	** Start the task entry init api/function
	*/
	//Input message id = 0, param_ptr = NULL, param_len = 0.
    /*
    ** call the entry
    ** 这一块代码不能复用，因为这个任务和被执行任务并不再同一个空间之内，它只能被目标任务的入口函数进行执行
    ** 该函数API必须在每个任务中进行表达，否则一定会出错， 该代码需要移动到任务创建成功后的入口函数中进行调用
    */
	//(zHcuFsmTable.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_COM_MIN].stateFunc)(task_id, 0, NULL, 0);

	/*
	** Set the state to init state(FSM_STATE_IDLE).
	** 一旦任务创建，自动转移到IDLE状态，因而每个任务独特的初始化不能处理状态，也不能被FsmRunEngine调度，而必须被任务创建的入口函数所执行
	*/
	ret = FsmSetState(task_id, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-VM: Error set FSM State!\n");
	}

    return SUCCESS;
}

UINT32 FsmRemove(UINT32 task_id)
{
	//入参检查
	if((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX))
	{
		HcuErrorPrint("HCU-VM: The task_ID is invalid.\n");
		return FAILURE;
	}

	//设置无效
	zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].taskId = TASK_ID_INVALID;

    return SUCCESS;
}


/*******************************************************************************
** METHOD : FsmProcessingLaunch
**------------------------------------------------------------------------------
** Description :
**    check and handle message loop as each task entry
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 none
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : SUCCESS OR FAILURE
*******************************************************************************/
UINT32 FsmProcessingLaunch(void)
{
	OPSTAT ret;
	HcuMsgSruct_t rcv;
	UINT32 task_id = 0; //Get current working task_id

	task_id = zHcuVmCtrTab.fsm.currentTaskId;
	zHcuVmCtrTab.fsm.currentTaskId = TASK_ID_INVALID;
	/*
	** Check the task_id
	*/
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	/*
	** Run each task entry API, give each task a chance to init any specific information
	*/
	if (zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc != NULL){
		(zHcuVmCtrTab.fsm.pFsmCtrlTable[task_id].pFsmArray[FSM_STATE_ENTRY][MSG_ID_ENTRY].stateFunc)(task_id, 0, NULL, 0);
	}else{
		HCU_DEBUG_PRINT_NOR("HCU-VM: Task (ID=%d) get no init entry fetched!\n", task_id);
	}

	//Loop for ever
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(task_id, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuDebugPrint("HCU-VM: STM kernal receive message error, hold for further action!\n");
			//continue;
			//To be defined here the error case, to recover everything
			hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			//hcu_sleep(1); //to be removed
			ret = FsmRunEngine(rcv.msgType, rcv.dest_id, rcv.src_id, rcv.msgBody, rcv.msgLen);
			if (ret == FAILURE){
				HcuErrorPrint("HCU-VM: Error execute FsmRun state machine!\n");
			}
		}
	}//While(1)
	return SUCCESS;
}

/*******************************************************************************
** METHOD : FsmApiRun
**------------------------------------------------------------------------------
** Description :
**    supply action function entry of task that receive current Event in current state
**------------------------------------------------------------------------------
** Arguments :
**    IN:	 src_id: identification of task that send out current message
** 		     dest_id: identification of task that receive current message
** 		     commandId: identification of current message
** 		     param_ptr: pointer to current message structure
** 		     param_len: message length
**    OUT:   none
**    INOUT: none
**------------------------------------------------------------------------------
** Return value : SUCCESS OR FAILURE
*******************************************************************************/
UINT32 FsmRunEngine(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len)
{
	UINT8  state;
	OPSTAT ret;
	UINT32 mid;

	/*
	** Check the task_id, message_id and par_length
	*/
	if ((dest_id < TASK_ID_MIN) || (dest_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, dest_id=%d!!!\n", dest_id);
		return FAILURE;
	}
	if ((src_id < TASK_ID_MIN) || (src_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, src_id=%d!!!\n", src_id);
		return FAILURE;
	}
	if (param_len>MAX_HCU_MSG_BODY_LENGTH){
		HcuErrorPrint("HCU-VM: Too large message length than HCU set capability, param_len=%d!!!\n", param_len);
		return FAILURE;
	}
	if ((msg_id < MSG_ID_COM_MIN) || (msg_id > MSG_ID_COM_MAX)){
		HcuErrorPrint("HCU-VM: Error on Msg_Id, msg_id=%d!!!\n", msg_id);
		return FAILURE;
	}

	/*
	** Search for the dest_id task state
	*/

	if( zHcuVmCtrTab.fsm.pFsmCtrlTable[dest_id].taskId != dest_id )
	{
		HcuErrorPrint("HCU-VM: The destination process does not exist.\n");
		return FAILURE;
	}

	/*
	** Get current FSM state
	*/
	state = FsmGetState(dest_id);
	mid = msg_id & MASK_MSGID_NUM_IN_ONE_TASK;

	//check the state and messageId of task
	if((state >= MAX_STATE_NUM_IN_ONE_TASK)||(mid >= MAX_MSGID_NUM_IN_ONE_TASK))
	{
		HcuErrorPrint("HCU-VM: The state(%d) or msgId(0x%x) of task(0x%x) is error\n", 	state, mid, dest_id);
		return FAILURE;
	}

	//未来可以提升到IPT层面
	HCU_DEBUG_PRINT_NOR("HCU-VM: Call state function(0x%x) in state(%d) of task(0x%x)[%s] for msg(0x%x)[%s], and from task(0x%x)[%s]\n",
				zHcuVmCtrTab.fsm.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc, state, dest_id, zHcuVmCtrTab.task[dest_id].taskName, mid, zHcuSysEngTrcMsgCtrStaticCfg[mid].msgName, src_id, zHcuVmCtrTab.task[src_id].taskName);

	/*
	** Call the state function.
	*/
	if(zHcuVmCtrTab.fsm.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc != NULL)
	{
		ret = (zHcuVmCtrTab.fsm.pFsmCtrlTable[dest_id].pFsmArray[state][mid].stateFunc)
			(dest_id, src_id, param_ptr, param_len);
		if( FAILURE == ret)
		{
			HcuErrorPrint("HCU-VM: Internal error is found in the state function.\n");
			return FAILURE;
		}
	}
	else
	{
		if(NULL != param_ptr)
		{
			//Free memory, here do nothing.
		}
		HcuErrorPrint("HCU-VM: Receive invalid msg(%x)[%s] in state(%d) of task(0x%x)[%s]\n",
			mid, zHcuSysEngTrcMsgCtrStaticCfg[mid].msgName, state, dest_id, zHcuVmCtrTab.task[dest_id].taskName);
		return FAILURE;
	}

    return SUCCESS;
}

OPSTAT FsmSetState(UINT32 task_id, UINT8 newState)
{
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: FsmSetState error on task_id, task_id=%d!!!\n", task_id);
		return FAILURE;
	}

	//Checking newState range
	if (newState > MAX_STATE_NUM_IN_ONE_TASK){
		HcuErrorPrint("HCU-VM: FsmSetState error on state, State=%d!!!\n", newState);
		return FAILURE;
	}

	zHcuVmCtrTab.task[task_id].state = newState;
	return SUCCESS;
}

UINT8  FsmGetState(UINT32 task_id)
{
	/*
	** Check the task_id
	*/
	//Checking task_id range
	if ((task_id < TASK_ID_MIN) || (task_id > TASK_ID_MAX)){
		HcuErrorPrint("HCU-VM: Error on task_id, task_id=%d!!!\n", task_id);
		return FSM_STATE_INVALID;
	}
	/*
	** Check the state info
	*/
	if (zHcuVmCtrTab.task[task_id].state <= MAX_STATE_NUM_IN_ONE_TASK){
		return zHcuVmCtrTab.task[task_id].state;
	}else{
		return FSM_STATE_INVALID;
	}
}

OPSTAT fsm_com_do_nothing(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_com_heart_beat_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//接收参数检查
	msg_struct_com_heart_beat_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_heart_beat_t));
	if ((param_ptr == NULL) || (param_len > sizeof(msg_struct_com_heart_beat_t))){
		HcuErrorPrint("%s: Receive heart beat message error!\n", zHcuVmCtrTab.task[dest_id].taskName);
		return FAILURE;
	}
	//优化的结果，可以不用拷贝，暂时没有意义
	//memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[dest_id]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[dest_id] > HCU_RUN_ERROR_LEVEL_4_DEAD){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[dest_id] = zHcuSysStaPm.taskRunErrCnt[dest_id] - HCU_RUN_ERROR_LEVEL_4_DEAD;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, dest_id, dest_id, &snd0, snd0.length);
		if (ret == FAILURE)
			HCU_ERROR_PRINT_TASK(dest_id, "%s: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[dest_id].taskName, zHcuVmCtrTab.task[dest_id].taskName, zHcuVmCtrTab.task[dest_id].taskName);
	}

	//发送消息
	if ((src_id > TASK_ID_MIN) && (src_id < TASK_ID_MAX) && (dest_id > TASK_ID_MIN) && (dest_id < TASK_ID_MAX) ){
		//发送心跳回复消息给src_id
		msg_struct_com_heart_beat_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_heart_beat_fb_t));
		snd.length = sizeof(msg_struct_com_heart_beat_fb_t);
		ret = hcu_message_send(MSG_ID_COM_HEART_BEAT_FB, src_id, dest_id, &snd, snd.length);
		if (ret == FAILURE)
			HCU_ERROR_PRINT_TASK(dest_id, "%s: Send message error, TASK[%s] to TASK[%s]!\n", zHcuVmCtrTab.task[dest_id].taskName, zHcuVmCtrTab.task[dest_id].taskName, zHcuVmCtrTab.task[src_id].taskName);
	}
	//也可能是调用关系，故而直接采用SRC_ID=0的方式，这种情况原则上也允许

	return SUCCESS;
}


/******************函数CRC_16()*********小端
返回short int
参数data【】数组、len 数组长度
例子：datax【8】={01, 03, 00, 00, 00, 01, 00, 00}16 进制
unsigned short int x=CRC_16(datax,6);
得到x=0x840a;
***************************************************/
UINT16 hcu_CRC_16(unsigned char *data,int len)
{
	unsigned char *buf;
	unsigned short int * CRC;
	unsigned short int crch,crcl;
	unsigned char p;
	unsigned char j;
	char err;
	buf= & data[len];
	CRC=(unsigned short int *)buf;
	buf[0]=0xff;//lsb
	buf[1]=0xff;//msb
	for(p=0;p<len;p++)
	{
		buf[1]=buf[1]^data[p];
		for(j=0;j<8;j++)
		{
			err=buf[1]&1;
			*CRC=*CRC/2;
			if(err) *CRC=*CRC^0xa001;
		}
	}
	crch=*CRC>>8;
	crcl=*CRC<<8;
	*CRC=crch+crcl;
	return(*CRC);
}

//单进程模式，当前的工作模式！！！
void hcu_vm_working_mode_single_process_start(void)
{
	zHcuVmCtrTab.process.curProcId = getpid();
	strcpy(zHcuVmCtrTab.process.curProcName, "PS_MAINAPP");
	hcu_vm_process_single_mainapp_entry();
}

//双进程模式
void hcu_vm_working_mode_double_process_start(void)
{
	int pid = 0;

	pid = fork();
	if (pid < 0){
		HcuErrorPrint("HCU: Create process error!\n");
		return;
	} else if(pid == 0){
		zHcuVmCtrTab.process.curProcId = getpid();
		strcpy(zHcuVmCtrTab.process.curProcName, "PS_MAINAPP");
		hcu_vm_process_double_mainapp_entry();
	} else{
		zHcuVmCtrTab.process.curProcId = getpid();
		strcpy(zHcuVmCtrTab.process.curProcName, "PS_AVORION");
		hcu_vm_process_create_sensor_avorion_only();
	}
}

//多进程模式
void hcu_vm_working_mode_multipy_process_start(void)
{
	int pid = 0;

	pid = fork();
	if (pid < 0){
		HcuErrorPrint("HCU: Create process error!\n");
		return;
	} else if(pid == 0){
		zHcuVmCtrTab.process.curProcId = getpid();
		strcpy(zHcuVmCtrTab.process.curProcName, "PS_SUPVOR");
		hcu_vm_process_multipy_entry_supervisor();
	} else{
	    pid = fork();
		if (pid < 0){
			HcuErrorPrint("HCU: Create process error!\n");
			return;
		} else if(pid == 0){
			zHcuVmCtrTab.process.curProcId = getpid();
			strcpy(zHcuVmCtrTab.process.curProcName, "PS_MAINAPP");
			hcu_vm_process_multipy_mainapp_entry();
		} else{
			zHcuVmCtrTab.process.curProcId = getpid();
			strcpy(zHcuVmCtrTab.process.curProcName, "PS_AVORION");
			hcu_vm_process_create_sensor_avorion_only();
		}
	}
}

//单进程模式入口程序组，当前的工作模式！！！
void hcu_vm_process_single_mainapp_entry(void)
{
	//创建队列环境
	hcu_vm_task_create_hcumain_env();
	//创建所有任务
    hcu_vm_task_create_all();
    //先启动TIMER和SVRCON任务，然后让SVRCON任务发送初始化消息给其它所有任务
	hcu_vm_task_send_init_to_timer();
	hcu_vm_task_send_init_to_svrcon();

	//进入循环状态，发送心跳消息
	while (1){
		//可以设置为5秒的定时，甚至更长
		hcu_sleep(60);
		hcu_vm_task_send_hb_to_svrcon();
	}

	//如果出现问题，将退出并删除所有的任务和队列
	hcu_vm_task_delete_all_and_queue();

	return;
}

//双进程模式入口程序组
void hcu_vm_process_double_mainapp_entry(void)
{
	//创建队列环境
	hcu_vm_task_create_hcumain_env();

	//创建所有任务
    hcu_vm_task_create_all_but_avorion();
	hcu_sleep(2);

	//先启动TIMER和SVRCON任务，然后让SVRCON任务发送初始化消息给其它所有任务
	hcu_vm_task_send_init_to_timer();
	hcu_vm_task_send_init_to_svrcon();

	//进入循环状态，发送心跳消息
	while (1){
		hcu_sleep(160);
		hcu_vm_task_send_hb_to_svrcon();
	}

	//如果出现问题，将退出并删除所有的任务和队列
	hcu_vm_task_delete_all_and_queue();

	return;
}

void hcu_vm_process_create_sensor_avorion_only(void)
{
	//Create task Avorion environments/24
	hcu_vm_system_task_init_call(TASK_ID_AVORION, HcuFsmAvorion);
	HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
			zHcuVmCtrTab.process.curProcId, zHcuVmCtrTab.process.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);

	//进入循环状态，发送心跳消息
	while (1){
		hcu_sleep(1); //1S减少信号中断的概率
		//hcu_usleep(100);
		//HcuDebugPrint("HCUMAIN: PS_23_AVORION test!\n");
	}

	//如果出现问题，将退出并删除所有的任务和队列
	hcu_vm_task_delete_all_and_queue();

	return;
}

//多进程模式入口程序组
//由于时钟函数依然跟其它任务之间采用共享内存变量的方式，所以这种模式并不能正常工作，留待未来继续改进TIMER任务后才能正常调试和运行
void hcu_vm_process_multipy_entry_supervisor(void)
{
	int ret = 0;

	//创建队列环境
	hcu_vm_task_create_hcumain_env();

	//先启动TIMER和SVRCON任务，然后让SVRCON任务发送初始化消息给其它所有任务
    hcu_sleep(2);
	hcu_vm_task_send_init_to_timer();
	hcu_vm_task_send_init_to_svrcon();

	//进入循环状态，发送心跳消息
	while (1){
		hcu_sleep(160);
		hcu_vm_task_send_hb_to_svrcon();

		//Wait for SVRCON feedback
		HcuMsgSruct_t rcv;
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv_no_wait(TASK_ID_HCUMAIN, &rcv);
		if (ret > 0){
			if ((rcv.msgType == MSG_ID_COM_PROCESS_REBOOT) && (rcv.src_id == TASK_ID_SVRCON)){
				HcuDebugPrint("HCU-MAIN: Reboot process start, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
						zHcuVmCtrTab.process.curProcId, zHcuVmCtrTab.process.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);
				int pid = 0;
				pid = fork();
				if (pid > 0){
					zHcuVmCtrTab.process.curProcId = getpid();
					strcpy(zHcuVmCtrTab.process.curProcName, "PS_AVORION");
					hcu_vm_process_create_sensor_avorion_only();
				}else if (pid == 0){
					hcu_sleep(2);
					//hcu_msgque_resync(); //重新同步AVORION任务的QUEID
					//hcu_sleep(2);
					hcu_vm_task_send_init_to_avorion(); //初始化该任务，不然它不启动开始工作
				}else{
					HcuErrorPrint("HCUMAIN: Error create AVORION process!\n");
				}
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
			}
		}

	}

	//如果出现问题，将退出并删除所有的任务和队列
	hcu_vm_task_delete_all_and_queue();

	return;
}

void hcu_vm_process_multipy_mainapp_entry(void)
{
	//创建队列环境
    hcu_vm_task_create_all_but_avorion();
	HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
			zHcuVmCtrTab.process.curProcId, zHcuVmCtrTab.process.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);

	//进入循环状态，发送心跳消息
	while (1){
		hcu_sleep(1); //1S减少信号中断的概率
		//hcu_usleep(100);
		//HcuDebugPrint("HCUMAIN: PS_3_MAINAPP test!\n");
	}

	//如果出现问题，将退出并删除所有的任务和队列
	hcu_vm_task_delete_all_and_queue();

	return;
}

//当前的工作模式！！！
//不同项目的特定任务启动，只改造了这个一个函数，其它的函数暂时未能启用
//目前系统启动暂时就是使用了这种方式，并没有采用其他多进程的方式
void hcu_vm_task_create_all(void)
{
	HcuFsmStateItem_t *p;
	int task_id = 0;

	for (task_id = TASK_ID_MIN + 1; task_id < TASK_ID_MAX; task_id++){
		p = (HcuFsmStateItem_t *)zHcuVmCtrTab.task[task_id].taskFuncEntry;
		if ((p != NULL) && (zHcuVmCtrTab.task[task_id].pnpState == HCU_TASK_PNP_ON)){
			hcu_vm_system_task_init_call(task_id, p);
		}
	}
}

void hcu_vm_task_create_all_but_avorion(void)
{
	HcuFsmStateItem_t *p;
	int task_id = 0;

	for (task_id = TASK_ID_MIN + 1; task_id < TASK_ID_MAX; task_id++){
		p = (HcuFsmStateItem_t *)zHcuVmCtrTab.task[task_id].taskFuncEntry;
		if ((p != NULL) && (zHcuVmCtrTab.task[task_id].pnpState == HCU_TASK_PNP_ON) && (task_id != TASK_ID_AVORION)){
			hcu_vm_system_task_init_call(task_id, p);
		}
	}


}

OPSTAT hcu_vm_system_task_init_call(UINT32 task_id, HcuFsmStateItem_t *p)
{
	int ret = 0;
	if (zHcuVmCtrTab.task[task_id].pnpState != HCU_TASK_PNP_ON){
		HcuErrorPrint("HCU-MAIN: no need create this task [%s]!\n", zHcuVmCtrTab.task[task_id].taskName);
		return FAILURE;
	}
	HcuDebugPrint("HCU-MAIN: Staring to create task [%s] related environments...\n", zHcuVmCtrTab.task[task_id].taskName);
	ret = hcu_task_create_and_run(task_id, p);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: create task env [%s] un-successfully, program exit.\n", zHcuVmCtrTab.task[task_id].taskName);
		return FAILURE;
	}

	//将该任务执行时对应的状态转移机指针存入，以供以后扫描时使用
	zHcuVmCtrTab.task[task_id].fsmPtr = p;
	return SUCCESS;
}

//创建队列环境
void hcu_vm_task_create_hcumain_env(void)
{
	int ret = 0, taskId = 0;

	//Create all child task and relevant massage queue
	//一个不成功，就应该返回，如果不提前返回，纯粹是为了测试
	//Create HCU-Main Queid /1  该队列创建，纯粹是为了测试，以后需要删掉
	taskId = TASK_ID_HCUMAIN;
	HcuDebugPrint("HCU-MAIN: Staring to create task [%s] related environments...\n", zHcuVmCtrTab.task[taskId].taskName);
   	ret = hcu_msgque_create(taskId);
    if (ret == FAILURE){
    	HcuErrorPrint("HCU-MAIN: create queue [%s] un-successfully, program exit.\n", zHcuVmCtrTab.task[taskId].taskName);
    	return;
    }else{
    	HcuDebugPrint("HCU-MAIN: create queue Task Name[%s] successfully.\n", zHcuVmCtrTab.task[taskId].taskName);
    }
    return;
}

void hcu_vm_task_send_init_to_timer(void)
{
	int ret = 0;

    //先初始化TIMER任务，不然后面的其它任务没法正常工作
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_TIMER, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HCUMAIN].taskName, zHcuVmCtrTab.task[TASK_ID_TIMER].taskName);
	}

	//Wait for TIMER task feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_TIMER)){
				HcuDebugPrint("HCU-MAIN: TIMER task init completed. \n");
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}

}

void hcu_vm_task_send_init_to_avorion(void)
{
	int ret = 0;

    //先初始化TIMER任务，不然后面的其它任务没法正常工作
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_AVORION, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HCUMAIN].taskName, zHcuVmCtrTab.task[TASK_ID_AVORION].taskName);
	}

	//Wait for AVORION task feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_AVORION)){
				HcuDebugPrint("HCU-MAIN: AVORION task init completed. \n");
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}

}

void hcu_vm_task_send_init_to_svrcon(void)
{
	int ret = 0;

	//发送初始化消息给主控任务SVRCON，以便初始化所有任务模块
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_SVRCON, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HCUMAIN].taskName, zHcuVmCtrTab.task[TASK_ID_SVRCON].taskName);
	}

	//Wait for SVRCON feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			hcu_sleep(1);
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_SVRCON)){
				HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
						zHcuVmCtrTab.process.curProcId, zHcuVmCtrTab.process.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}
}

//当做定时时钟使用，不依赖于任何时钟任务，比较安全可靠，废物利用
void hcu_vm_task_send_hb_to_svrcon(void)
{
	int ret = 0;

	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = hcu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_SVRCON, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HCUMAIN].taskName, zHcuVmCtrTab.task[TASK_ID_SVRCON].taskName);
	}
	return;
}

void hcu_vm_task_delete_all_and_queue(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	HcuDebugPrint("HCU-MAIN: Clean all running task and message queue for next time debug!\n");
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if (task_id != TASK_ID_HCUMAIN){hcu_task_delete(task_id);}
		hcu_msgque_delete(task_id);
	}
	return;
}

void hcu_vm_task_delete_except_avorion_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_AVORION)){hcu_task_delete(task_id);}
	}
	return;
}

void hcu_vm_task_delete_except_svrcon_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_SVRCON)){hcu_task_delete(task_id);}
	}
	return;
}

void hcu_vm_task_delete_except_timer_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_TIMER)){hcu_task_delete(task_id);}
	}
	return;
}

int hcu_vm_main_entry(void)
{
	//系统状态初始化
	if (hcu_vm_system_ctr_table_init() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Init system level environment error!\n");
		return EXIT_SUCCESS;
	}

	//智能初始化：将MODULE TRACE初始化表单存入数据库，降低研发工作复杂度
	if ((HCU_HARDWARE_MASSIVE_PRODUTION_SET == HCU_HARDWARE_MASSIVE_PRODUTION_NO) && (HCU_TRACE_DB_SET_INIT_BY_VM_STATIC_TABLE_MOD_SET == HCU_TRACE_DB_SET_INIT_BY_VM_STATIC_TABLE_YES)){
		if (dbi_HcuTraceModuleCtr_engpar_intelligence_init() == FAILURE){
			HcuDebugPrint("HCU-MAIN: Init Module Trace set error!\n");
			return EXIT_SUCCESS;
		}
	}

	//智能初始化：将MSG TRACE初始化表单存入数据库，降低研发工作复杂度
	if ((HCU_HARDWARE_MASSIVE_PRODUTION_SET == HCU_HARDWARE_MASSIVE_PRODUTION_NO) && (HCU_TRACE_DB_SET_INIT_BY_VM_STATIC_TABLE_MSG_SET == HCU_TRACE_DB_SET_INIT_BY_VM_STATIC_TABLE_YES)){
		if (dbi_HcuTraceMsgCtr_engpar_intelligence_init() == FAILURE){
			HcuDebugPrint("HCU-MAIN: Init Message Trace set error!\n");
			return EXIT_SUCCESS;
		}
	}

	//从数据库或者系统缺省配置中，读取系统配置的工程参数
	if (hcu_hwinv_engpar_read_pop_data_into_mem() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Read database or system init parameters into memory error!\n");
		return EXIT_SUCCESS;
	}

	//智能初始化：FuncHandler方式对物理配置信息进行分析
	//系统硬件标识区初始化：必须先读取工参，然后再使用物理地址，因为物理信息很可能覆盖工参，所以必须放在后面处理，物理地址具备更高的优先级
	if (hcu_vm_engpar_get_phy_burn_block_data() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Init system hardware physical burn ID block error!\n");
		return EXIT_SUCCESS;
	}

	//创建目录存储环境：这个任务需要移到下载任务模块的初始化之中
	if (hcu_hwinv_engpar_create_storage_dir_env() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Create storage directory environment error!\n");
		return EXIT_SUCCESS;
	}

	//读取宿主机eth0 Mac地址
	if (hcu_hwinv_engpar_read_mac_address() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Read MAC address error!\n");
		return EXIT_SUCCESS;
	}

	//智能初始化：任务模块启动初始化，既可以通过SYSCONFIG.H配置模块是否启动，而且还可以通过VMLAYER中的初始化表进行
	//启动的逻辑是：VMLAYER中的表单必须首先初始化，然后再看SYSCONFIG中的开关是否打开。如果没有开关，则缺省启动
	if (hcu_vm_application_task_env_init() == FAILURE){
		HcuDebugPrint("HCU-MAIN: Prepare to init system level task environments error!\n");
		return EXIT_SUCCESS;
	}

	//Now Starting application task
	HcuDebugPrint("HCU-MAIN: Application level task starting...\n");

	//单进程方式，当前的工作模式！！！
	if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_SINGLE){
		hcu_vm_working_mode_single_process_start();
	}
	//双进程方式
	else if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_DOUBLE){
		hcu_vm_working_mode_double_process_start();
	}
	//多进程方式
	else if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_TRIPPLE){
		hcu_vm_working_mode_multipy_process_start();
	}
	//差错启动模式
	else{
		HcuErrorPrint("HCU: Can not work due to starting mode error!\n");
	}

	return EXIT_SUCCESS;
}

//需要单独设计一种的存储设备敏感数据的方式
OPSTAT hcu_vm_engpar_get_phy_burn_block_data(void)
{
	int ret = 0;
	//硬件烧录区域，系统唯一标识部分，后面程序中访问到这些系统参数都必须从这个地方读取
	//具体读取的过程，目前暂时空白，因为还未决定使用哪种方式来存储这个敏感信息
	ret = hcu_vm_engpar_read_phy_boot_cfg();
	if ((HCU_HARDWARE_MASSIVE_PRODUTION_SET == HCU_HARDWARE_MASSIVE_PRODUTION_YES) && (ret == FAILURE)){
		HcuErrorPrint("HCU-VM: Massive production phase but not yet pop physical boot configuration!\n");
		return FAILURE;
	}

	//对硬件类型进行相同性检查，如果不一致，必然发生了生产性错误，或者硬件搞错，或者Factory Load用错，应该严重警告
	if ((HCU_HARDWARE_MASSIVE_PRODUTION_SET == HCU_HARDWARE_MASSIVE_PRODUTION_YES) && (zHcuSysEngPar.hwBurnId.hwType != HCU_HARDWARE_PRODUCT_CAT_TYPE)){
		HcuErrorPrint("HCU-VM: Fatal error, using wrong hardware type or factory load!!!\n");
		return FAILURE;
	}
	//由于硬件部分并没有真正起作用，所以暂时需要从系统定义区重复写入，一旦批量生产这部分可以去掉
	if (HCU_HARDWARE_MASSIVE_PRODUTION_SET == HCU_HARDWARE_MASSIVE_PRODUTION_NO){
		strncpy(zHcuSysEngPar.hwBurnId.equLable, HCU_CLOUDVELA_BH_HCU_NAME, (sizeof(HCU_CLOUDVELA_BH_HCU_NAME)<sizeof(zHcuSysEngPar.hwBurnId.equLable))?(sizeof(HCU_CLOUDVELA_BH_HCU_NAME)):(sizeof(zHcuSysEngPar.hwBurnId.equLable)));
		zHcuSysEngPar.hwBurnId.hwType  = HCU_HARDWARE_PRODUCT_CAT_TYPE;
		zHcuSysEngPar.hwBurnId.hwPemId = HCU_CURRENT_HW_TYPE; //PEM小型号
		zHcuSysEngPar.hwBurnId.swRelId = HCU_CURRENT_SW_RELEASE;
		zHcuSysEngPar.hwBurnId.swVerId = HCU_CURRENT_SW_DELIVERY;
		zHcuSysEngPar.hwBurnId.swUpgradeFlag = HCU_HARDWARE_BURN_ID_FW_UPGRADE_SET;
		zHcuSysEngPar.hwBurnId.swUpgPollId = HCU_HARDWARE_BURN_ID_FW_UPGRADE_METHOD_UART_GPRS;
		//cipherKey[16];
	}

	//物理地址配置具备更高的优先级
	if (strlen(zHcuSysEngPar.hwBurnId.equLable) != 0){
		strcpy(zHcuSysEngPar.cloud.cloudBhHcuName, zHcuSysEngPar.hwBurnId.equLable);
	}

	//初始化之后的系统标识信息
	HcuDebugPrint("HCU-VM: Initialized Hardware Burn Physical Id/Address: CURRENT_PRJ=[%s], HW_LABLE=[%s], PRODUCT_CAT=[0x%x], HW_TYPE=[0x%x], SW_RELEASE_VER=[%d.%d], FW_UPGRADE_FLAG=[%d].\n", \
			HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE, \
		zHcuSysEngPar.hwBurnId.equLable, \
		zHcuSysEngPar.hwBurnId.hwType, \
		zHcuSysEngPar.hwBurnId.hwPemId, \
		zHcuSysEngPar.hwBurnId.swRelId, \
		zHcuSysEngPar.hwBurnId.swVerId, \
		zHcuSysEngPar.hwBurnId.swUpgradeFlag);

	return SUCCESS;
}


OPSTAT hcu_vm_engpar_read_phy_boot_cfg(void)
{
	//int fHandler = 0;
	FILE *fp;
	int bytes_read = 0, len = 0, index =0;
	int file_len=0;
	char *pRecord;

	//打开源文件
	if((fp=fopen(HCU_HARDWARE_PHY_BOOT_CFG_FILE, "rt+"))== NULL){
		HcuErrorPrint("HCU-VM: Open %s Error!\n", HCU_HARDWARE_PHY_BOOT_CFG_FILE);
		return FAILURE;
	}

	//测得文件大小
	//curpos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (file_len <=0){
		HcuErrorPrint("HCU-VM: Not file content!\n");
		fclose(fp);
		return FAILURE;
	}

	//读取文件
	pRecord = malloc(file_len);
	if (pRecord==NULL){
		HcuErrorPrint("HCU-VM: Allocate memory fail!\n");
		return FAILURE;
	}
	bytes_read=fread(pRecord, 1, file_len, fp);
	if (bytes_read != file_len){
		HcuErrorPrint("HCU-VM: Read file fail, byte_read = %d, file_len = %d!\n", bytes_read, file_len);
		free(pRecord);
		fclose(fp);
		return FAILURE;
	}

	char *p1, *p2;
	//按照zHcuXmlBootPhyCfgHead处理
	index = 0;
	p1 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].left);
	p2 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].right);
	if ((p1==NULL) || (p2==NULL) || (p1>=p2)){
		  HcuErrorPrint("HCU-VM: Read file fail!\n");
		  free(pRecord);
		  fclose(fp);
		  return FAILURE;
	}

	//equLable
	index++;
	p1 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].left);
	p2 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].right);
	len = p2 - p1 - strlen(zHcuSysEngPhyBootCfg[index].left);
	if ((p1!=NULL) && (p2!=NULL) &&  (len>0)){
		p1 = p1+strlen(zHcuSysEngPhyBootCfg[index].left);
		strncpy(zHcuSysEngPar.hwBurnId.equLable, p1, len<sizeof(zHcuSysEngPar.hwBurnId.equLable)?len:sizeof(zHcuSysEngPar.hwBurnId.equLable));
	}
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.hwType));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.hwPemId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.swRelId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.swVerId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.swUpgradeFlag));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.swUpgPollId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootIndex));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootAreaMax));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.facLoadAddr));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.facLoadSwRel));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.facLoadSwVer));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.facLoadCheckSum));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.facLoadValid));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.spare2));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad1Addr));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad1RelId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad1VerId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad1CheckSum));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad1Valid));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.spare3));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad2Addr));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad2RelId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad2VerId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad2CheckSum));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad2Valid));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.spare4));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad3Addr));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad3RelId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad3VerId));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad3CheckSum));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.bootLoad3Valid));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.spare5));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.cipherKey[0]));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.cipherKey[8]));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.rsv[0]));
	hcu_vm_engpar_translate_phy_boot_cfg_into_mem(pRecord, ++index, (UINT8*)&(zHcuSysEngPar.hwBurnId.rsv[8]));

	//关闭文件
	free(pRecord);
	fclose(fp);

	return SUCCESS;
}


void hcu_vm_engpar_translate_phy_boot_cfg_into_mem(char *pRecord, int index, UINT8 *target)
{
	char *p1, *p2;
	int len =0;
	char s[10];
	UINT64 res = 0;
	UINT8 res1 = 0;
	UINT16 res2 = 0;
	UINT32 res3 = 0;

	p1 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].left);
	p2 = strstr(pRecord, zHcuSysEngPhyBootCfg[index].right);
	len = p2 - p1 - strlen(zHcuSysEngPhyBootCfg[index].left);
	if ((p1!=NULL) && (p2!=NULL) && (len>0)){
		p1 = p1+strlen(zHcuSysEngPhyBootCfg[index].left);
		memset(s, 0, sizeof(s));
		strncpy(s, p1, len<sizeof(s)?len:sizeof(s));
		res = strtoul(s, NULL, 10);
		if (zHcuSysEngPhyBootCfg[index].level==1){
			res1 = res & 0xFF;
			memcpy(target, &res1, 1);
		}
		else if (zHcuSysEngPhyBootCfg[index].level==2){
			res2 = res & 0xFFFF;
			memcpy(target, &res2, 2);
		}
		else if (zHcuSysEngPhyBootCfg[index].level==4){
			res3 = res & 0xFFFFFFFF;
			memcpy(target, &res3, 4);
		}
		else if (zHcuSysEngPhyBootCfg[index].level==8){
			memcpy(target, &res, 8);
		}
	}
}

