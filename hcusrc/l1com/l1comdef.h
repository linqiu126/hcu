/*
 * l1comdef.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L1COM_L1COMDEF_H_
#define L1COM_L1COMDEF_H_

#include "../l0comvm/vmlayer.h"
#include "../l0dbi/l0dbi_inc.h"

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
	UINT32 pmTSPValue;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	UINT32 windspdValue;
	UINT32 winddirValue;
	UINT32 tempValue;
	UINT32 humidValue;
	UINT32 noiseValue;
	char hsmmpFdir[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char hsmmpFname[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char hsmmpLink[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	UINT32 equipid;
	CHAR ns;
	CHAR ew;
	UINT32 gpsx;
	UINT32 gpsy;
	UINT32 gpsz;
}HcuDiscDataSampleStorageArray_t;
#define DISC_DATA_SAMPLE_ONLINE 1
#define DISC_DATA_SAMPLE_OFFLINE 2

//自定义，服从自定义XML规范
//CLOUD<->HCU之间自行定义的命令字
typedef enum
{
	L3CI_none = 0,
	L3CI_blood_glucose = 1,  //血糖
	L3CI_single_sports = 2,  //单次运动
	L3CI_single_sleep = 3, //单次睡眠
	L3CI_body_fat = 4,  //体脂
	L3CI_blood_pressure = 5, //血压
	L3CI_runner_machine_report = 0x0A, //跑步机数据上报
	L3CI_runner_machine_control = 0x0B, //跑步机任务控制
	L3CI_gps = 0x0C, //GPS地址
	L3CI_hcu_iau_control = 0x10, //HCU与IAU之间控制命令
	L3CI_emc = 0x20, //电磁辐射强度
	L3CI_emc_accumulation = 0x21, //电磁辐射剂量
	L3CI_emc_indicator = 0x20, //电磁辐射强度
	L3CI_co = 0x22, //一氧化碳
	L3CI_formaldehyde = 0x23, //甲醛HCHO
	L3CI_alcohol = 0x24, //酒精
	L3CI_pm25 = 0x25, //PM1/2.5/10
	L3CI_windspd = 0x26, //风速Wind Speed
	L3CI_winddir = 0x27, //风向Wind Direction
	L3CI_temp = 0x28, //温度Temperature
	L3CI_humid = 0x29, //湿度Humidity
	L3CI_airprs = 0x2A, //气压Air pressure
	L3CI_noise = 0x2B, //噪声Noise
	L3CI_hsmmp = 0x2C, //相机Camer or audio high speed
	L3CI_audio = 0x2D, //声音
	L3CI_video = 0x2E, //视频
	L3CI_picture = 0x2F, //图片
	L3CI_lock = 0x30, //云控锁
	L3CI_water_meter = 0x31, //水表
	L3CI_heat_meter = 0x32, //热表
	L3CI_gas_meter = 0x33, //气表
	L3CI_power_meter = 0x34, //电表
	L3CI_light_strength = 0x35, //光照强度
	L3CI_toxicgas = 0x36, //有毒气体VOC
	L3CI_altitude = 0x37, //海拔高度
	L3CI_moto = 0x38, //马达
	L3CI_switch = 0x39, //继电器
	L3CI_transporter = 0x3A, //导轨传送带
	L3CI_bfsc_comb_scale = 0x3B, //组合秤
	L3CI_fhys_cci = 0x40,  //智能锁
	L3CI_fhys_door = 0x41, //光交箱门
	L3CI_fhys_rfid = 0x42, //光交箱RFID模块
	L3CI_fhys_ble = 0x43, //光交箱BLE模块
	L3CI_fhys_gprs = 0x44, //光交箱GPRS模块
	L3CI_fhys_battery = 0x45, //光交箱电池模块
	L3CI_fhys_vibration = 0x46, //光交箱震动
	L3CI_fhys_smoke = 0x47, //光交箱烟雾
	L3CI_fhys_water = 0x48, //光交箱水浸
	L3CI_fhys_temp = 0x49, //光交箱温度
	L3CI_fhys_humid = 0x4A, //光交箱湿度
	L3CI_itf_sps = 0x50, //串口读取命令/返回结果
	L3CI_itf_adc = 0x51, //ADC读取命令/返回结果
	L3CI_itf_dac = 0x52, //DAC读取命令/返回结果
	L3CI_itf_i2c = 0x53, //I2C读取命令/返回结果
	L3CI_itf_pwm = 0x54, //PWM读取命令/返回结果
	L3CI_itf_di = 0x55, //DI读取命令/返回结果
	L3CI_itf_do = 0x56, //DO读取命令/返回结果
	L3CI_itf_can = 0x57, //CAN读取命令/返回结果
	L3CI_itf_spi = 0x58, //SPI读取命令/返回结果
	L3CI_itf_usb = 0x59, //USB读取命令/返回结果
	L3CI_itf_eth = 0x5A, //网口读取命令/返回结果
	L3CI_itf_485 = 0x5B, //485读取命令/返回结果
	L3CI_hcu_inventory= 0xA0,	//软件清单
	L3CI_sw_package = 0xA1,	//软件版本体
	L3CI_alarm = 0xB0, //for alarm report
	L3CI_performance = 0xB1, // or PM report
	L3CI_equipment_info = 0xF0,	//设备基本信息
	L3CI_personal_info = 0xF1,	//个人基本信息
	L3CI_time_sync = 0xF2,	//时间同步
	L3CI_read_data = 0xF3,	//读取数据
	L3CI_clock_timeout = 0xF4,	//定时闹钟及久坐提醒
	L3CI_sync_charging = 0xF5,	//同步充电，双击情况
	L3CI_sync_trigger = 0xF6,	//同步通知信息
	L3CI_cmd_control = 0xFD,  //for cmd control by Shanchun
	L3CI_heart_beat = 0xFE, //心跳
	L3CI_null = 0xFF, //无效
}L3UserCmdIdDef;

//L3PO的通用定义，如果不属于任何传感器，则可以使用这个定义
typedef enum
{
	L3PO_generic_min = 0,
	L3PO_generic_none = 0,
	L3PO_generic_data_req = 0x01, //Data Request
	L3PO_generic_set_switch = 0x02,
	L3PO_generic_set_modbus_address =0x03,
	L3PO_generic_set_work_cycle = 0x04, //In second
	L3PO_generic_set_sample_cycle = 0x05, //In second
	L3PO_generic_set_sample_number = 0x06,
	L3PO_generic_data_report = 0x81, //Data Report
	L3PO_generic_read_switch = 0x82,
	L3PO_generic_read_modbus_address = 0x83,
	L3PO_generic_read_work_cycle = 0x84, //In second
	L3PO_generic_read_sample_cycle = 0x85, //In second
	L3PO_generic_read_sample_number = 0x86,
	L3PO_generic_max,
}L3GenericOptIdDef;

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

typedef enum
{
	L3PO_hcualarm_min = 0,
	L3PO_hcualarm_none = 0,
	L3PO_hcualarm_report = 0x81, //Hcu alarm report
	L3PO_hcualarm_max,
}L3HcuAlarmOptIdDef;

typedef enum
{
	L3PO_hcupm_min = 0,
	L3PO_hcupm_none = 0,
	L3PO_hcupm_report = 0x81, //Hcu perfromance report
	L3PO_hcupm_max,
}L3HcuPmOptIdDef;


typedef enum
{
	L3PO_bfsc_min = 0,
	L3PO_bfsc_none = 0,
	L3PO_bfsc_data_req = 0x01, //Data Request
	L3PO_bfsc_start_cmd = 0x02, //start
	L3PO_bfsc_stop_cmd = 0x03, //stop
	L3PO_bfsc_data_report = 0x81, //Data Report
	L3PO_bfsc_start_resp = 0x82, //start
	L3PO_bfsc_stop_resp = 0x83, //stop
	L3PO_bfsc_max,
}L3BfscOptIdDef;

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
	MODBUS_PM25_RTU_EQUIPMENT_ID_PMS = 0x42,//for PMS
	//MODBUS_WINDSPD_RTU_EQUIPMENT_ID = 0x02,
	//MODBUS_WINDDIR_RTU_EQUIPMENT_ID = 0x03,
	MODBUS_WINDSPD_RTU_EQUIPMENT_ID = 0x21,//for KUANKER sensor, don't know how to modify equid for the mement, to update later
	MODBUS_WINDDIR_RTU_EQUIPMENT_ID = 0x23,
	MODBUS_EMC_RTU_EQUIPMENT_ID = 0x05,
	MODBUS_TEMP_RTU_EQUIPMENT_ID = 0x06,
	MODBUS_HUMID_RTU_EQUIPMENT_ID = 0x06,
	MODBUS_NOISE_RTU_EQUIPMENT_ID = 0x07,
	SPSVIRGO_NOISE_RTU_EQUIPMENT_ID = 0x41,
	SPSVIRGO_NOISE_RTU_EQUIPMENT_IND = 0x64,
	SPSVIRGO_NOISE_RTU_EQUIPMENT_END = 0x03,
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
//PM25寄存器定义
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

	PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY_PMS = 0x4D,//for PMS5003T
	PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY_PMS_CMD = 0xE2,//for PMS5003T

	//Added by shanchun for control cmd
	PM25_MODBUS_GENERIC_FUNC_DATA_SET = 0x06,

	// 型号SJFC200/T独享, Added by shanchun for bug fix
	PM25_REG_DATA_PMTSP_HIGH = 0x9C,  //2B
	PM25_REG_DATA_PMTSP_LOW = 0x46,  //2B

	// 型号阿尔森4-20MA/RS485
	PM25_LENGTH_OF_REG_NEW = 0x03, //3个寄存器，返回6B

	// PMS
	PM25_LENGTH_OF_REG_NEW_PMS = 0x0E, //3个寄存器，返回6B

	PM25_REG_DATA_PMTSP_NEW_HIGH = 0x00,  //2B
	PM25_REG_DATA_PMTSP_NEW_LOW = 0x00,  //2B

	// 型号朗亿LPM1051
	PM25_LENGTH_OF_REG_NEW_LY = 0x02, //2个寄存器，返回4B


}Pm25RegisterSjfc200tDef;
//WIND DIRECTION寄存器定义
typedef enum
{
	WINDDIR_REG_DATA_READ_YIGU = 0x00,  //2B
	WINDDIR_REG_DATA_READ = 0x07,  //2B
	WINDDIR_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
	WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU = 0x03, // 易谷风向风速
	WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x04,//modify for KUANKE sensor
}WinddirRegisterSelfDef;
//WIND SPEED寄存器定义
typedef enum
{
	WINDSPD_REG_DATA_READ_YIGU = 0x00,  //2B
	WINDSPD_REG_DATA_READ = 0x06,  //2B
	WINDSPD_LENGTH_OF_REG = 0x01, //2个寄存器，返回2B
	WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU = 0x03, // 易谷风向风速
	WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY = 0x04,//modify for KUANKE sensor

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
	NOISE_LENGTH_OF_REG = 0x01, //1个寄存器，返回2B
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

/*
//for alarm report added by ZSC
typedef enum
{
	ALARM_TYPE_NONE = 0,
	ALARM_TYPE_SENSOR,
	ALARM_TYPE_NETWORK,
	ALARM_TYPE_PM25_VALUE,
	ALARM_TYPE_NOISE_VALUE,
	ALARM_TYPE_INVALID = 0xFF,
}AlarmTypeEnum;

typedef enum
{
	ALARM_CONTENT_NONE = 0,
	ALARM_CONTENT_PM25_NO_CONNECT,
	ALARM_CONTENT_TEMP_NO_CONNECT,
	ALARM_CONTENT_HUMID_NO_CONNECT,
	ALARM_CONTENT_WINDDIR_NO_CONNECT,
	ALARM_CONTENT_WINDSPD_NO_CONNECT,
	ALARM_CONTENT_NOISE_NO_CONNECT,
	ALARM_CONTENT_VIDEO_NO_CONNECT,
	ALARM_CONTENT_PM25_VALUE_EXCEED_THRESHLOD,
	ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD,
	ALARM_CONTENT_INVALID = 0xFF,
}AlarmContentEnum;

typedef enum
{
	ALARM_SEVERITY_NONE = 0,
	ALARM_SEVERITY_HIGH,
	ALARM_SEVERITY_MEDIUM,
	ALARM_SEVERITY_MINOR,
	ALARM_SEVERITY_INVALID = 0xFF,
}AlarmSeverityEnum;

typedef enum
{
	ALARM_CLEAR_FLAG_OFF = 0,
	ALARM_CLEAR_FLAG_ON,
}AlarmClearFlagEnum;
*/

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
	char curBuf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];  //内部还包括格式化的参数部分，所以需要多加100个字节的长度
}CloudDataSendBuf_t;

