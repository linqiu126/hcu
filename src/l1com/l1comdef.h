/*
 * l1comdef.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L1COM_L1COMDEF_H_
#define L1COM_L1COMDEF_H_

#include "../l0comvm/vmlayer.h"

//自定义存储数据结构
//DISC data, 只存储周期性数据
#define DISC_DATA_SAMPLE_STORAGE_NBR_MAX 20000
typedef struct HcuDiscDataSampleStorageArray
{
	UINT32 sid;
	UINT8  sensortype;  //0x20->EMC, 0x25->PM1025, L3UserCmdId
	UINT8  onOffLine;
	//UINT8  powerOnOff;  //Added by Shanchun for sensor control, may need to add more cmd later
	UINT32 timestamp;
	UINT8  dataFormat;
	UINT32 emcValue;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	UINT32 windspdValue;
	UINT32 winddirValue;
	UINT32 tempValue;
	UINT32 humidValue;
	UINT32 noiseValue;
	char hsmmpFdir[HCU_FILE_NAME_LENGTH_MAX];
	char hsmmpFname[HCU_FILE_NAME_LENGTH_MAX];
	char hsmmpLink[HCU_FILE_NAME_LENGTH_MAX];
	UINT32 equipid;
	CHAR ns;
	CHAR ew;
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
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

//自定义，服从自定义XML规范
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
	L3CI_airprs = 0x2A, //Air pressure
	L3CI_noise = 0x2B, //Noise
	L3CI_hsmmp = 0x2C, //Camer or audio high speed
	L3CI_hcu_inventory = 0xA0,	//Èí¼þÇåµ¥
	L3CI_sw_package = 0xA1,	//Èí¼þ°æ±¾Ìå
	L3CI_equipment_info = 0xF0,	//Éè±¸»ù±¾ÐÅÏ¢
	L3CI_personal_info = 0xF1,	//¸öÈË»ù±¾ÐÅÏ¢
	L3CI_time_sync = 0xF2,	//Ê±¼äÍ¬²½
	L3CI_read_data = 0xF3,	//¶ÁÈ¡Êý¾Ý
	L3CI_clock_timeout = 0xF4,	//¶¨Ê±ÄÖÖÓ¼°¾Ã×øÌáÐÑ
	L3CI_sync_charging = 0xF5,	//Í¬²½³äµç£¬Ë«»÷Çé¿ö
	L3CI_sync_trigger = 0xF6,	//Í¬²½Í¨ÖªÐÅÏ¢

	L3CI_cmd_control = 0xFD, //for cmd control by Shanchun
	L3CI_heart_beat = 0xFE,
}L3UserCmdIdDef;
//CLOUD<->HCU之间定义的操作字
//所有的操作字，需要极度的丰富化，以形成完整的处理任务模块
typedef enum
{
	L3PO_emc_min = 0,
	L3PO_emc_none = 0,
	L3PO_emc_data_req = 0x01, //Data Request
	L3PO_emc_set_switch = 0x02,
	L3PO_emc_set_modbus_address =0x03,
	L3PO_emc_set_work_cycle = 0x04, //In second
	L3PO_emc_set_sample_cycle = 0x05, //In second
	L3PO_emc_set_sample_number = 0x06,
	L3PO_emc_data_report = 0x81, //Data Report
	L3PO_emc_read_switch = 0x82,
	L3PO_emc_read_modbus_address = 0x83,
	L3PO_emc_read_work_cycle = 0x84, //In second
	L3PO_emc_read_sample_cycle = 0x85, //In second
	L3PO_emc_read_sample_number = 0x86,
	L3PO_emc_max,
}L3EmcOptIdDef;
typedef enum
{
	L3PO_pm25_min = 0,
	L3PO_pm25_none = 0,
	L3PO_pm25_data_req = 0x01, //PM3 Request
	L3PO_pm25_set_switch = 0x02,
	L3PO_pm25_set_modbus_address =0x03,
	L3PO_pm25_set_work_cycle = 0x04, //In second
	L3PO_pm25_set_sample_cycle = 0x05, //In second
	L3PO_pm25_set_sample_number = 0x06,

	L3PO_pm25_data_report = 0x81,//PM3 Response
	L3PO_pm25_set_switch_ack = 0x82,
	L3PO_pm25_set_modbus_address_ack =0x83,
	L3PO_pm25_set_work_cycle_ack = 0x84, //In second
	L3PO_pm25_set_sample_cycle_ack = 0x85, //In second
	L3PO_pm25_set_sample_number_ack = 0x86,

	L3PO_pm25_read_switch = 0x07,
	L3PO_pm25_read_modbus_address = 0x08,
	L3PO_pm25_read_work_cycle = 0x09, //In second
	L3PO_pm25_read_sample_cycle = 0x0A, //In second
	L3PO_pm25_read_sample_number = 0x0B,

	L3PO_pm25_read_switch_ack = 0x87,
	L3PO_pm25_read_modbus_address_ack = 0x88,
	L3PO_pm25_read_work_cycle_ack = 0x89, //In second
	L3PO_pm25_read_sample_cycle_ack = 0x8A, //In second
	L3PO_pm25_read_sample_number_ack = 0x8B,

	L3PO_pm25_max,
}L3Pm25OptIdDef;
typedef enum
{
	L3PO_winddir_min = 0,
	L3PO_winddir_none = 0,
	L3PO_winddir_data_req = 0x01, //Data Request
	L3PO_winddir_set_switch = 0x02,
	L3PO_winddir_set_modbus_address =0x03,
	L3PO_winddir_set_work_cycle = 0x04, //In second
	L3PO_winddir_set_sample_cycle = 0x05, //In second
	L3PO_winddir_set_sample_number = 0x06,
	L3PO_winddir_data_report = 0x81, //Data Report
	L3PO_winddir_read_switch = 0x82,
	L3PO_winddir_read_modbus_address = 0x83,
	L3PO_winddir_read_work_cycle = 0x84, //In second
	L3PO_winddir_read_sample_cycle = 0x85, //In second
	L3PO_winddir_read_sample_number = 0x86,
	L3PO_winddir_max,
}L3WinddirOptId;

typedef enum
{
	L3PO_windspd_min = 0,
	L3PO_windspd_none = 0,
	L3PO_windspd_data_req = 0x01, //Data Request
	L3PO_windspd_set_switch = 0x02,
	L3PO_windspd_set_modbus_address =0x03,
	L3PO_windspd_set_work_cycle = 0x04, //In second
	L3PO_windspd_set_sample_cycle = 0x05, //In second
	L3PO_windspd_set_sample_number = 0x06,
	L3PO_windspd_data_report = 0x81, //Data Report
	L3PO_windspd_read_switch = 0x82,
	L3PO_windspd_read_modbus_address = 0x83,
	L3PO_windspd_read_work_cycle = 0x84, //In second
	L3PO_windspd_read_sample_cycle = 0x85, //In second
	L3PO_windspd_read_sample_number = 0x86,
	L3PO_windspd_max,
}L3WindspdOptIdDef;
typedef enum
{
	L3PO_temp_min = 0,
	L3PO_temp_none = 0,
	L3PO_temp_data_req = 0x01, //Data Request
	L3PO_temp_set_switch = 0x02,
	L3PO_temp_set_modbus_address =0x03,
	L3PO_temp_set_work_cycle = 0x04, //In second
	L3PO_temp_set_sample_cycle = 0x05, //In second
	L3PO_temp_set_sample_number = 0x06,
	L3PO_temp_data_report = 0x81, //Data Report
	L3PO_temp_read_switch = 0x82,
	L3PO_temp_read_modbus_address = 0x83,
	L3PO_temp_read_work_cycle = 0x84, //In second
	L3PO_temp_read_sample_cycle = 0x85, //In second
	L3PO_temp_read_sample_number = 0x86,
	L3PO_temp_max,
}L3TempOptIdDef;
typedef enum
{
	L3PO_humid_min = 0,
	L3PO_humid_none = 0,
	L3PO_humid_data_req = 0x01, //Data Request
	L3PO_humid_set_switch = 0x02,
	L3PO_humid_set_modbus_address =0x03,
	L3PO_humid_set_work_cycle = 0x04, //In second
	L3PO_humid_set_sample_cycle = 0x05, //In second
	L3PO_humid_set_sample_number = 0x06,
	L3PO_humid_data_report = 0x81, //Data Report
	L3PO_humid_read_switch = 0x82,
	L3PO_humid_read_modbus_address = 0x83,
	L3PO_humid_read_work_cycle = 0x84, //In second
	L3PO_humid_read_sample_cycle = 0x85, //In second
	L3PO_humid_read_sample_number = 0x86,
	L3PO_humid_max,
}L3HumidOptIdDef;
typedef enum
{
	L3PO_hsmmp_min = 0,
	L3PO_hsmmp_none = 0,
	L3PO_hsmmp_data_req = 0x01, //Data Request
	L3PO_hsmmp_upload_req = 0x02,
	L3PO_hsmmp_set_modbus_address =0x03,
	L3PO_hsmmp_set_work_cycle = 0x04, //In second
	L3PO_hsmmp_set_sample_cycle = 0x05, //In second
	L3PO_hsmmp_set_sample_number = 0x06,
	L3PO_hsmmp_data_report = 0x81, //Data Report
	L3PO_hsmmp_upload_report = 0x82,
	L3PO_hsmmp_read_modbus_address = 0x83,
	L3PO_hsmmp_read_work_cycle = 0x84, //In second
	L3PO_hsmmp_read_sample_cycle = 0x85, //In second
	L3PO_hsmmp_read_sample_number = 0x86,
	L3PO_hsmmp_max,
}L3HsmmpOptIdDef;
typedef enum
{
	L3PO_noise_min = 0,
	L3PO_noise_none = 0,
	L3PO_noise_data_req = 0x01, //Data Request
	L3PO_noise_set_switch = 0x02,
	L3PO_noise_set_modbus_address =0x03,
	L3PO_noise_set_work_cycle = 0x04, //In second
	L3PO_noise_set_sample_cycle = 0x05, //In second
	L3PO_noise_set_sample_number = 0x06,
	L3PO_noise_data_report = 0x81, //Data Report
	L3PO_noise_read_switch = 0x82,
	L3PO_noise_read_modbus_address = 0x83,
	L3PO_noise_read_work_cycle = 0x84, //In second
	L3PO_noise_read_sample_cycle = 0x85, //In second
	L3PO_noise_read_sample_number = 0x86,
	L3PO_noise_max,
}L3NoiseOptIdDef;

typedef enum
{
	L3PO_swdownload_min = 0,
	L3PO_swdownload_none = 0,
	L3PO_swdownload_req = 0x01, //Sw download Request
	L3PO_swdownload_report = 0x81, //Sw download Response
	L3PO_swdownload_max,
}L3SwDownloadOptIdDef;

typedef enum
{
	L3PO_hcuinventory_min = 0,
	L3PO_hcuinventory_none = 0,
	L3PO_hcuinventory_req = 0x01, //Hcu Inventory Request
	L3PO_hcuinventory_report = 0x81, //Hcu Inventory Response
	L3PO_hcuinventory_max,
}L3HcuInventoryOptIdDef;


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
	MODBUS_NOISE_RTU_EQUIPMENT_ID = 0x0A,  //暂时改不了，所以就先改为缺省的0A，未来再改动
	SPSVIRGO_NOISE_RTU_EQUIPMENT_ID = 0x81,
	MODBUS_INVALID_RTU_EQUIPMENT_ID = 0xFF,
}ModbusSensorEquipmentIdDef;


//Added by Shanchun for sensor type
typedef enum
{
	MODBUS_NONE_RTU_EQUIPMENT_TYPE = 0,
	MODBUS_PM25_RTU_EQUIPMENT_TYPE = 0x01,
	MODBUS_WINDSPD_RTU_EQUIPMENT_TYPE = 0x02,
	MODBUS_WINDDIR_RTU_EQUIPMENT_TYPE = 0x03,
	MODBUS_EMC_RTU_EQUIPMENT_TYPE = 0x04,
	MODBUS_TEMP_RTU_EQUIPMENT_TYPE = 0x05,
	MODBUS_HUMID_RTU_EQUIPMENT_TYPE = 0x06,
	MODBUS_NOISE_RTU_EQUIPMENT_TYPE = 0x07,  //暂时改不了，所以就先改为缺省的0A，未来再改动
	MODBUS_INVALID_RTU_EQUIPMENT_TYPE = 0xFF,
}ModbusSensorEquipmentTypeDef;

//功能码定义
//#define MODBUS_GENERIC_FUNC_DATA_INQUERY  0x03
//EMC寄存器定义 => 假设的，还不存在
typedef enum
{
	EMC_REG_DATA_READ = 4,  //2B, just for example
	EMC_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
	EMC_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
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
	PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
	//Added by shanchun for control cmd
	PM25_MODBUS_GENERIC_FUNC_DATA_SET = 0x06,
}Pm25RegisterSjfc200tDef;
//WIND DIRECTION寄存器定义
typedef enum
{
	WINDDIR_REG_DATA_READ = 0,  //2B
	WINDDIR_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
	WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
}WinddirRegisterSelfDef;
//WIND SPEED寄存器定义
typedef enum
{
	WINDSPD_REG_DATA_READ = 0,  //2B
	WINDSPD_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
	WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
}WindSpdRegisterSelfDef;
//TEMP寄存器定义
typedef enum
{
	TEMP_REG_DATA_READ = 0,  //2B
	TEMP_LENGTH_OF_REG = 0x02, //2个寄存器，返回4B
	TEMP_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
}TempRegisterSelfDef;
//HUMIDITY寄存器定义
typedef enum
{
	HUMID_REG_DATA_READ = 0,  //2B
	HUMID_LENGTH_OF_REG = 0x02, //2个寄存器，返回4B
	HUMID_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x03,
}HumidRegisterSelfDef;
//NOISE寄存器定义
typedef enum
{
	NOISE_REG_DATA_READ = 0,  //2B
	NOISE_LENGTH_OF_REG = 0x02, //2个寄存器，返回4B
	NOISE_CRC16_PRESENT = FALSE, //CRC16不存在，取值TRUE/FALSE
	NOISE_FUNCTION_CODE = 0x57,
	NOISE_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x04,
}NoiseRegisterSelfDef;

//HSMMP寄存器定义
typedef enum
{
	HSMMP_REG_DATA_READ = 0,  //2B
	HSMMP_EQUIPMENT_ID_DEFAULT = 0xDD, //暂时只是为了方便整个系统的存储和转述
}HsmmpRegisterSelfDef;

//统一定义汇报给后台的数据格式
typedef enum
{
	CLOUD_SENSOR_DATA_FORMAT_NULL = 0,
	CLOUD_SENSOR_DATA_FOMAT_INT_ONLY = 1,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1 = 2,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2 = 3,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3 = 4,
	CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF4 = 5,
	CLOUD_SENSOR_DATA_FOMAT_MAX = 0x10,
	CLOUD_SENSOR_DATA_FOMAT_INVALID = 0xFF,
}CloudDataFormatDef;


//等待随机长度的时长，一分钟/60秒之类，然后再开始干活，以便减少所有传感器相互碰撞的几率，让所有任务分布更加平均
#define TIMER_DURATION_REDUCE_COLLAPTION_IN_1_MINUTES 60  //shall be 60second

//为了表征Sensor/Audio/Avorion文件的类型，这里定义不同的操作类型
typedef enum
{
	FILE_OPERATION_TYPE_NONE = 0,
	FILE_OPERATION_TYPE_SENSOR,
	FILE_OPERATION_TYPE_SPSVIRGO,
	FILE_OPERATION_TYPE_AVORION_AVI,
	FILE_OPERATION_TYPE_AVORION_H264,
	FILE_OPERATION_TYPE_AVORION_MKV,
	FILE_OPERATION_TYPE_MAX,
	FILE_OPERATION_TYPE_INVALID = 0xFF,
}FileOperationTypeEnum;

//公共定义的设备缺省标识，暂时以335D设备为模版
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
#define HCU_DEFAULT_DEVICE_I2C  "/dev/i2c"
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
#define HCU_CLOUDVELA_CONTROL_PHY_CON_NULL 0
#define HCU_CLOUDVELA_CONTROL_PHY_CON_ETHERNET 1
#define HCU_CLOUDVELA_CONTROL_PHY_CON_USBNET 2
#define HCU_CLOUDVELA_CONTROL_PHY_CON_WIFI 3
#define HCU_CLOUDVELA_CONTROL_PHY_CON_3G4G 4
#define HCU_CLOUDVELA_CONTROL_PHY_CON_INVALID 0xFF

//在CLOUD模块中定义的公共数据Buffer结构体，必须放在这儿，不然ETHERNET/USBNET/WIFI/CON3G4G/CLOUDCONT之间的包含关系混乱了
//这里的数据，已经是编码后的数据长度，故而必须考虑到字符串稀释后的长度
typedef struct CloudDataSendBuf
{
	UINT32 curLen;
	//char的意义是，底层均为字符串，而不是十六进制数据
	char curBuf[MAX_HCU_MSG_BUF_LENGTH];  //内部还包括格式化的参数部分，所以需要多加100个字节的长度
}CloudDataSendBuf_t;
//在CLOUD模块中定义的公共数据Buffer结构体，必须放在这儿
typedef struct CloudvelaTable
{
	UINT8 curCon;  //表示当前是哪一种物理接口在工作
	UINT32 ethConTry;  //用于断链的处理，确保高优先级物理链路可以处于优势地位，节省用户的链路成本消耗
	UINT32 usbnetConTry; //下同
	UINT32 wifiConTry;
	UINT32 g3g4ConTry;
}CloudvelaTable_t;

//跟后台的通信接口中，需要定义一些全局消息的结构体类型
enum CloudBhMsgTypeEnum
{
	CLOUDVELA_BH_MSG_TYPE_MIN = 0x00,
	CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8,
	CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8,
	CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8,
	CLOUDVELA_BH_MSG_TYPE_BIZ_ITG_UINT8,
	CLOUDVELA_BH_MSG_TYPE_MAX,
};
#define HCU_CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING  "hcu_text"  //"hcu_text"
#define HCU_CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING  "hcu_command" //"hcu_command"
#define HCU_CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING "hcu_heart_beat"   //"hcu_heart_beat"  //心跳协议，里面的数据内容是空的
#define HCU_CLOUDVELA_BH_MSG_TYPE_BIZ_ITG_STRING "hcu_biz_itg"  //业务智能 hcu_biz_inteligence


//XML格式定义
//主体内容中，各个不同的字段完全按照这个优先级进行编码，解码时将由各种操作字的隐含必选关系进行解码，从而简化编码函数
#define HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH HCU_FILE_NAME_LENGTH_MAX  //要传递HTTP名字会后台，只能扩大该域的长度
typedef struct CloudBhItfDevReportStdXml
{
	char xml_l[6];
	char ToUserName_l[22];
	char ToUserName[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char ToUserName_r[17];
	char FromUserName_l[24];
	char FromUserName[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char FromUserName_r[19];
	char CreateTime_l[13];
	char CreateTime[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char CreateTime_r[14];
	char MsgType_l[19];
	char MsgType[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char MsgType_r[14];
	char Content_l[19];
	char conCmdId[3];
	char conLen[3];  //1B
	char conOptId[3]; //1B
	char conBackType[3]; //1B
	char conEqpId[3];  //1B
	char conDataFormat[3]; //1B
	char conEmc[5];   //2B
	char conPm1d0[9];   //4B
	char conPm2d5[9];   //4B
	char conPm10d[9];   //4B
	char conWinddir[5];   //2B
	char conWindspd[5];   //2B
	char conTemp[5];   //2B
	char conHumid[5];   //2B
	char conNoise[9];   //4B
	char conEW[3];//1B
	char conGpsx[9];   //4B
	char conNS[3];//1B
	char conGpsy[9];   //4B
	char conGpsz[9];   //4B
	//Added by Shanchun for control cmd
	char conPowerOnOff[3];
	char conInterSample[3];
	char conMeausTimes[3];
	char conNewEquId[3];
	char conWorkCycle[3];

	//Added by Shanchun for hcu sw download
	char conSwDownload[3];


	//Added by Shanchun for hcu inventory

	char conHwUuid[12];//6B

	char conHwType[3];//1B
	char conHwVersion[5];//2B
	char conSwDelivery[5];//1B
	char conSwRelease[5];//2B

	//Added by Shanchun for av upload
	char conAvUpload[3];
	//char conAvFileName[HCU_FILE_NAME_LENGTH_MAX];

	char conTimeStamp[9]; //4B
	char conNtimes[5];   //2B
	char Content_r[14];
	char FuncFlag_l[11];
	char FuncFlag[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH+1];
	char FuncFlag_r[12];
	char xml_r[7];
}CloudBhItfDevReportStdXml_t;
/*
<xml>
<ToUserName><![CDATA[
%s
]]></ToUserName>
<FromUserName><![CDATA[
%s
]]></FromUserName>
<CreateTime>
%s
</CreateTime>
<MsgType><![CDATA[
%s
]]></MsgType>
<Content><![CDATA[
%s
]]></Content>
<FuncFlag>
0
</FuncFlag>
</xml>
*/

