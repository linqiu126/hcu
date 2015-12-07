/*
 * l2comdef.h
 *
 *  Created on: 2015年11月16日
 *      Author: test
 */

#ifndef L2FRAME_L2COMDEF_H_
#define L2FRAME_L2COMDEF_H_

#include "../comvm/vmlayer.h"

//自定义存储数据结构
//DISC data, 只存储周期性数据
#define DISC_DATA_SAMPLE_STORAGE_NBR_MAX 20000
typedef struct HcuDiscDataSampleStorageArray
{
	UINT32 sid;
	UINT8  sensortype;  //0x20->EMC, 0x25->PM1025, L3UserCmdId
	UINT8  onOffLine;
	UINT32 timestamp;
	UINT32 emcValue;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	UINT32 windspdValue;
	UINT32 winddirValue;
	UINT32 tempValue;
	UINT32 humidValue;
	UINT32 equipid;
}HcuDiscDataSampleStorageArray_t;
#define DISC_DATA_SAMPLE_ONLINE 1
#define DISC_DATA_SAMPLE_OFFLINE 2
typedef struct HcuDiscDataSampleStorage
{
	UINT32 rdCnt;
	UINT32 wrtCnt;
	UINT32 recordNbr;
	UINT32 offlineNbr;
	UINT32 lastSid;
	HcuDiscDataSampleStorageArray_t recordItem[DISC_DATA_SAMPLE_STORAGE_NBR_MAX];
}HcuDiscDataSampleStorage_t;

//自定义，暂时不服从任何规范
//CLOUD<->HCU之间自行定义的命令字
typedef enum
{
	L3CI_none = 0,
	L3CI_blood_glucose = 1,  //ÑªÌÇ
	L3CI_single_sports = 2,  //µ¥´ÎÔË¶¯
	L3CI_single_sleep = 3, //µ¥´ÎË¯Ãß
	L3CI_body_fat = 4,  //ÌåÖ¬
	L3CI_blood_pressure = 5, //ÑªÑ¹
	L3CI_runner_machine_report = 0x0A, //ÅÜ²½»úÊý¾ÝÉÏ±¨
	L3CI_runner_machine_control = 0x0B, //ÅÜ²½»úÈÎÎñ¿ØÖÆ
	L3CI_emc = 0x20, //µç´Å·øÉäÇ¿¶È
	L3CI_emc_accumulation = 0x21, //µç´Å·øÉä¼ÁÁ¿
	L3CI_co = 0x22, //COÒ»Ñõ»¯Ì¼
	L3CI_formaldehyde = 0x23, //¼×È©
	L3CI_alcohol = 0x24, //¾Æ¾«
	L3CI_pm25 = 0x25, //PM1/2.5/10
	L3CI_windspd = 0x26, //Wind Speed
	L3CI_winddir = 0x27, //Wind Direction
	L3CI_temp = 0x28, //Temperature
	L3CI_humid = 0x29, //Humidity
	L3CI_sw_inventory = 0xA0,	//Èí¼þÇåµ¥
	L3CI_sw_package = 0xA1,	//Èí¼þ°æ±¾Ìå
	L3CI_equipment_info = 0xF0,	//Éè±¸»ù±¾ÐÅÏ¢
	L3CI_personal_info = 0xF1,	//¸öÈË»ù±¾ÐÅÏ¢
	L3CI_time_sync = 0xF2,	//Ê±¼äÍ¬²½
	L3CI_read_data = 0xF3,	//¶ÁÈ¡Êý¾Ý
	L3CI_clock_timeout = 0xF4,	//¶¨Ê±ÄÖÖÓ¼°¾Ã×øÌáÐÑ
	L3CI_sync_charging = 0xF5,	//Í¬²½³äµç£¬Ë«»÷Çé¿ö
	L3CI_sync_trigger = 0xF6,	//Í¬²½Í¨ÖªÐÅÏ¢
}L3UserCmdIdDef;
//CLOUD<->HCU之间定义的操作字
typedef enum
{
	L3PO_emc_none = 0,
	L3PO_emc_data_req = 0x01, //Data Request
}L3EmcOptIdDef;
typedef enum
{
	L3PO_pm25_none = 0,
	L3PO_pm25_data_req = 0x01, //PM3 Request
	L3PO_pm25_set_pm1025_switch = 0x02,
	L3PO_pm25_set_modbus_address =0x03,
	L3PO_pm25_set_work_cycle = 0x04, //In second
	L3PO_pm25_set_sample_cycle = 0x05, //In second
	L3PO_pm25_set_sample_number = 0x06,
	L3PO_pm25_data_report = 0x81,
	L3PO_pm25_read_switch = 0x82,
	L3PO_pm25_read_modbus_address = 0x83,
	L3PO_pm25_read_work_cycle = 0x84, //In second
	L3PO_pm25_read_sample_cycle = 0x85, //In second
	L3PO_pm25_read_sample_number = 0x86,
}L3Pm25OptIdDef;
typedef enum
{
	L3PO_winddir_none = 0,
	L3PO_winddir_data_req = 0x01, //Data Request
}L3WinddirOptId;