//跟后台的通信接口中，需要定义一些全局消息的结构体类型
enum CloudBhMsgTypeEnum
{
	CLOUDVELA_BH_MSG_TYPE_MIN = 0x00,
	CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8,
	CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8,
	CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8,
	CLOUDVELA_BH_MSG_TYPE_BIZ_ITG_UINT8,
	CLOUDVELA_BH_MSG_TYPE_ALARM_REPORT_UINT8,//for alarm report
	CLOUDVELA_BH_MSG_TYPE_PM_REPORT_UINT8,//for pm report
	CLOUDVELA_BH_MSG_TYPE_MAX,
};
#define HCU_CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING  "hcu_text"  //"hcu_text"
#define HCU_CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING  "hcu_command" //"hcu_command"
#define HCU_CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING "hcu_heart_beat"   //"hcu_heart_beat"  //心跳协议，里面的数据内容是空的
#define HCU_CLOUDVELA_BH_MSG_TYPE_BIZ_ITG_STRING "hcu_biz_itg"  //业务智能 hcu_biz_inteligence
#define HCU_CLOUDVELA_BH_MSG_TYPE_ALARM_REPORT_STRING "hcu_alarm"  //for alarm report
#define HCU_CLOUDVELA_BH_MSG_TYPE_PM_REPORT_STRING "hcu_pm"  //for pm report