/*
 *   B.0.4 .4 .4 数据 段结构定义应符合下列要求 段结构定义应符合下列要求 段结构定义应符合下列要求 段结构定义应符合下列要求 段结构定义应符合下列要求
 *   段结构定义应符合下列要求 段结构定义应符合下列要求 ：
 *  	1 字段 与其值应用“=”连接。
 *   	2 在数 据区中，同一项 目的 不同 分类值间应用“,”来分 隔，不同项目 之间应用“;”来 分隔。
 *   B.0.5 字段定义 字段定义 字段定义 应符合下列 应符合下列 应符合下列 应符合下列 要求： 要求：
 *   	1 字段 名应区分大小写，单词的 首个字符应 为大写，其他部 分应 为小写 。
 *   	2 数据类型 数据类型 数据类型 应符合 应符合 表 B.0.5 .0.5.1 要求
 */

//中环保协议标准的格式定义
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_DATA_SEGMENT_MAX_LENGTH 30
//#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_DATA_TOTAL_MAX_LENGTH 1025
typedef struct CloudBhItfDevReportStdZhbData
{
	char qn[21]; //请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	char st[6]; //ST=系统编号
	char cn[8]; //CN=命令编号
	char mn[13]; //MN=设备编号
	char pw[7]; //PW=访问密码
	char pnum[5]; //HCU->SAE only, PNUM 指示本次通讯总共包含的包数
	char pno[5]; //HCU->SAE only, PNO 指示当前数据包的包号
	//char flag[4]; //SAE->HCU only UBit0：数据是否应答；Bit：1-应答，0-不应答 Bit1：是否有数据序号；Bit：1-数据包中包含包序号和总包号两部分，0- 数据包中不包含包序号和总包号两部分 如：Flag=3 表示拆分包并且需要应答（Flag 可扩展）
	//char cp[]; //SAE->HCU only, CP=&&数据区&&
	char conEmc[5];   //2B HCU->SAE only
	char conPm1d0[9];   //4B HCU->SAE only
	char conPm2d5[9];   //4B HCU->SAE only
	char conPm10d[9];   //4B HCU->SAE only
	char conWinddir[5];   //2B HCU->SAE only
	char conWindspd[5];   //2B HCU->SAE only
	char conTemp[5];   //2B HCU->SAE only
	char conHumid[5];   //2B HCU->SAE only
	char linkLen[5];    //2B
	char linkName[HCU_FILE_NAME_LENGTH_MAX + 1]; //不定长的回传字符串，描述视频资源库HTTP地址
	char conNoise[9];   //4B HCU->SAE only
	char conAirprs[5];   //2B HCU->SAE only
}CloudBhItfDevReportStdZhbData_t;
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_CN_FIX_LENGTH 7
typedef struct CloudBhItfDevReportStdZhb
{
	char zhb_header[3]; //1B 固定为##
	char dataLen[5]; //4, 十进制整数, 数据段的 ASCII 字符数 例如：长 255，则写为“0255”
	CloudBhItfDevReportStdZhbData_t data; //0≤n≤1024 变长的数据
	char crc16[5]; //2B, 数据段的校验结果
	char zhb_tail[3]; //1B, 固定为<CR><LF>
}CloudBhItfDevReportStdZhb_t;
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_EMC "a01000"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_TEMP "a01001"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_HUMID "a01002"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_AIRPRS "a01006"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_WINDSPD "a01007"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_WINDDIR "a01008"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_PM25 "a34001"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_NOISE "a50001"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_HSMMP "a60001"

