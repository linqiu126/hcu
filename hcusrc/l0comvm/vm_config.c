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
 *   全局变量，存储所有任务的状态信息，以便后面使用
 *
 */

/************************************************************************************************************************
 *
 *  全局变量命名规则
 *
 *  - 全局变量命名采用微软法，以大小写混用的方式，特别以zHcuXxxYyy的形式进行命名
 *  - 全局变量的长度不得少于10个字符串长度
 *  - 全局变量的第二段一般属于(TASKNAME)，以方面判定该变量属于哪个模块，也方面未来的维护，并防止重名
 *  - 全局变量中的结构，统一采用xxx_t结束
 *  - 全局变量中的消息和消息单元，统一采用msg_struct_xxx, msgie_struct_xxx的形式，也可以采用strAaaBbb的形式
 *  - 公共业务模块也可以自定义特定字头，比如HUITP消息采用HUITP_xxx作为相应常量／变量和结构的统一命名形式
 *
 *************************************************************************************************************************/

HcuVmCtrTab_t 		zHcuVmCtrTab;  		//全局系统总控表
HcuSysEngParTab_t 	zHcuSysEngPar; 		//全局工程参数控制表
HcuSysStaPm_t		zHcuSysStaPm;		//全局性能统计表

/*
 *
 *   任务初始化配置参数
 *
 */

/************************************************************************************************************************
 *
 *  模块级共享全局变量使用规则
 *
 *  - 尽量将一个模块的所有动态参数放在一个结构中
 *  - 该结构一般命名为gTaskXxxContext
 *  - 统一在一个结构中的好处是，统一的初始化，方便管理，方便模块移植，方便模块RESTART
 *  - 如果其它模块需要共享该模块的参数，需要在该模块的.H中extern该变量，以输出给其它共享模块
 *  - 为了减少相互共享变量导致的编译包含关系和依赖关系混乱，不需要共享的变量，不得输出
 *
 *************************************************************************************************************************/