//XML格式定义
//主体内容中，各个不同的字段完全按照这个优先级进行编码，解码时将由各种操作字的隐含必选关系进行解码，从而简化编码函数
#define HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH HCU_SYSDIM_FILE_NAME_LEN_MAX  //要传递HTTP名字会后台，只能扩大该域的长度
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

	//Added by Shanchun for alarm report
	char conAlarmType[3];   //1B
	char conAlarmContent[5];//2B
	char conAlarmSeverity[3];//1B
	char conAlarmClearFlag[3];//1B

	//Added by Shanchun for pm report
	char conPmCloudVelaConnCnt[5];   //2B
	char conPmCloudVelaConnFailCnt[5];   //2B
	char conPmCloudVelaDiscCnt[5];//2B
	char conPmSocketDiscCnt[5];//2B
	char conPmTaskRestartCnt[5];//2B
	char conPmCpuOccupy[5];//2B
	char conPmMemOccupy[5];//2B
	char conPmDiskOccupy[5];//2B

	//Added by Shanchun for control cmd
	char conPowerOnOff[3];
	char conInterSample[3];
	char conMeausTimes[3];
	char conNewEquId[3];
	char conWorkCycle[3];

	//Added by Shanchun for hcu sw download
	char conSwDownload[3];


	//Added by Shanchun for hcu inventory

	//char conHwUuid[12];//6B
	char conHwMac[35];//17B

	char conHwType[3];//1B
	char conHwVersion[5];//2B
	char conSwRelease[3];//1B
	char conSwDelivery[5];//2B
	char conDbDelivery[5];//2B


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
	char linkName[HCU_SYSDIM_FILE_NAME_LEN_MAX + 1]; //不定长的回传字符串，描述视频资源库HTTP地址
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