//定义ZHB中环保协议中的系统命令，以便区分心跳/普通包，该定义的详细阐述留待以后再明确
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_CN_HEART_BEAT "ZHB_HRB"
#define HCU_CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG "ZHB_NOM"


//L2 and L3 module used GPS information, defined structure here
typedef struct hcuStrDataTime
{
	UINT32 hour;
	UINT32 minute;
	UINT32 second;
	UINT32 day;
	UINT32 month;
	UINT32 year;
}hcuStrDateTime_t;

typedef struct GpsPosInfo
{
	hcuStrDateTime_t D; //UTC日期时间
	CHAR status;
	UINT32 gpsX; //纬度
	CHAR EW;
	UINT32 gpsY; //经度
	CHAR NS;
	UINT32 gpsZ;
	UINT32 speed; //速度
	UINT32 resistence; //反映多少个GPS模块扫描周期一直没有数值的情况
}GpsPosInfo_t;


typedef struct  HcuInventoryInfo
{
	UINT8 hw_uuid[6];
	UINT8 hw_type;
	UINT16 hw_version;
	UINT8 sw_release;
	UINT16 sw_delivery;
}HcuInventoryInfot;

//Adding by Shanchun for socket handling
#define HCU_CLOUDSRV_BH_PORT 9501
#define HCU_CLOUDSRV_BH_ADDRESS "121.40.185.177"
#define HCU_CLOUDSRV_BH_QUEUE 30