//请确保，该全局字符串的定义跟Task_Id的顺序保持完全一致，不然后面的显示内容会出现差错， 请服从最长长度HCU_SYSDIM_TASK_NAME_LEN_MAX的定义，不然Debug/Trace打印出的信息也会出错
//从极致优化内存的角度，这里浪费了2个TASK对应的内存空间（MIN=0/MAX=n+1)，但它却极大的改善了程序编写的效率，值得浪费！！！
//NULL条目保留，是为了初始化TASK NAME这一属性
//设置的技巧是：基础部分不要动，配置不一样的，请单独在分项目中，再设置一遍
HcuVmCtrTaskStaticCfg_t zHcuVmCtrTaskStaticCfg[] =
{
	//TASK_ID,              状态控制             状态机入口                 控制启动              		TRACE标志位       注释
	//START FLAG: 所有项目任务初始化清单：不要动，需要将任务全部初始化好，并且设置入口为NULL。在分项目中再配置是否真启动
	{TASK_ID_MIN,           "TASKMIN",          NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1}, //Starting
	{TASK_ID_HCUMAIN,       "HCUMAIN",          NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HCUVM,         "HCUVM",            NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_TRACE,         "TRACE",            NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_CONFIG,        "CONFIG",           NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_TIMER,         "TIMER",            &HcuFsmTimer,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MMC,           "MMC",              &HcuFsmMmc,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_GPIO,          "GPIO",             &HcuFsmGpio,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_I2C,           "I2C",              &HcuFsmI2c,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SPI,           "SPI",              &HcuFsmSpi,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_PWM,           "PWM",              &HcuFsmPwm,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_ADC,           "ADC",              &HcuFsmAdc,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SWITCH,        "SWITCH",           &HcuFsmSwitch,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_RELAY,         "RELAY",            &HcuFsmRelay,            HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_MOTOR,         "MOTOR",            &HcuFsmMotor,            HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_ZEEGBE,        "ZEEGBE",           &HcuFsmZeegbe,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_GPRS,          "GPRS",             NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SPS232,        "SPS232",           &HcuFsmSps232,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SPS485,        "SPS485",           &HcuFsmSps485,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_BLE,           "BLE",              &HcuFsmBle,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_WIFI,          "WIFI",             &HcuFsmWifi,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_USBNET,        "USBNET",           &HcuFsmUsbnet,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_3G4G,          "3G4G",             &HcuFsm3g4g,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HARDDISK,      "HARDDISK",         NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_CAMERA,        "CAMERA",           &HcuFsmCamera,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_MICROPHONE,    "MICROPHONE",       &HcuFsmMicrophone,       HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_FLASH,         "FLASH",            NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_GPS,           "GPS",              &HcuFsmGps,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_LCD,           "LCD",              &HcuFsmLcd,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_LED,           "LED",              &HcuFsmLed,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HWINV,         "HWINV",            &HcuFsmHwinv,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPSVIRGO,      "SPSVIRGO",         &HcuFsmSpsvirgo,         HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_MODBUS,        "MODBUS",           &HcuFsmModbus,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_AVORION,       "AVORION",          &HcuFsmAvorion,          HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_I2CBUSLIBRA,   "I2CBUSLIBRA",      &HcuFsmI2cbuslibra,      HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SPIBUSARIES,   "SPIBUSARIES",      &HcuFsmSpibusaries,      HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_NBIOTCJ188,    "NBIOTCJ188",       &HcuFsmNbiotcj188,       HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_NBIOTQG376,    "NBIOTQG376",       &HcuFsmNbiotqg376,       HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_LLCZHB,        "LLCZHB",           &HcuFsmLlczhb,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HSMMP,         "HSMMP",            &HcuFsmHsmmp,            HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_EMC,           "EMC",              &HcuFsmEmc,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HUMID,         "HUMID",            &HcuFsmHumid,            HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_PM25,          "PM25",             &HcuFsmPm25,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_TEMP,          "TEMP",             &HcuFsmTemp,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_WINDDIR,       "WINDDIR",          &HcuFsmWinddir,          HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_WINDSPD,       "WINDSPD",          &HcuFsmWindspd,          HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_NOISE,         "NOISE",            &HcuFsmNoise,            HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_AIRPRS,        "AIRPRS",           &HcuFsmAirprs,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_CO1,           "CO1",              &HcuFsmCo1,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_LIGHTSTR,      "LIGHTSTR",         &HcuFsmLightstr,         HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_ALCOHOL,       "ALCOHOL",          &HcuFsmAlcohol,          HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HCHO,          "HCHO",             &HcuFsmHcho,             HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_TOXICGAS,      "TOXICGAS",         &HcuFsmToxicgas,         HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_IWM,           "IWM",              &HcuFsmIwm,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_IHM,           "IHM",              &HcuFsmIhm,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_IGM,           "IGM",              &HcuFsmIgm,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_IPM,           "IPM",              &HcuFsmIpm,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_SVRCON,        "SVRCON",           &HcuFsmSvrcon,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SYSPM,         "SYSPM",            &HcuFsmSyspm,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SYSSWM,        "SYSSWM",           &HcuFsmSysswm,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PM25SHARP,     "PM25SHARP",        &HcuFsmPm25sharp,        HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        NULL,        			 HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3AQYCG10,     "L3AQYCG10",        NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3AQYCG20,     "L3AQYCG20",        NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3TBSWRG30,    "L3TBSWRG30",       NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3GQYBG40,     "L3GQYBG40",        NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3CXILC,       "L3CXILC",          NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3CXGLACM,     "L3CXGLACM",        NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3NBLPM,       "L3NBLPM",          NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3NBHPM,       "L3NBHPM",          NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3BFSC,        "L3BFSC",           NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_BFSCUICOMM,    "BFSCUICOMM",       NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_JSONINOTIFY,   "JSONINOTIFY",      NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3OPWLOTDR,    "L3OPWLOTDR",       NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3BFDF,        "L3BFDF",           NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3BFHS,        "L3BFHS",           NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_L3HATE,        "L3HATE",           NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
//END所有项目任务初始化清单
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
	{TASK_ID_GPIO,          "GPIO",             &HcuFsmGpio,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_I2C,           "I2C",              &HcuFsmI2c,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPI,           "SPI",              &HcuFsmSpi,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PWM,           "PWM",              &HcuFsmPwm,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ADC,           "ADC",              &HcuFsmAdc,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SWITCH,        "SWITCH",           &HcuFsmSwitch,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_RELAY,         "RELAY",            &HcuFsmRelay,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MOTOR,         "MOTOR",            &HcuFsmMotor,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ZEEGBE,        "ZEEGBE",           &HcuFsmZeegbe,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPS232,        "SPS232",           &HcuFsmSps232,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPS485,        "SPS485",           &HcuFsmSps485,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_BLE,           "BLE",              &HcuFsmBle,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WIFI,          "WIFI",             &HcuFsmWifi,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_USBNET,        "USBNET",           &HcuFsmUsbnet,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_3G4G,          "3G4G",             &HcuFsm3g4g,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MICROPHONE,    "MICROPHONE",       &HcuFsmMicrophone,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_GPS,           "GPS",              &HcuFsmGps,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LCD,           "LCD",              &HcuFsmLcd,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LED,           "LED",              &HcuFsmLed,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPSVIRGO,      "SPSVIRGO",         &HcuFsmSpsvirgo,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MODBUS,        "MODBUS",           &HcuFsmModbus,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_AVORION,       "AVORION",          &HcuFsmAvorion,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_I2CBUSLIBRA,   "I2CBUSLIBRA",      &HcuFsmI2cbuslibra,      HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPIBUSARIES,   "SPIBUSARIES",      &HcuFsmSpibusaries,      HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_NBIOTCJ188,    "NBIOTCJ188",       &HcuFsmNbiotcj188,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_NBIOTQG376,    "NBIOTQG376",       &HcuFsmNbiotqg376,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_HSMMP,         "HSMMP",            &HcuFsmHsmmp,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_EMC,           "EMC",              &HcuFsmEmc,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_HUMID,         "HUMID",            &HcuFsmHumid,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PM25,          "PM25",             &HcuFsmPm25,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_TEMP,          "TEMP",             &HcuFsmTemp,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WINDDIR,       "WINDDIR",          &HcuFsmWinddir,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WINDSPD,       "WINDSPD",          &HcuFsmWindspd,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_NOISE,         "NOISE",            &HcuFsmNoise,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_AIRPRS,        "AIRPRS",           &HcuFsmAirprs,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CO1,           "CO1",              &HcuFsmCo1,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_LIGHTSTR,      "LIGHTSTR",         &HcuFsmLightstr,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ALCOHOL,       "ALCOHOL",          &HcuFsmAlcohol,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_HCHO,          "HCHO",             &HcuFsmHcho,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_TOXICGAS,      "TOXICGAS",         &HcuFsmToxicgas,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_IWM,           "IWM",              &HcuFsmIwm,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_IHM,           "IHM",              &HcuFsmIhm,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_IGM,           "IGM",              &HcuFsmIgm,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_IPM,           "IPM",              &HcuFsmIpm,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PM25SHARP,     "PM25SHARP",        &HcuFsmPm25sharp,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        &HcuFsmCanitfleo,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	{TASK_ID_L3AQYCG10,     "L3AQYCG10",        &HcuFsmL3aqycg10,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPS232,        "SPS232",           &HcuFsmSps232,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPS485,        "SPS485",           &HcuFsmSps485,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WIFI,          "WIFI",             &HcuFsmWifi,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_USBNET,        "USBNET",           &HcuFsmUsbnet,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_3G4G,          "3G4G",             &HcuFsm3g4g,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_GPS,           "GPS",              &HcuFsmGps,              HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_SPSVIRGO,      "SPSVIRGO",         &HcuFsmSpsvirgo,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_MODBUS,        "MODBUS",           &HcuFsmModbus,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_AVORION,       "AVORION",          &HcuFsmAvorion,          HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_EMC,           "EMC",              &HcuFsmEmc,              HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_HUMID,         "HUMID",            &HcuFsmHumid,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_PM25,          "PM25",             &HcuFsmPm25,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_TEMP,          "TEMP",             &HcuFsmTemp,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WINDDIR,       "WINDDIR",          &HcuFsmWinddir,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_WINDSPD,       "WINDSPD",          &HcuFsmWindspd,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_NOISE,         "NOISE",            &HcuFsmNoise,            HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_AIRPRS,        "AIRPRS",           &HcuFsmAirprs,           HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},
	{TASK_ID_LLCZHB,        "LLCZHB",           &HcuFsmLlczhb,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3AQYCG20,     "L3AQYCG20",        &HcuFsmL3aqycg20,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3HATE,        "L3HATE",           &HcuFsmL3hate,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
	{TASK_ID_L3TBSWRG30,    "L3TBSWRG30",       &HcuFsmL3tbswrg30,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
	{TASK_ID_L3GQYBG40,     "L3GQYBG40",        &HcuFsmLgqgybg40,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
	{TASK_ID_L3CXILC,       "L3CXILC",          &HcuFsmL3cxilc,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
	{TASK_ID_L3CXGLACM,     "L3CXGLACM",        &HcuFsmL3cxglacm,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
	{TASK_ID_L3NBLPM,       "L3NBLPM",          &HcuFsmL3nblpm,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
	{TASK_ID_L3NBHPM,       "L3NBHPM",          &HcuFsmL3nbhpm,          HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        &HcuFsmCanitfleo,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3BFSC,        "L3BFSC",           &HcuFsmL3bfsc,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_BFSCUICOMM,    "BFSCUICOMM",       &HcuFsmBfscuicomm,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_JSONINOTIFY,   "JSONINOTIFY",      &HcuFsmJsoninotify,      HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3HATE,        "L3HATE",           &HcuFsmL3hate,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
	{TASK_ID_L3OPWLOTDR,    "L3OPWLOTDR",       &HcuFsmL3opwlotdr,       HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        &HcuFsmCanitfleo,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3BFDF,        "L3BFDF",           &HcuFsmL3bfdf,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3HATE,        "L3HATE",           &HcuFsmL3hate,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	{TASK_ID_MQTT,          "MQTT",             &HcuFsmMqtt,             HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_ETHERNET,      "ETHERNET",         &HcuFsmEthernet,         HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CLOUDVELA,     "CLOUDVELA",        &HcuFsmCloudvela,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_CANITFLEO,     "CANITFLEO",        &HcuFsmCanitfleo,        HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3BFHS,        "L3BFHS",           &HcuFsmL3bfhs,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
	{TASK_ID_L3HATE,        "L3HATE",           &HcuFsmL3hate,           HCU_SYSCFG_TASK_PNP_ON,     1, 1, 1, 1, 1},
#else
	#error Un-correct constant definition
#endif
	{TASK_ID_MAX,       "TASKMAX",              NULL,                    HCU_SYSCFG_TASK_PNP_OFF,    1, 1, 1, 1, 1},   //Ending
};

//消息ID的定义全局表，方便TRACE函数使用
//请服从HCU_SYSDIM_MSGID_NAME_LEN_MAX的最长定义，不然出错
HcuSysEngTrcMsgCtrStaticCfg_t zHcuSysEngTrcMsgCtrStaticCfg[] ={
	//MSG_ID                                      MsgName                                       TRACE_FLAG   注释
	//START FLAG
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
	//ALAMR-PM
	{MSG_ID_COM_ALARM_REPORT,                     "MSG_ID_COM_ALARM_REPORT",                    1, 1, 1},
	{MSG_ID_COM_PM_REPORT,                        "MSG_ID_COM_PM_REPORT",                       1, 1, 1},
	//MQTT
	{MSG_ID_COM_MQTT_SEND,                     	  "MSG_ID_COM_MQTT_SEND",                       1, 1, 1},
	{MSG_ID_COM_MQTT_RCV,                         "MSG_ID_COM_MQTT_RCV",                        1, 1, 1},
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
	//NBIOT message //IWM/IHM/IGM/IPM
	{MSG_ID_NBIOTCJ188_IWM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IWM_DATA_REQ",             1, 1, 1},
	{MSG_ID_IWM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IWM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IWM_CTRL_REQ,              "MSG_ID_NBIOTCJ188_IWM_CTRL_REQ",             1, 1, 1},
	{MSG_ID_IWM_NBIOTCJ188_CTRL_RESP,             "MSG_ID_IWM_NBIOTCJ188_CTRL_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IHM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IHM_DATA_REQ",             1, 1, 1},
	{MSG_ID_IHM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IHM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IHM_CTRL_REQ,              "MSG_ID_NBIOTCJ188_IHM_CTRL_REQ",             1, 1, 1},
	{MSG_ID_IHM_NBIOTCJ188_CTRL_RESP,             "MSG_ID_IHM_NBIOTCJ188_CTRL_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IGM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IGM_DATA_REQ",             1, 1, 1},
	{MSG_ID_IGM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IGM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IGM_CTRL_REQ,              "MSG_ID_NBIOTCJ188_IGM_CTRL_REQ",             1, 1, 1},
	{MSG_ID_IGM_NBIOTCJ188_CTRL_RESP,             "MSG_ID_IGM_NBIOTCJ188_CTRL_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IPM_DATA_REQ,              "MSG_ID_NBIOTCJ188_IPM_DATA_REQ",             1, 1, 1},
	{MSG_ID_IPM_NBIOTCJ188_DATA_RESP,             "MSG_ID_IPM_NBIOTCJ188_DATA_RESP",            1, 1, 1},
	{MSG_ID_NBIOTCJ188_IPM_CTRL_REQ,              "MSG_ID_NBIOTCJ188_IPM_CTRL_REQ",             1, 1, 1},
	{MSG_ID_IPM_NBIOTCJ188_CTRL_RESP,             "MSG_ID_IPM_NBIOTCJ188_CTRL_RESP",            1, 1, 1},
	{MSG_ID_NBIOTQG376_IPM_DATA_REQ,              "MSG_ID_NBIOTQG376_IPM_DATA_REQ",             1, 1, 1},
	{MSG_ID_IPM_NBIOTQG376_DATA_RESP,             "MSG_ID_IPM_NBIOTQG376_DATA_RESP",            1, 1, 1},
	{MSG_ID_NBIOTQG376_IPM_CTRL_REQ,              "MSG_ID_NBIOTQG376_IPM_CTRL_REQ",             1, 1, 1},
	{MSG_ID_IPM_NBIOTQG376_CTRL_RESP,             "MSG_ID_IPM_NBIOTQG376_CTRL_RESP",            1, 1, 1},
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
	{MSG_ID_EMC_MODBUS_CONTROL_CMD,               "MSG_ID_EMC_MODBUS_CONTROL_CMD",              1, 1, 1},
	//PM25 message
	{MSG_ID_PM25_MODBUS_DATA_READ,                "MSG_ID_PM25_MODBUS_DATA_READ",               1, 1, 1},
	{MSG_ID_PM25_MODBUS_CONTROL_CMD,              "MSG_ID_PM25_MODBUS_CONTROL_CMD",             1, 1, 1},
	//WIND_DIRECTION message
	{MSG_ID_WINDDIR_MODBUS_DATA_READ,             "MSG_ID_WINDDIR_MODBUS_DATA_READ",            1, 1, 1},
	{MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,           "MSG_ID_WINDDIR_MODBUS_CONTROL_CMD",          1, 1, 1},
	//WIND_SPEED message
	{MSG_ID_WINDSPD_MODBUS_DATA_READ,             "MSG_ID_WINDSPD_MODBUS_DATA_READ",            1, 1, 1},
	{MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,           "MSG_ID_WINDSPD_MODBUS_CONTROL_CMD",          1, 1, 1},
	//TEMPERATURE message
	{MSG_ID_TEMP_MODBUS_DATA_READ,                "MSG_ID_TEMP_MODBUS_DATA_READ",               1, 1, 1},
	{MSG_ID_TEMP_SPIBUSARIES_DATA_READ,           "MSG_ID_TEMP_SPIBUSARIES_DATA_READ",          1, 1, 1},
	{MSG_ID_TEMP_MODBUS_CONTROL_CMD,              "MSG_ID_TEMP_MODBUS_CONTROL_CMD",             1, 1, 1},
	{MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD,         "MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD",        1, 1, 1},
	//HUMIDITY message
	{MSG_ID_HUMID_MODBUS_DATA_READ,               "MSG_ID_HUMID_MODBUS_DATA_READ",              1, 1, 1},
	{MSG_ID_HUMID_MODBUS_CONTROL_CMD,             "MSG_ID_HUMID_MODBUS_CONTROL_CMD",            1, 1, 1},
	//HSMMP
	{MSG_ID_HSMMP_AVORION_DATA_READ,              "MSG_ID_HSMMP_AVORION_DATA_READ",             1, 1, 1},
	{MSG_ID_HSMMP_AVORION_STOP,                   "MSG_ID_HSMMP_AVORION_STOP",                  1, 1, 1},
	//NOISE
	{MSG_ID_NOISE_SPSVIRGO_DATA_READ,             "MSG_ID_NOISE_SPSVIRGO_DATA_READ",            1, 1, 1},
	{MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD,           "MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD",          1, 1, 1},
	{MSG_ID_NOISE_SPSVIRGO_STOP,                  "MSG_ID_NOISE_SPSVIRGO_STOP",                 1, 1, 1},
	{MSG_ID_NOISE_MODBUS_DATA_READ,               "MSG_ID_NOISE_MODBUS_DATA_READ",              1, 1, 1},
	{MSG_ID_NOISE_MODBUS_CONTROL_CMD,             "MSG_ID_NOISE_MODBUS_CONTROL_CMD",            1, 1, 1},
	//FM/PM/INVENTORY/SW-PACKAGE
	{MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ,            "MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ",           1, 1, 1},
	{MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP,           "MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP",          1, 1, 1},
	{MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT,         "MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT",        1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM,        "MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM",       1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ,            "MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ",           1, 1, 1},
	{MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP,           "MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP",          1, 1, 1},
	{MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT,         "MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT",        1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM,        "MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM",       1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,       "MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ",      1, 1, 1},
	{MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP,      "MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP",     1, 1, 1},
	{MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT,    "MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT",   1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM,   "MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM",  1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,      "MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ",     1, 1, 1},
	{MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP,     "MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP",    1, 1, 1},
	{MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT,   "MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT",  1, 1, 1},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,  "MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM", 1, 1, 1},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_REQ,  		  "MSG_ID_CLOUDVELA_TEST_COMMAND_REQ",          1, 1, 1},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_RESP,          "MSG_ID_CLOUDVELA_TEST_COMMAND_RESP",         1, 1, 1},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_REPORT,        "MSG_ID_CLOUDVELA_TEST_COMMAND_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM,       "MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM",      1, 1, 1},
	//SENSOR-CLOUDVELA
	{MSG_ID_CLOUDVELA_EMC_DATA_REQ,               "MSG_ID_CLOUDVELA_EMC_DATA_REQ",              1, 1, 1},
	{MSG_ID_EMC_CLOUDVELA_DATA_RESP,              "MSG_ID_EMC_CLOUDVELA_DATA_RESP",             1, 1, 1},
	{MSG_ID_CLOUDVELA_EMC_CTRL_REQ,               "MSG_ID_CLOUDVELA_EMC_CTRL_REQ",              1, 1, 1},
	{MSG_ID_EMC_CLOUDVELA_CTRL_RESP,              "MSG_ID_EMC_CLOUDVELA_CTRL_RESP",             1, 1, 1},
	{MSG_ID_EMC_CLOUDVELA_DATA_REPORT,            "MSG_ID_EMC_CLOUDVELA_DATA_REPORT",           1, 1, 1},
	{MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM,           "MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM",          1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25_DATA_REQ,              "MSG_ID_CLOUDVELA_PM25_DATA_REQ",             1, 1, 1},
	{MSG_ID_PM25_CLOUDVELA_DATA_RESP,             "MSG_ID_PM25_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25_CTRL_REQ,              "MSG_ID_CLOUDVELA_PM25_CTRL_REQ",             1, 1, 1},
	{MSG_ID_PM25_CLOUDVELA_CTRL_RESP,             "MSG_ID_PM25_CLOUDVELA_CTRL_RESP",            1, 1, 1},
	{MSG_ID_PM25_CLOUDVELA_DATA_REPORT,           "MSG_ID_PM25_CLOUDVELA_DATA_REPORT",          1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM,          "MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM",         1, 1, 1},
	{MSG_ID_CLOUDVELA_TEMP_DATA_REQ,              "MSG_ID_CLOUDVELA_TEMP_DATA_REQ",             1, 1, 1},
	{MSG_ID_TEMP_CLOUDVELA_DATA_RESP,             "MSG_ID_TEMP_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_CLOUDVELA_TEMP_CTRL_REQ,              "MSG_ID_CLOUDVELA_TEMP_CTRL_REQ",             1, 1, 1},
	{MSG_ID_TEMP_CLOUDVELA_CTRL_RESP,             "MSG_ID_TEMP_CLOUDVELA_CTRL_RESP",            1, 1, 1},
	{MSG_ID_TEMP_CLOUDVELA_DATA_REPORT,           "MSG_ID_TEMP_CLOUDVELA_DATA_REPORT",          1, 1, 1},
	{MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM,          "MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM",         1, 1, 1},
	{MSG_ID_CLOUDVELA_HUMID_DATA_REQ,             "MSG_ID_CLOUDVELA_HUMID_DATA_REQ",            1, 1, 1},
	{MSG_ID_HUMID_CLOUDVELA_DATA_RESP,            "MSG_ID_HUMID_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_CLOUDVELA_HUMID_CTRL_REQ,             "MSG_ID_CLOUDVELA_HUMID_CTRL_REQ",            1, 1, 1},
	{MSG_ID_HUMID_CLOUDVELA_CTRL_RESP,            "MSG_ID_HUMID_CLOUDVELA_CTRL_RESP",           1, 1, 1},
	{MSG_ID_HUMID_CLOUDVELA_DATA_REPORT,          "MSG_ID_HUMID_CLOUDVELA_DATA_REPORT",         1, 1, 1},
	{MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM,         "MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM",        1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,           "MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ",          1, 1, 1},
	{MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP,          "MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP",         1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ,           "MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ",          1, 1, 1},
	{MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP,          "MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP",         1, 1, 1},
	{MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT,        "MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM,       "MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM",      1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,           "MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ",          1, 1, 1},
	{MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,          "MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP",         1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ,           "MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ",          1, 1, 1},
	{MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP,          "MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP",         1, 1, 1},
	{MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT,        "MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM,       "MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM",      1, 1, 1},
	{MSG_ID_CLOUDVELA_HSMMP_DATA_REQ,             "MSG_ID_CLOUDVELA_HSMMP_DATA_REQ",            1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,            "MSG_ID_HSMMP_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ,             "MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ",            1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP,            "MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP",           1, 1, 1},
	{MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT,          "MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT",         1, 1, 1},
	{MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM,         "MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM",        1, 1, 1},
	{MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT,        "MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM,       "MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM",      1, 1, 1},
	{MSG_ID_CLOUDVELA_NOISE_DATA_REQ,             "MSG_ID_CLOUDVELA_NOISE_DATA_REQ",            1, 1, 1},
	{MSG_ID_NOISE_CLOUDVELA_DATA_RESP,            "MSG_ID_NOISE_CLOUDVELA_DATA_RESP",           1, 1, 1},
	{MSG_ID_CLOUDVELA_NOISE_CTRL_REQ,             "MSG_ID_CLOUDVELA_NOISE_CTRL_REQ",            1, 1, 1},
	{MSG_ID_NOISE_CLOUDVELA_CTRL_RESP,            "MSG_ID_NOISE_CLOUDVELA_CTRL_RESP",           1, 1, 1},
	{MSG_ID_NOISE_CLOUDVELA_DATA_REPORT,          "MSG_ID_NOISE_CLOUDVELA_DATA_REPORT",         1, 1, 1},
	{MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM,         "MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM",        1, 1, 1},
	{MSG_ID_CLOUDVELA_YCJK_DATA_REQ,              "MSG_ID_CLOUDVELA_YCJK_DATA_REQ",             1, 1, 1},
	{MSG_ID_YCJK_CLOUDVELA_DATA_RESP,             "MSG_ID_YCJK_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_CLOUDVELA_YCJK_CTRL_REQ,              "MSG_ID_CLOUDVELA_YCJK_CTRL_REQ",             1, 1, 1},
	{MSG_ID_YCJK_CLOUDVELA_CTRL_RESP,             "MSG_ID_YCJK_CLOUDVELA_CTRL_RESP",            1, 1, 1},
	{MSG_ID_YCJK_CLOUDVELA_DATA_REPORT,           "MSG_ID_YCJK_CLOUDVELA_DATA_REPORT",          1, 1, 1},
	{MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM,          "MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM",         1, 1, 1},
	//CONVENIENCE-SENSOR-CLOUDVELA
	{MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ,            "MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ",           1, 1, 1},
	{MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP,           "MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP",          1, 1, 1},
	{MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT,         "MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT",        1, 1, 1},
	{MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM,        "MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM",       1, 1, 1},
	{MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ,           "MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ",          1, 1, 1},
	{MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP,          "MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP",         1, 1, 1},
	{MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT,        "MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM,       "MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM",      1, 1, 1},
	{MSG_ID_CLOUDVELA_CO1_DATA_REQ,               "MSG_ID_CLOUDVELA_CO1_DATA_REQ",              1, 1, 1},
	{MSG_ID_CO1_CLOUDVELA_DATA_RESP,              "MSG_ID_CO1_CLOUDVELA_DATA_RESP",             1, 1, 1},
	{MSG_ID_CO1_CLOUDVELA_DATA_REPORT,            "MSG_ID_CO1_CLOUDVELA_DATA_REPORT",           1, 1, 1},
	{MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM,           "MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM",          1, 1, 1},
	{MSG_ID_CLOUDVELA_HCHO_DATA_REQ,              "MSG_ID_CLOUDVELA_HCHO_DATA_REQ",             1, 1, 1},
	{MSG_ID_HCHO_CLOUDVELA_DATA_RESP,             "MSG_ID_HCHO_CLOUDVELA_DATA_RESP",            1, 1, 1},
	{MSG_ID_HCHO_CLOUDVELA_DATA_REPORT,           "MSG_ID_HCHO_CLOUDVELA_DATA_REPORT",          1, 1, 1},
	{MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM,          "MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM",         1, 1, 1},
	{MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ,          "MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ",         1, 1, 1},
	{MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP,         "MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP",        1, 1, 1},
	{MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT,       "MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT",      1, 1, 1},
	{MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM,      "MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM",     1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25SP_DATA_REQ,            "MSG_ID_CLOUDVELA_PM25SP_DATA_REQ",           1, 1, 1},
	{MSG_ID_PM25SP_CLOUDVELA_DATA_RESP,           "MSG_ID_PM25SP_CLOUDVELA_DATA_RESP",          1, 1, 1},
	{MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT,         "MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT",        1, 1, 1},
	{MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM,        "MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM",       1, 1, 1},
	{MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ,          "MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ",         1, 1, 1},
	{MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP,         "MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP",        1, 1, 1},
	{MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT,       "MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT",      1, 1, 1},
	{MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM,      "MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM",     1, 1, 1},
	//CANITF
	{MSG_ID_CANITFLEO_DATA_REPORT,                "MSG_ID_CANITFLEO_DATA_REPORT",               1, 1, 1},
	{MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT,    "MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT",   1, 1, 1}, //187
	{MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM,   "MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM",  1, 1, 1},
	{MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT,   "MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT",  1, 1, 1},
	{MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM,  "MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM", 1, 1, 1},
	//BFSC/SCALE_WEIGHT组合秤
	{MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ,            "MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ",           1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP,           "MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP",          1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT,         "MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT",        1, 1, 1},
	{MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM,        "MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM",       1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT,        "MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT",       1, 1, 1},
	{MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM,       "MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM",      1, 1, 1},
	{MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ,            "MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ",           1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_CTRL_RESP,           "MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP",           1, 1, 1},
	{MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT,    "MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT",   1, 1, 1},
	{MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM,   "MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM",  1, 1, 1},
	//CANITFLEO
	{MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,          	  "MSG_ID_L3BFSC_CAN_SYS_CFG_REQ",         		1, 1, 1},
	{MSG_ID_CAN_L3BFSC_SYS_CFG_RESP,          	  "MSG_ID_CAN_L3BFSC_SYS_CFG_RESP",         	1, 1, 1},
	{MSG_ID_L3BFSC_CAN_SYS_START_REQ,          	  "MSG_ID_L3BFSC_CAN_SYS_START_REQ",         	1, 1, 1},
	{MSG_ID_CAN_L3BFSC_SYS_START_RESP,            "MSG_ID_CAN_L3BFSC_SYS_START_RESP",         	1, 1, 1},
	{MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,          	  "MSG_ID_L3BFSC_CAN_SYS_STOP_REQ",         	1, 1, 1},
	{MSG_ID_CAN_L3BFSC_SYS_STOP_RESP,         	  "MSG_ID_CAN_L3BFSC_SYS_STOP_RESP",         	1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,        "MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT",       1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,               "MSG_ID_L3BFSC_CAN_WS_COMB_OUT",              1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,            "MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB",           1, 1, 1},
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,                "MSG_ID_L3BFSC_CAN_WS_GIVE_UP",               1, 1, 1},
	{MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,             "MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB",            1, 1, 1},
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,         "MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ",        1, 1, 1},
	{MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,        "MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP",       1, 1, 1},
	{MSG_ID_USBCAN_L2FRAME_RCV,        			  "MSG_ID_USBCAN_L2FRAME_RCV",       			1, 1, 1},
	//BFSCUICOMM
	{MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND,       "MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND",      1, 1, 1},
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,                "MSG_ID_UICOMM_L3BFSC_CMD_REQ",               1, 1, 1},
	{MSG_ID_L3BFSC_UICOMM_CMD_RESP,               "MSG_ID_L3BFSC_UICOMM_CMD_RESP",              1, 1, 1},
	{MSG_ID_UICOMM_L3BFSC_CFG_REQ,       		  "MSG_ID_UICOMM_L3BFSC_CFG_REQ",      			1, 1, 1},
	{MSG_ID_L3BFSC_UICOMM_CFG_RESP,       		  "MSG_ID_L3BFSC_UICOMM_CFG_RESP",      		1, 1, 1},
	{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,       		  "MSG_ID_UICOMM_CAN_TEST_CMD_REQ",      		1, 1, 1},
	{MSG_ID_CAN_UICOMM_TEST_CMD_RESP,       	  "MSG_ID_CAN_UICOMM_TEST_CMD_RESP",      		1, 1, 1},

	//L3AQYCG20
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,                  "MSG_ID_L3AQYC_EXG_CTRL_REQ",                 1, 1, 1},
	{MSG_ID_L3AQYC_EXG_CTRL_RESP,                 "MSG_ID_L3AQYC_EXG_CTRL_RESP",                1, 1, 1},
	{MSG_ID_L3AQYC_EXG_DATA_REPORT,               "MSG_ID_L3AQYC_EXG_DATA_REPORT",              1, 1, 1},
	//ZHBHJT
	{MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ,           "MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ",          1, 1, 1},
	{MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP,          "MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP",         1, 1, 1},
	{MSG_ID_LLCZHB_L3MOD_CTRL_REQ,                "MSG_ID_LLCZHB_L3MOD_CTRL_REQ",               1, 1, 1},
	{MSG_ID_L3MOD_LLCZHB_CTRL_RESP,               "MSG_ID_L3MOD_LLCZHB_CTRL_RESP",              1, 1, 1},
	{MSG_ID_L3MOD_LLCZHB_DATA_REPORT,             "MSG_ID_L3MOD_LLCZHB_DATA_REPORT",            1, 1, 1},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,                "MSG_ID_ZHBL3MOD_EXG_CTRL_REQ",               1, 1, 1},
	{MSG_ID_ZHBL3MOD_EXG_CTRL_RESP,               "MSG_ID_ZHBL3MOD_EXG_CTRL_RESP",              1, 1, 1},
	{MSG_ID_ZHBL3MOD_EXG_DATA_REPORT,             "MSG_ID_ZHBL3MOD_EXG_DATA_REPORT",            1, 1, 1},
	//L3HATE:测试环境对应的消息
	{MSG_ID_L3HATE_TC_START,             	  	  "MSG_ID_L3HATE_TC_START",                     1, 1, 1},
	{MSG_ID_ETH_L3HATE_FRAME_RCV,             	  "MSG_ID_ETH_L3HATE_FRAME_RCV",                1, 1, 1},
	{MSG_ID_SPS_L3HATE_FRAME_RCV,             	  "MSG_ID_SPS_L3HATE_FRAME_RCV",                1, 1, 1},
	{MSG_ID_CAN_L3HATE_FRAME_RCV,             	  "MSG_ID_CAN_L3HATE_FRAME_RCV",                1, 1, 1},
	{MSG_ID_MQTT_L3HATE_FRAME_RCV,             	  "MSG_ID_MQTT_L3HATE_FRAME_RCV",               1, 1, 1},
	//END FLAG
	{MSG_ID_COM_MAX,                              "MSG_ID_COM_MAX",                             0, 0, 0},    //Ending
};

//消息ID的定义全局表，方便TRACE函数使用
//请服从HCU_SYSDIM_TIMERID_NAME_LEN_MAX的最长长度，否则出错
//设置的技巧是：基础部分不要动，配置不一样的，请单独在分项目中，再设置一遍
HcuSysEngTimerStaticCfg_t zHcuSysEngTimerStaticCfg[] = {
	//TIMER_ID                                       定时器名字                                 定时长度   精度                      注释
	//STARING FLAG: 公共基础部分，不要删除，新的定时器，需要在此增加，确保完整性。各个项目重复的定时器且不同数值部分，可以再分项目中第二次进行设置，后一次设置的数据将覆盖前一次的数值
	{TIMER_ID_MIN,                                   "TID_MIN",                                0,       TIMER_RESOLUTION_1S},    //STARTING
	{TIMER_ID_1S_MIN,                                "TID_1S_MIN",                             0,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SVRCON_INIT_FB,                     "TID_1S_SVRCON_INIT_FB",                  10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_LED_GALOWAG_SCAN,                   "TID_1S_LED_GALOWAG_SCAN",                1,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_EMC_PERIOD_READ,                    "TID_1S_EMC_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_EMC_MODBUS_FB,                      "TID_1S_EMC_MODBUS_FB",                   10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_PM25_PERIOD_READ,                   "TID_1S_PM25_PERIOD_READ",                600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_PM25_MODBUS_FB,                     "TID_1S_PM25_MODBUS_FB",                  10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDDIR_PERIOD_READ,                "TID_1S_WINDDIR_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDDIR_MODBUS_FB,                  "TID_1S_WINDDIR_MODBUS_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDSPD_PERIOD_READ,                "TID_1S_WINDSPD_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDSPD_MODBUS_FB,                  "TID_1S_WINDSPD_MODBUS_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_TEMP_PERIOD_READ,                   "TID_1S_TEMP_PERIOD_READ",                600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_TEMP_FB,                            "TID_1S_TEMP_FB",                         10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HUMID_PERIOD_READ,                  "TID_1S_HUMID_PERIOD_READ",               600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HUMID_MODBUS_FB,                    "TID_1S_HUMID_MODBUS_FB",                 10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_PERIOD_READ,                  "TID_1S_NOISE_PERIOD_READ",               600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_MODBUS_FB,                    "TID_1S_NOISE_MODBUS_FB",                 10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_SPSVIRGO_FB,                  "TID_1S_NOISE_SPSVIRGO_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ,          "TID_1S_HSMMP_PERIOD_AVORION_READ",       600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HSMMP_AVORION_FB,                   "TID_1S_HSMMP_AVORION_FB",                10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HSMMP_PERIOD_CURL_PICTURE,          "TID_ID_1S_HSMMP_PERIOD_CURL_PICTURE",    3600,    TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CLOUDVELA_PERIOD_LINK_HEART_BEAT,   "TID_1S_CLOUDVELA_PERIOD_LINK_HBT",       10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NBIOTCJ188_PERIOD_LINK_HEART_BEAT,  "TID_1S_NBIOTCJ188_PERIOD_LINK_HBT",      600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NBIOTCJ188_SEND_DATA_BACK,          "TID_1S_NBIOTCJ188_SEND_DATA_BACK",       10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NBIOTQG376_PERIOD_LINK_HEART_BEAT,  "TID_1S_NBIOTQG376_PERIOD_LINK_HBT",      600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NBIOTQG376_SEND_DATA_BACK,          "TID_1S_NBIOTQG376_SEND_DATA_BACK",       10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_LLCZHB_LINK_CTRL,                   "TID_1S_LLCZHB_LINK_CTRL",                10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_AIRPRS_PERIOD_READ,                 "TID_1S_AIRPRS_PERIOD_READ",              600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CO1_PERIOD_READ,                    "TID_1S_CO1_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_LIGHTSTR_PERIOD_READ,               "TID_1S_LIGHTSTR_PERIOD_READ",            600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_ALCOHOL_PERIOD_READ,                "TID_1S_ALCOHOL_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HCHO_PERIOD_READ,                   "TID_1S_HCHO_PERIOD_READ",                600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_TOXICGAS_PERIOD_READ,               "TID_1S_TOXICGAS_PERIOD_READ",            600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_PM25SHARP_PERIOD_READ,              "TID_1S_PM25SHARP_PERIOD_READ",           600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_IWM_PERIOD_READ,                    "TID_1S_IWM_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_IHM_PERIOD_READ,                    "TID_1S_IHM_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_IGM_PERIOD_READ,                    "TID_1S_IGM_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_IPM_PERIOD_READ,                    "TID_1S_IPM_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSPM_PERIOD_WORKING,               "TID_1S_SYSPM_PERIOD_WORKING",            311,     TIMER_RESOLUTION_1S},//for test(3600)
	{TIMER_ID_1S_SYSSWM_PERIOD_WORKING,              "TID_1S_SYSSWM_PERIOD_WORKING",           900,     TIMER_RESOLUTION_1S},//900, 正常15一次，分别来轮４个软件体
	{TIMER_ID_1S_SYSSWM_SEG_DL_WAIT,             	 "TID_1S_SYSSWM_SEG_DL_WAIT",              3,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CANITFLEO_WORKING_SCAN,             "TID_1S_CANITFLEO_WORKING_SCAN",          10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_CFG_WAIT_FB,             "TID_1S_L3BFSC_SYS_CFG_WAIT_FB",          15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_START_WAIT_FB,           "TID_1S_L3BFSC_SYS_START_WAIT_FB",        15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_STOP_WAIT_FB,            "TID_1S_L3BFSC_SYS_STOP_WAIT_FB",         15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TTT_WAIT_FB,                 "TID_1S_L3BFSC_TTT_WAIT_FB",              15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TGU_WAIT_FB,                 "TID_1S_L3BFSC_TGU_WAIT_FB",              15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_ERROR_INQ,           		 "TID_1S_L3BFSC_ERROR_INQ",                60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_BFSCUICOMM_PERIOD_READ,             "TID_1S_BFSCUICOMM_PERIOD_READ",          600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG10_PERIOD_READ,              "TID_1S_L3AQYCG10_PERIOD_READ",           600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_PERIOD_READ,              "TID_1S_L3AQYCG20_PERIOD_READ",           600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_PERIOD_STA_SCAN,          "TID_1S_L3AQYCG20_PERIOD_STA_SCAN",       30,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_START_MIN_REPORT,         "TID_1S_L3AQYCG20_START_MIN_REPORT",      60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_STOP_MIN_REPORT,          "TID_1S_L3AQYCG20_STOP_MIN_REPORT",       60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_START_HOUR_REPORT,        "TID_1S_L3AQYCG20_START_HOUR_REPORT",     60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_STOP_HOUR_REPORT,         "TID_1S_L3AQYCG20_STOP_HOUR_REPORT",      60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_START_DAY_REPORT,         "TID_1S_L3AQYCG20_START_DAY_REPORT",      60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3AQYCG20_STOP_DAY_REPORT,          "TID_1S_L3AQYCG20_STOP_DAY_REPORT",       60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3TBSWRG30_PERIOD_READ,             "TID_1S_L3TBSWRG30_PERIOD_READ",          600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3GQYBG40_PERIOD_READ,              "TID_1S_L3GQYBG40_PERIOD_READ",           600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3CXGLACM_PERIOD_READ,              "TID_1S_L3CXGLACM_PERIOD_READ",           600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3CXILC_PERIOD_READ,                "TID_1S_L3CXILC_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3NBHPM_PERIOD_READ,                "TID_1S_L3NBHPM_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3NBLPM_PERIOD_READ,                "TID_1S_L3NBLPM_PERIOD_READ",             600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3OPWLOTDR_PERIOD_READ,             "TID_1S_L3OPWLOTDR_PERIOD_READ",          600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3HATE_WORK_CTRL_FB,             	 "TID_1S_L3HATE_WORK_CTRL_FB",             5,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_MAX,                                "TID_1S_MAX",                             0,       TIMER_RESOLUTION_1S},
	{TIMER_ID_10MS_MIN,                              "TID_10MS_MIN",                           0,       TIMER_RESOLUTION_10MS},
	{TIMER_ID_10MS_SVRCON_TEST,                      "TID_10MS_SVRCON_TEST",                   10000,   TIMER_RESOLUTION_10MS},
	{TIMER_ID_10MS_L3BFSC_PERIOD_STA_SCAN,           "TID_10MS_L3BFSC_PERIOD_STA_SCAN",        50,      TIMER_RESOLUTION_10MS},
	{TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA,        "TID_10MS_CANITFLEO_SIMULATION_DATA",     20,      TIMER_RESOLUTION_10MS},
	{TIMER_ID_10MS_MAX,                              "TID_10MS_MAX",                           0,       TIMER_RESOLUTION_10MS},
	{TIMER_ID_1MS_MIN,                               "TID_1MS_MIN",                            0,       TIMER_RESOLUTION_1MS},
	{TIMER_ID_1MS_SVRCON_TEST,                       "TID_1MS_SVRCON_TEST",                    100,     TIMER_RESOLUTION_1MS},
	{TIMER_ID_1MS_MAX,                               "TID_1MS_MAX",                            0,       TIMER_RESOLUTION_1MS},
	//END公共基础部分
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	{TIMER_ID_1S_EMC_PERIOD_READ,                    "TID_1S_EMC_PERIOD_READ",                 600,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_EMC_MODBUS_FB,                      "TID_1S_EMC_MODBUS_FB",                   10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_PM25_PERIOD_READ,                   "TID_1S_PM25_PERIOD_READ",                31,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_PM25_MODBUS_FB,                     "TID_1S_PM25_MODBUS_FB",                  10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDDIR_PERIOD_READ,                "TID_1S_WINDDIR_PERIOD_READ",             13,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDDIR_MODBUS_FB,                  "TID_1S_WINDDIR_MODBUS_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDSPD_PERIOD_READ,                "TID_1S_WINDSPD_PERIOD_READ",             15,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_WINDSPD_MODBUS_FB,                  "TID_1S_WINDSPD_MODBUS_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_TEMP_PERIOD_READ,                   "TID_1S_TEMP_PERIOD_READ",                17,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_TEMP_FB,                            "TID_1S_TEMP_FB",                         10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HUMID_PERIOD_READ,                  "TID_1S_HUMID_PERIOD_READ",               19,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HUMID_MODBUS_FB,                    "TID_1S_HUMID_MODBUS_FB",                 10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_PERIOD_READ,                  "TID_1S_NOISE_PERIOD_READ",               7,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_MODBUS_FB,                    "TID_1S_NOISE_MODBUS_FB",                 5,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_NOISE_SPSVIRGO_FB,                  "TID_1S_NOISE_SPSVIRGO_FB",               10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HSMMP_PERIOD_AVORION_READ,          "TID_1S_HSMMP_PERIOD_AVORION_READ",       1120,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_HSMMP_AVORION_FB,                   "TID_1S_HSMMP_AVORION_FB",                10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSPM_PERIOD_WORKING,               "TID_1S_SYSPM_PERIOD_WORKING",            311,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSSWM_PERIOD_WORKING,              "TID_1S_SYSSWM_PERIOD_WORKING",           60,     TIMER_RESOLUTION_1S},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{TIMER_ID_1S_EMC_PERIOD_READ,                    "TID_1S_EMC_PERIOD_READ",                 100,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CANITFLEO_WORKING_SCAN,             "TID_1S_CANITFLEO_WORKING_SCAN",          10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_CFG_WAIT_FB,             "TID_1S_L3BFSC_SYS_CFG_WAIT_FB",          15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_START_WAIT_FB,           "TID_1S_L3BFSC_SYS_START_WAIT_FB",        15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_STOP_WAIT_FB,            "TID_1S_L3BFSC_SYS_STOP_WAIT_FB",         15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TTT_WAIT_FB,                 "TID_1S_L3BFSC_TTT_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TGU_WAIT_FB,                 "TID_1S_L3BFSC_TGU_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_ERROR_INQ,          		 "TID_1S_L3BFSC_ERROR_INQ",                15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_BFSCUICOMM_PERIOD_READ,             "TID_1S_BFSCUICOMM_PERIOD_READ",          5,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSSWM_PERIOD_WORKING,              "TID_1S_SYSSWM_PERIOD_WORKING",           90,      TIMER_RESOLUTION_1S}, //本项目独特
	{TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA,        "TID_10MS_CANITFLEO_SIMULATION_DATA",     40,      TIMER_RESOLUTION_10MS},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	{TIMER_ID_1S_EMC_PERIOD_READ,                    "TID_1S_EMC_PERIOD_READ",                 100,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CANITFLEO_WORKING_SCAN,             "TID_1S_CANITFLEO_WORKING_SCAN",          10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_CFG_WAIT_FB,             "TID_1S_L3BFSC_SYS_CFG_WAIT_FB",          15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_START_WAIT_FB,           "TID_1S_L3BFSC_SYS_START_WAIT_FB",        15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_STOP_WAIT_FB,            "TID_1S_L3BFSC_SYS_STOP_WAIT_FB",         15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TTT_WAIT_FB,                 "TID_1S_L3BFSC_TTT_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TGU_WAIT_FB,                 "TID_1S_L3BFSC_TGU_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_ERROR_INQ,          		 "TID_1S_L3BFSC_ERROR_INQ",                15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_BFSCUICOMM_PERIOD_READ,             "TID_1S_BFSCUICOMM_PERIOD_READ",          5,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSSWM_PERIOD_WORKING,              "TID_1S_SYSSWM_PERIOD_WORKING",           90,      TIMER_RESOLUTION_1S}, //本项目独特
	{TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA,        "TID_10MS_CANITFLEO_SIMULATION_DATA",     40,      TIMER_RESOLUTION_10MS},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	{TIMER_ID_1S_EMC_PERIOD_READ,                    "TID_1S_EMC_PERIOD_READ",                 100,     TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_CANITFLEO_WORKING_SCAN,             "TID_1S_CANITFLEO_WORKING_SCAN",          10,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_CFG_WAIT_FB,             "TID_1S_L3BFSC_SYS_CFG_WAIT_FB",          15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_START_WAIT_FB,           "TID_1S_L3BFSC_SYS_START_WAIT_FB",        15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_SYS_STOP_WAIT_FB,            "TID_1S_L3BFSC_SYS_STOP_WAIT_FB",         15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TTT_WAIT_FB,                 "TID_1S_L3BFSC_TTT_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_TGU_WAIT_FB,                 "TID_1S_L3BFSC_TGU_WAIT_FB",              60,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_L3BFSC_ERROR_INQ,          		 "TID_1S_L3BFSC_ERROR_INQ",                15,      TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_BFSCUICOMM_PERIOD_READ,             "TID_1S_BFSCUICOMM_PERIOD_READ",          5,       TIMER_RESOLUTION_1S},
	{TIMER_ID_1S_SYSSWM_PERIOD_WORKING,              "TID_1S_SYSSWM_PERIOD_WORKING",           90,      TIMER_RESOLUTION_1S}, //本项目独特
	{TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA,        "TID_10MS_CANITFLEO_SIMULATION_DATA",     40,      TIMER_RESOLUTION_10MS},
#else
	#error Un-correct constant definition
#endif
	//END FLAG
	{TIMER_ID_MAX,                                   "TID_MAX",                                0,       TIMER_RESOLUTION_1S},     //END
};

//启动区XML关键字定义
HcuSysEngPhyBootCfg_t zHcuSysEngPhyBootCfg[] = {
	{0,   	"<xml>", 				    "</xml>"},
	{HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX,  	"<equLable>",               "</equLable>"},
	{HCU_SYSENG_PAR_HWBURN_MAC_LEN_MAX,  	"<hw_mac>",                 "</hw_mac>"},
	{HCU_SYSENG_PAR_HWBURN_ZHBMN_LEN_MAX,  	"<zhbMnLable>",             "</zhbMnLable>"},
	{2,		"<hwType>",                 "</hwType>"},
	{2,		"<hwPemId>",                "</hwPemId>"},
	{2,		"<swRelId>",                "</swRelId>"},
	{2,		"<swVerId>",                "</swVerId>"},
	{2,		"<dbVerId>",                "</dbVerId>"},
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
	{8,		"<rsv>",                    "</rsv>"},
	{2,		"<nodeHwType>",             "</nodeHwType>"},
	{2,		"<nodeHwPemId>",            "</nodeHwPemId>"}
};

//系统设备传感器配置表
HcuVmCtrCodabStaticCfg_t zHcuVmCtrCodabStaticCfg[] = {
	//START FLAG                        传感器名字                         是否配置激活
	{SENSOR_ID_MIN,                     "SNR_MIN",                       HCU_SYSCFG_SENSOR_PRESENT_NO},
	//正式的传感器列表
	{SENSOR_ID_SPSPM25SHARP,            "SNR_spsPm25Sharp",              HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_SPSHCHOZE08CH2O,         "SNR_spsHchoZe08ch2o",           HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_GPIOTEMPDHT11,           "SNR_gpioTempDht11",             HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_GPIOHUMIDDHT11,          "SNR_gpioHumidDht11",            HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_GPIOTOXICGASMQ135,       "SNR_gpioToxicgasMq135",         HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_GPIOALCOHOLMQ3ALCO,      "SNR_gpioAlcoholMq3alco",        HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_GPIOTOXICGASZP01VOC,     "SNR_gpioToxicgasZp01voc",       HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CTEMPSHT20,            "SNR_i2cTempSht20",              HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CHUMIDSHT20,           "SNR_i2cHumidSht20",             HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CLIGHTSTRBH1750,       "SNR_i2cLightstrBh1750",         HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CAIRPRSBMP180,         "SNR_i2cAirprsBmp180",           HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CTEMPBMP180,           "SNR_i2cTempBmp180",             HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CALTITUDEBMP180,       "SNR_i2cAltitudeBmp180",         HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_I2CPM25BMPD300,          "SNR_i2cPm25Bmpd300",            HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_SPITEMPRHT03,            "SNR_spiTempRht03",              HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_SPIHUMIDRHT03,           "SNR_spiHumidRht03",             HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_SPITEMPMTH01,            "SNR_spiTempMth01",              HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_SPIHUMIDMTH01,           "SNR_spiHumidMth01",             HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_PWMMOTOSG90,             "SNR_pwmMotoSg90",               HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_PWMLED2PIN,              "SNR_pwmLed2Pin",                HCU_SYSCFG_SENSOR_PRESENT_NO},
	{SENSOR_ID_LEDGPIO2PIN,             "SNR_ledGpio2Pin",               HCU_SYSCFG_SENSOR_PRESENT_NO},
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	{SENSOR_ID_GPIOTEMPDHT11,           "SNR_gpioTempDht11",             HCU_SYSCFG_SENSOR_PRESENT_YES},
	{SENSOR_ID_GPIOHUMIDDHT11,          "SNR_gpioHumidDht11",            HCU_SYSCFG_SENSOR_PRESENT_YES},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{SENSOR_ID_GPIOTEMPDHT11,           "SNR_gpioTempDht11",             HCU_SYSCFG_SENSOR_PRESENT_YES},
	{SENSOR_ID_GPIOHUMIDDHT11,          "SNR_gpioHumidDht11",            HCU_SYSCFG_SENSOR_PRESENT_YES},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	{SENSOR_ID_GPIOTEMPDHT11,           "SNR_gpioTempDht11",             HCU_SYSCFG_SENSOR_PRESENT_YES},
	{SENSOR_ID_GPIOHUMIDDHT11,          "SNR_gpioHumidDht11",            HCU_SYSCFG_SENSOR_PRESENT_YES},
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	{SENSOR_ID_GPIOTEMPDHT11,           "SNR_gpioTempDht11",             HCU_SYSCFG_SENSOR_PRESENT_YES},
	{SENSOR_ID_GPIOHUMIDDHT11,          "SNR_gpioHumidDht11",            HCU_SYSCFG_SENSOR_PRESENT_YES},
#else
	#error Un-correct constant definition
#endif
	//END_FLAG
	{SENSOR_ID_MAX,                     "SNR_MAX",                          HCU_SYSCFG_SENSOR_PRESENT_NO},
};