//Adding by Shanchun for socket heart beat
#define HCU_CLOUDSRV_SOCKET_KEEPALIVE 1  // set KeepAlive
#define HCU_CLOUDSRV_SOCKET_KEEPIDLE 5   //tcp idle time before first KeepAlive checking
#define HCU_CLOUDSRV_SOCKET_KEEPINTERVAL 1  //interval between two KeepAlive checking
#define HCU_CLOUDSRV_SOCKET_KEEPCOUNT 1  //count before disconnect Keepalive


//Adding by Shanchun for cmd timer flag
//#define HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_ON 1
//#define HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_OFF 2
//#define HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_ON 1
//#define HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_OFF 2
//#define HCU_CLOUDVELA_CMD_POLLING_NO_COMMAND_MAX 10

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
//#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyAMA0"
#define HCU_SPS_COM_PORT_PATH_0  "/dev/hcu_serial0"
#define HCU_SPS_COM_PORT_PATH_1  "/dev/hcu_serial1"
//#define HCU_SPS_COM_PORT_PATH_0  "/dev/ttyUSB0"
//#define HCU_SPS_COM_PORT_PATH_1  "/dev/ttyUSB1"
//#define HCU_SPS_COM_PORT_PATH_2  "/dev/ttyS0"
//#define HCU_SPS_COM_PORT_PATH_3  "/dev/ttyS1"
#define HCU_SPS_COM_PORT_PATH_4  "/dev/ttyACM0"

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