//Adding by Shanchun for cmd timer flag
#define HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_ON 1
#define HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_OFF 2
#define HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_ON 1
#define HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_OFF 2
#define HCU_CLOUDVELA_CMD_POLLING_NO_COMMAND_MAX 10

//定义本地I2C设备
#define HCU_RPI_DEV_I2C_ADDRESS "/dev/i2c-1"

//传感器合理范围
#define HCU_SENSOR_VALUE_NULL 0x0FFFFFFF
#define HCU_SENSOR_TEMP_VALUE_MIN -40 //in degree
#define HCU_SENSOR_TEMP_VALUE_MAX 125
#define HCU_SENSOR_HUMID_VALUE_MIN -0 //in %
#define HCU_SENSOR_HUMID_VALUE_MAX 100
#define HCU_SENSOR_AIRPRS_VALUE_MIN 0 //in pa
#define HCU_SENSOR_AIRPRS_VALUE_MAX 120000 //海平面大气压为103125pa，故而超过120000不太可能
#define HCU_SENSOR_LIGHTSTR_VALUE_MIN 0 //in lx
#define HCU_SENSOR_LIGHTSTR_VALUE_MAX 100000
#define HCU_SENSOR_ALTITUDE_VALUE_MIN -10000 //in meter, 海拔高度，由气压推测计算出来的
#define HCU_SENSOR_ALTITUDE_VALUE_MAX 100000
#define HCU_SENSOR_CO1_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_CO1_VALUE_MAX 10000
#define HCU_SENSOR_EMC_VALUE_MIN 0  //in mv/cm2
#define HCU_SENSOR_EMC_VALUE_MAX 10000
#define HCU_SENSOR_HCHO_VALUE_MIN 0  //in ppb，跟ug/m3之间的换算，待定
#define HCU_SENSOR_HCHO_VALUE_MAX 10000
#define HCU_SENSOR_ALCOHOL_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_ALCOHOL_VALUE_MAX 10000
#define HCU_SENSOR_NOISE_VALUE_MIN 0  //in 分贝
#define HCU_SENSOR_NOISE_VALUE_MAX 10000
#define HCU_SENSOR_PM1D0_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_PM1D0_VALUE_MAX 10000
#define HCU_SENSOR_PM25_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_PM25_VALUE_MAX 10000
#define HCU_SENSOR_PM10_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_PM10_VALUE_MAX 10000
#define HCU_SENSOR_TOXICGAS_VALUE_MIN 0  //in mg/L
#define HCU_SENSOR_TOXICGAS_VALUE_MAX 10000
#define HCU_SENSOR_WINDDIR_VALUE_MIN 0  //in degree
#define HCU_SENSOR_WINDDIR_VALUE_MAX 360
#define HCU_SENSOR_WINDSPD_VALUE_MIN 0  //in m/s
#define HCU_SENSOR_WINDSPD_VALUE_MAX 100
#define HCU_SENSOR_GPSX_VALUE_MIN 0  //in m
#define HCU_SENSOR_GPSX_VALUE_MAX 1000000
#define HCU_SENSOR_GPSY_VALUE_MIN 0  //in m
#define HCU_SENSOR_GPSY_VALUE_MAX 1000000
#define HCU_SENSOR_GPSZ_VALUE_MIN 0  //in m
#define HCU_SENSOR_GPSZ_VALUE_MAX 30000