typedef enum
{
	L3PO_windspd_none = 0,
	L3PO_windspd_data_req = 0x01, //Data Request
}L3WindspdOptIdDef;
typedef enum
{
	L3PO_temp_none = 0,
	L3PO_temp_data_req = 0x01, //Data Request
}L3TempOptIdDef;
typedef enum
{
	L3PO_humid_none = 0,
	L3PO_humid_data_req = 0x01, //Data Request
}L3HumidOptIdDef;
//UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数，针对读数到底是周期性的还是一次性的
//内部定义，内部使用
typedef enum
{
	L3CI_cmdid_back_type_none = 0,
	L3CI_cmdid_back_type_instance = 1,
	L3CI_cmdid_back_type_period = 2,
	L3CI_cmdid_back_type_control = 3,
	L3CI_cmdid_back_type_invalid = 0xFF,
}L3UserCmdIdBackTypeDef;


//服从标准MODBUS规范
//MODBUS中缺省配置的设备地址，被上层传感器所使用，自定义
typedef enum
{
	MODBUS_NONE_RTU_EQUIPMENT_ID = 0,
	MODBUS_PM25_RTU_EQUIPMENT_ID = 0x1,
	MODBUS_WINDSPD_RTU_EQUIPMENT_ID = 0x02,
	MODBUS_WINDDIR_RTU_EQUIPMENT_ID = 0x03,
	MODBUS_EMC_RTU_EQUIPMENT_ID = 0x05,
	MODBUS_TEMP_RTU_EQUIPMENT_ID = 0x06,
	MODBUS_HUMID_RTU_EQUIPMENT_ID = 0x06,
	MODBUS_INVALID_RTU_EQUIPMENT_ID = 0xFF,
}ModbusSensorEquipmentIdDef;
//功能码定义
#define MODBUS_GENERIC_FUNC_DATA_INQUERY  0x03
#define MODBUS_PM25_FUNC_DATA_INQUERY  0x03
//EMC寄存器定义 => 假设的，还不存在
typedef enum
{
	EMC_REG_DATA_READ = 4,  //2B, just for example
	EMC_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
}EmcRegisterSelfDef;
//PM25寄存器定义 => 型号SJFC200/T独享
typedef enum
{
	PM25_REG_DATA_PM1D0 = 4,  //2B
	PM25_REG_DATA_PM25 = 6,  //2B
	PM25_REG_DATA_PM10 = 8,  //2B
	PM25_REG_CYCLE_SUM = 31,  //1B, in second
	PM25_REG_CYCLE_CUR = 32,  //1B
	PM25_REG_STOP_START = 33,  //1B measurement
	PM25_REG_PM1D0_ADJUST = 34,  //1B
	PM25_REG_PM25_ADJUST = 35,  //1B
	PM25_REG_PM10_ADJUST = 36,  //1B
	PM25_REG_MEASURE_CNT = 37,  //1B
	PM25_REG_INTERVAL = 38, //1B
	PM25_REG_DELAY = 39, //1B
	PM25_REG_CUR_EQU_ID = 60, //1B
	PM25_LENGTH_OF_REG = 0x06, //6个寄存器，返回12B
}Pm25RegisterSjfc200tDef;
//WIND DIRECTION寄存器定义
typedef enum
{
	WINDDIR_REG_DATA_READ = 0,  //2B
	WINDDIR_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
}WinddirRegisterSelfDef;
//WIND SPEED寄存器定义
typedef enum
{
	WINDSPD_REG_DATA_READ = 0,  //2B
	WINDSPD_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
}WindSpdRegisterSelfDef;
//TEMP寄存器定义
typedef enum
{
	TEMP_REG_DATA_READ = 0,  //2B
	TEMP_LENGTH_OF_REG = 0x02, //2个寄存器，返回4B
}TempRegisterSelfDef;
//HUMIDITY寄存器定义
typedef enum
{
	HUMID_REG_DATA_READ = 0,  //2B
	HUMID_LENGTH_OF_REG = 0x02, //2个寄存器，返回4B
}HumidRegisterSelfDef;


//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
#define TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES 60  //shall be 60second