/**************************************************************************************
 *   HUITP标准链路层FRAME定义                                                           *
 *************************************************************************************/
//帧结构定义
typedef struct StrHuiFrame
{
    UINT8 	start; //0xFE
    UINT8 	checksum1; //加总头的结果
    UINT16 	len; //0-1500，原则上不能超过系统定义的消息体长度
    UINT8 	msgBody[];
}StrHuiFrame_t;


/**************************************************************************************
 *   CANITF接口的FRAME格式定义：因为CLOUD/L3BFSC/CANITFLEO都要用到这个定义                   *
 *   所以必须放在这个进行公共定义                                                          *
 *************************************************************************************/
/*
叶工，测试指令和之前你的测试平台使用一致，没有改变：

重量读取：23 00 65 02 00 00 00 00
自动0点跟踪：23 00 65 09 xx xx xx xx （值设定及含义同modbus协议）
最小灵敏度：23 00 65 0a xx xx xx xx （值设定及含义同modbus协议）
手动清零：23 00 65 0c xx xx xx xx (值设定及含义同modbus协议)
静止检测范围：23 00 65 0d xx xx xx xx （值设定及含义同modbus协议）
静止检测时间：23 00 65 0e xx xx xx xx （值设定及含义同modbus协议）
称量0校准：23 00 65 10 01 00 00 00
称量1kg校准：23 00 65 10 02 00 00 00
称量退出校准：23 00 65 10 00 00 00 00
退出校正的指令要改成：23 00 65 10 03 00 00 00
电机正转：23 00 65 20 01 00 00 00
电机反转：23 00 65 20 02 00 00 00
电机停止：23 00 65 20 03 00 00 00
电机速度：23 00 65 21 xx xx xx xx
量程：23 00 65 25 xx xx xx xx

校准顺序是，首先空称台，然后点零载，
放砝码，然后点满载。然后点退出校准保存。
32位的数据低位在前，例如，23 00 65 0d 01 00 00 00
 */