#define HCU_CLOUDVELA_PATH_MAX 1024  //for readlink by shanchun
#define HCU_CLOUDVELA_HSMMP_PATH_MAX 254  //for readlink by shanchun


//串口设备地址相关的公共定义，跟配置以及使用的硬件环境非常相关
#ifdef TARGET_LINUX_ARM
//#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttySAC0"
//#define HCU_SPS_COM_PORT_PATH_1  "/dev/ttySAC1"
//#define HCU_SPS_COM_PORT_PATH_2  "/dev/ttySAC2"
//#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttySAC3"
//#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttySAC4"
#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyAMA0"
#define HCU_SPS_COM_PORT_PATH_1  "/dev/ttyO1"
#define HCU_SPS_COM_PORT_PATH_2  "/dev/ttyO2"
#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyO3"
#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttyO4"
#endif

#ifdef TARGET_LINUX_X86
#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyUSB0"
#define HCU_SPS_COM_PORT_PATH_1  "/dev/ttyUSB1"
#define HCU_SPS_COM_PORT_PATH_2  "/dev/ttyUSB2"
#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyS0"
#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttyS1"
//#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyUSB3"
//#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttyUSB4"
#endif

#ifdef TARGET_RASPBERRY_PI3B
#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyAMA0"
//#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyUSB0"
#define HCU_SPS_COM_PORT_PATH_1  "/dev/ttyUSB1"
#define HCU_SPS_COM_PORT_PATH_2  "/dev/ttyUSB2"
#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyS0"
#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttyS1"
//#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyUSB3"
//#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyUSB4"
#endif