//为了表征Sensor/Audio/Camera文件的类型，这里定义不同的操作类型
typedef enum
{
	FILE_OPERATION_TYPE_NONE = 0,
	FILE_OPERATION_TYPE_SENSOR,
	FILE_OPERATION_TYPE_AUDIO,
	FILE_OPERATION_TYPE_CAMERA,
	FILE_OPERATION_TYPE_MAX,
	FILE_OPERATION_TYPE_INVALID = 0xFF,
}FileOperationTypeEnum;

//公共定义的设备缺省标识，暂时已TI MX6设备为模版
#define	HCU_DEFAULT_DEVICE_ETHERNET "eth0"
#define	HCU_DEFAULT_DEVICE_LCD  "/sys/class/blacklight"
#define	HCU_DEFAULT_DEVICE_USB_DISK  "/dev/sDX"
#define	HCU_DEFAULT_DEVICE_USB_MOUSE  "/dev/input/mice"
#define	HCU_DEFAULT_DEVICE_USB_3G4G  "/dev/ttyUSB"
#define	HCU_DEFAULT_DEVICE_USB_CAMERA0 "/dev/video0"
#define	HCU_DEFAULT_DEVICE_USB_CAMERA  "/dev/video2"
#define	HCU_DEFAULT_DEVICE_USB_WIFI  "eth1"
#define	HCU_DEFAULT_DEVICE_USB_BLE  "wlan0"
#define	HCU_DEFAULT_DEVICE_USB_NET  "eth2"
#define	HCU_DEFAULT_DEVICE_SD  "/dev/block/mmcblk0pX"
#define	HCU_DEFAULT_DEVICE_SATA  "/dev/sda"
#define	HCU_DEFAULT_DEVICE_LCD_FRAMEBUFFER  "/dev/fb0"
#define	HCU_DEFAULT_DEVICE_TOUCH_SCREEN  "/dev/input/event"
#define	HCU_DEFAULT_DEVICE_RTC  "/dev/rtc0"
#define	HCU_DEFAULT_DEVICE_SPI  "/dev/"  //to be defined later
#define	HCU_DEFAULT_DEVICE_SPS232  "/dev/" //to be defined later
#define	HCU_DEFAULT_DEVICE_SPS485  "/dev/" //to be defined later
#define	HCU_DEFAULT_DEVICE_KEYBOARD  "/dev/" //to be defined later
#define	HCU_DEFAULT_DEVICE_WATCH_DOG  "/dev/watchdog"
#define	HCU_DEFAULT_DEVICE_CAN  "can0"
#define	HCU_DEFAULT_DEVICE_AUDIO  "/dev/snd/"
//串口名字
#define HCU_DEFAULT_DEVICE_TTY0_NAME   "/dev/ttySAC0"
#define HCU_DEFAULT_DEVICE_TTY1_NAME   "/dev/ttySAC1"
#define HCU_DEFAULT_DEVICE_TTY2_NAME   "/dev/ttySAC2"
//公共定义的设备缺省标识====>END

//HWINV中定义的公共状态，其他模块也要使用，故而放在这里进行定义 HcuHwinvBaseElement.hwStatus => zHcuHwinvTable
#define HCU_HWINV_STATUS_INSTALL_ACTIVE 1
#define HCU_HWINV_STATUS_INSTALL_DEACTIVE 2
#define HCU_HWINV_STATUS_NOT_INSTALL 0xFE
#define HCU_HWINV_STATUS_INVALID 0xFF

//CLOUD模块中zHcuCloudConTable对当前工作物理接口的定义，为了互斥
#define CLOUD_CONTROL_PHY_CON_NULL 0
#define CLOUD_CONTROL_PHY_CON_ETHERNET 1
#define CLOUD_CONTROL_PHY_CON_USBNET 2
#define CLOUD_CONTROL_PHY_CON_WIFI 3
#define CLOUD_CONTROL_PHY_CON_3G4G 4
#define CLOUD_CONTROL_PHY_CON_INVALID 0xFF

//在CLOUD模块中定义的公共数据Buffer结构体，必须放在这儿，不然ETHERNET/USBNET/WIFI/CON3G4G/CLOUDCONT之间的包含关系混乱了
typedef struct CloudDataSendBuf
{
	UINT32 curLen;
	UINT8  curBuf[MAX_HCU_MSG_BODY_LENGTH];
}CloudDataSendBuf_t;
//在CLOUD模块中定义的公共数据Buffer结构体，必须放在这儿
typedef struct CloudControlTable
{
	UINT8 curCon;
	UINT32 ethConTry;
	UINT32 usbnetConTry;
	UINT32 wifiConTry;
	UINT32 g3g4ConTry;
}CloudControlTable_t;

#endif /* L2FRAME_L2COMDEF_H_ */