typedef struct strHcuCanitfleoCmdFrame
{
	UINT8 bfscCmdPrefixH;
	UINT8 bfscCmdPrefixL;
	UINT8 bfscCmdId;
	UINT8 bfscOptId;
	UINT8 bfscOptPar;
	UINT8 bfscPar1;
	UINT8 bfscPar2;
	UINT8 bfscPar3;
}strHcuCanitfleoCmdFrame_t;
#define HCU_CANITFLEO_CMD_PREFIXL 0x00
#define HCU_CANITFLEO_CMD_BFSC_ID 0x65
typedef enum
{
	HCU_CANITFLEO_PREFIXH_none = 0,
	HCU_CANITFLEO_PREFIXH_min = 0x22,
	HCU_CANITFLEO_PREFIXH_motor_ctrl = 0x23,
	HCU_CANITFLEO_PREFIXH_motor_resp = 0x60,
	HCU_CANITFLEO_PREFIXH_ws_ctrl = 0x40,
	HCU_CANITFLEO_PREFIXH_ws_resp = 0x43,
	HCU_CANITFLEO_PREFIXH_heart_beat = 0x7F,  //心跳
	HCU_CANITFLEO_PREFIXH_max,
	HCU_CANITFLEO_PREFIXH_invalid = 0xFF,
}HcuCanitfleoPrefixHCmdidEnmu;

//不同的控制命令
typedef enum
{
	HCU_CANITFLEO_OPTID_none = 0,
	HCU_CANITFLEO_OPTID_min = 0x01,
	HCU_CANITFLEO_OPTID_wegith_read = 0x02,
	HCU_CANITFLEO_OPTID_auto_zero_track = 0x09,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_min_sensitivity = 0x0A,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_manual_set_zero = 0x0C,  //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_static_detect_range = 0x0D, //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_static_detect_duration = 0x0E, //（值设定及含义同modbus协议）
	HCU_CANITFLEO_OPTID_weight_scale_calibration = 0x10,
	HCU_CANITFLEO_OPTID_motor_turn_around = 0x20,
	HCU_CANITFLEO_OPTID_motor_speed = 0x21,
	HCU_CANITFLEO_OPTID_scale_range = 0x25,
	HCU_CANITFLEO_OPTID_max,
	HCU_CANITFLEO_OPTID_invalid = 0xFF,
}HcuCanitfleoOptidEnmu;
typedef enum
{
	HCU_CANITFLEO_OPTPAR_none = 0,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_normal = 0x01,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_reverse = 0x02,
	HCU_CANITFLEO_OPTPAR_motor_turn_around_stop = 0x03,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_0 = 0x01,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_1kg = 0x02,
	HCU_CANITFLEO_OPTPAR_weight_scale_calibration_exit = 0x03,
	HCU_CANITFLEO_OPTPAR_invalid = 0xFF,
}HcuCanitfleoOptParEnmu;
//如果是32为量值数据，则采用little_endian_windows的风格，低位在前

/*HKVISION OPERATIONS OPTIONS*/
typedef struct HKVisionOption
{
	char url_photo[HCU_SYSDIM_FILE_NAME_LEN_MAX];		     /*url of HKvision photo server*/
	char url_video_start[HCU_SYSDIM_FILE_NAME_LEN_MAX];		 /*url of HKvision video server*/
	char url_video_stop[HCU_SYSDIM_FILE_NAME_LEN_MAX];		 /*url of HKvision video server*/
	char user_key[HCU_SYSDIM_FILE_NAME_LEN_MAX];		 /*username:password*/
	char file_photo[HCU_SYSDIM_FILE_NAME_LEN_MAX];		     /*filename include path*/
	char file_photo_pure[HCU_SYSDIM_FILE_NAME_LEN_MAX];		 /*pure filename*/
	char file_video[HCU_SYSDIM_FILE_NAME_LEN_MAX];		     /*filepath*/
}HKVisionOption_t;


#endif /* L1COM_L1COMDEF_H_ */