/**************************************************************************************
 *                            NBIOT IPM376消息定义                                    *
 *************************************************************************************/
//AFN消息字段
#define  HCU_NBIOT_IPM376_AFN_CMDID_CNFNG 0x00
#define  HCU_NBIOT_IPM376_AFN_CMDID_RESET 0x01
#define  HCU_NBIOT_IPM376_AFN_CMDID_LICK 0x02  //Link Interface Check
#define  HCU_NBIOT_IPM376_AFN_CMDID_RELAY 0x03
#define  HCU_NBIOT_IPM376_AFN_CMDID_SETPAR 0x04
#define  HCU_NBIOT_IPM376_AFN_CMDID_CONTROL 0x05
#define  HCU_NBIOT_IPM376_AFN_CMDID_SECNEG 0x06
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQREP 0x08
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQCFG 0x09
#define  HCU_NBIOT_IPM376_AFN_CMDID_INQPAR 0x0A
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQTSK 0x0B
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQDATA1 0x0C
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQDATA2 0x0D
#define  HCU_NBIOT_IPM376_AFN_CMDID_REQDATA3 0x0E
#define  HCU_NBIOT_IPM376_AFN_CMDID_FILETRNS 0x0F
#define  HCU_NBIOT_IPM376_AFN_CMDID_DATAFWD 0x10

//消息头的全局定义
#define  HCU_NBIOT_IPM376_FRAME_FIX_HEAD 0x68
#define  HCU_NBIOT_IPM376_FRAME_FIX_START 0x68
#define  HCU_NBIOT_IPM376_FRAME_FIX_TAIL 0x16
#define  HCU_NBIOT_IPM376_FRAME_MAX_LEN 255 //16383 under fix network transmission


/**************************************************************************************
 *                            NBIOT CJ188消息固定结构定义                                *
 *************************************************************************************/
//T代码标识不同的仪表
#define  HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN 0x10
#define  HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX 0x19
#define  HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN 0x20
#define  HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX 0x29
#define  HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MIN 0x30
#define  HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MAX 0x39
#define  HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MIN 0x40
#define  HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MAX 0x49

#define  HCU_NBIOT_CJ188_T_TYPE_COLD_WATER_METER 0x10
#define  HCU_NBIOT_CJ188_T_TYPE_HOT_WATER_METER 0x11
#define  HCU_NBIOT_CJ188_T_TYPE_DRINK_WATER_METER 0x12
#define  HCU_NBIOT_CJ188_T_TYPE_MIDDLE_WATER_METER 0x13
#define  HCU_NBIOT_CJ188_T_TYPE_HEAT_ENERGY_METER 0x20
#define  HCU_NBIOT_CJ188_T_TYPE_COLD_ENERGY_METER 0x21
#define  HCU_NBIOT_CJ188_T_TYPE_GAS_METER 0x30
#define  HCU_NBIOT_CJ188_T_TYPE_ELECTRONIC_POWER_METER 0x40

//Control码子代表的不同含义
#define  HCU_NBIOT_CJ188_CTRL_MIN 0x0
#define  HCU_NBIOT_CJ188_CTRL_RESERVED 0x0
#define  HCU_NBIOT_CJ188_CTRL_READ_DATA 0x01
#define  HCU_NBIOT_CJ188_CTRL_WRITE_DATA 0x04
#define  HCU_NBIOT_CJ188_CTRL_READ_KEY_VER 0x09
#define  HCU_NBIOT_CJ188_CTRL_READ_ADDR 0x03
#define  HCU_NBIOT_CJ188_CTRL_WRITE_ADDR 0x05
#define  HCU_NBIOT_CJ188_CTRL_WRITE_DEVICE_SYN 0x16
#define  HCU_NBIOT_CJ188_CTRL_MAX 0x16

//应用层DI0DI1的定义
#define  HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA 0x901F
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA1 0xD120
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA2 0xD121
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA3 0xD122
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA4 0xD123
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA5 0xD124
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA6 0xD125
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA7 0xD126
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA8 0xD127
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA9 0xD128
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA10 0xD129
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA11 0xD12A
#define  HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA12 0xD12B
#define  HCU_NBIOT_CJ188_READ_DI0DI1_PRICE_TABLE 0x8102
#define  HCU_NBIOT_CJ188_READ_DI0DI1_BILL_TODAY_DATE 0x8103  //结算日
#define  HCU_NBIOT_CJ188_READ_DI0DI1_READ_ACCOUNT_CUR_DATE 0x8104 //抄表日
#define  HCU_NBIOT_CJ188_READ_DI0DI1_BUY_AMOUNT 0x8105 //购入金额
#define  HCU_NBIOT_CJ188_READ_DI0DI1_KEY_VER 0x8106
#define  HCU_NBIOT_CJ188_READ_DI0DI1_ADDRESS 0x810A
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE 0xA010
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE 0xA011    //结算日
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE 0xA012   //抄表日
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT 0xA013
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY 0xA014
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME 0xA015
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL 0xA017
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START 0xA019
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS 0xA018
#define  HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA 0xA016

//单位代号表
#define  HCU_NBIOT_CJ188_UNIT_WH 0x02
#define  HCU_NBIOT_CJ188_UNIT_KWH 0x05
#define  HCU_NBIOT_CJ188_UNIT_MWH 0x08
#define  HCU_NBIOT_CJ188_UNIT_100MWH 0x0A
#define  HCU_NBIOT_CJ188_UNIT_J 0x01
#define  HCU_NBIOT_CJ188_UNIT_KJ 0x0B
#define  HCU_NBIOT_CJ188_UNIT_MJ 0x0E
#define  HCU_NBIOT_CJ188_UNIT_GJ 0x11
#define  HCU_NBIOT_CJ188_UNIT_100GJ 0x13
#define  HCU_NBIOT_CJ188_UNIT_W 0x14
#define  HCU_NBIOT_CJ188_UNIT_KW 0x17
#define  HCU_NBIOT_CJ188_UNIT_MW 0x1A
#define  HCU_NBIOT_CJ188_UNIT_L 0x29
#define  HCU_NBIOT_CJ188_UNIT_M3 0x2C
#define  HCU_NBIOT_CJ188_UNIT_L_H 0x32
#define  HCU_NBIOT_CJ188_UNIT_M3_H 0x35


//消息头的全局定义
#define  HCU_NBIOT_CJ188_FRAME_FIX_HEAD 0x68
#define  HCU_NBIOT_CJ188_FRAME_FIX_TAIL 0x16
#define  HCU_NBIOT_CJ188_FRAME_READ_MAX_LEN 77 //最少64+13的固定长度
#define  HCU_NBIOT_CJ188_FRAME_WRITE_MAX_LEN 45 //最少32+13的固定长度

//为了进行编解码，生成该结构
typedef struct  NbiotCj188BhItfComElement //
{
	INT8 equtype;
	sensor_general_cj188_control_head_t head;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	float heatpower;
	INT8 heatpowerunit;
	float currentheat;
	INT8 currentheatunit;
	float billtodayheat;
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t data;
	UINT32 length;
}NbiotCj188BhItfComElement_t;
typedef struct  NbiotCj188BhItfComCtrFlag //
{
	UINT8 d0d1Flag;
	UINT8 billtodayaccuvolumeFlag;
	UINT8 heatpowerFlag;
	UINT8 currentheatFlag;
	UINT8 billtodayheatFlag;
	UINT8 currentaccuvolumeFlag;
	UINT8 flowvolumeFlag;
	UINT8 lastmonthFlag;
	UINT8 accumuworktimeFlag;
	UINT8 supplywatertempFlag;
	UINT8 backwatertempFlag;
	UINT8 realtimeFlag;
	UINT8 stFlag;
	UINT8 billtodaydateFlag;
	UINT8 readamountcurdateFlag;
	UINT8 startdateFlag;
	UINT8 keyFlag;
	UINT8 price1Flag;
	UINT8 volume1Flag;
	UINT8 price2Flag;
	UINT8 volume2Flag;
	UINT8 price3Flag;
	UINT8 buycodeFlag;
	UINT8 thisamountFlag;
	UINT8 accuamountFlag;
	UINT8 remainamountFlag;
	UINT8 keyverFlag;
	UINT8 switchctrlFlag;
}NbiotCj188BhItfComCtrFlag_t;

#define HCU_NBIOT_CJ188_COMM_CHANNEL_NORMAL 1
#define HCU_NBIOT_CJ188_COMM_CHANNEL_ABNORMAL 2
#define HCU_NBIOT_CJ188_SWITCH_CONTROL_ON 55
#define HCU_NBIOT_CJ188_SWITCH_CONTROL_OFF 99

#endif /* L1COM_L1COMDEF_H_ */
