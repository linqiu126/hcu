/*
 * huitp.h
 *
 *  Created on: 2016年12月25日
 *      Author: test
 */

#ifndef _HUITP_H_
#define _HUITP_H_
#pragma pack (1) //强制1字节对齐

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//考虑到TCPIP中底层最长1500B的情形，这里先如此定义，不要超过这个限制，所以16进制的缓冲区最好不要超过500字节
#define HUITP_MSG_BUF_WITH_HEAD_MAX_LEN 134
#define HUITP_MSG_BUF_BODY_ONLY_MAX_LEN HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - 4 //MSG头+长度域共4BYTE
#define HUITP_IE_BUF_MAX_LEN HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - 30 //MSG头4B，BASE_REQ 5B，IE头4B，为了安全，先减去30。在具体的消息定义中，如果独立IE较多，这个需要再检查
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN 300  //人为定义，目前不包括具体内容在内的纯格式部分，已经达到210BYTE长度
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN 200
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_VARIABLE_LEN HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN - HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN
//<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>
#define HUITP_MSG_HUIXML_TOTAL_BUF_IN_CHAR_MAX_LEN HUITP_MSG_XML_HEAD_IN_CHAR_MAX_LEN + 2 * HUITP_MSG_BUF_WITH_HEAD_MAX_LEN + 1 //CHAR方式，用于INTERNET传输，留下一个做为尾部缓冲
#define HUITP_MSG_HUIFRAME_HEAD_MAX_LEN 4 //固定长度，采用SUM(CONTENT)的方式进行校验
#define HUITP_MSG_HUIFRAME_TOTAL_BUF_MAX_LEN HUITP_MSG_HUIFRAME_HEAD_MAX_LEN + HUITP_MSG_BUF_WITH_HEAD_MAX_LEN //这个是16进制BYTE方式
//HUIXML固定头部常量定义
#define HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L  					"<xml>"
#define HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R  					"</xml>"
#define HUITP_MSG_HUIXML_CONSTANT_TO_USER_L  						"<ToUserName><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_TO_USER_R  						"]]></ToUserName>"
#define HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L  					"<FromUserName><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R  					"]]></FromUserName>"
#define HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L  				"<CreateTime>"
#define HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R  				"</CreateTime>"
#define HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L  					"<MsgType><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R  					"]]></MsgType>"
#define HUITP_MSG_HUIXML_CONSTANT_CONTENT_L  						"<Content><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_CONTENT_R  						"]]></Content>"
#define HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L  					"<FuncFlag>"
#define HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R  					"</FuncFlag>"
#define HUITP_MSG_HUIXML_CONTSANT_CREATE_TIME_MAX_LEN		16 //以10进制存储的时间戳，最长长度
//HUIXML MsgType定义
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID 			1 
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING  	"hcu_text"  //"hcu_text"
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID 			2
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING  "hcu_command" //"hcu_command"
#define HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID 					3
#define HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING 			"hcu_heart_beat"   //"hcu_heart_beat"  //心跳协议，里面的数据内容是空的
#define HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID 						4
#define HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING 				"hcu_biz_itg"  //业务智能 hcu_biz_inteligence
#define HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID 				5
#define HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING 		"hcu_alarm"  //for alarm report
#define HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID 					6
#define HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING 			"hcu_pm"  //for pm report

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP字典定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//标准命令字统一定义
typedef enum
{
	HUITP_CMDID_uni_none                  = 0x00,
	HUITP_CMDID_uni_blood_glucose         = 0x01, //血糖
	HUITP_CMDID_uni_single_sports         = 0x02, //单次运动
	HUITP_CMDID_uni_single_sleep          = 0x03, //单次睡眠
	HUITP_CMDID_uni_body_fat              = 0x04, //体脂
	HUITP_CMDID_uni_blood_pressure        = 0x05, //血压
	HUITP_CMDID_uni_runner_machine_report = 0x0A, //跑步机数据上报
	HUITP_CMDID_uni_runner_machine_control= 0x0B, //跑步机任务控制
	HUITP_CMDID_uni_gps                   = 0x0C, //GPS地址
	HUITP_CMDID_uni_iau_control           = 0x10, //IHU与IAU之间控制命令
	HUITP_CMDID_uni_emc                   = 0x20, //电磁辐射强度
	HUITP_CMDID_uni_emc_accumulation      = 0x21, //电磁辐射剂量
	HUITP_CMDID_uni_co                    = 0x22, //一氧化碳
	HUITP_CMDID_uni_formaldehyde          = 0x23, //甲醛HCHO
	HUITP_CMDID_uni_alcohol               = 0x24, //酒精
	HUITP_CMDID_uni_pm25                  = 0x25, //PM1/2.5/10
	HUITP_CMDID_uni_windspd               = 0x26, //风速Wind Speed
	HUITP_CMDID_uni_winddir               = 0x27, //风向Wind Direction
	HUITP_CMDID_uni_temp                  = 0x28, //温度Temperature
	HUITP_CMDID_uni_humid                 = 0x29, //湿度Humidity
	HUITP_CMDID_uni_airprs                = 0x2A, //气压Air pressure
	HUITP_CMDID_uni_noise                 = 0x2B, //噪声Noise
	HUITP_CMDID_uni_hsmmp                 = 0x2C, //相机Camer or audio high speed
	HUITP_CMDID_uni_audio                 = 0x2D, //声音
	HUITP_CMDID_uni_video                 = 0x2E, //视频
	HUITP_CMDID_uni_picture               = 0x2F, //图片
	HUITP_CMDID_uni_ycjk                  = 0x30, //扬尘监控
	HUITP_CMDID_uni_water_meter           = 0x31, //水表
	HUITP_CMDID_uni_heat_meter            = 0x32, //热表
	HUITP_CMDID_uni_gas_meter             = 0x33, //气表
	HUITP_CMDID_uni_power_meter           = 0x34, //电表
	HUITP_CMDID_uni_light_strength        = 0x35, //光照强度
	HUITP_CMDID_uni_toxicgas              = 0x36, //有毒气体VOC
	HUITP_CMDID_uni_altitude              = 0x37, //海拔高度
	HUITP_CMDID_uni_moto                  = 0x38, //马达
	HUITP_CMDID_uni_switch_resistor       = 0x39, //继电器
	HUITP_CMDID_uni_transporter           = 0x3A, //导轨传送带
	HUITP_CMDID_uni_bfsc_comb_scale       = 0x3B, //组合秤
	HUITP_CMDID_uni_ccl_lock_old          = 0x40, //智能锁，兼容老系统
	HUITP_CMDID_uni_ccl_door              = 0x41, //光交箱门，兼容老系统
	HUITP_CMDID_uni_ccl_rfid              = 0x42, //光交箱RFID模块，兼容老系统
	HUITP_CMDID_uni_ccl_ble               = 0x43, //光交箱BLE模块，兼容老系统
	HUITP_CMDID_uni_ccl_gprs              = 0x44, //光交箱GPRS模块，兼容老系统
	HUITP_CMDID_uni_ccl_battery           = 0x45, //光交箱电池模块，兼容老系统
	HUITP_CMDID_uni_ccl_shake             = 0x46, //光交箱震动，兼容老系统
	HUITP_CMDID_uni_ccl_smoke             = 0x47, //光交箱烟雾，兼容老系统
	HUITP_CMDID_uni_ccl_water             = 0x48, //光交箱水浸，兼容老系统
	HUITP_CMDID_uni_ccl_temp              = 0x49, //光交箱温度，兼容老系统
	HUITP_CMDID_uni_ccl_humid             = 0x4A, //光交箱湿度，兼容老系统
	HUITP_CMDID_uni_ccl_fall              = 0x4B, //倾倒，兼容老系统
	HUITP_CMDID_uni_ccl_state_old         = 0x4C, //状态聚合，兼容老系统
	HUITP_CMDID_uni_ccl_lock              = 0x4D, //光交箱智能锁
	HUITP_CMDID_uni_ccl_state             = 0x4E, //光交箱状态聚合	
	HUITP_CMDID_uni_itf_sps               = 0x50, //串口读取命令/返回结果
	HUITP_CMDID_uni_itf_adc               = 0x51, //ADC读取命令/返回结果
	HUITP_CMDID_uni_itf_dac               = 0x52, //DAC读取命令/返回结果
	HUITP_CMDID_uni_itf_i2c               = 0x53, //I2C读取命令/返回结果
	HUITP_CMDID_uni_itf_pwm               = 0x54, //PWM读取命令/返回结果
	HUITP_CMDID_uni_itf_di                = 0x55, //DI读取命令/返回结果
	HUITP_CMDID_uni_itf_do                = 0x56, //DO读取命令/返回结果
	HUITP_CMDID_uni_itf_can               = 0x57, //CAN读取命令/返回结果
	HUITP_CMDID_uni_itf_spi               = 0x58, //SPI读取命令/返回结果
	HUITP_CMDID_uni_itf_usb               = 0x59, //USB读取命令/返回结果
	HUITP_CMDID_uni_itf_eth               = 0x5A, //网口读取命令/返回结果
	HUITP_CMDID_uni_itf_485               = 0x5B, //485读取命令/返回结果
	HUITP_CMDID_uni_inventory             = 0xA0,	//软件清单
	HUITP_CMDID_uni_sw_package            = 0xA1,	//软件版本体
	HUITP_CMDID_uni_alarm_info            = 0xB0,  //for alarm report
	HUITP_CMDID_uni_performance_info      = 0xB1,  // or PM report
	HUITP_CMDID_uni_equipment_info        = 0xF0,	//设备基本信息
	HUITP_CMDID_uni_personal_info         = 0xF1,	//个人基本信息
	HUITP_CMDID_uni_time_sync             = 0xF2,	//时间同步
	HUITP_CMDID_uni_read_data             = 0xF3,	//读取数据
	HUITP_CMDID_uni_clock_timeout         = 0xF4,	//定时闹钟及久坐提醒
	HUITP_CMDID_uni_sync_charging         = 0xF5,	//同步充电，双击情况
	HUITP_CMDID_uni_sync_trigger          = 0xF6,	//同步通知信息
	HUITP_CMDID_uni_cmd_control           = 0xFD,  //for cmd control by Shanchun
	HUITP_CMDID_uni_heart_beat            = 0xFE, //心跳
	HUITP_CMDID_uni_null                  = 0xFF, //无效
}HuitpCmdidUniDefination;

//标准操作字统一定义
typedef enum
{
  HUITP_OPTID_uni_min                       = 0x00,
	HUITP_OPTID_uni_data_req                  = 0x00,   //Data Request, 或命令请求  
	HUITP_OPTID_uni_data_resp                 = 0x80,   //Data Response
	HUITP_OPTID_uni_data_report_confirm       = 0x01,   //Data report confirm  
	HUITP_OPTID_uni_data_report               = 0x81,   //Data Report，或命令响应       
	HUITP_OPTID_uni_set_switch                = 0x02,   //Set Switch                    
	HUITP_OPTID_uni_set_switch_ack            = 0x82,   //Set Switch ack                  
	HUITP_OPTID_uni_set_modbus_address        = 0x03,   //Set Modbus Address            
	HUITP_OPTID_uni_set_modbus_address_ack    = 0x83,   //Set Modbus Address ack        
	HUITP_OPTID_uni_set_work_cycle            = 0x04,   //Work cycle, in second         
	HUITP_OPTID_uni_set_work_cycle_ack        = 0x84,   //Work cycle, in second         
	HUITP_OPTID_uni_set_sample_cycle          = 0x05,   //Set Sample cycle, in second   
	HUITP_OPTID_uni_set_sample_cycle_ack      = 0x85,   //Set Sample cycle, in second   
	HUITP_OPTID_uni_set_sample_number         = 0x06,   //Set Sample number             
	HUITP_OPTID_uni_set_sample_number_ack     = 0x86,   //Set Sample number 
	HUITP_OPTID_uni_read_switch               = 0x07,   //Read switch                   
	HUITP_OPTID_uni_read_switch_ack           = 0x87,   //Read switch                   
	HUITP_OPTID_uni_read_modbus_address       = 0x08,   //Read Modbus Address           
	HUITP_OPTID_uni_read_modbus_address_ack   = 0x88,   //Read Modbus Address           
	HUITP_OPTID_uni_read_work_cycle           = 0x09,   //Read Work Cycle               
	HUITP_OPTID_uni_read_work_cycle_ack       = 0x89,   //Read Work Cycle               
	HUITP_OPTID_uni_read_sample_cycle         = 0x0A,   //Read Sample Cycle             
	HUITP_OPTID_uni_read_sample_cycle_ack     = 0x8A,   //Read Sample Cycle             
	HUITP_OPTID_uni_read_sample_number        = 0x0B,   //Read Sample Number            
	HUITP_OPTID_uni_read_sample_number_ack    = 0x8B,   //Read Sample Number            
	HUITP_OPTID_uni_auth_inq                  = 0x90,   //授权询问
	HUITP_OPTID_uni_auth_resp                 = 0x10,   //授权应答                 
  HUITP_OPTID_uni_max,
}HuitpOptIdUniDefination;

typedef enum
{
	//无效
	HUITP_MSGID_uni_none                             = 0x0000,
	HUITP_MSGID_uni_min                              = 0x0100,

	//血糖
	HUITP_MSGID_uni_blood_glucose_min                = 0x0100, 
	HUITP_MSGID_uni_blood_glucose_req                = 0x0100, 
	HUITP_MSGID_uni_blood_glucose_resp               = 0x0180,
	HUITP_MSGID_uni_blood_glucose_report             = 0x0181,
	HUITP_MSGID_uni_blood_glucose_confirm            = 0x0101,
	HUITP_MSGID_uni_blood_glucose_max,
	
	//单次运动
	HUITP_MSGID_uni_single_sports_min                = 0x0200, 
	HUITP_MSGID_uni_single_sports_req                = 0x0200, 
	HUITP_MSGID_uni_single_sports_resp               = 0x0280,
	HUITP_MSGID_uni_single_sports_report             = 0x0281, 
	HUITP_MSGID_uni_single_sports_confirm            = 0x0201,
	HUITP_MSGID_uni_single_sports_max,
	
	//单次睡眠
	HUITP_MSGID_uni_single_sleep_min                 = 0x0300, 
	HUITP_MSGID_uni_single_sleep_req                 = 0x0300, 
	HUITP_MSGID_uni_single_sleep_resp                = 0x0380, 
	HUITP_MSGID_uni_single_sleep_report              = 0x0381, 
	HUITP_MSGID_uni_single_sleep_confirm             = 0x0301, 
	HUITP_MSGID_uni_single_sleep_max,
		
	//体脂
	HUITP_MSGID_uni_body_fat_min                     = 0x0400, 
	HUITP_MSGID_uni_body_fat_req                     = 0x0400, 
	HUITP_MSGID_uni_body_fat_resp                    = 0x0480, 
	HUITP_MSGID_uni_body_fat_report                  = 0x0481, 
	HUITP_MSGID_uni_body_fat_confirm                 = 0x0401, 
	HUITP_MSGID_uni_body_fat_max,
	
	//血压
	HUITP_MSGID_uni_blood_pressure_min               = 0x0500, 
	HUITP_MSGID_uni_blood_pressure_req               = 0x0500, 
	HUITP_MSGID_uni_blood_pressure_resp              = 0x0580, 
	HUITP_MSGID_uni_blood_pressure_report            = 0x0581, 
	HUITP_MSGID_uni_blood_pressure_confirm           = 0x0501, 
	HUITP_MSGID_uni_blood_pressure_max,

  //跑步机数据上报
	HUITP_MSGID_uni_runner_machine_rep_min           = 0x0A00, 
	HUITP_MSGID_uni_runner_machine_rep_req           = 0x0A00, 
	HUITP_MSGID_uni_runner_machine_rep_resp          = 0x0A80, 
	HUITP_MSGID_uni_runner_machine_rep_report        = 0x0A81, 
	HUITP_MSGID_uni_runner_machine_rep_confirm       = 0x0A01, 
	HUITP_MSGID_uni_runner_machine_rep_max,
  
  //跑步机任务控制
	HUITP_MSGID_uni_runner_machine_ctrl_min          = 0x0B00, 
	HUITP_MSGID_uni_runner_machine_ctrl_req          = 0x0B00, 
	HUITP_MSGID_uni_runner_machine_ctrl_resp         = 0x0B80, 
	HUITP_MSGID_uni_runner_machine_ctrl_report       = 0x0B81, 
	HUITP_MSGID_uni_runner_machine_ctrl_confirm      = 0x0B01, 
	HUITP_MSGID_uni_runner_machine_ctrl_max,

  //GPS地址
	HUITP_MSGID_uni_gps_specific_min                 = 0x0C00, 
	HUITP_MSGID_uni_gps_specific_req                 = 0x0C00, 
	HUITP_MSGID_uni_gps_specific_resp                = 0x0C80, 
	HUITP_MSGID_uni_gps_specific_report              = 0x0C81, 
	HUITP_MSGID_uni_gps_specific_confirm             = 0x0C01, 
	HUITP_MSGID_uni_gps_specific_max,

  //IHU与IAU之间控制命令
	HUITP_MSGID_uni_iau_ctrl_min                     = 0x1000, 
	HUITP_MSGID_uni_iau_ctrl_req                     = 0x1000, 
	HUITP_MSGID_uni_iau_ctrl_resp                    = 0x1080, 
	HUITP_MSGID_uni_iau_ctrl_report                  = 0x1081, 
	HUITP_MSGID_uni_iau_ctrl_confirm                 = 0x1001, 
	HUITP_MSGID_uni_iau_ctrl_max,

  //电磁辐射强度
	HUITP_MSGID_uni_emc_data_min                     = 0x2000, 
	HUITP_MSGID_uni_emc_data_req                     = 0x2000, 
	HUITP_MSGID_uni_emc_data_resp                    = 0x2080, 
	HUITP_MSGID_uni_emc_data_report                  = 0x2081, 
	HUITP_MSGID_uni_emc_data_confirm                 = 0x2001, 
	HUITP_MSGID_uni_emc_set_switch                   = 0x2002, 
	HUITP_MSGID_uni_emc_set_switch_ack               = 0x2082, 	
	HUITP_MSGID_uni_emc_set_modbus_address           = 0x2003, 
	HUITP_MSGID_uni_emc_set_modbus_address_ack       = 0x2083, 
	HUITP_MSGID_uni_emc_set_work_cycle               = 0x2004,   //In second
	HUITP_MSGID_uni_emc_set_work_cycle_ack           = 0x2084,   //In second
	HUITP_MSGID_uni_emc_set_sample_cycle             = 0x2005,   //In second
	HUITP_MSGID_uni_emc_set_sample_cycle_ack         = 0x2085,   //In second
	HUITP_MSGID_uni_emc_set_sample_number            = 0x2006,   
	HUITP_MSGID_uni_emc_set_sample_number_ack        = 0x2086,   
	HUITP_MSGID_uni_emc_read_switch                  = 0x2007,
	HUITP_MSGID_uni_emc_read_switch_ack              = 0x2087,
	HUITP_MSGID_uni_emc_read_modbus_address          = 0x2008,
	HUITP_MSGID_uni_emc_read_modbus_address_ack      = 0x2088,
	HUITP_MSGID_uni_emc_read_work_cycle              = 0x2009,
	HUITP_MSGID_uni_emc_read_work_cycle_ack          = 0x2089,
	HUITP_MSGID_uni_emc_read_sample_cycle            = 0x200A, 
	HUITP_MSGID_uni_emc_read_sample_cycle_ack        = 0x208A, 
  HUITP_MSGID_uni_emc_read_sample_number           = 0x200B,
  HUITP_MSGID_uni_emc_read_sample_number_ack       = 0x208B,
	HUITP_MSGID_uni_emc_data_max,

  //电磁辐射剂量
	HUITP_MSGID_uni_emc_accu_min                     = 0x2100, 
	HUITP_MSGID_uni_emc_accu_req                     = 0x2100, 
	HUITP_MSGID_uni_emc_accu_resp                    = 0x2180, 
	HUITP_MSGID_uni_emc_accu_report                  = 0x2181, 
	HUITP_MSGID_uni_emc_accu_confirm                 = 0x2101, 
	HUITP_MSGID_uni_emc_accu_max,

  //一氧化碳
	HUITP_MSGID_uni_co_min                           = 0x2200, 
	HUITP_MSGID_uni_co_req                           = 0x2200, 
	HUITP_MSGID_uni_co_resp                          = 0x2280, 
	HUITP_MSGID_uni_co_report                        = 0x2281, 
	HUITP_MSGID_uni_co_confirm                       = 0x2201,
	HUITP_MSGID_uni_co_max,

  //甲醛HCHO
	HUITP_MSGID_uni_formaldehyde_min                 = 0x2300, 
	HUITP_MSGID_uni_formaldehyde_req                 = 0x2300, 
	HUITP_MSGID_uni_formaldehyde_resp                = 0x2380, 
	HUITP_MSGID_uni_formaldehyde_report              = 0x2381, 
	HUITP_MSGID_uni_formaldehyde_confirm             = 0x2301, 
	HUITP_MSGID_uni_formaldehyde_max,

  //酒精
	HUITP_MSGID_uni_alcohol_min                      = 0x2400, 
	HUITP_MSGID_uni_alcohol_req                      = 0x2400, 
	HUITP_MSGID_uni_alcohol_resp                     = 0x2480, 
	HUITP_MSGID_uni_alcohol_report                   = 0x2481, 
	HUITP_MSGID_uni_alcohol_confirm                  = 0x2401, 
	HUITP_MSGID_uni_alcohol_max,
 
  //PM1/2.5/10
	HUITP_MSGID_uni_pm25_min                         = 0x2500,
	HUITP_MSGID_uni_pm25_data_req                    = 0x2500,
	HUITP_MSGID_uni_pm25_data_resp                   = 0x2580, 
	HUITP_MSGID_uni_pm25_data_report                 = 0x2581, 
	HUITP_MSGID_uni_pm25_data_confirm                = 0x2501, 
	HUITP_MSGID_uni_pm25_set_switch                  = 0x2502, 
	HUITP_MSGID_uni_pm25_set_switch_ack              = 0x2582, 
	HUITP_MSGID_uni_pm25_set_modbus_address          = 0x2503, 
	HUITP_MSGID_uni_pm25_set_modbus_address_ack      = 0x2583, 
	HUITP_MSGID_uni_pm25_set_work_cycle              = 0x2504,  //In second
	HUITP_MSGID_uni_pm25_set_work_cycle_ack          = 0x2584,  
	HUITP_MSGID_uni_pm25_set_sample_cycle            = 0x2505,  //In second
	HUITP_MSGID_uni_pm25_set_sample_cycle_ack        = 0x2585,  
	HUITP_MSGID_uni_pm25_set_sample_number           = 0x2506,
	HUITP_MSGID_uni_pm25_set_sample_number_ack       = 0x2586,  
	HUITP_MSGID_uni_pm25_read_switch                 = 0x2507,
	HUITP_MSGID_uni_pm25_read_switch_ack             = 0x2587,  
	HUITP_MSGID_uni_pm25_read_modbus_address         = 0x2508,
	HUITP_MSGID_uni_pm25_read_modbus_address_ack     = 0x2588,  
	HUITP_MSGID_uni_pm25_read_work_cycle             = 0x2509,  //In second
	HUITP_MSGID_uni_pm25_read_work_cycle_ack         = 0x2589,  
	HUITP_MSGID_uni_pm25_read_sample_cycle           = 0x250A,  //In second
	HUITP_MSGID_uni_pm25_read_sample_cycle_ack       = 0x258A,  
	HUITP_MSGID_uni_pm25_read_sample_number          = 0x250B,
	HUITP_MSGID_uni_pm25_read_sample_number_ack      = 0x258B,  
	HUITP_MSGID_uni_pm25_max,

  //风速Wind Speed
	HUITP_MSGID_uni_windspd_min                      = 0x2600, 
	HUITP_MSGID_uni_windspd_data_req                 = 0x2600, 
	HUITP_MSGID_uni_windspd_data_resp                = 0x2680, 
	HUITP_MSGID_uni_windspd_data_report              = 0x2681, 
	HUITP_MSGID_uni_windspd_data_confirm             = 0x2601,
	HUITP_MSGID_uni_windspd_set_switch               = 0x2602, 
	HUITP_MSGID_uni_windspd_set_switch_ack           = 0x2682, 
	HUITP_MSGID_uni_windspd_set_modbus_address       = 0x2603, 
	HUITP_MSGID_uni_windspd_set_modbus_address_ack   = 0x2683, 
	HUITP_MSGID_uni_windspd_set_work_cycle           = 0x2604, //In second
	HUITP_MSGID_uni_windspd_set_work_cycle_ack       = 0x2684, 
	HUITP_MSGID_uni_windspd_set_sample_cycle         = 0x2605,  //In second
	HUITP_MSGID_uni_windspd_set_sample_cycle_ack     = 0x2685, 
	HUITP_MSGID_uni_windspd_set_sample_number        = 0x2606, 
	HUITP_MSGID_uni_windspd_set_sample_number_ack    = 0x2686, 
	HUITP_MSGID_uni_windspd_read_switch              = 0x2607, 
	HUITP_MSGID_uni_windspd_read_switch_ack          = 0x2687, 
	HUITP_MSGID_uni_windspd_read_modbus_address      = 0x2608, 
	HUITP_MSGID_uni_windspd_read_modbus_address_ack  = 0x2688, 
	HUITP_MSGID_uni_windspd_read_work_cycle          = 0x2609, //In second
	HUITP_MSGID_uni_windspd_read_work_cycle_ack      = 0x2689, 
	HUITP_MSGID_uni_windspd_read_sample_cycle        = 0x260A,  //In second
	HUITP_MSGID_uni_windspd_read_sample_cycle_ack    = 0x268A, 
	HUITP_MSGID_uni_windspd_read_sample_number       = 0x260B, 
	HUITP_MSGID_uni_windspd_read_sample_number_ack   = 0x268B, 
	HUITP_MSGID_uni_windspd_max,

	//风向Wind Direction
	HUITP_MSGID_uni_winddir_min                      = 0x2700, 
	HUITP_MSGID_uni_winddir_data_req                 = 0x2700, 
	HUITP_MSGID_uni_winddir_data_resp                = 0x2780, 
	HUITP_MSGID_uni_winddir_data_report              = 0x2781, 
	HUITP_MSGID_uni_winddir_data_confirm             = 0x2701,
	HUITP_MSGID_uni_winddir_set_switch               = 0x2702, 
	HUITP_MSGID_uni_winddir_set_switch_ack           = 0x2782, 
	HUITP_MSGID_uni_winddir_set_modbus_address       = 0x2703, 
	HUITP_MSGID_uni_winddir_set_modbus_address_ack   = 0x2783, 
	HUITP_MSGID_uni_winddir_set_work_cycle           = 0x2704, //In second
	HUITP_MSGID_uni_winddir_set_work_cycle_ack       = 0x2784, 
	HUITP_MSGID_uni_winddir_set_sample_cycle         = 0x2705,  //In second
	HUITP_MSGID_uni_winddir_set_sample_cycle_ack     = 0x2785, 
	HUITP_MSGID_uni_winddir_set_sample_number        = 0x2706, 
	HUITP_MSGID_uni_winddir_set_sample_number_ack    = 0x2786, 
	HUITP_MSGID_uni_winddir_read_switch              = 0x2707, 
	HUITP_MSGID_uni_winddir_read_switch_ack          = 0x2787, 
	HUITP_MSGID_uni_winddir_read_modbus_address      = 0x2708, 
	HUITP_MSGID_uni_winddir_read_modbus_address_ack  = 0x2788, 
	HUITP_MSGID_uni_winddir_read_work_cycle          = 0x2709, //In second
	HUITP_MSGID_uni_winddir_read_work_cycle_ack      = 0x2789, 
	HUITP_MSGID_uni_winddir_read_sample_cycle        = 0x270A,  //In second
	HUITP_MSGID_uni_winddir_read_sample_cycle_ack    = 0x278A, 
	HUITP_MSGID_uni_winddir_read_sample_number       = 0x270B, 
	HUITP_MSGID_uni_winddir_read_sample_number_ack   = 0x278B, 
	HUITP_MSGID_uni_winddir_max,
	
	//温度Temperature
	HUITP_MSGID_uni_temp_min                         = 0x2800, 
	HUITP_MSGID_uni_temp_data_req                    = 0x2800, 
	HUITP_MSGID_uni_temp_data_resp                   = 0x2880, 
	HUITP_MSGID_uni_temp_data_report                 = 0x2881, 
	HUITP_MSGID_uni_temp_data_confirm                = 0x2801,
	HUITP_MSGID_uni_temp_set_switch                  = 0x2802, 
	HUITP_MSGID_uni_temp_set_switch_ack              = 0x2882, 
	HUITP_MSGID_uni_temp_set_modbus_address          = 0x2803, 
	HUITP_MSGID_uni_temp_set_modbus_address_ack      = 0x2883, 
	HUITP_MSGID_uni_temp_set_work_cycle              = 0x2804, //In second
	HUITP_MSGID_uni_temp_set_work_cycle_ack          = 0x2884, 
	HUITP_MSGID_uni_temp_set_sample_cycle            = 0x2805,  //In second
	HUITP_MSGID_uni_temp_set_sample_cycle_ack        = 0x2885, 
	HUITP_MSGID_uni_temp_set_sample_number           = 0x2806, 
	HUITP_MSGID_uni_temp_set_sample_number_ack       = 0x2886, 
	HUITP_MSGID_uni_temp_read_switch                 = 0x2807, 
	HUITP_MSGID_uni_temp_read_switch_ack             = 0x2887, 
	HUITP_MSGID_uni_temp_read_modbus_address         = 0x2808, 
	HUITP_MSGID_uni_temp_read_modbus_address_ack     = 0x2888, 
	HUITP_MSGID_uni_temp_read_work_cycle             = 0x2809, //In second
	HUITP_MSGID_uni_temp_read_work_cycle_ack         = 0x2889, 
	HUITP_MSGID_uni_temp_read_sample_cycle           = 0x280A,  //In second
	HUITP_MSGID_uni_temp_read_sample_cycle_ack       = 0x288A, 
	HUITP_MSGID_uni_temp_read_sample_number          = 0x280B, 
	HUITP_MSGID_uni_temp_read_sample_number_ack      = 0x288B, 
	HUITP_MSGID_uni_temp_max,

  //湿度Humidity
	HUITP_MSGID_uni_humid_min                        = 0x2900, 
	HUITP_MSGID_uni_humid_data_req                   = 0x2900, 
	HUITP_MSGID_uni_humid_data_resp                  = 0x2980, 
	HUITP_MSGID_uni_humid_data_report                = 0x2981, 
	HUITP_MSGID_uni_humid_data_confirm               = 0x2901,
	HUITP_MSGID_uni_humid_set_switch                 = 0x2902, 
	HUITP_MSGID_uni_humid_set_switch_ack             = 0x2982, 
	HUITP_MSGID_uni_humid_set_modbus_address         = 0x2903, 
	HUITP_MSGID_uni_humid_set_modbus_address_ack     = 0x2983, 
	HUITP_MSGID_uni_humid_set_work_cycle             = 0x2904, //In second
	HUITP_MSGID_uni_humid_set_work_cycle_ack         = 0x2984, 
	HUITP_MSGID_uni_humid_set_sample_cycle           = 0x2905,  //In second
	HUITP_MSGID_uni_humid_set_sample_cycle_ack       = 0x2985, 
	HUITP_MSGID_uni_humid_set_sample_number          = 0x2906, 
	HUITP_MSGID_uni_humid_set_sample_number_ack      = 0x2986, 
	HUITP_MSGID_uni_humid_read_switch                = 0x2907, 
	HUITP_MSGID_uni_humid_read_switch_ack            = 0x2987, 
	HUITP_MSGID_uni_humid_read_modbus_address        = 0x2908, 
	HUITP_MSGID_uni_humid_read_modbus_address_ack    = 0x2988, 
	HUITP_MSGID_uni_humid_read_work_cycle            = 0x2909, //In second
	HUITP_MSGID_uni_humid_read_work_cycle_ack        = 0x2989, 
	HUITP_MSGID_uni_humid_read_sample_cycle          = 0x290A,  //In second
	HUITP_MSGID_uni_humid_read_sample_cycle_ack      = 0x298A, 
	HUITP_MSGID_uni_humid_read_sample_number         = 0x290B, 
	HUITP_MSGID_uni_humid_read_sample_number_ack     = 0x298B, 
	HUITP_MSGID_uni_humid_max,

  //气压Air pressure
	HUITP_MSGID_uni_airprs_min                       = 0x2A00, 
	HUITP_MSGID_uni_airprs_data_req                  = 0x2A00, 
	HUITP_MSGID_uni_airprs_data_resp                 = 0x2A80, 
	HUITP_MSGID_uni_airprs_data_report               = 0x2A81, 
	HUITP_MSGID_uni_airprs_data_confirm              = 0x2A01,
	HUITP_MSGID_uni_airprs_set_switch                = 0x2A02, 
	HUITP_MSGID_uni_airprs_set_switch_ack            = 0x2A82, 
	HUITP_MSGID_uni_airprs_set_modbus_address        = 0x2A03, 
	HUITP_MSGID_uni_airprs_set_modbus_address_ack    = 0x2A83, 
	HUITP_MSGID_uni_airprs_set_work_cycle            = 0x2A04, //In second
	HUITP_MSGID_uni_airprs_set_work_cycle_ack        = 0x2A84, 
	HUITP_MSGID_uni_airprs_set_sample_cycle          = 0x2A05,  //In second
	HUITP_MSGID_uni_airprs_set_sample_cycle_ack      = 0x2A85, 
	HUITP_MSGID_uni_airprs_set_sample_number         = 0x2A06, 
	HUITP_MSGID_uni_airprs_set_sample_number_ack     = 0x2A86, 
	HUITP_MSGID_uni_airprs_read_switch               = 0x2A07, 
	HUITP_MSGID_uni_airprs_read_switch_ack           = 0x2A87, 
	HUITP_MSGID_uni_airprs_read_modbus_address       = 0x2A08, 
	HUITP_MSGID_uni_airprs_read_modbus_address_ack   = 0x2A88, 
	HUITP_MSGID_uni_airprs_read_work_cycle           = 0x2A09, //In second
	HUITP_MSGID_uni_airprs_read_work_cycle_ack       = 0x2A89, 
	HUITP_MSGID_uni_airprs_read_sample_cycle         = 0x2A0A,  //In second
	HUITP_MSGID_uni_airprs_read_sample_cycle_ack     = 0x2A8A, 
	HUITP_MSGID_uni_airprs_read_sample_number        = 0x2A0B, 
	HUITP_MSGID_uni_airprs_read_sample_number_ack    = 0x2A8B, 
	HUITP_MSGID_uni_airprs_max,

  //噪声Noise
	HUITP_MSGID_uni_noise_min                        = 0x2B00, 
	HUITP_MSGID_uni_noise_data_req                   = 0x2B00, 
	HUITP_MSGID_uni_noise_data_resp                  = 0x2B80, 
	HUITP_MSGID_uni_noise_data_report                = 0x2B81, 
	HUITP_MSGID_uni_noise_data_confirm               = 0x2B01,
	HUITP_MSGID_uni_noise_set_switch                 = 0x2B02, 
	HUITP_MSGID_uni_noise_set_switch_ack             = 0x2B82, 
	HUITP_MSGID_uni_noise_set_modbus_address         = 0x2B03, 
	HUITP_MSGID_uni_noise_set_modbus_address_ack     = 0x2B83, 
	HUITP_MSGID_uni_noise_set_work_cycle             = 0x2B04, //In second
	HUITP_MSGID_uni_noise_set_work_cycle_ack         = 0x2B84, 
	HUITP_MSGID_uni_noise_set_sample_cycle           = 0x2B05,  //In second
	HUITP_MSGID_uni_noise_set_sample_cycle_ack       = 0x2B85, 
	HUITP_MSGID_uni_noise_set_sample_number          = 0x2B06, 
	HUITP_MSGID_uni_noise_set_sample_number_ack      = 0x2B86, 
	HUITP_MSGID_uni_noise_read_switch                = 0x2B07, 
	HUITP_MSGID_uni_noise_read_switch_ack            = 0x2B87, 
	HUITP_MSGID_uni_noise_read_modbus_address        = 0x2B08, 
	HUITP_MSGID_uni_noise_read_modbus_address_ack    = 0x2B88, 
	HUITP_MSGID_uni_noise_read_work_cycle            = 0x2B09, //In second
	HUITP_MSGID_uni_noise_read_work_cycle_ack        = 0x2B89, 
	HUITP_MSGID_uni_noise_read_sample_cycle          = 0x2B0A,  //In second
	HUITP_MSGID_uni_noise_read_sample_cycle_ack      = 0x2B8A, 
	HUITP_MSGID_uni_noise_read_sample_number         = 0x2B0B, 
	HUITP_MSGID_uni_noise_read_sample_number_ack     = 0x2B8B, 
	HUITP_MSGID_uni_noise_max,

  //相机Camer or audio high speed
	HUITP_MSGID_uni_hsmmp_min                        = 0x2C00, 
	HUITP_MSGID_uni_hsmmp_data_req                   = 0x2C00, 
	HUITP_MSGID_uni_hsmmp_data_resp                  = 0x2C80, 
	HUITP_MSGID_uni_hsmmp_data_report                = 0x2C81, 
	HUITP_MSGID_uni_hsmmp_data_confirm               = 0x2C01,
	HUITP_MSGID_uni_hsmmp_set_switch                 = 0x2C02, 
	HUITP_MSGID_uni_hsmmp_set_switch_ack             = 0x2C82, 
	HUITP_MSGID_uni_hsmmp_set_modbus_address         = 0x2C03, 
	HUITP_MSGID_uni_hsmmp_set_modbus_address_ack     = 0x2C83, 
	HUITP_MSGID_uni_hsmmp_set_work_cycle             = 0x2C04, //In second
	HUITP_MSGID_uni_hsmmp_set_work_cycle_ack         = 0x2C84, 
	HUITP_MSGID_uni_hsmmp_set_sample_cycle           = 0x2C05,  //In second
	HUITP_MSGID_uni_hsmmp_set_sample_cycle_ack       = 0x2C85, 
	HUITP_MSGID_uni_hsmmp_set_sample_number          = 0x2C06, 
	HUITP_MSGID_uni_hsmmp_set_sample_number_ack      = 0x2C86, 
	HUITP_MSGID_uni_hsmmp_read_switch                = 0x2C07, 
	HUITP_MSGID_uni_hsmmp_read_switch_ack            = 0x2C87, 
	HUITP_MSGID_uni_hsmmp_read_modbus_address        = 0x2C08, 
	HUITP_MSGID_uni_hsmmp_read_modbus_address_ack    = 0x2C88, 
	HUITP_MSGID_uni_hsmmp_read_work_cycle            = 0x2C09, //In second
	HUITP_MSGID_uni_hsmmp_read_work_cycle_ack        = 0x2C89, 
	HUITP_MSGID_uni_hsmmp_read_sample_cycle          = 0x2C0A,  //In second
	HUITP_MSGID_uni_hsmmp_read_sample_cycle_ack      = 0x2C8A, 
	HUITP_MSGID_uni_hsmmp_read_sample_number         = 0x2C0B, 
	HUITP_MSGID_uni_hsmmp_read_sample_number_ack     = 0x2C8B, 
	HUITP_MSGID_uni_hsmmp_max,

  //声音
	HUITP_MSGID_uni_audio_min                        = 0x2D00, 
	HUITP_MSGID_uni_audio_data_req                   = 0x2D00, 
	HUITP_MSGID_uni_audio_data_resp                  = 0x2D80, 
	HUITP_MSGID_uni_audio_data_report                = 0x2D81, 
	HUITP_MSGID_uni_audio_data_confirm               = 0x2D01,
	HUITP_MSGID_uni_audio_set_switch                 = 0x2D02, 
	HUITP_MSGID_uni_audio_set_switch_ack             = 0x2D82, 
	HUITP_MSGID_uni_audio_set_modbus_address         = 0x2D03, 
	HUITP_MSGID_uni_audio_set_modbus_address_ack     = 0x2D83, 
	HUITP_MSGID_uni_audio_set_work_cycle             = 0x2D04, //In second
	HUITP_MSGID_uni_audio_set_work_cycle_ack         = 0x2D84, 
	HUITP_MSGID_uni_audio_set_sample_cycle           = 0x2D05,  //In second
	HUITP_MSGID_uni_audio_set_sample_cycle_ack       = 0x2D85, 
	HUITP_MSGID_uni_audio_set_sample_number          = 0x2D06, 
	HUITP_MSGID_uni_audio_set_sample_number_ack      = 0x2D86, 
	HUITP_MSGID_uni_audio_read_switch                = 0x2D07, 
	HUITP_MSGID_uni_audio_read_switch_ack            = 0x2D87, 
	HUITP_MSGID_uni_audio_read_modbus_address        = 0x2D08, 
	HUITP_MSGID_uni_audio_read_modbus_address_ack    = 0x2D88, 
	HUITP_MSGID_uni_audio_read_work_cycle            = 0x2D09, //In second
	HUITP_MSGID_uni_audio_read_work_cycle_ack        = 0x2D89, 
	HUITP_MSGID_uni_audio_read_sample_cycle          = 0x2D0A,  //In second
	HUITP_MSGID_uni_audio_read_sample_cycle_ack      = 0x2D8A, 
	HUITP_MSGID_uni_audio_read_sample_number         = 0x2D0B, 
	HUITP_MSGID_uni_audio_read_sample_number_ack     = 0x2D8B, 
	HUITP_MSGID_uni_audio_max,

  //视频
	HUITP_MSGID_uni_video_min                        = 0x2E00, 
	HUITP_MSGID_uni_video_data_req                   = 0x2E00, 
	HUITP_MSGID_uni_video_data_resp                  = 0x2E80, 
	HUITP_MSGID_uni_video_data_report                = 0x2E81, 
	HUITP_MSGID_uni_video_data_confirm               = 0x2E01,
	HUITP_MSGID_uni_video_set_switch                 = 0x2E02, 
	HUITP_MSGID_uni_video_set_switch_ack             = 0x2E82, 
	HUITP_MSGID_uni_video_set_modbus_address         = 0x2E03, 
	HUITP_MSGID_uni_video_set_modbus_address_ack     = 0x2E83, 
	HUITP_MSGID_uni_video_set_work_cycle             = 0x2E04, //In second
	HUITP_MSGID_uni_video_set_work_cycle_ack         = 0x2E84, 
	HUITP_MSGID_uni_video_set_sample_cycle           = 0x2E05,  //In second
	HUITP_MSGID_uni_video_set_sample_cycle_ack       = 0x2E85, 
	HUITP_MSGID_uni_video_set_sample_number          = 0x2E06, 
	HUITP_MSGID_uni_video_set_sample_number_ack      = 0x2E86, 
	HUITP_MSGID_uni_video_read_switch                = 0x2E07, 
	HUITP_MSGID_uni_video_read_switch_ack            = 0x2E87, 
	HUITP_MSGID_uni_video_read_modbus_address        = 0x2E08, 
	HUITP_MSGID_uni_video_read_modbus_address_ack    = 0x2E88, 
	HUITP_MSGID_uni_video_read_work_cycle            = 0x2E09, //In second
	HUITP_MSGID_uni_video_read_work_cycle_ack        = 0x2E89, 
	HUITP_MSGID_uni_video_read_sample_cycle          = 0x2E0A,  //In second
	HUITP_MSGID_uni_video_read_sample_cycle_ack      = 0x2E8A, 
	HUITP_MSGID_uni_video_read_sample_number         = 0x2E0B, 
	HUITP_MSGID_uni_video_read_sample_number_ack     = 0x2E8B, 
	HUITP_MSGID_uni_video_max,

  //图片
	HUITP_MSGID_uni_picture_min                      = 0x2F00, 
	HUITP_MSGID_uni_picture_data_req                 = 0x2F00, 
	HUITP_MSGID_uni_picture_data_resp                = 0x2F80, 
	HUITP_MSGID_uni_picture_data_report              = 0x2F81, 
	HUITP_MSGID_uni_picture_data_confirm             = 0x2F01,
	HUITP_MSGID_uni_picture_set_switch               = 0x2F02, 
	HUITP_MSGID_uni_picture_set_switch_ack           = 0x2F82, 
	HUITP_MSGID_uni_picture_set_modbus_address       = 0x2F03, 
	HUITP_MSGID_uni_picture_set_modbus_address_ack   = 0x2F83, 
	HUITP_MSGID_uni_picture_set_work_cycle           = 0x2F04, //In second
	HUITP_MSGID_uni_picture_set_work_cycle_ack       = 0x2F84, 
	HUITP_MSGID_uni_picture_set_sample_cycle         = 0x2F05,  //In second
	HUITP_MSGID_uni_picture_set_sample_cycle_ack     = 0x2F85, 
	HUITP_MSGID_uni_picture_set_sample_number        = 0x2F06, 
	HUITP_MSGID_uni_picture_set_sample_number_ack    = 0x2F86, 
	HUITP_MSGID_uni_picture_read_switch              = 0x2F07, 
	HUITP_MSGID_uni_picture_read_switch_ack          = 0x2F87, 
	HUITP_MSGID_uni_picture_read_modbus_address      = 0x2F08, 
	HUITP_MSGID_uni_picture_read_modbus_address_ack  = 0x2F88, 
	HUITP_MSGID_uni_picture_read_work_cycle          = 0x2F09, //In second
	HUITP_MSGID_uni_picture_read_work_cycle_ack      = 0x2F89, 
	HUITP_MSGID_uni_picture_read_sample_cycle        = 0x2F0A,  //In second
	HUITP_MSGID_uni_picture_read_sample_cycle_ack    = 0x2F8A, 
	HUITP_MSGID_uni_picture_read_sample_number       = 0x2F0B, 
	HUITP_MSGID_uni_picture_read_sample_number_ack   = 0x2F8B, 
	HUITP_MSGID_uni_picture_max,

  //扬尘监控系统
	HUITP_MSGID_uni_ycjk_min                         = 0x3000, 
	HUITP_MSGID_uni_ycjk_req                         = 0x3000, 
	HUITP_MSGID_uni_ycjk_resp                        = 0x3080, 
	HUITP_MSGID_uni_ycjk_report                      = 0x3081, 
	HUITP_MSGID_uni_ycjk_confirm                     = 0x3001, 
	HUITP_MSGID_uni_ycjk_max,

  //水表
	HUITP_MSGID_uni_water_meter_min                  = 0x3100, 
	HUITP_MSGID_uni_water_meter_req                  = 0x3100, 
	HUITP_MSGID_uni_water_meter_resp                 = 0x3180, 
	HUITP_MSGID_uni_water_meter_report               = 0x3181, 
	HUITP_MSGID_uni_water_meter_confirm              = 0x3101, 
	HUITP_MSGID_uni_water_meter_max,

  //热表
	HUITP_MSGID_uni_heat_meter_min                   = 0x3200, 
	HUITP_MSGID_uni_heat_meter_req                   = 0x3200, 
	HUITP_MSGID_uni_heat_meter_resp                  = 0x3280, 
	HUITP_MSGID_uni_heat_meter_report                = 0x3281, 
	HUITP_MSGID_uni_heat_meter_confirm               = 0x3201, 
	HUITP_MSGID_uni_heat_meter_max,

  //气表
	HUITP_MSGID_uni_gas_meter_min                    = 0x3300, 
	HUITP_MSGID_uni_gas_meter_req                    = 0x3300, 
	HUITP_MSGID_uni_gas_meter_resp                   = 0x3380, 
	HUITP_MSGID_uni_gas_meter_report                 = 0x3381, 
	HUITP_MSGID_uni_gas_meter_confirm                = 0x3301,
	HUITP_MSGID_uni_gas_meter_max,

  //电表
	HUITP_MSGID_uni_power_meter_min                  = 0x3400, 
	HUITP_MSGID_uni_power_meter_req                  = 0x3400, 
	HUITP_MSGID_uni_power_meter_resp                 = 0x3480, 
	HUITP_MSGID_uni_power_meter_report               = 0x3481, 
	HUITP_MSGID_uni_power_meter_confirm              = 0x3401, 
	HUITP_MSGID_uni_power_meter_max,

  //光照强度
	HUITP_MSGID_uni_light_strength_min               = 0x3500, 
	HUITP_MSGID_uni_light_strength_req               = 0x3500, 
	HUITP_MSGID_uni_light_strength_resp              = 0x3580, 
	HUITP_MSGID_uni_light_strength_report            = 0x3581, 
	HUITP_MSGID_uni_light_strength_confirm           = 0x3501, 
	HUITP_MSGID_uni_light_strength_max,

  //有毒气体VOC
	HUITP_MSGID_uni_toxicgas_min                     = 0x3600, 
	HUITP_MSGID_uni_toxicgas_req                     = 0x3600, 
	HUITP_MSGID_uni_toxicgas_resp                    = 0x3680, 
	HUITP_MSGID_uni_toxicgas_report                  = 0x3681, 
	HUITP_MSGID_uni_toxicgas_confirm                 = 0x3601, 
	HUITP_MSGID_uni_toxicgas_max,

  //海拔高度
	HUITP_MSGID_uni_altitude_min                     = 0x3700, 
	HUITP_MSGID_uni_altitude_req                     = 0x3700, 
	HUITP_MSGID_uni_altitude_resp                    = 0x3780, 
	HUITP_MSGID_uni_altitude_report                  = 0x3781, 
	HUITP_MSGID_uni_altitude_confirm                 = 0x3701, 
	HUITP_MSGID_uni_altitude_max,

  //马达
	HUITP_MSGID_uni_moto_min                         = 0x3800, 
	HUITP_MSGID_uni_moto_req                         = 0x3800, 
	HUITP_MSGID_uni_moto_resp                        = 0x3880, 
	HUITP_MSGID_uni_moto_report                      = 0x3881, 
	HUITP_MSGID_uni_moto_confirm                     = 0x3801, 
	HUITP_MSGID_uni_moto_max,

  //继电器
	HUITP_MSGID_uni_switch_resistor_min              = 0x3900, 
	HUITP_MSGID_uni_switch_resistor_req              = 0x3900, 
	HUITP_MSGID_uni_switch_resistor_resp             = 0x3980, 
	HUITP_MSGID_uni_switch_resistor_report           = 0x3981, 
	HUITP_MSGID_uni_switch_resistor_confirm          = 0x3901, 
	HUITP_MSGID_uni_switch_resistor_max,

  //导轨传送带
	HUITP_MSGID_uni_transporter_min                  = 0x3A00, 
	HUITP_MSGID_uni_transporter_req                  = 0x3A00, 
	HUITP_MSGID_uni_transporter_resp                 = 0x3A80, 
	HUITP_MSGID_uni_transporter_report               = 0x3A81, 
	HUITP_MSGID_uni_transporter_confirm              = 0x3A01, 
	HUITP_MSGID_uni_transporter_max,

  //组合秤BFSC
	HUITP_MSGID_uni_bfsc_comb_scale_min              = 0x3B00, 
	HUITP_MSGID_uni_bfsc_comb_scale_req              = 0x3B00, 
	HUITP_MSGID_uni_bfsc_comb_scale_resp             = 0x3B80, 
	HUITP_MSGID_uni_bfsc_comb_scale_report           = 0x3B81, 
	HUITP_MSGID_uni_bfsc_comb_scale_confirm          = 0x3B01, 
	HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_req    = 0x3B02, 
	HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_resp   = 0x3B82, 
	HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_req     = 0x3B03, 
	HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_resp    = 0x3B83, 
	HUITP_MSGID_uni_bfsc_comb_scale_max,

  //云控锁-锁-旧系统兼容
	HUITP_MSGID_uni_ccl_lock_old_min                 = 0x4000,  
	HUITP_MSGID_uni_ccl_lock_old_req                 = 0x4000,  
	HUITP_MSGID_uni_ccl_lock_old_resp                = 0x4080, 
	HUITP_MSGID_uni_ccl_lock_old_report              = 0x4081, 
	HUITP_MSGID_uni_ccl_lock_old_confirm             = 0x4001, 
	HUITP_MSGID_uni_ccl_lock_old_auth_inq            = 0x4090, 
	HUITP_MSGID_uni_ccl_lock_old_auth_resp           = 0x4010,
	HUITP_MSGID_uni_ccl_lock_old_max,

  //云控锁-门
	HUITP_MSGID_uni_ccl_door_min                     = 0x4100, 
	HUITP_MSGID_uni_ccl_door_req                     = 0x4100, 
	HUITP_MSGID_uni_ccl_door_resp                    = 0x4180, 
	HUITP_MSGID_uni_ccl_door_report                  = 0x4181, 
	HUITP_MSGID_uni_ccl_door_confirm                 = 0x4101, 
	HUITP_MSGID_uni_ccl_door_max,

  //云控锁-RFID模块
	HUITP_MSGID_uni_ccl_rfid_min                     = 0x4200, 
	HUITP_MSGID_uni_ccl_rfid_req                     = 0x4200, 
	HUITP_MSGID_uni_ccl_rfid_resp                    = 0x4280, 
	HUITP_MSGID_uni_ccl_rfid_report                  = 0x4281, 
	HUITP_MSGID_uni_ccl_rfid_confirm                 = 0x4201, 
	HUITP_MSGID_uni_ccl_rfid_max,
	
	//云控锁-BLE模块
	HUITP_MSGID_uni_ccl_ble_min                      = 0x4300, 
	HUITP_MSGID_uni_ccl_ble_req                      = 0x4300, 
	HUITP_MSGID_uni_ccl_ble_resp                     = 0x4380, 
	HUITP_MSGID_uni_ccl_ble_report                   = 0x4381, 
	HUITP_MSGID_uni_ccl_ble_confirm                  = 0x4301, 
	HUITP_MSGID_uni_ccl_ble_max,

  //云控锁-GPRS模块
	HUITP_MSGID_uni_ccl_gprs_min                     = 0x4400, 
	HUITP_MSGID_uni_ccl_gprs_req                     = 0x4400, 
	HUITP_MSGID_uni_ccl_gprs_resp                    = 0x4480, 
	HUITP_MSGID_uni_ccl_gprs_report                  = 0x4481, 
	HUITP_MSGID_uni_ccl_gprs_confirm                 = 0x4401, 
	HUITP_MSGID_uni_ccl_gprs_max,
 
  //云控锁-电池模块
	HUITP_MSGID_uni_ccl_battery_min                  = 0x4500, 
	HUITP_MSGID_uni_ccl_battery_req                  = 0x4500, 
	HUITP_MSGID_uni_ccl_battery_resp                 = 0x4580, 
	HUITP_MSGID_uni_ccl_battery_report               = 0x4581, 
	HUITP_MSGID_uni_ccl_battery_confirm              = 0x4501, 
	HUITP_MSGID_uni_ccl_battery_max,	

  //云控锁-震动
	HUITP_MSGID_uni_ccl_shake_min                    = 0x4600, 
	HUITP_MSGID_uni_ccl_shake_req                    = 0x4600, 
	HUITP_MSGID_uni_ccl_shake_resp                   = 0x4680, 
	HUITP_MSGID_uni_ccl_shake_report                 = 0x4681, 
	HUITP_MSGID_uni_ccl_shake_confirm                = 0x4601, 
	HUITP_MSGID_uni_ccl_shake_max,

  //云控锁-烟雾
	HUITP_MSGID_uni_ccl_smoke_min                    = 0x4700, 
	HUITP_MSGID_uni_ccl_smoke_req                    = 0x4700, 
	HUITP_MSGID_uni_ccl_smoke_resp                   = 0x4780, 
	HUITP_MSGID_uni_ccl_smoke_report                 = 0x4781, 
	HUITP_MSGID_uni_ccl_smoke_confirm                = 0x4701, 
	HUITP_MSGID_uni_ccl_smoke_max,

  //云控锁-水浸
	HUITP_MSGID_uni_ccl_water_min                    = 0x4800, 
	HUITP_MSGID_uni_ccl_water_req                    = 0x4800, 
	HUITP_MSGID_uni_ccl_water_resp                   = 0x4880, 
	HUITP_MSGID_uni_ccl_water_report                 = 0x4881, 
	HUITP_MSGID_uni_ccl_water_confirm                = 0x4801, 
	HUITP_MSGID_uni_ccl_water_max,

  //云控锁-温度
	HUITP_MSGID_uni_ccl_temp_min                     = 0x4900, 
	HUITP_MSGID_uni_ccl_temp_req                     = 0x4900, 
	HUITP_MSGID_uni_ccl_temp_resp                    = 0x4980, 
	HUITP_MSGID_uni_ccl_temp_report                  = 0x4981, 
	HUITP_MSGID_uni_ccl_temp_confirm                 = 0x4901, 
	HUITP_MSGID_uni_ccl_temp_max,

  //云控锁-湿度
	HUITP_MSGID_uni_ccl_humid_min                    = 0x4A00, 
	HUITP_MSGID_uni_ccl_humid_req                    = 0x4A00, 
	HUITP_MSGID_uni_ccl_humid_resp                   = 0x4A80, 
	HUITP_MSGID_uni_ccl_humid_report                 = 0x4A81, 
	HUITP_MSGID_uni_ccl_humid_confirm                = 0x4A01, 
	HUITP_MSGID_uni_ccl_humid_max,
  
  //云控锁-倾倒
	HUITP_MSGID_uni_ccl_fall_min                     = 0x4B00, 
	HUITP_MSGID_uni_ccl_fall_req                     = 0x4B00, 
	HUITP_MSGID_uni_ccl_fall_resp                    = 0x4B80, 
	HUITP_MSGID_uni_ccl_fall_report                  = 0x4B81, 
	HUITP_MSGID_uni_ccl_fall_confirm                 = 0x4B01,
	HUITP_MSGID_uni_ccl_fall_max,

  //云控锁-状态聚合-旧系统兼容
	HUITP_MSGID_uni_ccl_state_old_min                = 0x4C00, 
	HUITP_MSGID_uni_ccl_state_old_req                = 0x4C00, 
	HUITP_MSGID_uni_ccl_state_old_resp               = 0x4C80, 
	HUITP_MSGID_uni_ccl_state_old_report             = 0x4C81, 
	HUITP_MSGID_uni_ccl_state_old_confirm            = 0x4C01,
	HUITP_MSGID_uni_ccl_state_old_pic_report         = 0x4C82, 
	HUITP_MSGID_uni_ccl_state_old_pic_confirm        = 0x4C02,
	HUITP_MSGID_uni_ccl_state_old_max,

  //云控锁-锁
	HUITP_MSGID_uni_ccl_lock_min                     = 0x4D00,  
	HUITP_MSGID_uni_ccl_lock_req                     = 0x4D00,  
	HUITP_MSGID_uni_ccl_lock_resp                    = 0x4D80, 
	HUITP_MSGID_uni_ccl_lock_report                  = 0x4D81, 
	HUITP_MSGID_uni_ccl_lock_confirm                 = 0x4D01, 
	HUITP_MSGID_uni_ccl_lock_auth_inq                = 0x4D90, 
	HUITP_MSGID_uni_ccl_lock_auth_resp               = 0x4D10,
	HUITP_MSGID_uni_ccl_lock_max,

  //云控锁-状态聚合
	HUITP_MSGID_uni_ccl_state_min                    = 0x4E00, 
	HUITP_MSGID_uni_ccl_state_req                    = 0x4E00, 
	HUITP_MSGID_uni_ccl_state_resp                   = 0x4E80, 
	HUITP_MSGID_uni_ccl_state_report                 = 0x4E81, 
	HUITP_MSGID_uni_ccl_state_confirm                = 0x4E01,
	HUITP_MSGID_uni_ccl_state_pic_report         		 = 0x4E82, 
	HUITP_MSGID_uni_ccl_state_pic_confirm        		 = 0x4E02,	
	HUITP_MSGID_uni_ccl_state_max,

	//串口读取命令/返回结果
	HUITP_MSGID_uni_itf_sps_min                      = 0x5000, 
	HUITP_MSGID_uni_itf_sps_req                      = 0x5000, 
	HUITP_MSGID_uni_itf_sps_resp                     = 0x5080, 
	HUITP_MSGID_uni_itf_sps_report                   = 0x5001, 
	HUITP_MSGID_uni_itf_sps_confirm                  = 0x5081, 
	HUITP_MSGID_uni_itf_sps_max,

  //ADC读取命令/返回结果
	HUITP_MSGID_uni_itf_adc_min                      = 0x5100, 
	HUITP_MSGID_uni_itf_adc_req                      = 0x5100, 
	HUITP_MSGID_uni_itf_adc_resp                     = 0x5180, 
	HUITP_MSGID_uni_itf_adc_report                   = 0x5181, 
	HUITP_MSGID_uni_itf_adc_confirm                  = 0x5101, 
	HUITP_MSGID_uni_itf_adc_max,

  //DAC读取命令/返回结果
	HUITP_MSGID_uni_itf_dac_min                      = 0x5200, 
	HUITP_MSGID_uni_itf_dac_req                      = 0x5200, 
	HUITP_MSGID_uni_itf_dac_resp                     = 0x5280, 
	HUITP_MSGID_uni_itf_dac_report                   = 0x5281, 
	HUITP_MSGID_uni_itf_dac_confirm                  = 0x5201, 
	HUITP_MSGID_uni_itf_dac_max,
 
  //I2C读取命令/返回结果
	HUITP_MSGID_uni_itf_i2c_min                      = 0x5300, 
	HUITP_MSGID_uni_itf_i2c_req                      = 0x5300, 
	HUITP_MSGID_uni_itf_i2c_resp                     = 0x5380, 
	HUITP_MSGID_uni_itf_i2c_report                   = 0x5381, 
	HUITP_MSGID_uni_itf_i2c_confirm                  = 0x5301, 
	HUITP_MSGID_uni_itf_i2c_max,

  //PWM读取命令/返回结果
	HUITP_MSGID_uni_itf_pwm_min                      = 0x5400, 
	HUITP_MSGID_uni_itf_pwm_req                      = 0x5400, 
	HUITP_MSGID_uni_itf_pwm_resp                     = 0x5480, 
	HUITP_MSGID_uni_itf_pwm_report                   = 0x5481, 
	HUITP_MSGID_uni_itf_pwm_confirm                  = 0x5401, 
	HUITP_MSGID_uni_itf_pwm_max,

  //DI读取命令/返回结果
	HUITP_MSGID_uni_itf_di_min                       = 0x5500, 
	HUITP_MSGID_uni_itf_di_req                       = 0x5500, 
	HUITP_MSGID_uni_itf_di_resp                      = 0x5580, 
	HUITP_MSGID_uni_itf_di_report                    = 0x5581, 
	HUITP_MSGID_uni_itf_di_confirm                   = 0x5501, 
	HUITP_MSGID_uni_itf_di_max,

  //DO读取命令/返回结果
	HUITP_MSGID_uni_itf_do_min                       = 0x5600, 
	HUITP_MSGID_uni_itf_do_req                       = 0x5600, 
	HUITP_MSGID_uni_itf_do_resp                      = 0x5680, 
	HUITP_MSGID_uni_itf_do_report                    = 0x5681, 
	HUITP_MSGID_uni_itf_do_confirm                   = 0x5601, 
	HUITP_MSGID_uni_itf_do_max,

  //CAN读取命令/返回结果
	HUITP_MSGID_uni_itf_can_min                      = 0x5700, 
	HUITP_MSGID_uni_itf_can_req                      = 0x5700, 
	HUITP_MSGID_uni_itf_can_resp                     = 0x5780, 
	HUITP_MSGID_uni_itf_can_report                   = 0x5781, 
	HUITP_MSGID_uni_itf_can_confirm                  = 0x5701, 
	HUITP_MSGID_uni_itf_can_max,

  //SPI读取命令/返回结果
	HUITP_MSGID_uni_itf_spi_min                      = 0x5800, 
	HUITP_MSGID_uni_itf_spi_req                      = 0x5800, 
	HUITP_MSGID_uni_itf_spi_resp                     = 0x5880, 
	HUITP_MSGID_uni_itf_spi_report                   = 0x5881, 
	HUITP_MSGID_uni_itf_spi_confirm                  = 0x5801,  
	HUITP_MSGID_uni_itf_spi_max,

  //USB读取命令/返回结果
	HUITP_MSGID_uni_itf_usb_min                      = 0x5900, 
	HUITP_MSGID_uni_itf_usb_req                      = 0x5900, 
	HUITP_MSGID_uni_itf_usb_resp                     = 0x5980, 
	HUITP_MSGID_uni_itf_usb_report                   = 0x5981, 
	HUITP_MSGID_uni_itf_usb_confirm                  = 0x5901, 
	HUITP_MSGID_uni_itf_usb_max,

  //网口读取命令/返回结果
	HUITP_MSGID_uni_itf_eth_min                      = 0x5A00, 
	HUITP_MSGID_uni_itf_eth_req                      = 0x5A00, 
	HUITP_MSGID_uni_itf_eth_resp                     = 0x5A80, 
	HUITP_MSGID_uni_itf_eth_report                   = 0x5A81, 
	HUITP_MSGID_uni_itf_eth_confirm                  = 0x5A01, 
	HUITP_MSGID_uni_itf_eth_max,

  //485读取命令/返回结果 
	HUITP_MSGID_uni_itf_485_min                      = 0x5B00, 
	HUITP_MSGID_uni_itf_485_req                      = 0x5B00, 
	HUITP_MSGID_uni_itf_485_resp                     = 0x5B80, 
	HUITP_MSGID_uni_itf_485_report                   = 0x5B81, 
	HUITP_MSGID_uni_itf_485_confirm                  = 0x5B01, 
	HUITP_MSGID_uni_itf_485_max,

  //软件清单
	HUITP_MSGID_uni_inventory_min                    = 0xA000,	
	HUITP_MSGID_uni_inventory_req                    = 0xA000,	
	HUITP_MSGID_uni_inventory_resp                   = 0xA080, 
	HUITP_MSGID_uni_inventory_report                 = 0xA081, 
	HUITP_MSGID_uni_inventory_confirm                = 0xA001, 
	HUITP_MSGID_uni_inventory_max,

  //软件版本体
	HUITP_MSGID_uni_sw_package_min                   = 0xA100,	
	HUITP_MSGID_uni_sw_package_req                   = 0xA100,	
	HUITP_MSGID_uni_sw_package_resp                  = 0xA180,	 
	HUITP_MSGID_uni_sw_package_report                = 0xA181,	 
	HUITP_MSGID_uni_sw_package_confirm               = 0xA101, 
	HUITP_MSGID_uni_sw_package_max,

  //ALARM REPORT
	HUITP_MSGID_uni_alarm_info_min                   = 0xB000, 
	HUITP_MSGID_uni_alarm_info_req                   = 0xB000, 
	HUITP_MSGID_uni_alarm_info_resp                  = 0xB080, 
	HUITP_MSGID_uni_alarm_info_report                = 0xB081, 
	HUITP_MSGID_uni_alarm_info_confirm               = 0xB001, 
	HUITP_MSGID_uni_alarm_info_max,

  //PM Report
	HUITP_MSGID_uni_performance_info_min             = 0xB100, 
	HUITP_MSGID_uni_performance_info_req             = 0xB100, 
	HUITP_MSGID_uni_performance_info_resp            = 0xB180, 
	HUITP_MSGID_uni_performance_info_report          = 0xB181, 
	HUITP_MSGID_uni_performance_info_confirm         = 0xB101, 
	HUITP_MSGID_uni_performance_info_max,

  //设备基本信息
	HUITP_MSGID_uni_equipment_info_min               = 0xF000,	
	HUITP_MSGID_uni_equipment_info_req               = 0xF000,	
	HUITP_MSGID_uni_equipment_info_resp              = 0xF080, 
	HUITP_MSGID_uni_equipment_info_report            = 0xF081, 
	HUITP_MSGID_uni_equipment_info_confirm           = 0xF001, 
	HUITP_MSGID_uni_equipment_info_max,

  //个人基本信息
	HUITP_MSGID_uni_personal_info_min                = 0xF100,	
	HUITP_MSGID_uni_personal_info_req                = 0xF100,	
	HUITP_MSGID_uni_personal_info_resp               = 0xF180, 
	HUITP_MSGID_uni_personal_info_report             = 0xF181,	 
	HUITP_MSGID_uni_personal_info_confirm            = 0xF101,	 
	HUITP_MSGID_uni_personal_info_max,

	//时间同步
	HUITP_MSGID_uni_time_sync_min                    = 0xF200,
	HUITP_MSGID_uni_time_sync_req                    = 0xF200,	
	HUITP_MSGID_uni_time_sync_resp                   = 0xF280,	 
	HUITP_MSGID_uni_time_sync_report                 = 0xF281,	 
	HUITP_MSGID_uni_time_sync_confirm                = 0xF201, 
	HUITP_MSGID_uni_time_sync_max,

  //读取数据
	HUITP_MSGID_uni_general_read_data_min            = 0xF300,	
	HUITP_MSGID_uni_general_read_data_req            = 0xF300,	
	HUITP_MSGID_uni_general_read_data_resp           = 0xF380,	 
	HUITP_MSGID_uni_general_read_data_report         = 0xF381, 
	HUITP_MSGID_uni_general_read_data_confirm        = 0xF301, 
	HUITP_MSGID_uni_general_read_data_max,

  //定时闹钟及久坐提醒
	HUITP_MSGID_uni_clock_timeout_min                = 0xF400,
	HUITP_MSGID_uni_clock_timeout_req                = 0xF400,	
	HUITP_MSGID_uni_clock_timeout_resp               = 0xF480,	 
	HUITP_MSGID_uni_clock_timeout_report             = 0xF481, 
	HUITP_MSGID_uni_clock_timeout_confirm            = 0xF401, 
	HUITP_MSGID_uni_clock_timeout_max,

  //同步充电，双击情况
	HUITP_MSGID_uni_sync_charging_min                = 0xF500,	
	HUITP_MSGID_uni_sync_charging_req                = 0xF500,	
	HUITP_MSGID_uni_sync_charging_resp               = 0xF580,	 
	HUITP_MSGID_uni_sync_charging_report             = 0xF581, 
	HUITP_MSGID_uni_sync_charging_confirm            = 0xF501, 
	HUITP_MSGID_uni_sync_charging_max,

  //同步通知信息
	HUITP_MSGID_uni_sync_trigger_min                 = 0xF600,	
	HUITP_MSGID_uni_sync_trigger_req                 = 0xF600,	
	HUITP_MSGID_uni_sync_trigger_resp                = 0xF680,	 
	HUITP_MSGID_uni_sync_trigger_report              = 0xF681, 
	HUITP_MSGID_uni_sync_trigger_confirm             = 0xF601, 
	HUITP_MSGID_uni_sync_trigger_max,

  //CMD CONTROL
	HUITP_MSGID_uni_cmd_ctrl_min                     = 0xFD00,  
	HUITP_MSGID_uni_cmd_ctrl_req                     = 0xFD00,  
	HUITP_MSGID_uni_cmd_ctrl_resp                    = 0xFD80,  
	HUITP_MSGID_uni_cmd_ctrl_report                  = 0xFD81, 
	HUITP_MSGID_uni_cmd_ctrl_confirm                 = 0xFD01,
	HUITP_MSGID_uni_cmd_ctrl_max,	

  //心跳
	HUITP_MSGID_uni_heart_beat_min                   = 0xFE00, 
	HUITP_MSGID_uni_heart_beat_req                   = 0xFE00, 
	HUITP_MSGID_uni_heart_beat_resp                  = 0xFE80, 
	HUITP_MSGID_uni_heart_beat_report                = 0xFE81,  
	HUITP_MSGID_uni_heart_beat_confirm               = 0xFE01, 
	HUITP_MSGID_uni_heart_beat_max,

  //无效
	HUITP_MSGID_uni_max,	
	HUITP_MSGID_uni_null                             = 0xFF,	
}HuitpMsgIdUniDefination;


//标准IE信息单元字统一定义
typedef enum
{
	HUITP_IEID_uni_min                              = 0x0000, 
	HUITP_IEID_uni_none                             = 0x0000,

	//公共IE区域
	HUITP_IEID_uni_com_req                          = 0x0001,
	HUITP_IEID_uni_com_resp                         = 0x0002,
	HUITP_IEID_uni_com_report                       = 0x0003,
	HUITP_IEID_uni_com_confirm                      = 0x0004,
	HUITP_IEID_uni_com_state                        = 0x0010,
	HUITP_IEID_uni_com_auth                         = 0x0011,
	HUITP_IEID_uni_com_warning                      = 0x0012,
	HUITP_IEID_uni_com_action                       = 0x0013,
	HUITP_IEID_uni_com_switch_onoff                 = 0x0014,
	HUITP_IEID_uni_com_command                      = 0x0015,
	HUITP_IEID_uni_com_back_type                    = 0x0016,
	HUITP_IEID_uni_com_equp_id                      = 0x0020,
	HUITP_IEID_uni_com_format_type                  = 0x0021,
	HUITP_IEID_uni_com_work_cycle                   = 0x0022,
	HUITP_IEID_uni_com_sample_cycle                 = 0x0023,
	HUITP_IEID_uni_com_sample_number                = 0x0024,
	HUITP_IEID_uni_com_unix_time                    = 0x0025,
	HUITP_IEID_uni_com_ymd_time                     = 0x0026,
	HUITP_IEID_uni_com_ntimes                       = 0x0027,
	HUITP_IEID_uni_com_gps_x                        = 0x0028,
	HUITP_IEID_uni_com_gps_y                        = 0x0029,
	HUITP_IEID_uni_com_gps_z                        = 0x002A,
	HUITP_IEID_uni_com_gps_direction                = 0x002B,
	HUITP_IEID_uni_com_grade                        = 0x002C,
	HUITP_IEID_uni_com_percentage                   = 0x002E,
	HUITP_IEID_uni_com_modbus_address               = 0x002F,	
	HUITP_IEID_uni_com_file_name                    = 0x0030,
	HUITP_IEID_uni_com_http_link                    = 0x0031,
	HUITP_IEID_uni_com_segment_total                = 0x0032,
	HUITP_IEID_uni_com_segment_index                = 0x0033,

	//血糖
	HUITP_IEID_uni_blood_glucose_min                = 0x0100, 
	HUITP_IEID_uni_blood_glucose_value              = 0x0100, 
	HUITP_IEID_uni_blood_glucose_max,
	
	//单次运动
	HUITP_IEID_uni_single_sports_min                = 0x0200, 
	HUITP_IEID_uni_single_sports_value              = 0x0200, 
	HUITP_IEID_uni_single_sports_max,
	
	//单次睡眠
	HUITP_IEID_uni_single_sleep_min                 = 0x0300, 
	HUITP_IEID_uni_single_sleep_value               = 0x0300, 
	HUITP_IEID_uni_single_sleep_max,
		
	//体脂
	HUITP_IEID_uni_body_fat_min                     = 0x0400, 
	HUITP_IEID_uni_body_fat_value                   = 0x0400, 
	HUITP_IEID_uni_body_fat_max,
	
	//血压
	HUITP_IEID_uni_blood_pressure_min               = 0x0500, 
	HUITP_IEID_uni_blood_pressure_value             = 0x0500, 
	HUITP_IEID_uni_blood_pressure_max,

  //跑步机数据上报
	HUITP_IEID_uni_runner_machine_rep_min           = 0x0A00,
	HUITP_IEID_uni_runner_machine_rep_value         = 0x0A00,
	HUITP_IEID_uni_runner_machine_rep_max,
  
  //跑步机任务控制
	HUITP_IEID_uni_runner_machine_ctrl_min          = 0x0B00, 
	HUITP_IEID_uni_runner_machine_ctrl_value        = 0x0B00, 
	HUITP_IEID_uni_runner_machine_ctrl_max,

  //GPS地址
	HUITP_IEID_uni_gps_specific_min                 = 0x0C00, 
	HUITP_IEID_uni_gps_specific_x                   = 0x0C00, 
	HUITP_IEID_uni_gps_specific_y                   = 0x0C01, 
	HUITP_IEID_uni_gps_specific_z                   = 0x0C02, 
	HUITP_IEID_uni_gps_sensor_max,

  //IHU与IAU之间控制命令
	HUITP_IEID_uni_iau_ctrl_min                     = 0x1000, 
	HUITP_IEID_uni_iau_ctrl_value                   = 0x1000, 
	HUITP_IEID_uni_iau_ctrl_max,

  //电磁辐射强度
	HUITP_IEID_uni_emc_data_min                     = 0x2000, 
	HUITP_IEID_uni_emc_data_value                   = 0x2000, 
	HUITP_IEID_uni_emc_data_max,

  //电磁辐射剂量
	HUITP_IEID_uni_emc_accu_min                     = 0x2100, 
	HUITP_IEID_uni_emc_accu_value                   = 0x2100, 
	HUITP_IEID_uni_emc_accu_max,

  //一氧化碳
	HUITP_IEID_uni_co_min                           = 0x2200, 
	HUITP_IEID_uni_co_value                         = 0x2200, 
	HUITP_IEID_uni_co_max,

  //甲醛HCHO
	HUITP_IEID_uni_formaldehyde_min                 = 0x2300, 
	HUITP_IEID_uni_formaldehyde_value               = 0x2300, 
	HUITP_IEID_uni_hcho_value                       = 0x2301, 
	HUITP_IEID_uni_formaldehyde_max,

  //酒精
	HUITP_IEID_uni_alcohol_min                      = 0x2400, 
	HUITP_IEID_uni_alcohol_value                    = 0x2400, 
	HUITP_IEID_uni_alcohol_max,
 
  //PM1/2.5/10
	HUITP_IEID_uni_pm25_min                         = 0x2500,
	HUITP_IEID_uni_pm01_value                       = 0x2500, 
	HUITP_IEID_uni_pm25_value                       = 0x2501, 
	HUITP_IEID_uni_pm10_value                       = 0x2502, 
	HUITP_IEID_uni_pm25_max,

  //风速Wind Speed
	HUITP_IEID_uni_windspd_min                      = 0x2600, 
	HUITP_IEID_uni_windspd_value                    = 0x2600, 
	HUITP_IEID_uni_windspd_max,

	//风向Wind Direction
	HUITP_IEID_uni_winddir_min                      = 0x2700, 
	HUITP_IEID_uni_winddir_value                    = 0x2700, 
	HUITP_IEID_uni_winddir_max,
	
	//温度Temperature
	HUITP_IEID_uni_temp_min                         = 0x2800, 
	HUITP_IEID_uni_temp_value                       = 0x2800, 
	HUITP_IEID_uni_temp_max,

  //湿度Humidity
	HUITP_IEID_uni_humid_min                        = 0x2900, 
	HUITP_IEID_uni_humid_value                      = 0x2900, 
	HUITP_IEID_uni_humid_max,

  //气压Air pressure
	HUITP_IEID_uni_airprs_min                       = 0x2A00, 
	HUITP_IEID_uni_airprs_value                     = 0x2A00, 
	HUITP_IEID_uni_airprs_max,

  //噪声Noise
	HUITP_IEID_uni_noise_min                        = 0x2B00, 
	HUITP_IEID_uni_noise_value                      = 0x2B00, 
	HUITP_IEID_uni_noise_max,

  //相机Camer or audio high speed
	HUITP_IEID_uni_hsmmp_min                        = 0x2C00, 
	HUITP_IEID_uni_hsmmp_value                      = 0x2C00, 
	HUITP_IEID_uni_hsmmp_max,

  //声音
	HUITP_IEID_uni_audio_min                        = 0x2D00, 
	HUITP_IEID_uni_audio_value                      = 0x2D00, 
	HUITP_IEID_uni_audio_max,

  //视频
	HUITP_IEID_uni_video_min                        = 0x2D00, 
	HUITP_IEID_uni_video_value                      = 0x2D00, 
	HUITP_IEID_uni_video_max,

  //图片
	HUITP_IEID_uni_picture_min                      = 0x2F00, 
	HUITP_IEID_uni_picture_value                    = 0x2F00,
	HUITP_IEID_uni_picture_segment                  = 0x2F01,
	HUITP_IEID_uni_picture_format                  	= 0x2F02,	
	HUITP_IEID_uni_picture_body                  		= 0x2F03,	
	HUITP_IEID_uni_picture_max,

  //扬尘监控系统
	HUITP_IEID_uni_ycjk_min                         = 0x3000, 
	HUITP_IEID_uni_ycjk_value                       = 0x3000, 
	HUITP_IEID_uni_ycjk_max,

  //水表
	HUITP_IEID_uni_water_meter_min                  = 0x3100, 
	HUITP_IEID_uni_water_meter_value                = 0x3100, 
	HUITP_IEID_uni_water_meter_max,

  //热表
	HUITP_IEID_uni_heat_meter_min                   = 0x3200, 
	HUITP_IEID_uni_heat_meter_value                 = 0x3200, 
	HUITP_IEID_uni_heat_meter_max,

  //气表
	HUITP_IEID_uni_gas_meter_min                    = 0x3300, 
	HUITP_IEID_uni_gas_meter_value                  = 0x3300, 
	HUITP_IEID_uni_gas_meter_max,

  //电表
	HUITP_IEID_uni_power_meter_min                  = 0x3400, 
	HUITP_IEID_uni_power_meter_value                = 0x3400, 
	HUITP_IEID_uni_power_meter_max,

  //光照强度
	HUITP_IEID_uni_light_strength_min               = 0x3500, 
	HUITP_IEID_uni_light_strength_value             = 0x3500, 
	HUITP_IEID_uni_light_strength_max,

  //有毒气体VOC
	HUITP_IEID_uni_toxicgas_min                     = 0x3600, 
	HUITP_IEID_uni_toxicgas_value                   = 0x3600, 
	HUITP_IEID_uni_toxicgas_max,

  //海拔高度
	HUITP_IEID_uni_altitude_min                     = 0x3700, 
	HUITP_IEID_uni_altitude_value                   = 0x3700, 
	HUITP_IEID_uni_altitude_max,

  //马达
	HUITP_IEID_uni_moto_min                         = 0x3800, 
	HUITP_IEID_uni_moto_value                       = 0x3800, 
	HUITP_IEID_uni_moto_max,

  //继电器
	HUITP_IEID_uni_switch_resistor_min              = 0x3900, 
	HUITP_IEID_uni_switch_resistor_value            = 0x3900, 
	HUITP_IEID_uni_switch_resistor_max,

  //导轨传送带
	HUITP_IEID_uni_transporter_min                  = 0x3A00, 
	HUITP_IEID_uni_transporter_value                = 0x3A00, 
	HUITP_IEID_uni_transporter_max,

  //组合秤BFSC
	HUITP_IEID_uni_bfsc_comb_scale_min              = 0x3B00, 
	HUITP_IEID_uni_scale_weight_value               = 0x3B00,
	HUITP_IEID_uni_scale_weight_cmd                 = 0x3B01,
	HUITP_IEID_uni_bfsc_comb_scale_max,

  //云控锁-锁-旧系统
	HUITP_IEID_uni_ccl_lock_old_min                 = 0x4000,  
	HUITP_IEID_uni_ccl_lock_old_state               = 0x4000,
	HUITP_IEID_uni_ccl_lock_old_auth_req            = 0x4001,
	HUITP_IEID_uni_ccl_lock_old_auth_resp           = 0x4002,
	HUITP_IEID_uni_ccl_lock_old_max,

  //云控锁-门
	HUITP_IEID_uni_ccl_door_min                     = 0x4100, 
	HUITP_IEID_uni_ccl_door_state                   = 0x4100, 
	HUITP_IEID_uni_ccl_door_max,

  //云控锁-RFID模块
	HUITP_IEID_uni_ccl_rfid_min                     = 0x4200, 
	HUITP_IEID_uni_ccl_rfid_value                   = 0x4200, 
	HUITP_IEID_uni_ccl_rfid_max,
	
	//云控锁-BLE模块
	HUITP_IEID_uni_ccl_ble_min                      = 0x4300, 
	HUITP_IEID_uni_ccl_ble_value                    = 0x4300, 
	HUITP_IEID_uni_ccl_ble_max,

  //云控锁-GPRS模块
	HUITP_IEID_uni_ccl_gprs_min                     = 0x4400, 
	HUITP_IEID_uni_ccl_rssi_value                   = 0x4400, 
	HUITP_IEID_uni_ccl_gprs_max,
 
  //云控锁-电池模块
	HUITP_IEID_uni_ccl_battery_min                  = 0x4500, 
	HUITP_IEID_uni_ccl_bat_state                    = 0x4500,
	HUITP_IEID_uni_ccl_bat_value                    = 0x4501,	
	HUITP_IEID_uni_ccl_battery_max,	

  //云控锁-震动
	HUITP_IEID_uni_ccl_shake_min                    = 0x4600, 
	HUITP_IEID_uni_ccl_shake_state                  = 0x4600, 
	HUITP_IEID_uni_ccl_shake_max,

  //云控锁-烟雾
	HUITP_IEID_uni_ccl_smoke_min                    = 0x4700, 
	HUITP_IEID_uni_ccl_smoke_state                  = 0x4700, 
	HUITP_IEID_uni_ccl_smoke_max,

  //云控锁-水浸
	HUITP_IEID_uni_ccl_water_min                    = 0x4800, 
	HUITP_IEID_uni_ccl_water_state                  = 0x4800, 
	HUITP_IEID_uni_ccl_water_max,

  //云控锁-温度
	HUITP_IEID_uni_ccl_temp_min                     = 0x4900, 
	HUITP_IEID_uni_ccl_temp_value                   = 0x4900, 
	HUITP_IEID_uni_ccl_temp_max,

  //云控锁-湿度
	HUITP_IEID_uni_ccl_humid_min                    = 0x4A00, 
	HUITP_IEID_uni_ccl_humid_value                  = 0x4A00, 
	HUITP_IEID_uni_ccl_humid_max,
  
  //云控锁-倾倒
	HUITP_IEID_uni_ccl_fall_min                     = 0x4B00, 
	HUITP_IEID_uni_ccl_fall_state                   = 0x4B00, 
	HUITP_IEID_uni_ccl_fall_max,

  //云控锁-状态聚合-旧系统
	HUITP_IEID_uni_ccl_state_old_min                = 0x4C00, 
	HUITP_IEID_uni_ccl_general_old_value1           = 0x4C00, 
	HUITP_IEID_uni_ccl_general_old_value2           = 0x4C01, 
	HUITP_IEID_uni_ccl_dcmi_old_value               = 0x4C02,
	HUITP_IEID_uni_ccl_report_old_type              = 0x4C03,
	HUITP_IEID_uni_ccl_state_old_max,
	
  //云控锁-锁
	HUITP_IEID_uni_ccl_lock_min                 		= 0x4D00,  
	HUITP_IEID_uni_ccl_lock_state               		= 0x4D00,
	HUITP_IEID_uni_ccl_lock_auth_req            		= 0x4D01,
	HUITP_IEID_uni_ccl_lock_auth_resp           		= 0x4D02,
	HUITP_IEID_uni_ccl_lock_max,	
	
  //云控锁-状态聚合
	HUITP_IEID_uni_ccl_state_min                    = 0x4E00, 
	HUITP_IEID_uni_ccl_general_value1               = 0x4E00, 
	HUITP_IEID_uni_ccl_general_value2               = 0x4E01, 
	HUITP_IEID_uni_ccl_dcmi_value                   = 0x4E02,
	HUITP_IEID_uni_ccl_report_type                  = 0x4E03,
	HUITP_IEID_uni_ccl_state_max,

  //串口读取命令/返回结果
	HUITP_IEID_uni_itf_sps_min                      = 0x5000, 
	HUITP_IEID_uni_itf_sps_value                    = 0x5000, 
	HUITP_IEID_uni_itf_sps_max,

  //ADC读取命令/返回结果
	HUITP_IEID_uni_itf_adc_min                      = 0x5100, 
	HUITP_IEID_uni_itf_adc_value                    = 0x5100, 
	HUITP_IEID_uni_itf_adc_max,

  //DAC读取命令/返回结果
	HUITP_IEID_uni_itf_dac_min                      = 0x5200, 
	HUITP_IEID_uni_itf_dac_value                    = 0x5200, 
	HUITP_IEID_uni_itf_dac_max,
 
  //I2C读取命令/返回结果
	HUITP_IEID_uni_itf_i2c_min                      = 0x5300, 
	HUITP_IEID_uni_itf_i2c_value                    = 0x5300, 
	HUITP_IEID_uni_itf_i2c_max,

  //PWM读取命令/返回结果
	HUITP_IEID_uni_itf_pwm_min                      = 0x5400, 
	HUITP_IEID_uni_itf_pwm_value                    = 0x5400, 
	HUITP_IEID_uni_itf_pwm_max,

  //DI读取命令/返回结果
	HUITP_IEID_uni_itf_di_min                       = 0x5500, 
	HUITP_IEID_uni_itf_di_value                     = 0x5500, 
	HUITP_IEID_uni_itf_di_max,

  //DO读取命令/返回结果
	HUITP_IEID_uni_itf_do_min                       = 0x5600, 
	HUITP_IEID_uni_itf_do_value                     = 0x5600, 
	HUITP_IEID_uni_itf_do_max,

  //CAN读取命令/返回结果
	HUITP_IEID_uni_itf_can_min                      = 0x5700, 
	HUITP_IEID_uni_itf_can_value                    = 0x5700, 
	HUITP_IEID_uni_itf_can_max,

  //SPI读取命令/返回结果
	HUITP_IEID_uni_itf_spi_min                      = 0x5800, 
	HUITP_IEID_uni_itf_spi_value                    = 0x5800, 
	HUITP_IEID_uni_itf_spi_max,

  //USB读取命令/返回结果
	HUITP_IEID_uni_itf_usb_min                      = 0x5900, 
	HUITP_IEID_uni_itf_usb_value                    = 0x5900, 
	HUITP_IEID_uni_itf_usb_max,

  //网口读取命令/返回结果
	HUITP_IEID_uni_itf_eth_min                      = 0x5A00, 
	HUITP_IEID_uni_itf_eth_value                    = 0x5A00, 
	HUITP_IEID_uni_itf_eth_max,

  //485读取命令/返回结果 
	HUITP_IEID_uni_itf_485_min                      = 0x5B00, 
	HUITP_IEID_uni_itf_485_value                    = 0x5B00, 
	HUITP_IEID_uni_itf_485_max,

  //软件清单
	HUITP_IEID_uni_inventory_min                    = 0xA000,	
	HUITP_IEID_uni_inventory_hw_type                = 0xA000, 
	HUITP_IEID_uni_inventory_hw_id                  = 0xA001, 
	HUITP_IEID_uni_inventory_sw_rel                 = 0xA002, 
	HUITP_IEID_uni_inventory_sw_ver                 = 0xA003, 
	HUITP_IEID_uni_inventory_max,

  //软件版本体
	HUITP_IEID_uni_sw_package_min                   = 0xA100,	
	HUITP_IEID_uni_sw_package_body                  = 0xA100, 
	HUITP_IEID_uni_sw_package_max,

  //ALARM REPORT
	HUITP_IEID_uni_alarm_info_min                   = 0xB000, 
	HUITP_IEID_uni_alarm_info_type                  = 0xB000, 
	HUITP_IEID_uni_alarm_info_value                 = 0xB001, 
	HUITP_IEID_uni_alarm_info_max,

  //PM Report
	HUITP_IEID_uni_performance_info_min             = 0xB100, 
	HUITP_IEID_uni_performance_info_type            = 0xB100, 
	HUITP_IEID_uni_performance_info_value           = 0xB101, 
	HUITP_IEID_uni_performance_info_max,

  //设备基本信息
	HUITP_IEID_uni_equipment_info_min               = 0xF000,	
	HUITP_IEID_uni_equipment_info_value             = 0xF000,	
	HUITP_IEID_uni_equipment_info_max,

  //个人基本信息
	HUITP_IEID_uni_personal_info_min                = 0xF100,	
	HUITP_IEID_uni_personal_info_value              = 0xF100,	
	HUITP_IEID_uni_personal_info_max,

	//时间同步
	HUITP_IEID_uni_time_sync_min                    = 0xF200,
	HUITP_IEID_uni_time_sync_value                  = 0xF200,
	HUITP_IEID_uni_time_sync_max,

  //读取数据
	HUITP_IEID_uni_general_read_data_min            = 0xF300,	
	HUITP_IEID_uni_general_read_data_value          = 0xF300,	
	HUITP_IEID_uni_general_read_data_max,

  //定时闹钟及久坐提醒
	HUITP_IEID_uni_clock_timeout_min                = 0xF400,
	HUITP_IEID_uni_clock_timeout_value              = 0xF400,
	HUITP_IEID_uni_clock_timeout_max,

  //同步充电，双击情况
	HUITP_IEID_uni_sync_charging_min                = 0xF500,	
	HUITP_IEID_uni_sync_charging_value              = 0xF500,	
	HUITP_IEID_uni_sync_charging_max,

  //同步通知信息
	HUITP_IEID_uni_sync_trigger_min                 = 0xF600,	
	HUITP_IEID_uni_sync_trigger_value               = 0xF600,	
	HUITP_IEID_uni_sync_trigger_max,

  //CMD CONTROL
	HUITP_IEID_uni_cmd_ctrl_min                     = 0xFD00,  
	HUITP_IEID_uni_cmd_ctrl_send                    = 0xFD00,  
	HUITP_IEID_uni_cmd_ctrl_confirm                 = 0xFD01,  
	HUITP_IEID_uni_cmd_ctrl_max,	

  //心跳
	HUITP_IEID_uni_heart_beat_min                   = 0xFE00, 
	HUITP_IEID_uni_heart_beat_ping                  = 0xFE00, 
	HUITP_IEID_uni_heart_beat_pong                  = 0xFE01, 
	HUITP_IEID_uni_heart_beat_max,

	//最大值
	HUITP_IEID_uni_max,

  //无效
	HUITP_IEID_uni_null                             = 0xFFFF,
}HuitpIeIdUniDefination;


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP信息字段IE的结果定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//公共IE区域
//HUITP_IEID_uni_com_req                          = 0x0001,
typedef struct StrIe_HUITP_IEID_uni_com_req
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comReq;
}StrIe_HUITP_IEID_uni_com_req_t;
#define HUITP_IEID_UNI_COM_REQUEST_NULL 0
#define HUITP_IEID_UNI_COM_REQUEST_YES 1
#define HUITP_IEID_UNI_COM_REQUEST_NO 2
#define HUITP_IEID_UNI_COM_REQUEST_INVALID 0xFF

//HUITP_IEID_uni_com_resp                         = 0x0002,
typedef struct StrIe_HUITP_IEID_uni_com_resp
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comResp;
}StrIe_HUITP_IEID_uni_com_resp_t;
#define HUITP_IEID_UNI_COM_RESPONSE_NULL 0
#define HUITP_IEID_UNI_COM_RESPONSE_YES 1
#define HUITP_IEID_UNI_COM_RESPONSE_NO 2
#define HUITP_IEID_UNI_COM_RESPONSE_INVALID 0xFF

//HUITP_IEID_uni_com_report                       = 0x0003,
typedef struct StrIe_HUITP_IEID_uni_com_report
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comReport;
}StrIe_HUITP_IEID_uni_com_report_t;
#define HUITP_IEID_UNI_COM_REPORT_NULL 0
#define HUITP_IEID_UNI_COM_REPORT_YES 1
#define HUITP_IEID_UNI_COM_REPORT_NO 2
#define HUITP_IEID_UNI_COM_REPORT_INVALID 0xFF

//HUITP_IEID_uni_com_confirm                      = 0x0004,
typedef struct StrIe_HUITP_IEID_uni_com_confirm
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comConfirm;
}StrIe_HUITP_IEID_uni_com_confirm_t;
#define HUITP_IEID_UNI_COM_CONFIRM_NULL 0
#define HUITP_IEID_UNI_COM_CONFIRM_POSITIVE 1
#define HUITP_IEID_UNI_COM_CONFIRM_NEGATIVE 2
#define HUITP_IEID_UNI_COM_CONFIRM_YES 1
#define HUITP_IEID_UNI_COM_CONFIRM_NO 2
#define HUITP_IEID_UNI_COM_CONFIRM_INVALID 0xFF

//HUITP_IEID_uni_com_state                        = 0x0010,
typedef struct StrIe_HUITP_IEID_uni_com_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comState;
}StrIe_HUITP_IEID_uni_com_state_t;
#define HUITP_IEID_UNI_COM_STATE_NULL 0
#define HUITP_IEID_UNI_COM_STATE_ACTIVE 1
#define HUITP_IEID_UNI_COM_STATE_DEACTIVE 2
#define HUITP_IEID_UNI_COM_STATE_YES 1
#define HUITP_IEID_UNI_COM_STATE_NO 2
#define HUITP_IEID_UNI_COM_STATE_INVALID 0xFF

//HUITP_IEID_uni_com_auth                         = 0x0011,
typedef struct StrIe_HUITP_IEID_uni_com_auth
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comAuth;
}StrIe_HUITP_IEID_uni_com_auth_t;
#define HUITP_IEID_UNI_COM_AUTH_NULL 0
#define HUITP_IEID_UNI_COM_AUTH_POSITIVE 1
#define HUITP_IEID_UNI_COM_AUTH_NEGATIVE 2
#define HUITP_IEID_UNI_COM_AUTH_INVALID 0xFF

//HUITP_IEID_uni_com_warning                      = 0x0012,
typedef struct StrIe_HUITP_IEID_uni_com_warning
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comWarning;
}StrIe_HUITP_IEID_uni_com_warning_t;
#define HUITP_IEID_UNI_COM_WARNING_NULL 0
#define HUITP_IEID_UNI_COM_WARNING_YES 1
#define HUITP_IEID_UNI_COM_WARNING_NO 2
#define HUITP_IEID_UNI_COM_WARNING_INVALID 0xFF

//HUITP_IEID_uni_com_action                       = 0x0013,
typedef struct StrIe_HUITP_IEID_uni_com_action
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comAction;
}StrIe_HUITP_IEID_uni_com_action_t;
#define HUITP_IEID_UNI_COM_ACTION_NULL 0
#define HUITP_IEID_UNI_COM_ACTION_YES 1
#define HUITP_IEID_UNI_COM_ACTION_NO 2
#define HUITP_IEID_UNI_COM_ACTION_INVALID 0xFF

//HUITP_IEID_uni_com_switch_onoff                       = 0x0014,
typedef struct StrIe_HUITP_IEID_uni_com_switch_onoff
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comSwitch;
}StrIe_HUITP_IEID_uni_com_switch_onoff_t;
#define HUITP_IEID_UNI_COM_SWITCH_ONOFF_NULL 0
#define HUITP_IEID_UNI_COM_SWITCH_ONOFF_YES 1
#define HUITP_IEID_UNI_COM_SWITCH_ONOFF_NO 2
#define HUITP_IEID_UNI_COM_SWITCH_ONOFF_INVALID 0xFF

//HUITP_IEID_uni_com_command                      = 0x0015,
typedef struct StrIe_HUITP_IEID_uni_com_command
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comCommand;
}StrIe_HUITP_IEID_uni_com_command_t;
#define HUITP_IEID_UNI_COM_COMMAND_NULL 0
#define HUITP_IEID_UNI_COM_COMMAND_YES 1
#define HUITP_IEID_UNI_COM_COMMAND_NO 2
#define HUITP_IEID_UNI_COM_COMMAND_INVALID 0xFF

//HUITP_IEID_uni_com_back_type                    = 0x0016,
typedef struct StrIe_HUITP_IEID_uni_com_back_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comBackType;
}StrIe_HUITP_IEID_uni_com_back_type_t;
#define HUITP_IEID_UNI_COM_BACK_TYPE_NULL 0
#define HUITP_IEID_UNI_COM_BACK_TYPE_PERIOD 1
#define HUITP_IEID_UNI_COM_BACK_TYPE_ONE_TIME 2
#define HUITP_IEID_UNI_COM_BACK_TYPE_INVALID 0xFF

//HUITP_IEID_uni_com_equp_id                      = 0x0020,
typedef struct StrIe_HUITP_IEID_uni_com_equp_id
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comEqupIdValue;
}StrIe_HUITP_IEID_uni_com_equp_id_t;

//HUITP_IEID_uni_com_format_type                  = 0x0021,
typedef struct StrIe_HUITP_IEID_uni_com_format_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
}StrIe_HUITP_IEID_uni_com_format_type_t;
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_NULL 0
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_INT_ONLY 1
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF1 2
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2 3
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF3 4
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF4 5
#define HUITP_IEID_UNI_COM_FORMAT_TYPE_INVALID 0xFF

//HUITP_IEID_uni_com_work_cycle                   = 0x0022,
typedef struct StrIe_HUITP_IEID_uni_com_work_cycle
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comWorkCycleValue;
}StrIe_HUITP_IEID_uni_com_work_cycle_t;

//HUITP_IEID_uni_com_sample_cycle                 = 0x0023,
typedef struct StrIe_HUITP_IEID_uni_com_sample_cycle
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comSampleCycleValue;
}StrIe_HUITP_IEID_uni_com_sample_cycle_t;

//HUITP_IEID_uni_com_sample_number                = 0x0024,
typedef struct StrIe_HUITP_IEID_uni_com_sample_number
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comSampleNumberValue;
}StrIe_HUITP_IEID_uni_com_sample_number_t;

//HUITP_IEID_uni_com_unix_time                    = 0x0025,
typedef struct StrIe_HUITP_IEID_uni_com_unix_time
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 comUnixTimeValue;
}StrIe_HUITP_IEID_uni_com_unix_time_t;

//HUITP_IEID_uni_com_ymd_time                     = 0x0026,
typedef struct StrIe_HUITP_IEID_uni_com_ymd_time
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8 comYmdTimeValue[9];  //YY YY MM DD HH MM SS TS TS
}StrIe_HUITP_IEID_uni_com_ymd_time_t;

//HUITP_IEID_uni_com_ntimes                       = 0x0027,
typedef struct StrIe_HUITP_IEID_uni_com_ntimes
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comNtimesValue; 
}StrIe_HUITP_IEID_uni_com_ntimes_t;

//HUITP_IEID_uni_com_gps_x                        = 0x0028,
typedef struct StrIe_HUITP_IEID_uni_com_gps_x
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 comGpsxValue; 
}StrIe_HUITP_IEID_uni_com_gps_x_t;

//HUITP_IEID_uni_com_gps_y                        = 0x0029,
typedef struct StrIe_HUITP_IEID_uni_com_gps_y
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 comGpsyValue; 
}StrIe_HUITP_IEID_uni_com_gps_y_t;

//HUITP_IEID_uni_com_gps_z                        = 0x002A,
typedef struct StrIe_HUITP_IEID_uni_com_gps_z
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 comGpszValue; 
}StrIe_HUITP_IEID_uni_com_gps_z_t;

//HUITP_IEID_uni_com_gps_direction                = 0x002B,
typedef struct StrIe_HUITP_IEID_uni_com_gps_direction
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  comGpsDirection; 
}StrIe_HUITP_IEID_uni_com_gps_direction_t;
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_NULL 0
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_EAST 1
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_SOUTH 2
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_WEST 3
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_NORTH 4
#define HUITP_IEID_UNI_COM_GPS_DIRECTION_INVALID 0xFF

//HUITP_IEID_uni_com_grade                        = 0x002C,
typedef struct StrIe_HUITP_IEID_uni_com_grade
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comGradeValue;
}StrIe_HUITP_IEID_uni_com_grade_t;

//HUITP_IEID_uni_com_percentage                   = 0x002E,
typedef struct StrIe_HUITP_IEID_uni_com_percentage
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 comPercentageValue;
}StrIe_HUITP_IEID_uni_com_percentage_t;

//HUITP_IEID_uni_com_modbus_address               = 0x002F,
typedef struct StrIe_HUITP_IEID_uni_com_modbus_address
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 comModbusAddressValue;
}StrIe_HUITP_IEID_uni_com_modbus_address_t;

//HUITP_IEID_uni_com_file_name                    = 0x0030,
#define HUITP_IEID_UNI_COM_FILE_NAME_MAX_LEN  100
typedef struct StrIe_HUITP_IEID_uni_com_file_name
{
	UINT16 ieId;
	UINT16 ieLen;
	char   comFileName[HUITP_IEID_UNI_COM_FILE_NAME_MAX_LEN];
}StrIe_HUITP_IEID_uni_com_file_name_t;

//HUITP_IEID_uni_com_http_link                    = 0x0031,
#define HUITP_IEID_UNI_COM_HTTP_LINK_MAX_LEN  100
typedef struct StrIe_HUITP_IEID_uni_com_http_link
{
	UINT16 ieId;
	UINT16 ieLen;
	char   comHttpLink[HUITP_IEID_UNI_COM_HTTP_LINK_MAX_LEN];
}StrIe_HUITP_IEID_uni_com_http_link_t;

//HUITP_IEID_uni_com_segment_total                = 0x0032,
typedef struct StrIe_HUITP_IEID_uni_com_segment_total
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comSegmentTotalValue;
}StrIe_HUITP_IEID_uni_com_segment_total_t;

//HUITP_IEID_uni_com_segment_index                = 0x0033,
typedef struct StrIe_HUITP_IEID_uni_com_segment_index
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 comSegmentIndexValue;
}StrIe_HUITP_IEID_uni_com_segment_index_t;

//血糖
//HUITP_IEID_uni_blood_glucose_min                = 0x0100,
//HUITP_IEID_uni_blood_glucose_value              = 0x0100,
typedef struct StrIe_HUITP_IEID_uni_blood_glucose_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 bloodGlucoseValue;
}StrIe_HUITP_IEID_uni_blood_glucose_value_t;

//HUITP_IEID_uni_blood_glucose_max,

//单次运动
//HUITP_IEID_uni_single_sports_min                = 0x0200, 
//HUITP_IEID_uni_single_sports_value              = 0x0200,
typedef struct StrIe_HUITP_IEID_uni_single_sports_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 singleSportsValue;
}StrIe_HUITP_IEID_uni_single_sports_value_t;

//HUITP_IEID_uni_single_sports_max,

//单次睡眠
//HUITP_IEID_uni_single_sleep_min                 = 0x0300, 
//HUITP_IEID_uni_single_sleep_value               = 0x0300, 
typedef struct StrIe_HUITP_IEID_uni_single_sleep_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 singleSleepValue;
}StrIe_HUITP_IEID_uni_single_sleep_value_t;

//HUITP_IEID_uni_single_sleep_max,
	
//体脂
//HUITP_IEID_uni_body_fat_min                     = 0x0400, 
//HUITP_IEID_uni_body_fat_value                   = 0x0400, 
typedef struct StrIe_HUITP_IEID_uni_body_fat_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 bodyFatValue;
}StrIe_HUITP_IEID_uni_body_fat_value_t;

//HUITP_IEID_uni_body_fat_max,

//血压
//HUITP_IEID_uni_blood_pressure_min               = 0x0500, 
//HUITP_IEID_uni_blood_pressure_value             = 0x0500, 
typedef struct StrIe_HUITP_IEID_uni_blood_pressure_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 bloodPressureValue;
}StrIe_HUITP_IEID_uni_blood_pressure_value_t;

//HUITP_IEID_uni_blood_pressure_max,

//跑步机数据上报
//HUITP_IEID_uni_runner_machine_rep_min           = 0x0A00,
//HUITP_IEID_uni_runner_machine_rep_value         = 0x0A00,
typedef struct StrIe_HUITP_IEID_uni_runner_machine_rep_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 runnerMachineRepValue;
}StrIe_HUITP_IEID_uni_runner_machine_rep_value_t;

//HUITP_IEID_uni_runner_machine_rep_max,

//跑步机任务控制
//HUITP_IEID_uni_runner_machine_ctrl_min          = 0x0B00, 
//HUITP_IEID_uni_runner_machine_ctrl_value        = 0x0B00, 
typedef struct StrIe_HUITP_IEID_uni_runner_machine_ctrl_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 runnerMachineCtrlValue;
}StrIe_HUITP_IEID_uni_runner_machine_ctrl_value_t;

//HUITP_IEID_uni_runner_machine_ctrl_max,

//GPS地址
//HUITP_IEID_uni_gps_specific_min                 = 0x0C00, 
//HUITP_IEID_uni_gps_specific_x                   = 0x0C00, 
typedef struct StrIe_HUITP_IEID_uni_gps_specific_x
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 gpsSpecificxValue;
}StrIe_HUITP_IEID_uni_gps_specific_x_t;

//HUITP_IEID_uni_gps_specific_y                   = 0x0C01, 
typedef struct y
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 gpsSpecificyValue;
}StrIe_HUITP_IEID_uni_gps_specific_y_t;

//HUITP_IEID_uni_gps_specific_z                   = 0x0C02, 
typedef struct StrIe_HUITP_IEID_uni_gps_specific_z
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 gpsSpecificzValue;
}StrIe_HUITP_IEID_uni_gps_specific_z_t;

//HUITP_IEID_uni_gps_sensor_max,

//IHU与IAU之间控制命令
//HUITP_IEID_uni_iau_ctrl_min                 = 0x1000, 
//HUITP_IEID_uni_iau_ctrl_value               = 0x1000, 
typedef struct StrIe_HUITP_IEID_uni_iau_ctrl_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 iauCtrlValue;
}StrIe_HUITP_IEID_uni_iau_ctrl_value_t;

//HUITP_IEID_uni_iau_ctrl_max,

//电磁辐射强度
//HUITP_IEID_uni_emc_data_min                     = 0x2000, 
//HUITP_IEID_uni_emc_data_value                   = 0x2000, 
typedef struct StrIe_HUITP_IEID_uni_emc_data_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 emcDataValue;
}StrIe_HUITP_IEID_uni_emc_data_value_t;

//HUITP_IEID_uni_emc_data_max,

//电磁辐射剂量
//HUITP_IEID_uni_emc_accu_min                     = 0x2100, 
//HUITP_IEID_uni_emc_accu_value                   = 0x2100, 
typedef struct StrIe_HUITP_IEID_uni_emc_accu_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  emcFormat;
	UINT16 emcAccuValue;
}StrIe_HUITP_IEID_uni_emc_accu_value_t;

//HUITP_IEID_uni_emc_accu_max,

//一氧化碳
//HUITP_IEID_uni_co_min                           = 0x2200, 
//HUITP_IEID_uni_co_value                         = 0x2200, 
typedef struct StrIe_HUITP_IEID_uni_co_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 coValue;
}StrIe_HUITP_IEID_uni_co_value_t;

//HUITP_IEID_uni_co_max,

//甲醛HCHO
//HUITP_IEID_uni_formaldehyde_min                 = 0x2300, 
//HUITP_IEID_uni_formaldehyde_value               = 0x2300,
typedef struct StrIe_HUITP_IEID_uni_formaldehyde_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  formaldehydeFormat;
	UINT16 formaldehydeValue;
}StrIe_HUITP_IEID_uni_formaldehyde_value_t;

//HUITP_IEID_uni_hcho_value                       = 0x2301, 
typedef struct StrIe_HUITP_IEID_uni_hcho_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 hchoValue;
}StrIe_HUITP_IEID_uni_hcho_value_t;

//HUITP_IEID_uni_formaldehyde_max,

//酒精
//HUITP_IEID_uni_alcohol_min                      = 0x2400, 
//HUITP_IEID_uni_alcohol_value                    = 0x2400, 
typedef struct StrIe_HUITP_IEID_uni_alcohol_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 alcoholValue;
}StrIe_HUITP_IEID_uni_alcohol_value_t;

//HUITP_IEID_uni_alcohol_max,

//PM1/2.5/10
//HUITP_IEID_uni_pm25_min                         = 0x2500,
//HUITP_IEID_uni_pm01_value                       = 0x2500,
typedef struct StrIe_HUITP_IEID_uni_pm01_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 pm01Value;
}StrIe_HUITP_IEID_uni_pm01_value_t;

//HUITP_IEID_uni_pm25_value                       = 0x2501,
typedef struct StrIe_HUITP_IEID_uni_pm25_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 pm25Value;
}StrIe_HUITP_IEID_uni_pm25_value_t;

//HUITP_IEID_uni_pm10_value                       = 0x2502,
typedef struct StrIe_HUITP_IEID_uni_pm10_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 pm10Value;
}StrIe_HUITP_IEID_uni_pm10_value_t;

//HUITP_IEID_uni_pm25_max,

//风速Wind Speed
//HUITP_IEID_uni_windspd_min                      = 0x2600, 
//HUITP_IEID_uni_windspd_value                    = 0x2600,
typedef struct StrIe_HUITP_IEID_uni_windspd_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 windspdValue;
}StrIe_HUITP_IEID_uni_windspd_value_t;

//HUITP_IEID_uni_windspd_max,

//风向Wind Direction
//HUITP_IEID_uni_winddir_min                      = 0x2700, 
//HUITP_IEID_uni_winddir_value                    = 0x2700,
typedef struct StrIe_HUITP_IEID_uni_winddir_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 winddirValue;
}StrIe_HUITP_IEID_uni_winddir_value_t;

//HUITP_IEID_uni_winddir_max,

//温度Temperature
//HUITP_IEID_uni_temp_min                         = 0x2800, 
//HUITP_IEID_uni_temp_value                       = 0x2800,
typedef struct StrIe_HUITP_IEID_uni_temp_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 tempValue;
}StrIe_HUITP_IEID_uni_temp_value_t;

//HUITP_IEID_uni_temp_max,

//湿度Humidity
//HUITP_IEID_uni_humid_min                        = 0x2900, 
//HUITP_IEID_uni_humid_value                      = 0x2900,
typedef struct StrIe_HUITP_IEID_uni_humid_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 humidValue;
}StrIe_HUITP_IEID_uni_humid_value_t;

//HUITP_IEID_uni_humid_max,

//气压Air pressure
//HUITP_IEID_uni_airprs_min                       = 0x2A00, 
//HUITP_IEID_uni_airprs_value                     = 0x2A00,
typedef struct StrIe_HUITP_IEID_uni_airprs_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 airprsValue;
}StrIe_HUITP_IEID_uni_airprs_value_t;

//HUITP_IEID_uni_airprs_max,

//噪声Noise
//HUITP_IEID_uni_noise_min                        = 0x2B00, 
//HUITP_IEID_uni_noise_value                      = 0x2B00,
typedef struct StrIe_HUITP_IEID_uni_noise_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 noiseValue;
}StrIe_HUITP_IEID_uni_noise_value_t;

//HUITP_IEID_uni_noise_max,

//相机Camer or audio high speed
//HUITP_IEID_uni_hsmmp_min                        = 0x2C00, 
//HUITP_IEID_uni_hsmmp_value                      = 0x2C00,
typedef struct StrIe_HUITP_IEID_uni_hsmmp_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 hsmmpValue;
}StrIe_HUITP_IEID_uni_hsmmp_value_t;

//HUITP_IEID_uni_hsmmp_max,

//声音
//HUITP_IEID_uni_audio_min                        = 0x2D00, 
//HUITP_IEID_uni_audio_value                      = 0x2D00,
typedef struct StrIe_HUITP_IEID_uni_audio_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 audioValue;
}StrIe_HUITP_IEID_uni_audio_value_t;

//HUITP_IEID_uni_audio_max,

//视频
//HUITP_IEID_uni_video_min                        = 0x2D00, 
//HUITP_IEID_uni_video_value                      = 0x2D00,
typedef struct StrIe_HUITP_IEID_uni_video_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 videoValue;
}StrIe_HUITP_IEID_uni_video_value_t;

//HUITP_IEID_uni_video_max,

//图片
//HUITP_IEID_uni_picture_min                      = 0x2F00, 
//HUITP_IEID_uni_picture_value                    = 0x2F00,
typedef struct StrIe_HUITP_IEID_uni_picture_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 pictureValue;
}StrIe_HUITP_IEID_uni_picture_value_t;

//HUITP_IEID_uni_picture_segment                  = 0x2F01,
typedef struct StrIe_HUITP_IEID_uni_picture_segment
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 segIndex;
	UINT16 segTotal;
}StrIe_HUITP_IEID_uni_picture_segment_t;

//HUITP_IEID_uni_picture_format                  	= 0x2F02,	
typedef struct StrIe_HUITP_IEID_uni_picture_format
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8 picFormat;
}StrIe_HUITP_IEID_uni_picture_format_t;
#define HUITP_IEID_UNI_PICTURE_FORMAT_NULL 0
#define HUITP_IEID_UNI_PICTURE_FORMAT_BITMAP 1
#define HUITP_IEID_UNI_PICTURE_FORMAT_JPG 2
#define HUITP_IEID_UNI_PICTURE_FORMAT_JPEG 3
#define HUITP_IEID_UNI_PICTURE_FORMAT_GIF 4
#define HUITP_IEID_UNI_PICTURE_FORMAT_TIFF 5
#define HUITP_IEID_UNI_PICTURE_FORMAT_RAW 6
#define HUITP_IEID_UNI_PICTURE_FORMAT_PCX 7
#define HUITP_IEID_UNI_PICTURE_FORMAT_TGA 8
#define HUITP_IEID_UNI_PICTURE_FORMAT_EXIF 9
#define HUITP_IEID_UNI_PICTURE_FORMAT_FPX 10
#define HUITP_IEID_UNI_PICTURE_FORMAT_SVG 11
#define HUITP_IEID_UNI_PICTURE_FORMAT_PSD 12
#define HUITP_IEID_UNI_PICTURE_FORMAT_CDR 13
#define HUITP_IEID_UNI_PICTURE_FORMAT_PCD 14
#define HUITP_IEID_UNI_PICTURE_FORMAT_DXF 15
#define HUITP_IEID_UNI_PICTURE_FORMAT_UFO 16
#define HUITP_IEID_UNI_PICTURE_FORMAT_EPS 17
#define HUITP_IEID_UNI_PICTURE_FORMAT_RNG 18
#define HUITP_IEID_UNI_PICTURE_FORMAT_INVALID 0xFF

//HUITP_IEID_uni_picture_body                  		= 0x2F03,
#define HUITP_IEID_UNI_PICTURE_BODY_DATA_LEN 400
typedef struct StrIe_HUITP_IEID_uni_picture_body
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  picData[HUITP_IEID_UNI_PICTURE_BODY_DATA_LEN];
}StrIe_HUITP_IEID_uni_picture_body_t;

//HUITP_IEID_uni_picture_max,

//扬尘监控系统
//HUITP_IEID_uni_ycjk_min                         = 0x3000, 
//HUITP_IEID_uni_ycjk_value                       = 0x3000,
typedef struct StrIe_HUITP_IEID_uni_ycjk_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 ycjkTestValue;
}StrIe_HUITP_IEID_uni_ycjk_value_t;

//HUITP_IEID_uni_ycjk_max,

//水表
//HUITP_IEID_uni_water_meter_min                  = 0x3100, 
//HUITP_IEID_uni_water_meter_value                = 0x3100,
typedef struct StrIe_HUITP_IEID_uni_water_meter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  waterMeterFormat;
	UINT16 waterMeterValue;
}StrIe_HUITP_IEID_uni_water_meter_value_t;

//HUITP_IEID_uni_water_meter_max,

//热表
//HUITP_IEID_uni_heat_meter_min                   = 0x3200, 
//HUITP_IEID_uni_heat_meter_value                 = 0x3200,
typedef struct StrIe_HUITP_IEID_uni_heat_meter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 heatMeterValue;
}StrIe_HUITP_IEID_uni_heat_meter_value_t;

//HUITP_IEID_uni_heat_meter_max,

//气表
//HUITP_IEID_uni_gas_meter_min                    = 0x3300, 
//HUITP_IEID_uni_gas_meter_value                  = 0x3300, 
typedef struct StrIe_HUITP_IEID_uni_gas_meter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 gasMeterValue;
}StrIe_HUITP_IEID_uni_gas_meter_value_t;

//HUITP_IEID_uni_gas_meter_max,

//电表
//HUITP_IEID_uni_power_meter_min                  = 0x3400, 
//HUITP_IEID_uni_power_meter_value                = 0x3400,
typedef struct StrIe_HUITP_IEID_uni_power_meter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 powerMeterValue;
}StrIe_HUITP_IEID_uni_power_meter_value_t;

//HUITP_IEID_uni_power_meter_max,

//光照强度
//HUITP_IEID_uni_light_strength_min               = 0x3500, 
//HUITP_IEID_uni_light_strength_value             = 0x3500, 
typedef struct StrIe_HUITP_IEID_uni_light_strength_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 lightStrengthValue;
}StrIe_HUITP_IEID_uni_light_strength_value_t;

//HUITP_IEID_uni_light_strength_max,

//有毒气体VOC
//HUITP_IEID_uni_toxicgas_min                     = 0x3600, 
//HUITP_IEID_uni_toxicgas_value                   = 0x3600,
typedef struct StrIe_HUITP_IEID_uni_toxicgas_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 toxicgasValue;
}StrIe_HUITP_IEID_uni_toxicgas_value_t;

//HUITP_IEID_uni_toxicgas_max,

//海拔高度
//HUITP_IEID_uni_altitude_min                     = 0x3700, 
//HUITP_IEID_uni_altitude_value                   = 0x3700,
typedef struct StrIe_HUITP_IEID_uni_altitude_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 altitudeValue;
}StrIe_HUITP_IEID_uni_altitude_value_t;

//HUITP_IEID_uni_altitude_max,

//马达
//HUITP_IEID_uni_moto_min                         = 0x3800, 
//HUITP_IEID_uni_moto_value                       = 0x3800,
typedef struct StrIe_HUITP_IEID_uni_moto_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 motoValue;
}StrIe_HUITP_IEID_uni_moto_value_t;

//HUITP_IEID_uni_moto_max,

//继电器
//HUITP_IEID_uni_switch_resistor_min               = 0x3900, 
//HUITP_IEID_uni_switch_resistor_value             = 0x3900,
typedef struct StrIe_HUITP_IEID_uni_switch_resistor_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 switchResistorValue;
}StrIe_HUITP_IEID_uni_switch_resistor_value_t;

//HUITP_IEID_uni_switch_max,

//导轨传送带
//HUITP_IEID_uni_transporter_min                  = 0x3A00, 
//HUITP_IEID_uni_transporter_value                = 0x3A00,
typedef struct StrIe_HUITP_IEID_uni_transporter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 transporterValue;
}StrIe_HUITP_IEID_uni_transporter_value_t;

//HUITP_IEID_uni_transporter_max,

//组合秤BFSC
//HUITP_IEID_uni_bfsc_comb_scale_min              = 0x3B00, 
//HUITP_IEID_uni_scale_weight_value               = 0x3B00,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 scaleWeightValue;
}StrIe_HUITP_IEID_uni_scale_weight_value_t;

//HUITP_IEID_uni_scale_weight_cmd                 = 0x3B01,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_cmd
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 scaleWeightCmd;
}StrIe_HUITP_IEID_uni_scale_weight_cmd_t;
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_NULL 0
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_START 1
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_STOP 2
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_REVERSE 3
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_CONTINUE 4
#define HUITP_IEID_UNI_BFSC_SCALE_WEIGHT_INVALID 0xFF

//HUITP_IEID_uni_bfsc_comb_scale_max,

//云控锁-锁
//HUITP_IEID_uni_ccl_lock_min                     = 0x4000,  
//HUITP_IEID_uni_ccl_lock_state                   = 0x4000,
#define HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER 4
typedef struct StrIe_HUITP_IEID_uni_ccl_lock_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  maxLockNo;
	UINT8  lockId;
	UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER];
}StrIe_HUITP_IEID_uni_ccl_lock_state_t;
#define HUITP_IEID_UNI_LOCK_STATE_NULL 0
#define HUITP_IEID_UNI_LOCK_STATE_OPEN 1
#define HUITP_IEID_UNI_LOCK_STATE_CLOSE 2
#define HUITP_IEID_UNI_LOCK_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_lock_auth_req                = 0x4001,
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_MAX_LEN 20
typedef struct StrIe_HUITP_IEID_uni_ccl_lock_auth_req
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  authReqType;
	UINT8  bleAddrLen;
	UINT8  bleMacAddr[HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_MAX_LEN];
	UINT8  rfidAddrLen;
	UINT8  rfidAddr[HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_MAX_LEN];
}StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t;
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_NULL 0
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_LOCK 1
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_BLE 2
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_RFID 3
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_PHONE 4
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_PID 5
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_INVALID 0xFF

//HUITP_IEID_uni_ccl_lock_auth_resp               = 0x4002,
typedef struct StrIe_HUITP_IEID_uni_ccl_lock_auth_resp
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  authResp;
}StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t;
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_NULL 0
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_YES 1
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_NO 2
#define HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_INVALID 0xFF

//HUITP_IEID_uni_ccl_lock_max,

//云控锁-门
//HUITP_IEID_uni_ccl_door_min                     = 0x4100, 
//HUITP_IEID_uni_ccl_door_state                   = 0x4100,
#define HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER 4
typedef struct StrIe_HUITP_IEID_uni_ccl_door_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  maxDoorNo;
	UINT8  doorId;
	UINT8  doorState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER];
}StrIe_HUITP_IEID_uni_ccl_door_state_t;
#define HUITP_IEID_UNI_DOOR_STATE_NULL 0
#define HUITP_IEID_UNI_DOOR_STATE_OPEN 1
#define HUITP_IEID_UNI_DOOR_STATE_CLOSE 2
#define HUITP_IEID_UNI_DOOR_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_door_max,

//云控锁-RFID模块
//HUITP_IEID_uni_ccl_rfid_min                     = 0x4200, 
//HUITP_IEID_uni_ccl_rfid_value                   = 0x4200,
typedef struct StrIe_HUITP_IEID_uni_ccl_rfid_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  rfidFormat;
	UINT16 rfidValue;
}StrIe_HUITP_IEID_uni_ccl_rfid_value_t;

//HUITP_IEID_uni_ccl_rfid_max,

//云控锁-BLE模块
//HUITP_IEID_uni_ccl_ble_min                      = 0x4300, 
//HUITP_IEID_uni_ccl_ble_value                    = 0x4300,
typedef struct StrIe_HUITP_IEID_uni_ccl_ble_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 bleValue;
}StrIe_HUITP_IEID_uni_ccl_ble_value_t;

//HUITP_IEID_uni_ccl_ble_max,

//云控锁-GPRS模块
//HUITP_IEID_uni_ccl_gprs_min                     = 0x4400, 
//HUITP_IEID_uni_ccl_rssi_value                   = 0x4400,
typedef struct StrIe_HUITP_IEID_uni_ccl_rssi_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 rssiValue;
}StrIe_HUITP_IEID_uni_ccl_rssi_value_t;

//HUITP_IEID_uni_ccl_gprs_max,

//云控锁-电池模块
//HUITP_IEID_uni_ccl_battery_min                  = 0x4500, 
//HUITP_IEID_uni_ccl_bat_state                    = 0x4500,
typedef struct StrIe_HUITP_IEID_uni_ccl_bat_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  batState;
}StrIe_HUITP_IEID_uni_ccl_bat_state_t;
#define HUITP_IEID_UNI_BAT_STATE_NULL 0
#define HUITP_IEID_UNI_BAT_STATE_NORMAL 1
#define HUITP_IEID_UNI_BAT_STATE_WARNING 2
#define HUITP_IEID_UNI_BAT_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_bat_value                    = 0x4501,
typedef struct StrIe_HUITP_IEID_uni_ccl_bat_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 batValue;
}StrIe_HUITP_IEID_uni_ccl_bat_value_t;

//HUITP_IEID_uni_ccl_battery_max,	

//云控锁-震动
//HUITP_IEID_uni_ccl_shake_min                    = 0x4600, 
//HUITP_IEID_uni_ccl_shake_state                  = 0x4600,
typedef struct StrIe_HUITP_IEID_uni_ccl_shake_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  shakeState;
}StrIe_HUITP_IEID_uni_ccl_shake_state_t;
#define HUITP_IEID_UNI_SHAKE_STATE_NULL 0
#define HUITP_IEID_UNI_SHAKE_STATE_DEACTIVE 1
#define HUITP_IEID_UNI_SHAKE_STATE_ACTIVE 2
#define HUITP_IEID_UNI_SHAKE_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_shake_max,

//云控锁-烟雾
//HUITP_IEID_uni_ccl_smoke_min                    = 0x4700, 
//HUITP_IEID_uni_ccl_smoke_state                  = 0x4700,
typedef struct StrIe_HUITP_IEID_uni_ccl_smoke_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  smokeState;
}StrIe_HUITP_IEID_uni_ccl_smoke_state_t;
#define HUITP_IEID_UNI_SMOKE_STATE_NULL 0
#define HUITP_IEID_UNI_SMOKE_STATE_DEACTIVE 1
#define HUITP_IEID_UNI_SMOKE_STATE_ACTIVE 2
#define HUITP_IEID_UNI_SMOKE_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_smoke_max,

//云控锁-水浸
//HUITP_IEID_uni_ccl_water_min                    = 0x4800, 
//HUITP_IEID_uni_ccl_water_state                  = 0x4800, 
typedef struct StrIe_HUITP_IEID_uni_ccl_water_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  waterState;
}StrIe_HUITP_IEID_uni_ccl_water_state_t;
#define HUITP_IEID_UNI_WATER_STATE_NULL 0
#define HUITP_IEID_UNI_WATER_STATE_DEACTIVE 1
#define HUITP_IEID_UNI_WATER_STATE_ACTIVE 2
#define HUITP_IEID_UNI_WATER_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_water_max,

//云控锁-温度
//HUITP_IEID_uni_ccl_temp_min                     = 0x4900, 
//HUITP_IEID_uni_ccl_temp_value                   = 0x4900,
typedef struct StrIe_HUITP_IEID_uni_ccl_temp_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 tempValue;
}StrIe_HUITP_IEID_uni_ccl_temp_value_t;

//HUITP_IEID_uni_ccl_temp_max,

//云控锁-湿度
//HUITP_IEID_uni_ccl_humid_min                    = 0x4A00, 
//HUITP_IEID_uni_ccl_humid_value                  = 0x4A00,
typedef struct StrIe_HUITP_IEID_uni_ccl_humid_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 humidValue;
}StrIe_HUITP_IEID_uni_ccl_humid_value_t;

//HUITP_IEID_uni_ccl_humid_max,

//云控锁-倾倒
//HUITP_IEID_uni_ccl_fall_min                     = 0x4B00, 
//HUITP_IEID_uni_ccl_fall_state                   = 0x4B00,
typedef struct StrIe_HUITP_IEID_uni_ccl_fall_state
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  fallState;
}StrIe_HUITP_IEID_uni_ccl_fall_state_t;
#define HUITP_IEID_UNI_FALL_STATE_NULL 0
#define HUITP_IEID_UNI_FALL_STATE_DEACTIVE 1
#define HUITP_IEID_UNI_FALL_STATE_ACTIVE 2
#define HUITP_IEID_UNI_FALL_STATE_INVALID 0xFF

//HUITP_IEID_uni_ccl_fall_max,

//云控锁-状态聚合
//HUITP_IEID_uni_ccl_state_min                    = 0x4C00, 
//HUITP_IEID_uni_ccl_general_value1               = 0x4C00,
typedef struct StrIe_HUITP_IEID_uni_ccl_general_value1
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 generalValue1;
}StrIe_HUITP_IEID_uni_ccl_general_value1_t;

//HUITP_IEID_uni_ccl_general_value2               = 0x4C01,
typedef struct StrIe_HUITP_IEID_uni_ccl_general_value2
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 generalValue2;
}StrIe_HUITP_IEID_uni_ccl_general_value2_t;

//HUITP_IEID_uni_ccl_dcmi_value                  = 0x4C02,
typedef struct StrIe_HUITP_IEID_uni_ccl_dcmi_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 dcmiValue;
}StrIe_HUITP_IEID_uni_ccl_dcmi_value_t;

//HUITP_IEID_uni_ccl_report_type                 = 0x4C03,
typedef struct StrIe_HUITP_IEID_uni_ccl_report_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  event;
}StrIe_HUITP_IEID_uni_ccl_report_type_t;
#define HUITP_IEID_UNI_CCL_REPORT_TYPE_NULL 0
#define HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT 1
#define HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT 2
#define HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT 3
#define HUITP_IEID_UNI_CCL_REPORT_TYPE_INVALID 0xFF

//HUITP_IEID_uni_ccl_state_max,

//串口读取命令/返回结果
//HUITP_IEID_uni_itf_sps_min                      = 0x5000, 
//HUITP_IEID_uni_itf_sps_value                    = 0x5000,
typedef struct StrIe_HUITP_IEID_uni_itf_sps_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfSpsValue;
}StrIe_HUITP_IEID_uni_itf_sps_value_t;

//HUITP_IEID_uni_itf_sps_max,

//ADC读取命令/返回结果
//HUITP_IEID_uni_itf_adc_min                      = 0x5100, 
//HUITP_IEID_uni_itf_adc_value                    = 0x5100,
typedef struct StrIe_HUITP_IEID_uni_itf_adc_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfAdcValue;
}StrIe_HUITP_IEID_uni_itf_adc_value_t;

//HUITP_IEID_uni_itf_adc_max,

//DAC读取命令/返回结果
//HUITP_IEID_uni_itf_dac_min                      = 0x5200, 
//HUITP_IEID_uni_itf_dac_value                    = 0x5200,
typedef struct StrIe_HUITP_IEID_uni_itf_dac_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfDacValue;
}StrIe_HUITP_IEID_uni_itf_dac_value_t;

//HUITP_IEID_uni_itf_dac_max,

//I2C读取命令/返回结果
//HUITP_IEID_uni_itf_i2c_min                      = 0x5300, 
//HUITP_IEID_uni_itf_i2c_value                    = 0x5300,
typedef struct StrIe_HUITP_IEID_uni_itf_i2c_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfI2cValue;
}StrIe_HUITP_IEID_uni_itf_i2c_value_t;

//HUITP_IEID_uni_itf_i2c_max,

//PWM读取命令/返回结果
//HUITP_IEID_uni_itf_pwm_min                      = 0x5400, 
//HUITP_IEID_uni_itf_pwm_value                    = 0x5400,
typedef struct StrIe_HUITP_IEID_uni_itf_pwm_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfPwmValue;
}StrIe_HUITP_IEID_uni_itf_pwm_value_t;

//HUITP_IEID_uni_itf_pwm_max,

//DI读取命令/返回结果
//HUITP_IEID_uni_itf_di_min                       = 0x5500, 
//HUITP_IEID_uni_itf_di_value                     = 0x5500,
typedef struct StrIe_HUITP_IEID_uni_itf_di_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfDiValue;
}StrIe_HUITP_IEID_uni_itf_di_value_t;

//HUITP_IEID_uni_itf_di_max,

//DO读取命令/返回结果
//HUITP_IEID_uni_itf_do_min                       = 0x5600, 
//HUITP_IEID_uni_itf_do_value                     = 0x5600, 
typedef struct StrIe_HUITP_IEID_uni_itf_do_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfDoValue;
}StrIe_HUITP_IEID_uni_itf_do_value_t;

//HUITP_IEID_uni_itf_do_max,

//CAN读取命令/返回结果
//HUITP_IEID_uni_itf_can_min                      = 0x5700, 
//HUITP_IEID_uni_itf_can_value                    = 0x5700, 
typedef struct StrIe_HUITP_IEID_uni_itf_can_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfCanValue;
}StrIe_HUITP_IEID_uni_itf_can_value_t;

//HUITP_IEID_uni_itf_can_max,

//SPI读取命令/返回结果
//HUITP_IEID_uni_itf_spi_min                      = 0x5800, 
//HUITP_IEID_uni_itf_spi_value                    = 0x5800, 
typedef struct StrIe_HUITP_IEID_uni_itf_spi_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfSpiValue;
}StrIe_HUITP_IEID_uni_itf_spi_value_t;

//HUITP_IEID_uni_itf_spi_max,

//USB读取命令/返回结果
//HUITP_IEID_uni_itf_usb_min                      = 0x5900, 
//HUITP_IEID_uni_itf_usb_value                    = 0x5900,
typedef struct StrIe_HUITP_IEID_uni_itf_usb_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfUsbValue;
}StrIe_HUITP_IEID_uni_itf_usb_value_t;

//HUITP_IEID_uni_itf_usb_max,

//网口读取命令/返回结果
//HUITP_IEID_uni_itf_eth_min                      = 0x5A00, 
//HUITP_IEID_uni_itf_eth_value                    = 0x5A00,
typedef struct StrIe_HUITP_IEID_uni_itf_eth_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itfEthValue;
}StrIe_HUITP_IEID_uni_itf_eth_value_t;

//HUITP_IEID_uni_itf_eth_max,

//485读取命令/返回结果 
//HUITP_IEID_uni_itf_485_min                      = 0x5B00, 
//HUITP_IEID_uni_itf_485_value                    = 0x5B00,
typedef struct StrIe_HUITP_IEID_uni_itf_485_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 itf485Value;
}StrIe_HUITP_IEID_uni_itf_485_value_t;

//HUITP_IEID_uni_itf_485_max,

//软件清单
//HUITP_IEID_uni_inventory_min                    = 0xA000,
//HUITP_IEID_uni_inventory_hw_type                = 0xA000,
typedef struct StrIe_HUITP_IEID_uni_inventory_hw_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 hwType;
}StrIe_HUITP_IEID_uni_inventory_hw_type_t;
//HW_TYPE的高字节将表示HCU、IHU等不同类型
//HW_TYPE的低字节将表示产品的具体型号
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G1 0x01
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G2 0x02
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G3 0x03
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G4 0x04
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G5 0x05
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G6 0x06
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G7 0x07
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G8 0x08
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E1 0x11
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E2 0x12
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E3 0x13
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E4 0x14
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G1_AQYC_335D_01 0x0101
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_01 0x0201
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_CXILC_RASP_01 0x0211
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_CXGLACM_RASP_01 0x0221
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_NBIOT_LPM_CJ_RASP_01 0x0231
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_NBIOT_HPM_QG_RASP_01 0x0241
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_SCY_WATER_01 0x0301
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_CB_01  0x0311
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_SB_01  0x0321
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G4_GQYB_01 0x0401
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5001 0x0501
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5002 0x0502
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5102 0x0512
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5104 0x0514
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5204 0x0524
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G6_ZNLD_01 0x0601
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_RFID_MS01 0x0701
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_HCU_01 0x0801
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_IHU_01 0x0881
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_58_01 0x1101
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_68_01 0x1111
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E2_HJJK_01 0x1201  //手持式环境监控
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E3_DZGP_01 0x1301  //电子工牌
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E4_ZNSH_01 0x1401  //智能手环


//HUITP_IEID_uni_inventory_hw_id                  = 0xA001, 
typedef struct StrIe_HUITP_IEID_uni_inventory_hw_id
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 hwId;
}StrIe_HUITP_IEID_uni_inventory_hw_id_t;
//用于指示单个硬件具体的PEM信息，或者小版本的演进修改。
//如果没有独特信息，可采用0000/FFFF来表示无效不用。


//HUITP_IEID_uni_inventory_sw_rel                 = 0xA002, 
typedef struct StrIe_HUITP_IEID_uni_inventory_sw_rel
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 swRel;
}StrIe_HUITP_IEID_uni_inventory_sw_rel_t;

//HUITP_IEID_uni_inventory_sw_ver                 = 0xA003,
typedef struct StrIe_HUITP_IEID_uni_inventory_sw_ver
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 swVer;
}StrIe_HUITP_IEID_uni_inventory_sw_ver_t;

//HUITP_IEID_uni_inventory_max,

//软件版本体
//HUITP_IEID_uni_sw_package_min                   = 0xA100,	
//HUITP_IEID_uni_sw_package_body                  = 0xA100,
#define HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN ((400 < HUITP_IE_BUF_MAX_LEN)? 400: HUITP_IE_BUF_MAX_LEN)
typedef struct StrIe_HUITP_IEID_uni_sw_package_body
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  swPkgBody[HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN];
}StrIe_HUITP_IEID_uni_sw_package_body_t;

//HUITP_IEID_uni_sw_package_max,

//ALARM REPORT
//HUITP_IEID_uni_alarm_info_min                   = 0xB000, 
//HUITP_IEID_uni_alarm_info_type                  = 0xB000,
typedef struct StrIe_HUITP_IEID_uni_alarm_info_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 alarmInfoType;
}StrIe_HUITP_IEID_uni_alarm_info_type_t;

//HUITP_IEID_uni_alarm_info_value                 = 0xB001, 
typedef struct StrIe_HUITP_IEID_uni_alarm_info_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 alarmInfoValue;
}StrIe_HUITP_IEID_uni_alarm_info_value_t;

//HUITP_IEID_uni_alarm_info_max,

//PM Report
//HUITP_IEID_uni_performance_info_min             = 0xB100, 
//HUITP_IEID_uni_performance_info_type            = 0xB100,
typedef struct StrIe_HUITP_IEID_uni_performance_info_type
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 pmInfoType;
}StrIe_HUITP_IEID_uni_performance_info_type_t;

//HUITP_IEID_uni_performance_info_value           = 0xB101, 
typedef struct StrIe_HUITP_IEID_uni_performance_info_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 pmInfoValue;
}StrIe_HUITP_IEID_uni_performance_info_value_t;

//HUITP_IEID_uni_performance_info_max,

//设备基本信息
//HUITP_IEID_uni_equipment_info_min               = 0xF000,	
//HUITP_IEID_uni_equipment_info_value             = 0xF000,
typedef struct StrIe_HUITP_IEID_uni_equipment_info_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 equpInfoValue;
}StrIe_HUITP_IEID_uni_equipment_info_value_t;

//HUITP_IEID_uni_equipment_info_max,

//个人基本信息
//HUITP_IEID_uni_personal_info_min                = 0xF100,	
//HUITP_IEID_uni_personal_info_value              = 0xF100,
typedef struct StrIe_HUITP_IEID_uni_personal_info_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 personalInfoValue;
}StrIe_HUITP_IEID_uni_personal_info_value_t;

//HUITP_IEID_uni_personal_info_max,

//时间同步
//HUITP_IEID_uni_time_sync_min                    = 0xF200,
//HUITP_IEID_uni_time_sync_value                  = 0xF200,
typedef struct StrIe_HUITP_IEID_uni_time_sync_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 timeSyncValue;
}StrIe_HUITP_IEID_uni_time_sync_value_t;

//HUITP_IEID_uni_time_sync_max,

//读取数据
//HUITP_IEID_uni_general_read_data_min            = 0xF300,	
//HUITP_IEID_uni_general_read_data_value          = 0xF300,
typedef struct StrIe_HUITP_IEID_uni_general_read_data_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 genReadDataValue;
}StrIe_HUITP_IEID_uni_general_read_data_value_t;

//HUITP_IEID_uni_general_read_data_max,

//定时闹钟及久坐提醒
//HUITP_IEID_uni_clock_timeout_min                = 0xF400,
//HUITP_IEID_uni_clock_timeout_value              = 0xF400,
typedef struct StrIe_HUITP_IEID_uni_clock_timeout_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 clockTimeoutValue;
}StrIe_HUITP_IEID_uni_clock_timeout_value_t;

//HUITP_IEID_uni_clock_timeout_max,

//同步充电，双击情况
//HUITP_IEID_uni_sync_charging_min                = 0xF500,	
//HUITP_IEID_uni_sync_charging_value              = 0xF500,
typedef struct StrIe_HUITP_IEID_uni_sync_charging_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 syncChargingValue;
}StrIe_HUITP_IEID_uni_sync_charging_value_t;

//HUITP_IEID_uni_sync_charging_max,

//同步通知信息
//HUITP_IEID_uni_sync_trigger_min                 = 0xF600,	
//HUITP_IEID_uni_sync_trigger_value               = 0xF600,
typedef struct StrIe_HUITP_IEID_uni_sync_trigger_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 syncTriggerValue;
}StrIe_HUITP_IEID_uni_sync_trigger_value_t;

//HUITP_IEID_uni_sync_trigger_max,

//CMD CONTROL
//HUITP_IEID_uni_cmd_ctrl_min                     = 0xFD00,  
//HUITP_IEID_uni_cmd_ctrl_send                    = 0xFD00,
typedef struct StrIe_HUITP_IEID_uni_cmd_ctrl_send
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  cmdCtrlSend;
}StrIe_HUITP_IEID_uni_cmd_ctrl_send_t;

//HUITP_IEID_uni_cmd_ctrl_confirm                     = 0xFD01,
typedef struct StrIe_HUITP_IEID_uni_cmd_ctrl_confirm
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  cmdCtrlCfm;
}StrIe_HUITP_IEID_uni_cmd_ctrl_confirm_t;

//HUITP_IEID_uni_cmd_ctrl_max,	

//心跳
//HUITP_IEID_uni_heart_beat_min                   = 0xFE00, 
//HUITP_IEID_uni_heart_beat_ping                  = 0xFE00,
typedef struct StrIe_HUITP_IEID_uni_heart_beat_ping
{
	UINT16 ieId;
	UINT16 ieLen;
}StrIe_HUITP_IEID_uni_heart_beat_ping_t;

//HUITP_IEID_uni_heart_beat_pong                  = 0xFE01, 
typedef struct StrIe_HUITP_IEID_uni_heart_beat_pong
{
	UINT16 ieId;
	UINT16 ieLen;
}StrIe_HUITP_IEID_uni_heart_beat_pong_t;

//HUITP_IEID_uni_heart_beat_max,


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP MSG消息结构定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//标准的消息格式
//为了彻底解决大小端的问题，这里按照比特进行定义
typedef struct StrMsg_HUITP_MSGID_uni_general_head_msgid
{
	UINT8 cmdId;
	UINT8 optId;
}StrMsg_HUITP_MSGID_uni_general_head_msgid_t;

typedef struct StrMsg_HUITP_MSGID_uni_general_message
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	UINT8  data[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
}StrMsg_HUITP_MSGID_uni_general_message_t;

//无效
//HUITP_MSGID_uni_none                             = 0x0000,

//血糖
//HUITP_MSGID_uni_blood_glucose_min                = 0x0100, 
//HUITP_MSGID_uni_blood_glucose_req                = 0x0100,
typedef struct StrMsg_HUITP_MSGID_uni_blood_glucose_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_blood_glucose_req_t;

//HUITP_MSGID_uni_blood_glucose_resp               = 0x0180,
typedef struct StrMsg_HUITP_MSGID_uni_blood_glucose_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_blood_glucose_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_blood_glucose_resp_t;

//HUITP_MSGID_uni_blood_glucose_report             = 0x0181,
typedef struct StrMsg_HUITP_MSGID_uni_blood_glucose_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_blood_glucose_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_blood_glucose_report_t;

//HUITP_MSGID_uni_blood_glucose_confirm                = 0x0101,
typedef struct StrMsg_HUITP_MSGID_uni_blood_glucose_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_blood_glucose_confirm_t;

//HUITP_MSGID_uni_blood_glucose_max,

//单次运动
//HUITP_MSGID_uni_single_sports_min                = 0x0200, 
//HUITP_MSGID_uni_single_sports_req                = 0x0200, 
typedef struct StrMsg_HUITP_MSGID_uni_single_sports_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_single_sports_req_t;

//HUITP_MSGID_uni_single_sports_resp               = 0x0280,
typedef struct StrMsg_HUITP_MSGID_uni_single_sports_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_single_sports_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_single_sports_resp_t;

//HUITP_MSGID_uni_single_sports_report             = 0x0281, 
typedef struct StrMsg_HUITP_MSGID_uni_single_sports_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_single_sports_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_single_sports_report_t;

//HUITP_MSGID_uni_single_sports_confirm                = 0x0201,
typedef struct StrMsg_HUITP_MSGID_uni_single_sports_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_single_sports_confirm_t;

//HUITP_MSGID_uni_single_sports_max,

//单次睡眠
//HUITP_MSGID_uni_single_sleep_min                 = 0x0300, 
//HUITP_MSGID_uni_single_sleep_req                 = 0x0300,
typedef struct StrMsg_HUITP_MSGID_uni_single_sleep_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_single_sleep_req_t;

//HUITP_MSGID_uni_single_sleep_resp                = 0x0380,
typedef struct StrMsg_HUITP_MSGID_uni_single_sleep_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_single_sleep_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_single_sleep_resp_t;

//HUITP_MSGID_uni_single_sleep_report              = 0x0381,
typedef struct StrMsg_HUITP_MSGID_uni_single_sleep_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_single_sleep_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_single_sleep_report_t;

//HUITP_MSGID_uni_single_sleep_confirm                 = 0x0301,
typedef struct StrMsg_HUITP_MSGID_uni_single_sleep_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_single_sleep_confirm_t;
 
//HUITP_MSGID_uni_single_sleep_max,
	
//体脂
//HUITP_MSGID_uni_body_fat_min                     = 0x0400, 
//HUITP_MSGID_uni_body_fat_req                     = 0x0400,
typedef struct StrMsg_HUITP_MSGID_uni_body_fat_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_body_fat_req_t;

//HUITP_MSGID_uni_body_fat_resp                    = 0x0480,
typedef struct StrMsg_HUITP_MSGID_uni_body_fat_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_body_fat_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_body_fat_resp_t;

//HUITP_MSGID_uni_body_fat_report                  = 0x0481,
typedef struct StrMsg_HUITP_MSGID_uni_body_fat_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_body_fat_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_body_fat_report_t;

//HUITP_MSGID_uni_body_fat_confirm                     = 0x0401,
typedef struct StrMsg_HUITP_MSGID_uni_body_fat_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_body_fat_confirm_t;

//HUITP_MSGID_uni_body_fat_max,

//血压
//HUITP_MSGID_uni_blood_pressure_min               = 0x0500, 
//HUITP_MSGID_uni_blood_pressure_req               = 0x0500,
typedef struct StrMsg_HUITP_MSGID_uni_blood_pressure_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_blood_pressure_req_t;

//HUITP_MSGID_uni_blood_pressure_resp              = 0x0580,
typedef struct StrMsg_HUITP_MSGID_uni_blood_pressure_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_blood_pressure_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_blood_pressure_resp_t;

//HUITP_MSGID_uni_blood_pressure_report            = 0x0581,
typedef struct StrMsg_HUITP_MSGID_uni_blood_pressure_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_blood_pressure_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_blood_pressure_report_t;
 
//HUITP_MSGID_uni_blood_pressure_confirm               = 0x0501,
typedef struct StrMsg_HUITP_MSGID_uni_blood_pressure_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_blood_pressure_confirm_t;
 
//HUITP_MSGID_uni_blood_pressure_max,

//跑步机数据上报
//HUITP_MSGID_uni_runner_machine_rep_min           = 0x0A00, 
//HUITP_MSGID_uni_runner_machine_rep_req           = 0x0A00,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_rep_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_runner_machine_rep_req_t;
 
//HUITP_MSGID_uni_runner_machine_rep_resp          = 0x0A80,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_rep_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_runner_machine_rep_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_runner_machine_rep_resp_t;

//HUITP_MSGID_uni_runner_machine_rep_report        = 0x0A81,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_rep_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_runner_machine_rep_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_runner_machine_rep_report_t;

//HUITP_MSGID_uni_runner_machine_rep_confirm           = 0x0A01,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_rep_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_runner_machine_rep_confirm_t;
 
//HUITP_MSGID_uni_runner_machine_rep_max,

//跑步机任务控制
//HUITP_MSGID_uni_runner_machine_ctrl_min          = 0x0B00,
//HUITP_MSGID_uni_runner_machine_ctrl_req          = 0x0B00, 
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_req_t;

//HUITP_MSGID_uni_runner_machine_ctrl_resp         = 0x0B80,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_runner_machine_ctrl_value_t respValue;	
}StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_resp_t;

//HUITP_MSGID_uni_runner_machine_ctrl_report       = 0x0B81,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_runner_machine_ctrl_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_report_t;
 
//HUITP_MSGID_uni_runner_machine_ctrl_confirm          = 0x0B01,
typedef struct StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_runner_machine_ctrl_confirm_t;
 
//HUITP_MSGID_uni_runner_machine_ctrl_max,

//GPS地址
//HUITP_MSGID_uni_gps_specific_min                 = 0x0C00, 
//HUITP_MSGID_uni_gps_specific_req                 = 0x0C00, 
typedef struct StrMsg_HUITP_MSGID_uni_gps_specific_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_gps_specific_req_t;

//HUITP_MSGID_uni_gps_specific_resp                = 0x0C80, 
typedef struct StrMsg_HUITP_MSGID_uni_gps_specific_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_gps_specific_x_t respGpsxValue;	
	StrIe_HUITP_IEID_uni_gps_specific_y_t respGpsyValue;	
	StrIe_HUITP_IEID_uni_gps_specific_z_t respGpszValue;		
}StrMsg_HUITP_MSGID_uni_gps_specific_resp_t;

//HUITP_MSGID_uni_gps_specific_report              = 0x0C81,
typedef struct StrMsg_HUITP_MSGID_uni_gps_specific_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_gps_specific_x_t reportGpsxValue;	
	StrIe_HUITP_IEID_uni_gps_specific_y_t reportGpsyValue;	
	StrIe_HUITP_IEID_uni_gps_specific_z_t reportGpszValue;	
}StrMsg_HUITP_MSGID_uni_gps_specific_report_t;

//HUITP_MSGID_uni_gps_specific_confirm                 = 0x0C01, 
typedef struct StrMsg_HUITP_MSGID_uni_gps_specific_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_gps_specific_confirm_t;

//HUITP_MSGID_uni_gps_specific_max,

//IHU与IAU之间控制命令
//HUITP_MSGID_uni_iau_ctrl_min                     = 0x1000, 
//HUITP_MSGID_uni_iau_ctrl_req                     = 0x1000,
typedef struct StrMsg_HUITP_MSGID_uni_iau_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_iau_ctrl_req_t;

//HUITP_MSGID_uni_iau_ctrl_resp                    = 0x1080,
typedef struct StrMsg_HUITP_MSGID_uni_iau_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_iau_ctrl_value_t respValue;
}StrMsg_HUITP_MSGID_uni_iau_ctrl_resp_t;

//HUITP_MSGID_uni_iau_ctrl_report                  = 0x1081,
typedef struct StrMsg_HUITP_MSGID_uni_iau_ctrl_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_iau_ctrl_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_iau_ctrl_report_t;
 
//HUITP_MSGID_uni_iau_ctrl_confirm                     = 0x1001,
typedef struct StrMsg_HUITP_MSGID_uni_iau_ctrl_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_iau_ctrl_confirm_t;

//HUITP_MSGID_uni_iau_ctrl_max,

//电磁辐射强度
//HUITP_MSGID_uni_emc_data_min                     = 0x2000, 
//HUITP_MSGID_uni_emc_data_req                     = 0x2000,
typedef struct StrMsg_HUITP_MSGID_uni_emc_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_data_req_t;

//HUITP_MSGID_uni_emc_data_resp                    = 0x2080,
typedef struct StrMsg_HUITP_MSGID_uni_emc_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_emc_data_value_t respValue;
}StrMsg_HUITP_MSGID_uni_emc_data_resp_t;
 
//HUITP_MSGID_uni_emc_data_report                  = 0x2081,
typedef struct StrMsg_HUITP_MSGID_uni_emc_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_emc_data_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_emc_data_report_t;

//HUITP_MSGID_uni_emc_data_confirm                     = 0x2001,
typedef struct StrMsg_HUITP_MSGID_uni_emc_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_emc_data_confirm_t;

//HUITP_MSGID_uni_emc_set_switch                   = 0x2002,
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_emc_set_switch_t;

//HUITP_MSGID_uni_emc_set_switch_ack               = 0x2082, 	
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_emc_set_switch_ack_t;

//HUITP_MSGID_uni_emc_set_modbus_address           = 0x2003,
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_emc_set_modbus_address_t;
 
//HUITP_MSGID_uni_emc_set_modbus_address_ack       = 0x2083, 
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_emc_set_modbus_address_ack_t;

//HUITP_MSGID_uni_emc_set_work_cycle               = 0x2004,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_emc_set_work_cycle_t;

//HUITP_MSGID_uni_emc_set_work_cycle_ack           = 0x2084,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_emc_set_work_cycle_ack_t;

//HUITP_MSGID_uni_emc_set_sample_cycle             = 0x2005,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_emc_set_sample_cycle_t;

//HUITP_MSGID_uni_emc_set_sample_cycle_ack         = 0x2085,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_emc_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_emc_set_sample_number            = 0x2006,
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_emc_set_sample_number_t;
   
//HUITP_MSGID_uni_emc_set_sample_number_ack        = 0x2086,
typedef struct StrMsg_HUITP_MSGID_uni_emc_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_emc_set_sample_number_ack_t;

//HUITP_MSGID_uni_emc_read_switch                  = 0x2007,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_read_switch_t;

//HUITP_MSGID_uni_emc_read_switch_ack              = 0x2087,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_emc_read_switch_ack_t;

//HUITP_MSGID_uni_emc_read_modbus_address          = 0x2008,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_read_modbus_address_t;

//HUITP_MSGID_uni_emc_read_modbus_address_ack      = 0x2088,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_emc_read_modbus_address_ack_t;

//HUITP_MSGID_uni_emc_read_work_cycle              = 0x2009,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_read_work_cycle_t;

//HUITP_MSGID_uni_emc_read_work_cycle_ack          = 0x2089,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_emc_read_work_cycle_ack_t;

//HUITP_MSGID_uni_emc_read_sample_cycle            = 0x200A, 
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_read_sample_cycle_t;

//HUITP_MSGID_uni_emc_read_sample_cycle_ack        = 0x208A,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_emc_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_emc_read_sample_number           = 0x200B,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_read_sample_number_t;

//HUITP_MSGID_uni_emc_read_sample_number_ack       = 0x208B,
typedef struct StrMsg_HUITP_MSGID_uni_emc_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_emc_read_sample_number_ack_t;

//HUITP_MSGID_uni_emc_data_max,

//电磁辐射剂量
//HUITP_MSGID_uni_emc_accu_min                     = 0x2100, 
//HUITP_MSGID_uni_emc_accu_req                     = 0x2100, 
typedef struct StrMsg_HUITP_MSGID_uni_emc_accu_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_emc_accu_req_t;

//HUITP_MSGID_uni_emc_accu_resp                    = 0x2180,
typedef struct StrMsg_HUITP_MSGID_uni_emc_accu_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_emc_accu_value_t respValue;
}StrMsg_HUITP_MSGID_uni_emc_accu_resp_t;

//HUITP_MSGID_uni_emc_accu_report                  = 0x2181,
typedef struct StrMsg_HUITP_MSGID_uni_emc_accu_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_emc_accu_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_emc_accu_report_t;
 
//HUITP_MSGID_uni_emc_accu_confirm                     = 0x2101,
typedef struct StrMsg_HUITP_MSGID_uni_emc_accu_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_emc_accu_confirm_t;

//HUITP_MSGID_uni_emc_accu_max,

//一氧化碳
//HUITP_MSGID_uni_co_min                           = 0x2200,
//HUITP_MSGID_uni_co_req                           = 0x2200,
typedef struct StrMsg_HUITP_MSGID_uni_co_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_co_req_t;

//HUITP_MSGID_uni_co_resp                          = 0x2280,
typedef struct StrMsg_HUITP_MSGID_uni_co_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_co_value_t respValue;
}StrMsg_HUITP_MSGID_uni_co_resp_t;

//HUITP_MSGID_uni_co_report                        = 0x2281,
typedef struct StrMsg_HUITP_MSGID_uni_co_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_co_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_co_report_t;
 
//HUITP_MSGID_uni_co_confirm                           = 0x2201,
typedef struct StrMsg_HUITP_MSGID_uni_co_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_co_confirm_t;

//HUITP_MSGID_uni_co_max,

//甲醛HCHO
//HUITP_MSGID_uni_formaldehyde_min                 = 0x2300, 
//HUITP_MSGID_uni_formaldehyde_req                 = 0x2300,
typedef struct StrMsg_HUITP_MSGID_uni_formaldehyde_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_formaldehyde_req_t;

//HUITP_MSGID_uni_formaldehyde_resp                = 0x2380,
typedef struct StrMsg_HUITP_MSGID_uni_formaldehyde_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_formaldehyde_value_t respValue;
}StrMsg_HUITP_MSGID_uni_formaldehyde_resp_t;
 
//HUITP_MSGID_uni_formaldehyde_report              = 0x2381,
typedef struct StrMsg_HUITP_MSGID_uni_formaldehyde_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_formaldehyde_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_formaldehyde_report_t;
 
//HUITP_MSGID_uni_formaldehyde_confirm                 = 0x2301,
typedef struct StrMsg_HUITP_MSGID_uni_formaldehyde_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_formaldehyde_confirm_t;

//HUITP_MSGID_uni_formaldehyde_max,

//酒精
//HUITP_MSGID_uni_alcohol_min                      = 0x2400, 
//HUITP_MSGID_uni_alcohol_req                      = 0x2400,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_alcohol_req_t;

//HUITP_MSGID_uni_alcohol_resp                     = 0x2480,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_alcohol_value_t respValue;
}StrMsg_HUITP_MSGID_uni_alcohol_resp_t;

//HUITP_MSGID_uni_alcohol_report                   = 0x2481,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_alcohol_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_alcohol_report_t;

//HUITP_MSGID_uni_alcohol_confirm                      = 0x2401, 
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_alcohol_confirm_t;

//HUITP_MSGID_uni_alcohol_max,

//PM1/2.5/10
//HUITP_MSGID_uni_pm25_min                         = 0x2500,
//HUITP_MSGID_uni_pm25_data_req                    = 0x2500,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_data_req_t;

//HUITP_MSGID_uni_pm25_data_resp                   = 0x2580,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_pm01_value_t respPm01Value;
	StrIe_HUITP_IEID_uni_pm25_value_t respPm25Value;
	StrIe_HUITP_IEID_uni_pm10_value_t respPm10Value;
}StrMsg_HUITP_MSGID_uni_pm25_data_resp_t;

//HUITP_MSGID_uni_pm25_data_report                 = 0x2581, 
typedef struct StrMsg_HUITP_MSGID_uni_pm25_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_pm01_value_t reportPm01Value;
	StrIe_HUITP_IEID_uni_pm25_value_t reportPm25Value;
	StrIe_HUITP_IEID_uni_pm10_value_t reportPm10Value;
}StrMsg_HUITP_MSGID_uni_pm25_data_report_t;

//HUITP_MSGID_uni_pm25_data_confirm                    = 0x2501,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t;

//HUITP_MSGID_uni_pm25_set_switch                  = 0x2502,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_pm25_set_switch_t;

//HUITP_MSGID_uni_pm25_set_switch_ack              = 0x2582,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_pm25_set_switch_ack_t;

//HUITP_MSGID_uni_pm25_set_modbus_address          = 0x2503,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_pm25_set_modbus_address_t;

//HUITP_MSGID_uni_pm25_set_modbus_address_ack      = 0x2583, 
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_pm25_set_modbus_address_ack_t;

//HUITP_MSGID_uni_pm25_set_work_cycle              = 0x2504,  //In second
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_pm25_set_work_cycle_t;

//HUITP_MSGID_uni_pm25_set_work_cycle_ack          = 0x2584,  
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_pm25_set_work_cycle_ack_t;

//HUITP_MSGID_uni_pm25_set_sample_cycle            = 0x2505,  //In second
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_pm25_set_sample_cycle_t;

//HUITP_MSGID_uni_pm25_set_sample_cycle_ack        = 0x2585,  
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_pm25_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_pm25_set_sample_number           = 0x2506,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_pm25_set_sample_number_t;

//HUITP_MSGID_uni_pm25_set_sample_number_ack       = 0x2586,  
typedef struct StrMsg_HUITP_MSGID_uni_pm25_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_pm25_set_sample_number_ack_t;

//HUITP_MSGID_uni_pm25_read_switch                 = 0x2507,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_read_switch_t;

//HUITP_MSGID_uni_pm25_read_switch_ack             = 0x2587,  
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_pm25_read_switch_ack_t;

//HUITP_MSGID_uni_pm25_read_modbus_address         = 0x2508,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_read_modbus_address_t;

//HUITP_MSGID_uni_pm25_read_modbus_address_ack     = 0x2588,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_pm25_read_modbus_address_ack_t;

//HUITP_MSGID_uni_pm25_read_work_cycle             = 0x2509,  //In second
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_read_work_cycle_t;

//HUITP_MSGID_uni_pm25_read_work_cycle_ack         = 0x2589,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_pm25_read_work_cycle_ack_t;

//HUITP_MSGID_uni_pm25_read_sample_cycle           = 0x250A,  //In 
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_read_sample_cycle_t;

//HUITP_MSGID_uni_pm25_read_sample_cycle_ack       = 0x258A,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_pm25_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_pm25_read_sample_number          = 0x250B,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25_read_sample_number_t;

//HUITP_MSGID_uni_pm25_read_sample_number_ack      = 0x258B,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_pm25_read_sample_number_ack_t;
 
//HUITP_MSGID_uni_pm25_max,

//风速Wind Speed
//HUITP_MSGID_uni_windspd_min                      = 0x2600, 
//HUITP_MSGID_uni_windspd_data_req                     = 0x2600,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_data_req_t;

//HUITP_MSGID_uni_windspd_data_resp                    = 0x2680,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_windspd_value_t respValue;
}StrMsg_HUITP_MSGID_uni_windspd_data_resp_t;
 
//HUITP_MSGID_uni_windspd_data_report                  = 0x2681,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_windspd_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_windspd_data_report_t;

//HUITP_MSGID_uni_windspd_data_confirm                     = 0x2601,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t;

//HUITP_MSGID_uni_windspd_set_switch                   = 0x2602,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_windspd_set_switch_t;

//HUITP_MSGID_uni_windspd_set_switch_ack               = 0x2682, 	
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_windspd_set_switch_ack_t;

//HUITP_MSGID_uni_windspd_set_modbus_address           = 0x2603,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_windspd_set_modbus_address_t;
 
//HUITP_MSGID_uni_windspd_set_modbus_address_ack       = 0x2683, 
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_windspd_set_modbus_address_ack_t;

//HUITP_MSGID_uni_windspd_set_work_cycle               = 0x2604,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_windspd_set_work_cycle_t;

//HUITP_MSGID_uni_windspd_set_work_cycle_ack           = 0x2684,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_windspd_set_work_cycle_ack_t;

//HUITP_MSGID_uni_windspd_set_sample_cycle             = 0x2605,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_windspd_set_sample_cycle_t;

//HUITP_MSGID_uni_windspd_set_sample_cycle_ack         = 0x2685,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_windspd_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_windspd_set_sample_number            = 0x2606,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_windspd_set_sample_number_t;
   
//HUITP_MSGID_uni_windspd_set_sample_number_ack        = 0x2686,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_windspd_set_sample_number_ack_t;

//HUITP_MSGID_uni_windspd_read_switch                  = 0x2607,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_read_switch_t;

//HUITP_MSGID_uni_windspd_read_switch_ack              = 0x2687,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_windspd_read_switch_ack_t;

//HUITP_MSGID_uni_windspd_read_modbus_address          = 0x2608,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_read_modbus_address_t;

//HUITP_MSGID_uni_windspd_read_modbus_address_ack      = 0x2688,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_windspd_read_modbus_address_ack_t;

//HUITP_MSGID_uni_windspd_read_work_cycle              = 0x2609,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_read_work_cycle_t;

//HUITP_MSGID_uni_windspd_read_work_cycle_ack          = 0x2689,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_windspd_read_work_cycle_ack_t;

//HUITP_MSGID_uni_windspd_read_sample_cycle            = 0x260A, 
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_read_sample_cycle_t;

//HUITP_MSGID_uni_windspd_read_sample_cycle_ack        = 0x268A,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_windspd_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_windspd_read_sample_number           = 0x260B,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_windspd_read_sample_number_t;

//HUITP_MSGID_uni_windspd_read_sample_number_ack       = 0x268B,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_windspd_read_sample_number_ack_t;

//HUITP_MSGID_uni_windspd_max,

//风向Wind Direction
//HUITP_MSGID_uni_winddir_min                      = 0x2700, 
//HUITP_MSGID_uni_winddir_data_req                     = 0x2700,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_data_req_t;

//HUITP_MSGID_uni_winddir_data_resp                    = 0x2780,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_winddir_value_t respValue;
}StrMsg_HUITP_MSGID_uni_winddir_data_resp_t;
 
//HUITP_MSGID_uni_winddir_data_report                  = 0x2781,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_winddir_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_winddir_data_report_t;

//HUITP_MSGID_uni_winddir_data_confirm                     = 0x2701,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t;

//HUITP_MSGID_uni_winddir_set_switch                   = 0x2702,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_winddir_set_switch_t;

//HUITP_MSGID_uni_winddir_set_switch_ack               = 0x2782, 	
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_winddir_set_switch_ack_t;

//HUITP_MSGID_uni_winddir_set_modbus_address           = 0x2703,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_winddir_set_modbus_address_t;
 
//HUITP_MSGID_uni_winddir_set_modbus_address_ack       = 0x2783, 
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_winddir_set_modbus_address_ack_t;

//HUITP_MSGID_uni_winddir_set_work_cycle               = 0x2704,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_winddir_set_work_cycle_t;

//HUITP_MSGID_uni_winddir_set_work_cycle_ack           = 0x2784,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_winddir_set_work_cycle_ack_t;

//HUITP_MSGID_uni_winddir_set_sample_cycle             = 0x2705,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_winddir_set_sample_cycle_t;

//HUITP_MSGID_uni_winddir_set_sample_cycle_ack         = 0x2785,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_winddir_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_winddir_set_sample_number            = 0x2706,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_winddir_set_sample_number_t;
   
//HUITP_MSGID_uni_winddir_set_sample_number_ack        = 0x2786,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_winddir_set_sample_number_ack_t;

//HUITP_MSGID_uni_winddir_read_switch                  = 0x2707,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_read_switch_t;

//HUITP_MSGID_uni_winddir_read_switch_ack              = 0x2787,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_winddir_read_switch_ack_t;

//HUITP_MSGID_uni_winddir_read_modbus_address          = 0x2708,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_read_modbus_address_t;

//HUITP_MSGID_uni_winddir_read_modbus_address_ack      = 0x2788,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_winddir_read_modbus_address_ack_t;

//HUITP_MSGID_uni_winddir_read_work_cycle              = 0x2709,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_read_work_cycle_t;

//HUITP_MSGID_uni_winddir_read_work_cycle_ack          = 0x2789,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_winddir_read_work_cycle_ack_t;

//HUITP_MSGID_uni_winddir_read_sample_cycle            = 0x270A, 
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_read_sample_cycle_t;

//HUITP_MSGID_uni_winddir_read_sample_cycle_ack        = 0x278A,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_winddir_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_winddir_read_sample_number           = 0x270B,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_winddir_read_sample_number_t;

//HUITP_MSGID_uni_winddir_read_sample_number_ack       = 0x278B,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_winddir_read_sample_number_ack_t;

//HUITP_MSGID_uni_winddir_max,

//温度Temperature
//HUITP_MSGID_uni_temp_min                         = 0x2800, 
//HUITP_MSGID_uni_temp_data_req                     = 0x2800,
typedef struct StrMsg_HUITP_MSGID_uni_temp_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_data_req_t;

//HUITP_MSGID_uni_temp_data_resp                    = 0x2880,
typedef struct StrMsg_HUITP_MSGID_uni_temp_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_temp_value_t respValue;
}StrMsg_HUITP_MSGID_uni_temp_data_resp_t;
 
//HUITP_MSGID_uni_temp_data_report                  = 0x2881,
typedef struct StrMsg_HUITP_MSGID_uni_temp_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_temp_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_temp_data_report_t;

//HUITP_MSGID_uni_temp_data_confirm                     = 0x2801,
typedef struct StrMsg_HUITP_MSGID_uni_temp_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_temp_data_confirm_t;

//HUITP_MSGID_uni_temp_set_switch                   = 0x2802,
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_temp_set_switch_t;

//HUITP_MSGID_uni_temp_set_switch_ack               = 0x2882, 	
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_temp_set_switch_ack_t;

//HUITP_MSGID_uni_temp_set_modbus_address           = 0x2803,
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_temp_set_modbus_address_t;
 
//HUITP_MSGID_uni_temp_set_modbus_address_ack       = 0x2883, 
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_temp_set_modbus_address_ack_t;

//HUITP_MSGID_uni_temp_set_work_cycle               = 0x2804,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_temp_set_work_cycle_t;

//HUITP_MSGID_uni_temp_set_work_cycle_ack           = 0x2884,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_temp_set_work_cycle_ack_t;

//HUITP_MSGID_uni_temp_set_sample_cycle             = 0x2805,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_temp_set_sample_cycle_t;

//HUITP_MSGID_uni_temp_set_sample_cycle_ack         = 0x2885,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_temp_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_temp_set_sample_number            = 0x2806,
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_temp_set_sample_number_t;
   
//HUITP_MSGID_uni_temp_set_sample_number_ack        = 0x2886,
typedef struct StrMsg_HUITP_MSGID_uni_temp_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_temp_set_sample_number_ack_t;

//HUITP_MSGID_uni_temp_read_switch                  = 0x2807,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_read_switch_t;

//HUITP_MSGID_uni_temp_read_switch_ack              = 0x2887,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_temp_read_switch_ack_t;

//HUITP_MSGID_uni_temp_read_modbus_address          = 0x2808,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_read_modbus_address_t;

//HUITP_MSGID_uni_temp_read_modbus_address_ack      = 0x2888,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_temp_read_modbus_address_ack_t;

//HUITP_MSGID_uni_temp_read_work_cycle              = 0x2809,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_read_work_cycle_t;

//HUITP_MSGID_uni_temp_read_work_cycle_ack          = 0x2889,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_temp_read_work_cycle_ack_t;

//HUITP_MSGID_uni_temp_read_sample_cycle            = 0x280A, 
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_read_sample_cycle_t;

//HUITP_MSGID_uni_temp_read_sample_cycle_ack        = 0x288A,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_temp_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_temp_read_sample_number           = 0x280B,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_temp_read_sample_number_t;

//HUITP_MSGID_uni_temp_read_sample_number_ack       = 0x288B,
typedef struct StrMsg_HUITP_MSGID_uni_temp_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_temp_read_sample_number_ack_t;

//HUITP_MSGID_uni_temp_max,

//湿度Humidity
//HUITP_MSGID_uni_humid_min                        = 0x2900, 
//HUITP_MSGID_uni_humid_data_req                     = 0x2900,
typedef struct StrMsg_HUITP_MSGID_uni_humid_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_data_req_t;

//HUITP_MSGID_uni_humid_data_resp                    = 0x2980,
typedef struct StrMsg_HUITP_MSGID_uni_humid_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_humid_value_t respValue;
}StrMsg_HUITP_MSGID_uni_humid_data_resp_t;
 
//HUITP_MSGID_uni_humid_data_report                  = 0x2981,
typedef struct StrMsg_HUITP_MSGID_uni_humid_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_humid_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_humid_data_report_t;

//HUITP_MSGID_uni_humid_data_confirm                     = 0x2901,
typedef struct StrMsg_HUITP_MSGID_uni_humid_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_humid_data_confirm_t;

//HUITP_MSGID_uni_humid_set_switch                   = 0x2902,
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_humid_set_switch_t;

//HUITP_MSGID_uni_humid_set_switch_ack               = 0x2982, 	
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_humid_set_switch_ack_t;

//HUITP_MSGID_uni_humid_set_modbus_address           = 0x2903,
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_humid_set_modbus_address_t;
 
//HUITP_MSGID_uni_humid_set_modbus_address_ack       = 0x2983, 
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_humid_set_modbus_address_ack_t;

//HUITP_MSGID_uni_humid_set_work_cycle               = 0x2904,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_humid_set_work_cycle_t;

//HUITP_MSGID_uni_humid_set_work_cycle_ack           = 0x2984,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_humid_set_work_cycle_ack_t;

//HUITP_MSGID_uni_humid_set_sample_cycle             = 0x2905,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_humid_set_sample_cycle_t;

//HUITP_MSGID_uni_humid_set_sample_cycle_ack         = 0x2985,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_humid_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_humid_set_sample_number            = 0x2906,
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_humid_set_sample_number_t;
   
//HUITP_MSGID_uni_humid_set_sample_number_ack        = 0x2986,
typedef struct StrMsg_HUITP_MSGID_uni_humid_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_humid_set_sample_number_ack_t;

//HUITP_MSGID_uni_humid_read_switch                  = 0x2907,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_read_switch_t;

//HUITP_MSGID_uni_humid_read_switch_ack              = 0x2987,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_humid_read_switch_ack_t;

//HUITP_MSGID_uni_humid_read_modbus_address          = 0x2908,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_read_modbus_address_t;

//HUITP_MSGID_uni_humid_read_modbus_address_ack      = 0x2988,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_humid_read_modbus_address_ack_t;

//HUITP_MSGID_uni_humid_read_work_cycle              = 0x2909,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_read_work_cycle_t;

//HUITP_MSGID_uni_humid_read_work_cycle_ack          = 0x2989,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_humid_read_work_cycle_ack_t;

//HUITP_MSGID_uni_humid_read_sample_cycle            = 0x290A, 
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_read_sample_cycle_t;

//HUITP_MSGID_uni_humid_read_sample_cycle_ack        = 0x298A,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_humid_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_humid_read_sample_number           = 0x290B,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_humid_read_sample_number_t;

//HUITP_MSGID_uni_humid_read_sample_number_ack       = 0x298B,
typedef struct StrMsg_HUITP_MSGID_uni_humid_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_humid_read_sample_number_ack_t;

//HUITP_MSGID_uni_humid_max,

//气压Air pressure
//HUITP_MSGID_uni_airprs_min                       = 0x2A00, 
//HUITP_MSGID_uni_airprs_data_req                     = 0x2A00,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_data_req_t;

//HUITP_MSGID_uni_airprs_data_resp                    = 0x2A80,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_airprs_value_t respValue;
}StrMsg_HUITP_MSGID_uni_airprs_data_resp_t;
 
//HUITP_MSGID_uni_airprs_data_report                  = 0x2A81,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_airprs_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_airprs_data_report_t;

//HUITP_MSGID_uni_airprs_data_confirm                     = 0x2A01,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t;

//HUITP_MSGID_uni_airprs_set_switch                   = 0x2A02,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_airprs_set_switch_t;

//HUITP_MSGID_uni_airprs_set_switch_ack               = 0x2A82, 	
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_airprs_set_switch_ack_t;

//HUITP_MSGID_uni_airprs_set_modbus_address           = 0x2A03,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_airprs_set_modbus_address_t;
 
//HUITP_MSGID_uni_airprs_set_modbus_address_ack       = 0x2A83, 
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_airprs_set_modbus_address_ack_t;

//HUITP_MSGID_uni_airprs_set_work_cycle               = 0x2A04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_airprs_set_work_cycle_t;

//HUITP_MSGID_uni_airprs_set_work_cycle_ack           = 0x2A84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_airprs_set_work_cycle_ack_t;

//HUITP_MSGID_uni_airprs_set_sample_cycle             = 0x2A05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_airprs_set_sample_cycle_t;

//HUITP_MSGID_uni_airprs_set_sample_cycle_ack         = 0x2A85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_airprs_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_airprs_set_sample_number            = 0x2A06,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_airprs_set_sample_number_t;
   
//HUITP_MSGID_uni_airprs_set_sample_number_ack        = 0x2A86,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_airprs_set_sample_number_ack_t;

//HUITP_MSGID_uni_airprs_read_switch                  = 0x2A07,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_read_switch_t;

//HUITP_MSGID_uni_airprs_read_switch_ack              = 0x2A87,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_airprs_read_switch_ack_t;

//HUITP_MSGID_uni_airprs_read_modbus_address          = 0x2A08,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_read_modbus_address_t;

//HUITP_MSGID_uni_airprs_read_modbus_address_ack      = 0x2A88,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_airprs_read_modbus_address_ack_t;

//HUITP_MSGID_uni_airprs_read_work_cycle              = 0x2A09,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_read_work_cycle_t;

//HUITP_MSGID_uni_airprs_read_work_cycle_ack          = 0x2A89,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_airprs_read_work_cycle_ack_t;

//HUITP_MSGID_uni_airprs_read_sample_cycle            = 0x2A0A, 
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_read_sample_cycle_t;

//HUITP_MSGID_uni_airprs_read_sample_cycle_ack        = 0x2A8A,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_airprs_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_airprs_read_sample_number           = 0x2A0B,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_airprs_read_sample_number_t;

//HUITP_MSGID_uni_airprs_read_sample_number_ack       = 0x2A8B,
typedef struct StrMsg_HUITP_MSGID_uni_airprs_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_airprs_read_sample_number_ack_t;

//HUITP_MSGID_uni_airprs_max,

//噪声Noise
//HUITP_MSGID_uni_noise_min                        = 0x2B00, 
//HUITP_MSGID_uni_noise_data_req                     = 0x2B00,
typedef struct StrMsg_HUITP_MSGID_uni_noise_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_data_req_t;

//HUITP_MSGID_uni_noise_data_resp                    = 0x2B80,
typedef struct StrMsg_HUITP_MSGID_uni_noise_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_noise_value_t respValue;
}StrMsg_HUITP_MSGID_uni_noise_data_resp_t;
 
//HUITP_MSGID_uni_noise_data_report                  = 0x2B81,
typedef struct StrMsg_HUITP_MSGID_uni_noise_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_noise_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_noise_data_report_t;

//HUITP_MSGID_uni_noise_data_confirm                     = 0x2B01,
typedef struct StrMsg_HUITP_MSGID_uni_noise_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_noise_data_confirm_t;

//HUITP_MSGID_uni_noise_set_switch                   = 0x2B02,
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_noise_set_switch_t;

//HUITP_MSGID_uni_noise_set_switch_ack               = 0x2B82, 	
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_noise_set_switch_ack_t;

//HUITP_MSGID_uni_noise_set_modbus_address           = 0x2B03,
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_noise_set_modbus_address_t;
 
//HUITP_MSGID_uni_noise_set_modbus_address_ack       = 0x2B83, 
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_noise_set_modbus_address_ack_t;

//HUITP_MSGID_uni_noise_set_work_cycle               = 0x2B04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_noise_set_work_cycle_t;

//HUITP_MSGID_uni_noise_set_work_cycle_ack           = 0x2B84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_noise_set_work_cycle_ack_t;

//HUITP_MSGID_uni_noise_set_sample_cycle             = 0x2B05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_noise_set_sample_cycle_t;

//HUITP_MSGID_uni_noise_set_sample_cycle_ack         = 0x2B85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_noise_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_noise_set_sample_number            = 0x2B06,
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_noise_set_sample_number_t;
   
//HUITP_MSGID_uni_noise_set_sample_number_ack        = 0x2B86,
typedef struct StrMsg_HUITP_MSGID_uni_noise_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_noise_set_sample_number_ack_t;

//HUITP_MSGID_uni_noise_read_switch                  = 0x2B07,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_read_switch_t;

//HUITP_MSGID_uni_noise_read_switch_ack              = 0x2B87,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_noise_read_switch_ack_t;

//HUITP_MSGID_uni_noise_read_modbus_address          = 0x2B08,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_read_modbus_address_t;

//HUITP_MSGID_uni_noise_read_modbus_address_ack      = 0x2B88,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_noise_read_modbus_address_ack_t;

//HUITP_MSGID_uni_noise_read_work_cycle              = 0x2B09,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_read_work_cycle_t;

//HUITP_MSGID_uni_noise_read_work_cycle_ack          = 0x2B89,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_noise_read_work_cycle_ack_t;

//HUITP_MSGID_uni_noise_read_sample_cycle            = 0x2B0A, 
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_read_sample_cycle_t;

//HUITP_MSGID_uni_noise_read_sample_cycle_ack        = 0x2B8A,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_noise_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_noise_read_sample_number           = 0x2B0B,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_noise_read_sample_number_t;

//HUITP_MSGID_uni_noise_read_sample_number_ack       = 0x2B8B,
typedef struct StrMsg_HUITP_MSGID_uni_noise_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_noise_read_sample_number_ack_t;

//HUITP_MSGID_uni_noise_max,

//相机Camer or audio high speed
//HUITP_MSGID_uni_hsmmp_min                        = 0x2C00, 
//HUITP_MSGID_uni_hsmmp_data_req                     = 0x2C00,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t;

//HUITP_MSGID_uni_hsmmp_data_resp                    = 0x2C80,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_hsmmp_value_t respValue;
}StrMsg_HUITP_MSGID_uni_hsmmp_data_resp_t;
 
//HUITP_MSGID_uni_hsmmp_data_report                  = 0x2C81,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_hsmmp_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_hsmmp_data_report_t;

//HUITP_MSGID_uni_hsmmp_data_confirm                     = 0x2C01,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t;

//HUITP_MSGID_uni_hsmmp_set_switch                   = 0x2C02,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_switch_t;

//HUITP_MSGID_uni_hsmmp_set_switch_ack               = 0x2C82, 	
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_switch_ack_t;

//HUITP_MSGID_uni_hsmmp_set_modbus_address           = 0x2C03,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_modbus_address_t;
 
//HUITP_MSGID_uni_hsmmp_set_modbus_address_ack       = 0x2C83, 
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_modbus_address_ack_t;

//HUITP_MSGID_uni_hsmmp_set_work_cycle               = 0x2C04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_work_cycle_t;

//HUITP_MSGID_uni_hsmmp_set_work_cycle_ack           = 0x2C84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_work_cycle_ack_t;

//HUITP_MSGID_uni_hsmmp_set_sample_cycle             = 0x2C05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_cycle_t;

//HUITP_MSGID_uni_hsmmp_set_sample_cycle_ack         = 0x2C85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_hsmmp_set_sample_number            = 0x2C06,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_number_t;
   
//HUITP_MSGID_uni_hsmmp_set_sample_number_ack        = 0x2C86,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_hsmmp_set_sample_number_ack_t;

//HUITP_MSGID_uni_hsmmp_read_switch                  = 0x2C07,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_switch_t;

//HUITP_MSGID_uni_hsmmp_read_switch_ack              = 0x2C87,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_switch_ack_t;

//HUITP_MSGID_uni_hsmmp_read_modbus_address          = 0x2C08,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_modbus_address_t;

//HUITP_MSGID_uni_hsmmp_read_modbus_address_ack      = 0x2C88,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_modbus_address_ack_t;

//HUITP_MSGID_uni_hsmmp_read_work_cycle              = 0x2C09,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_work_cycle_t;

//HUITP_MSGID_uni_hsmmp_read_work_cycle_ack          = 0x2C89,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_work_cycle_ack_t;

//HUITP_MSGID_uni_hsmmp_read_sample_cycle            = 0x2C0A, 
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_cycle_t;

//HUITP_MSGID_uni_hsmmp_read_sample_cycle_ack        = 0x2C8A,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_hsmmp_read_sample_number           = 0x2C0B,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_number_t;

//HUITP_MSGID_uni_hsmmp_read_sample_number_ack       = 0x2C8B,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_hsmmp_read_sample_number_ack_t;

//HUITP_MSGID_uni_hsmmp_max,

//声音
//HUITP_MSGID_uni_audio_min                        = 0x2D00, 
//HUITP_MSGID_uni_audio_data_req                     = 0x2D00,
typedef struct StrMsg_HUITP_MSGID_uni_audio_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_data_req_t;

//HUITP_MSGID_uni_audio_data_resp                    = 0x2D80,
typedef struct StrMsg_HUITP_MSGID_uni_audio_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_audio_value_t respValue;
}StrMsg_HUITP_MSGID_uni_audio_data_resp_t;
 
//HUITP_MSGID_uni_audio_data_report                  = 0x2D81,
typedef struct StrMsg_HUITP_MSGID_uni_audio_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_audio_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_audio_data_report_t;

//HUITP_MSGID_uni_audio_data_confirm                     = 0x2D01,
typedef struct StrMsg_HUITP_MSGID_uni_audio_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_audio_data_confirm_t;

//HUITP_MSGID_uni_audio_set_switch                   = 0x2D02,
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_audio_set_switch_t;

//HUITP_MSGID_uni_audio_set_switch_ack               = 0x2D82, 	
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_audio_set_switch_ack_t;

//HUITP_MSGID_uni_audio_set_modbus_address           = 0x2D03,
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_audio_set_modbus_address_t;
 
//HUITP_MSGID_uni_audio_set_modbus_address_ack       = 0x2D83, 
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_audio_set_modbus_address_ack_t;

//HUITP_MSGID_uni_audio_set_work_cycle               = 0x2D04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_audio_set_work_cycle_t;

//HUITP_MSGID_uni_audio_set_work_cycle_ack           = 0x2D84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_audio_set_work_cycle_ack_t;

//HUITP_MSGID_uni_audio_set_sample_cycle             = 0x2D05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_audio_set_sample_cycle_t;

//HUITP_MSGID_uni_audio_set_sample_cycle_ack         = 0x2D85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_audio_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_audio_set_sample_number            = 0x2D06,
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_audio_set_sample_number_t;
   
//HUITP_MSGID_uni_audio_set_sample_number_ack        = 0x2D86,
typedef struct StrMsg_HUITP_MSGID_uni_audio_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_audio_set_sample_number_ack_t;

//HUITP_MSGID_uni_audio_read_switch                  = 0x2D07,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_read_switch_t;

//HUITP_MSGID_uni_audio_read_switch_ack              = 0x2D87,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_audio_read_switch_ack_t;

//HUITP_MSGID_uni_audio_read_modbus_address          = 0x2D08,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_read_modbus_address_t;

//HUITP_MSGID_uni_audio_read_modbus_address_ack      = 0x2D88,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_audio_read_modbus_address_ack_t;

//HUITP_MSGID_uni_audio_read_work_cycle              = 0x2D09,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_read_work_cycle_t;

//HUITP_MSGID_uni_audio_read_work_cycle_ack          = 0x2D89,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_audio_read_work_cycle_ack_t;

//HUITP_MSGID_uni_audio_read_sample_cycle            = 0x2D0A, 
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_read_sample_cycle_t;

//HUITP_MSGID_uni_audio_read_sample_cycle_ack        = 0x2D8A,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_audio_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_audio_read_sample_number           = 0x2D0B,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_audio_read_sample_number_t;

//HUITP_MSGID_uni_audio_read_sample_number_ack       = 0x2D8B,
typedef struct StrMsg_HUITP_MSGID_uni_audio_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_audio_read_sample_number_ack_t;

//HUITP_MSGID_uni_audio_max,

//视频
//HUITP_MSGID_uni_video_min                        = 0x2E00, 
//HUITP_MSGID_uni_video_data_req                     = 0x2E00,
typedef struct StrMsg_HUITP_MSGID_uni_video_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_data_req_t;

//HUITP_MSGID_uni_video_data_resp                    = 0x2E80,
typedef struct StrMsg_HUITP_MSGID_uni_video_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_video_value_t respValue;
}StrMsg_HUITP_MSGID_uni_video_data_resp_t;
 
//HUITP_MSGID_uni_video_data_report                  = 0x2E81,
typedef struct StrMsg_HUITP_MSGID_uni_video_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_video_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_video_data_report_t;

//HUITP_MSGID_uni_video_data_confirm                     = 0x2E01,
typedef struct StrMsg_HUITP_MSGID_uni_video_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_video_data_confirm_t;

//HUITP_MSGID_uni_video_set_switch                   = 0x2E02,
typedef struct StrMsg_HUITP_MSGID_uni_video_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_video_set_switch_t;

//HUITP_MSGID_uni_video_set_switch_ack               = 0x2E82, 	
typedef struct StrMsg_HUITP_MSGID_uni_video_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_video_set_switch_ack_t;

//HUITP_MSGID_uni_video_set_modbus_address           = 0x2E03,
typedef struct StrMsg_HUITP_MSGID_uni_video_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_video_set_modbus_address_t;
 
//HUITP_MSGID_uni_video_set_modbus_address_ack       = 0x2E83, 
typedef struct StrMsg_HUITP_MSGID_uni_video_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_video_set_modbus_address_ack_t;

//HUITP_MSGID_uni_video_set_work_cycle               = 0x2E04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_video_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_video_set_work_cycle_t;

//HUITP_MSGID_uni_video_set_work_cycle_ack           = 0x2E84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_video_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_video_set_work_cycle_ack_t;

//HUITP_MSGID_uni_video_set_sample_cycle             = 0x2E05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_video_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_video_set_sample_cycle_t;

//HUITP_MSGID_uni_video_set_sample_cycle_ack         = 0x2E85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_video_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_video_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_video_set_sample_number            = 0x2E06,
typedef struct StrMsg_HUITP_MSGID_uni_video_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_video_set_sample_number_t;
   
//HUITP_MSGID_uni_video_set_sample_number_ack        = 0x2E86,
typedef struct StrMsg_HUITP_MSGID_uni_video_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_video_set_sample_number_ack_t;

//HUITP_MSGID_uni_video_read_switch                  = 0x2E07,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_read_switch_t;

//HUITP_MSGID_uni_video_read_switch_ack              = 0x2E87,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_video_read_switch_ack_t;

//HUITP_MSGID_uni_video_read_modbus_address          = 0x2E08,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_read_modbus_address_t;

//HUITP_MSGID_uni_video_read_modbus_address_ack      = 0x2E88,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_video_read_modbus_address_ack_t;

//HUITP_MSGID_uni_video_read_work_cycle              = 0x2E09,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_read_work_cycle_t;

//HUITP_MSGID_uni_video_read_work_cycle_ack          = 0x2E89,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_video_read_work_cycle_ack_t;

//HUITP_MSGID_uni_video_read_sample_cycle            = 0x2E0A, 
typedef struct StrMsg_HUITP_MSGID_uni_video_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_read_sample_cycle_t;

//HUITP_MSGID_uni_video_read_sample_cycle_ack        = 0x2E8A,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_video_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_video_read_sample_number           = 0x2E0B,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_video_read_sample_number_t;

//HUITP_MSGID_uni_video_read_sample_number_ack       = 0x2E8B,
typedef struct StrMsg_HUITP_MSGID_uni_video_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_video_read_sample_number_ack_t;

//HUITP_MSGID_uni_video_max,

//图片
//HUITP_MSGID_uni_picture_min                      = 0x2F00, 
//HUITP_MSGID_uni_picture_data_req                     = 0x2F00,
typedef struct StrMsg_HUITP_MSGID_uni_picture_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_data_req_t;

//HUITP_MSGID_uni_picture_data_resp                    = 0x2F80,
typedef struct StrMsg_HUITP_MSGID_uni_picture_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_picture_value_t respValue;
}StrMsg_HUITP_MSGID_uni_picture_data_resp_t;
 
//HUITP_MSGID_uni_picture_data_report                  = 0x2F81,
typedef struct StrMsg_HUITP_MSGID_uni_picture_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_picture_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_picture_data_report_t;

//HUITP_MSGID_uni_picture_data_confirm                     = 0x2F01,
typedef struct StrMsg_HUITP_MSGID_uni_picture_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_picture_data_confirm_t;

//HUITP_MSGID_uni_picture_set_switch                   = 0x2F02,
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t setSwitch;
}StrMsg_HUITP_MSGID_uni_picture_set_switch_t;

//HUITP_MSGID_uni_picture_set_switch_ack               = 0x2F82, 	
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_picture_set_switch_ack_t;

//HUITP_MSGID_uni_picture_set_modbus_address           = 0x2F03,
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_modbus_address_t setModbusAddr;
}StrMsg_HUITP_MSGID_uni_picture_set_modbus_address_t;
 
//HUITP_MSGID_uni_picture_set_modbus_address_ack       = 0x2F83, 
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_picture_set_modbus_address_ack_t;

//HUITP_MSGID_uni_picture_set_work_cycle               = 0x2F04,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_work_cycle_t setWorkCycle;
}StrMsg_HUITP_MSGID_uni_picture_set_work_cycle_t;

//HUITP_MSGID_uni_picture_set_work_cycle_ack           = 0x2F84,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_picture_set_work_cycle_ack_t;

//HUITP_MSGID_uni_picture_set_sample_cycle             = 0x2F05,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t setSampleCycle;
}StrMsg_HUITP_MSGID_uni_picture_set_sample_cycle_t;

//HUITP_MSGID_uni_picture_set_sample_cycle_ack         = 0x2F85,   //In second
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_picture_set_sample_cycle_ack_t;

//HUITP_MSGID_uni_picture_set_sample_number            = 0x2F06,
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_sample_number_t setSampleNumber;
}StrMsg_HUITP_MSGID_uni_picture_set_sample_number_t;
   
//HUITP_MSGID_uni_picture_set_sample_number_ack        = 0x2F86,
typedef struct StrMsg_HUITP_MSGID_uni_picture_set_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_picture_set_sample_number_ack_t;

//HUITP_MSGID_uni_picture_read_switch                  = 0x2F07,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_switch
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_read_switch_t;

//HUITP_MSGID_uni_picture_read_switch_ack              = 0x2F87,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_switch_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t readSwitch;
}StrMsg_HUITP_MSGID_uni_picture_read_switch_ack_t;

//HUITP_MSGID_uni_picture_read_modbus_address          = 0x2F08,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_modbus_address
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_read_modbus_address_t;

//HUITP_MSGID_uni_picture_read_modbus_address_ack      = 0x2F88,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_modbus_address_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_modbus_address_t readModbusAddr;
}StrMsg_HUITP_MSGID_uni_picture_read_modbus_address_ack_t;

//HUITP_MSGID_uni_picture_read_work_cycle              = 0x2F09,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_work_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_read_work_cycle_t;

//HUITP_MSGID_uni_picture_read_work_cycle_ack          = 0x2F89,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_work_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_work_cycle_t readWorkCycle;
}StrMsg_HUITP_MSGID_uni_picture_read_work_cycle_ack_t;

//HUITP_MSGID_uni_picture_read_sample_cycle            = 0x2F0A, 
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_sample_cycle
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_read_sample_cycle_t;

//HUITP_MSGID_uni_picture_read_sample_cycle_ack        = 0x2F8A,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_sample_cycle_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t readSampleCycle;
}StrMsg_HUITP_MSGID_uni_picture_read_sample_cycle_ack_t;
 
//HUITP_MSGID_uni_picture_read_sample_number           = 0x2F0B,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_sample_number
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_picture_read_sample_number_t;

//HUITP_MSGID_uni_picture_read_sample_number_ack       = 0x2F8B,
typedef struct StrMsg_HUITP_MSGID_uni_picture_read_sample_number_ack
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_sample_number_t readSampleNumber;
}StrMsg_HUITP_MSGID_uni_picture_read_sample_number_ack_t;

//HUITP_MSGID_uni_picture_max,

//扬尘监控系统
//HUITP_MSGID_uni_ycjk_min                         = 0x3000, 
//HUITP_MSGID_uni_ycjk_req                         = 0x3000,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ycjk_req_t;

//HUITP_MSGID_uni_ycjk_resp                        = 0x3080,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ycjk_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ycjk_resp_t;

//HUITP_MSGID_uni_ycjk_report                      = 0x3081,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ycjk_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_ycjk_report_t;

//HUITP_MSGID_uni_ycjk_confirm                         = 0x3001,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ycjk_confirm_t;

//HUITP_MSGID_uni_ycjk_max,

//水表
//HUITP_MSGID_uni_water_meter_min                  = 0x3100,
//HUITP_MSGID_uni_water_meter_req                  = 0x3100,
typedef struct StrMsg_HUITP_MSGID_uni_water_meter_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_water_meter_req_t;
 
//HUITP_MSGID_uni_water_meter_resp                 = 0x3180,
typedef struct StrMsg_HUITP_MSGID_uni_water_meter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_water_meter_value_t respValue;
}StrMsg_HUITP_MSGID_uni_water_meter_resp_t;
 
//HUITP_MSGID_uni_water_meter_report               = 0x3181,
typedef struct StrMsg_HUITP_MSGID_uni_water_meter_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_water_meter_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_water_meter_report_t;

//HUITP_MSGID_uni_water_meter_confirm                  = 0x3101,
typedef struct StrMsg_HUITP_MSGID_uni_water_meter_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_water_meter_confirm_t;

//HUITP_MSGID_uni_water_meter_max,

//热表
//HUITP_MSGID_uni_heat_meter_min                   = 0x3200, 
//HUITP_MSGID_uni_heat_meter_req                   = 0x3200,
typedef struct StrMsg_HUITP_MSGID_uni_heat_meter_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_heat_meter_req_t;

//HUITP_MSGID_uni_heat_meter_resp                  = 0x3280,
typedef struct StrMsg_HUITP_MSGID_uni_heat_meter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_heat_meter_value_t respValue;
}StrMsg_HUITP_MSGID_uni_heat_meter_resp_t;
 
//HUITP_MSGID_uni_heat_meter_report                = 0x3281,
typedef struct StrMsg_HUITP_MSGID_uni_heat_meter_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_heat_meter_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_heat_meter_report_t;

//HUITP_MSGID_uni_heat_meter_confirm                   = 0x3201,
typedef struct StrMsg_HUITP_MSGID_uni_heat_meter_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_heat_meter_confirm_t;

//HUITP_MSGID_uni_heat_meter_max,

//气表
//HUITP_MSGID_uni_gas_meter_min                    = 0x3300, 
//HUITP_MSGID_uni_gas_meter_req                    = 0x3300,
typedef struct StrMsg_HUITP_MSGID_uni_gas_meter_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_gas_meter_req_t;

//HUITP_MSGID_uni_gas_meter_resp                   = 0x3380,
typedef struct StrMsg_HUITP_MSGID_uni_gas_meter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_gas_meter_value_t respValue;
}StrMsg_HUITP_MSGID_uni_gas_meter_resp_t;
 
//HUITP_MSGID_uni_gas_meter_report                 = 0x3381,
typedef struct StrMsg_HUITP_MSGID_uni_gas_meter_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_gas_meter_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_gas_meter_report_t;

//HUITP_MSGID_uni_gas_meter_confirm                    = 0x3301,
typedef struct StrMsg_HUITP_MSGID_uni_gas_meter_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_gas_meter_confirm_t;

//HUITP_MSGID_uni_gas_meter_max,

//电表
//HUITP_MSGID_uni_power_meter_min                  = 0x3400, 
//HUITP_MSGID_uni_power_meter_req                  = 0x3400,
typedef struct StrMsg_HUITP_MSGID_uni_power_meter_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_power_meter_req_t;

//HUITP_MSGID_uni_power_meter_resp                 = 0x3480,
typedef struct StrMsg_HUITP_MSGID_uni_power_meter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_power_meter_value_t respValue;
}StrMsg_HUITP_MSGID_uni_power_meter_resp_t;
 
//HUITP_MSGID_uni_power_meter_report               = 0x3481,
typedef struct StrMsg_HUITP_MSGID_uni_power_meter_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_power_meter_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_power_meter_report_t;

//HUITP_MSGID_uni_power_meter_confirm                  = 0x3401,
typedef struct StrMsg_HUITP_MSGID_uni_power_meter_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_power_meter_confirm_t;

//HUITP_MSGID_uni_power_meter_max,

//光照强度
//HUITP_MSGID_uni_light_strength_min               = 0x3500, 
//HUITP_MSGID_uni_light_strength_req               = 0x3500,
typedef struct StrMsg_HUITP_MSGID_uni_light_strength_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_light_strength_req_t;

//HUITP_MSGID_uni_light_strength_resp              = 0x3580,
typedef struct StrMsg_HUITP_MSGID_uni_light_strength_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_light_strength_value_t respValue;
}StrMsg_HUITP_MSGID_uni_light_strength_resp_t;

//HUITP_MSGID_uni_light_strength_report            = 0x3581,
typedef struct StrMsg_HUITP_MSGID_uni_light_strength_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_light_strength_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_light_strength_report_t;

//HUITP_MSGID_uni_light_strength_confirm               = 0x3501,
typedef struct StrMsg_HUITP_MSGID_uni_light_strength_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_HUITP_MSGID_uni_light_strength_confirm_t;
 
//HUITP_MSGID_uni_light_strength_max,

//有毒气体VOC
//HUITP_MSGID_uni_toxicgas_min                     = 0x3600, 
//HUITP_MSGID_uni_toxicgas_req                     = 0x3600,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_toxicgas_req_t;
 
//HUITP_MSGID_uni_toxicgas_resp                    = 0x3680,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_toxicgas_value_t respValue;
}StrMsg_HUITP_MSGID_uni_toxicgas_resp_t;
 
//HUITP_MSGID_uni_toxicgas_report                  = 0x3681,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_toxicgas_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_toxicgas_report_t;

//HUITP_MSGID_uni_toxicgas_confirm                     = 0x3601,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_toxicgas_confirm_t;
 
//HUITP_MSGID_uni_toxicgas_max,

//海拔高度
//HUITP_MSGID_uni_altitude_min                     = 0x3700,
//HUITP_MSGID_uni_altitude_req                     = 0x3700,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_altitude_req_t;
 
//HUITP_MSGID_uni_altitude_resp                    = 0x3780,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_altitude_value_t respValue;
}StrMsg_HUITP_MSGID_uni_altitude_resp_t;
 
//HUITP_MSGID_uni_altitude_report                  = 0x3781,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_altitude_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_altitude_report_t;

//HUITP_MSGID_uni_altitude_confirm                     = 0x3701,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_altitude_confirm_t;

//HUITP_MSGID_uni_altitude_max,

//马达
//HUITP_MSGID_uni_moto_min                         = 0x3800,
//HUITP_MSGID_uni_moto_req                     = 0x3800,
typedef struct StrMsg_HUITP_MSGID_uni_moto_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_moto_req_t;
 
//HUITP_MSGID_uni_moto_resp                    = 0x3880,
typedef struct StrMsg_HUITP_MSGID_uni_moto_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_moto_value_t respValue;
}StrMsg_HUITP_MSGID_uni_moto_resp_t;
 
//HUITP_MSGID_uni_moto_report                  = 0x3881,
typedef struct StrMsg_HUITP_MSGID_uni_moto_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_moto_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_moto_report_t;

//HUITP_MSGID_uni_moto_confirm                     = 0x3801,
typedef struct StrMsg_HUITP_MSGID_uni_moto_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_moto_confirm_t;

//HUITP_MSGID_uni_moto_max,

//继电器
//HUITP_MSGID_uni_switch_resistor_min                       = 0x3900, 
//HUITP_MSGID_uni_switch_resistor_req                     = 0x3900,
typedef struct StrMsg_HUITP_MSGID_uni_switch_resistor_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_switch_resistor_req_t;
 
//HUITP_MSGID_uni_switch_resistor_resp                    = 0x3980,
typedef struct StrMsg_HUITP_MSGID_uni_switch_resistor_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_switch_resistor_value_t respValue;
}StrMsg_HUITP_MSGID_uni_switch_resistor_resp_t;
 
//HUITP_MSGID_uni_switch_resistor_report                  = 0x3981,
typedef struct StrMsg_HUITP_MSGID_uni_switch_resistor_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_switch_resistor_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_switch_resistor_report_t;

//HUITP_MSGID_uni_switch_resistor_confirm                     = 0x3901,
typedef struct StrMsg_HUITP_MSGID_uni_switch_resistor_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_switch_resistor_confirm_t;

//HUITP_MSGID_uni_switch_resistor_max,

//导轨传送带
//HUITP_MSGID_uni_transporter_min                  = 0x3A00, 
//HUITP_MSGID_uni_transporter_req                     = 0x3A00,
typedef struct StrMsg_HUITP_MSGID_uni_transporter_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_transporter_req_t;
 
//HUITP_MSGID_uni_transporter_resp                    = 0x3A80,
typedef struct StrMsg_HUITP_MSGID_uni_transporter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_transporter_value_t respValue;
}StrMsg_HUITP_MSGID_uni_transporter_resp_t;
 
//HUITP_MSGID_uni_transporter_report                  = 0x3A81,
typedef struct StrMsg_HUITP_MSGID_uni_transporter_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_transporter_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_transporter_report_t;

//HUITP_MSGID_uni_transporter_confirm                     = 0x3A01,
typedef struct StrMsg_HUITP_MSGID_uni_transporter_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_transporter_confirm_t;

//HUITP_MSGID_uni_transporter_max,

//组合秤BFSC
//HUITP_MSGID_uni_bfsc_comb_scale_min              = 0x3B00, 
//HUITP_MSGID_uni_bfsc_comb_scale_req                     = 0x3B00,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_req_t;
 
//HUITP_MSGID_uni_bfsc_comb_scale_resp                    = 0x3B80,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_value_t respValue;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_resp_t;
 
//HUITP_MSGID_uni_bfsc_comb_scale_report                  = 0x3B81,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_bfsc_comb_scale_report_t;

//HUITP_MSGID_uni_bfsc_comb_scale_confirm                     = 0x3B01,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_confirm_t;

//HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_req    = 0x3B02,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmd;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_req_t;
 
//HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_resp   = 0x3B82, 
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_start_resp_t;

//HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_req     = 0x3B03, 
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmd;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_req_t;

//HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_resp    = 0x3B83,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_stop_resp_t;

//HUITP_MSGID_uni_bfsc_comb_scale_max,

//云控锁-锁-兼容旧系统
//HUITP_MSGID_uni_ccl_lock_old_min                     = 0x4000,  
//HUITP_MSGID_uni_ccl_lock_old_req                     = 0x4000,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_req_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_lock_req_old_t;

//HUITP_MSGID_uni_ccl_lock_resp_old                    = 0x4080, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_resp_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_lock_resp_old_t;

//HUITP_MSGID_uni_ccl_lock_report_old                  = 0x4081, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_report_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_lock_report_old_t;

//HUITP_MSGID_uni_ccl_lock_confirm _old                    = 0x4001,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_old_t;

//HUITP_MSGID_uni_ccl_lock_auth_inq_old                = 0x4090, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t authReq;
}StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_old_t;

//HUITP_MSGID_uni_ccl_lock_auth_resp               = 0x4010,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_old_t;

//HUITP_MSGID_uni_ccl_lock_old_max,

//云控锁-门
//HUITP_MSGID_uni_ccl_door_min                     = 0x4100, 
//HUITP_MSGID_uni_ccl_door_req                     = 0x4100,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_door_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_door_req_t;

//HUITP_MSGID_uni_ccl_door_resp                    = 0x4180,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_door_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_door_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_door_resp_t;
 
//HUITP_MSGID_uni_ccl_door_report                  = 0x4181,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_door_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_door_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_door_report_t;

//HUITP_MSGID_uni_ccl_door_confirm                     = 0x4101, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_door_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t;

//HUITP_MSGID_uni_ccl_door_max,

//云控锁-RFID模块
//HUITP_MSGID_uni_ccl_rfid_min                     = 0x4200, 
//HUITP_MSGID_uni_ccl_rfid_req                     = 0x4200,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_rfid_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t;

//HUITP_MSGID_uni_ccl_rfid_resp                    = 0x4280,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_rfid_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_rfid_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_rfid_resp_t;

//HUITP_MSGID_uni_ccl_rfid_report                  = 0x4281,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_rfid_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_rfid_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_rfid_report_t;

//HUITP_MSGID_uni_ccl_rfid_confirm                     = 0x4201,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t;

//HUITP_MSGID_uni_ccl_rfid_max,

//云控锁-BLE模块
//HUITP_MSGID_uni_ccl_ble_min                      = 0x4300, 
//HUITP_MSGID_uni_ccl_ble_req                      = 0x4300,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_ble_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_ble_req_t;
 
//HUITP_MSGID_uni_ccl_ble_resp                     = 0x4380,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_ble_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_ble_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_ble_resp_t;

//HUITP_MSGID_uni_ccl_ble_report                   = 0x4381,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_ble_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_ble_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_ble_report_t;

//HUITP_MSGID_uni_ccl_ble_confirm                      = 0x4301,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_ble_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t;

//HUITP_MSGID_uni_ccl_ble_max,

//云控锁-GPRS模块
//HUITP_MSGID_uni_ccl_gprs_min                     = 0x4400, 
//HUITP_MSGID_uni_ccl_gprs_req                     = 0x4400, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_gprs_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t;

//HUITP_MSGID_uni_ccl_gprs_resp                    = 0x4480, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_gprs_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_gprs_resp_t;

//HUITP_MSGID_uni_ccl_gprs_report                  = 0x4481,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_gprs_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_gprs_report_t;

//HUITP_MSGID_uni_ccl_gprs_confirm                     = 0x4401,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t;

//HUITP_MSGID_uni_ccl_gprs_max,

//云控锁-电池模块
//HUITP_MSGID_uni_ccl_battery_min                  = 0x4500, 
//HUITP_MSGID_uni_ccl_battery_req                  = 0x4500,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_battery_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_battery_req_t;
 
//HUITP_MSGID_uni_ccl_battery_resp                 = 0x4580,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_batter_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_batter_resp_t;

//HUITP_MSGID_uni_ccl_battery_report               = 0x4581,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_battery_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_battery_report_t;
 
//HUITP_MSGID_uni_ccl_battery_confirm                  = 0x4501,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_battery_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t;

//HUITP_MSGID_uni_ccl_battery_max,	

//云控锁-震动
//HUITP_MSGID_uni_ccl_shake_min                    = 0x4600, 
//HUITP_MSGID_uni_ccl_shake_req                    = 0x4600,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_shake_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_shake_req_t;

//HUITP_MSGID_uni_ccl_shake_resp                   = 0x4680,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_shake_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_shake_resp_t;

//HUITP_MSGID_uni_ccl_shake_report                 = 0x4681,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_shake_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_shake_report_t;

//HUITP_MSGID_uni_ccl_shake_confirm                    = 0x4601,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_shake_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t;

//HUITP_MSGID_uni_ccl_shake_max,

//云控锁-烟雾
//HUITP_MSGID_uni_ccl_smoke_min                    = 0x4700, 
//HUITP_MSGID_uni_ccl_smoke_req                    = 0x4700,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_smoke_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t;

//HUITP_MSGID_uni_ccl_smoke_resp                   = 0x4780,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_smoke_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_smoke_resp_t;
 
//HUITP_MSGID_uni_ccl_smoke_report                 = 0x4781,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_smoke_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_smoke_report_t;

//HUITP_MSGID_uni_ccl_smoke_confirm                    = 0x4701,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t;

//HUITP_MSGID_uni_ccl_smoke_max,

//云控锁-水浸
//HUITP_MSGID_uni_ccl_water_min                    = 0x4800, 
//HUITP_MSGID_uni_ccl_water_req                    = 0x4800,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_water_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_water_req_t;

//HUITP_MSGID_uni_ccl_water_resp                   = 0x4880,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_water_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_water_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_water_resp_t;

//HUITP_MSGID_uni_ccl_water_report                 = 0x4881,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_water_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_water_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_water_report_t;

//HUITP_MSGID_uni_ccl_water_confirm                    = 0x4801,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_water_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t;

//HUITP_MSGID_uni_ccl_water_max,

//云控锁-温度
//HUITP_MSGID_uni_ccl_temp_min                     = 0x4900, 
//HUITP_MSGID_uni_ccl_temp_req                     = 0x4900,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_temp_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_temp_req_t;

//HUITP_MSGID_uni_ccl_temp_resp                    = 0x4980,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_temp_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_temp_resp_t;

//HUITP_MSGID_uni_ccl_temp_report                  = 0x4981,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_temp_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_temp_report_t;

//HUITP_MSGID_uni_ccl_temp_confirm                     = 0x4901,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_temp_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t;

//HUITP_MSGID_uni_ccl_temp_max,

//云控锁-湿度
//HUITP_MSGID_uni_ccl_humid_min                    = 0x4A00, 
//HUITP_MSGID_uni_ccl_humid_req                    = 0x4A00,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_humid_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_humid_req_t;

//HUITP_MSGID_uni_ccl_humid_resp                   = 0x4A80,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_humid_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ccl_humid_resp_t;
 
//HUITP_MSGID_uni_ccl_humid_report                 = 0x4A81,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_humid_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_ccl_humid_report_t;

//HUITP_MSGID_uni_ccl_humid_confirm                    = 0x4A01,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_humid_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t;

//HUITP_MSGID_uni_ccl_humid_max,

//云控锁-倾倒
//HUITP_MSGID_uni_ccl_fall_min                     = 0x4B00, 
//HUITP_MSGID_uni_ccl_fall_req                     = 0x4B00,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_fall_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_fall_req_t;

//HUITP_MSGID_uni_ccl_fall_resp                    = 0x4B80, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_fall_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_fall_resp_t;

//HUITP_MSGID_uni_ccl_fall_report                  = 0x4B81,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_fall_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_fall_report_t;

//HUITP_MSGID_uni_ccl_fall_confirm                     = 0x4B01,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_fall_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t;

//HUITP_MSGID_uni_ccl_fall_max,

//云控锁-状态聚合-兼容旧系统
//HUITP_MSGID_uni_ccl_state_old_min                    = 0x4C00, 
//HUITP_MSGID_uni_ccl_state_old_req                    = 0x4C00,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_req_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_state_req_old_t;

//HUITP_MSGID_uni_ccl_state_old_respold                   = 0x4C80,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_resp_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t lockState;
	StrIe_HUITP_IEID_uni_ccl_door_state_t doorState;
	StrIe_HUITP_IEID_uni_ccl_water_state_t waterState;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t fallState;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t shakeState;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t smokeState;
	StrIe_HUITP_IEID_uni_ccl_bat_state_t batState;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t tempValue;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t humidValue;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t batValue;
	StrIe_HUITP_IEID_uni_ccl_general_value1_t general1Value;
	StrIe_HUITP_IEID_uni_ccl_general_value2_t general2Value;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t rssiValue;
	StrIe_HUITP_IEID_uni_ccl_dcmi_value_t dcmiValue;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_resp_old_t;

//HUITP_MSGID_uni_ccl_state_old_reportold                 = 0x4C81,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_report_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t lockState;
	StrIe_HUITP_IEID_uni_ccl_door_state_t doorState;
	StrIe_HUITP_IEID_uni_ccl_water_state_t waterState;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t fallState;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t shakeState;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t smokeState;
	StrIe_HUITP_IEID_uni_ccl_bat_state_t batState;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t tempValue;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t humidValue;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t batValue;
	StrIe_HUITP_IEID_uni_ccl_general_value1_t general1Value;
	StrIe_HUITP_IEID_uni_ccl_general_value2_t general2Value;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t rssiValue;
	StrIe_HUITP_IEID_uni_ccl_dcmi_value_t dcmiValue;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_report_old_t;

//HUITP_MSGID_uni_ccl_state_old_confirm                    = 0x4C01,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_confirm_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_confirm_old_t;

//HUITP_MSGID_uni_ccl_state_old_pic_report                 = 0x4C82,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_pic_report_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_picture_segment_t picSeg;
	StrIe_HUITP_IEID_uni_picture_format_t picForamt;
	StrIe_HUITP_IEID_uni_picture_body_t picBody;
}StrMsg_HUITP_MSGID_uni_ccl_state_pic_report_old_t;

//HUITP_MSGID_uni_ccl_state_old_pic_confirm                   = 0x4C02,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_pic_confirm_old
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_picture_segment_t picSeg;
}StrMsg_HUITP_MSGID_uni_ccl_state_pic_confirm_old_t;

//HUITP_MSGID_uni_ccl_state_old_max,

//云控锁-锁
//HUITP_MSGID_uni_ccl_lock_min                     = 0x4D00,  
//HUITP_MSGID_uni_ccl_lock_req                     = 0x4D00,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_lock_req_t;

//HUITP_MSGID_uni_ccl_lock_resp                    = 0x4D80, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t;

//HUITP_MSGID_uni_ccl_lock_report                  = 0x4D81, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t reportState;	
}StrMsg_HUITP_MSGID_uni_ccl_lock_report_t;

//HUITP_MSGID_uni_ccl_lock_confirm                     = 0x4D01,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t;

//HUITP_MSGID_uni_ccl_lock_auth_inq                = 0x4D90, 
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t authReq;
}StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_t;

//HUITP_MSGID_uni_ccl_lock_auth_resp               = 0x4D10,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t respState;
}StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t;

//HUITP_MSGID_uni_ccl_lock_max,

//云控锁-状态聚合
//HUITP_MSGID_uni_ccl_state_min                    = 0x4E00, 
//HUITP_MSGID_uni_ccl_state_req                    = 0x4E00,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ccl_state_req_t;

//HUITP_MSGID_uni_ccl_state_resp                   = 0x4E80,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t lockState;
	StrIe_HUITP_IEID_uni_ccl_door_state_t doorState;
	StrIe_HUITP_IEID_uni_ccl_water_state_t waterState;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t fallState;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t shakeState;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t smokeState;
	StrIe_HUITP_IEID_uni_ccl_bat_state_t batState;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t tempValue;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t humidValue;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t batValue;
	StrIe_HUITP_IEID_uni_ccl_general_value1_t general1Value;
	StrIe_HUITP_IEID_uni_ccl_general_value2_t general2Value;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t rssiValue;
	StrIe_HUITP_IEID_uni_ccl_dcmi_value_t dcmiValue;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_resp_t;

//HUITP_MSGID_uni_ccl_state_report                 = 0x4E81,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ccl_lock_state_t lockState;
	StrIe_HUITP_IEID_uni_ccl_door_state_t doorState;
	StrIe_HUITP_IEID_uni_ccl_water_state_t waterState;
	StrIe_HUITP_IEID_uni_ccl_fall_state_t fallState;
	StrIe_HUITP_IEID_uni_ccl_shake_state_t shakeState;
	StrIe_HUITP_IEID_uni_ccl_smoke_state_t smokeState;
	StrIe_HUITP_IEID_uni_ccl_bat_state_t batState;
	StrIe_HUITP_IEID_uni_ccl_temp_value_t tempValue;
	StrIe_HUITP_IEID_uni_ccl_humid_value_t humidValue;
	StrIe_HUITP_IEID_uni_ccl_bat_value_t batValue;
	StrIe_HUITP_IEID_uni_ccl_general_value1_t general1Value;
	StrIe_HUITP_IEID_uni_ccl_general_value2_t general2Value;
	StrIe_HUITP_IEID_uni_ccl_rssi_value_t rssiValue;
	StrIe_HUITP_IEID_uni_ccl_dcmi_value_t dcmiValue;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_report_t;

//HUITP_MSGID_uni_ccl_state_confirm                    = 0x4E01,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_ccl_report_type_t reportType;
}StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t;

//HUITP_MSGID_uni_ccl_state_pic_report                 = 0x4E82,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_pic_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_picture_segment_t picSeg;
	StrIe_HUITP_IEID_uni_picture_format_t picForamt;
	StrIe_HUITP_IEID_uni_picture_body_t picBody;
}StrMsg_HUITP_MSGID_uni_ccl_state_pic_report_t;

//HUITP_MSGID_uni_ccl_state_pic_confirm                   = 0x4E02,
typedef struct StrMsg_HUITP_MSGID_uni_ccl_state_pic_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_picture_segment_t picSeg;
}StrMsg_HUITP_MSGID_uni_ccl_state_pic_confirm_t;

//HUITP_MSGID_uni_ccl_state_max,


//串口读取命令/返回结果
//HUITP_MSGID_uni_itf_sps_min                      = 0x5000, 
//HUITP_MSGID_uni_itf_sps_req                      = 0x5000,
typedef struct StrMsg_HUITP_MSGID_uni_itf_sps_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_sps_req_t;

//HUITP_MSGID_uni_itf_sps_resp                     = 0x5080,
typedef struct StrMsg_HUITP_MSGID_uni_itf_sps_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_sps_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_sps_resp_t;

//HUITP_MSGID_uni_itf_sps_report                   = 0x5001, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_sps_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_sps_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_sps_report_t;

//HUITP_MSGID_uni_itf_sps_confirm                      = 0x5081, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_sps_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_sps_confirm_t;

//HUITP_MSGID_uni_itf_sps_max,

//ADC读取命令/返回结果
//HUITP_MSGID_uni_itf_adc_min                      = 0x5100, 
//HUITP_MSGID_uni_itf_adc_req                      = 0x5100,
typedef struct StrMsg_HUITP_MSGID_uni_itf_adc_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_adc_req_t;

//HUITP_MSGID_uni_itf_adc_resp                     = 0x5180,
typedef struct StrMsg_HUITP_MSGID_uni_itf_adc_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_adc_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_adc_resp_t;

//HUITP_MSGID_uni_itf_adc_report                   = 0x5181, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_adc_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_adc_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_adc_report_t;

//HUITP_MSGID_uni_itf_adc_confirm                      = 0x5101, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_adc_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_adc_confirm_t;

//HUITP_MSGID_uni_itf_adc_max,

//DAC读取命令/返回结果
//HUITP_MSGID_uni_itf_dac_min                      = 0x5200, 
//HUITP_MSGID_uni_itf_dac_req                      = 0x5200,
typedef struct StrMsg_HUITP_MSGID_uni_itf_dac_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_dac_req_t;

//HUITP_MSGID_uni_itf_dac_resp                     = 0x5280, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_dac_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_dac_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_dac_resp_t;

//HUITP_MSGID_uni_itf_dac_report                   = 0x5281,
typedef struct StrMsg_HUITP_MSGID_uni_itf_dac_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_dac_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_dac_report_t;

//HUITP_MSGID_uni_itf_dac_confirm                      = 0x5201,
typedef struct StrMsg_HUITP_MSGID_uni_itf_dac_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_dac_confirm_t;

//HUITP_MSGID_uni_itf_dac_max,

//I2C读取命令/返回结果
//HUITP_MSGID_uni_itf_i2c_min                      = 0x5300, 
//HUITP_MSGID_uni_itf_i2c_req                      = 0x5300, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_i2c_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_i2c_req_t;

//HUITP_MSGID_uni_itf_i2c_resp                     = 0x5380, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_i2c_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_i2c_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_i2c_resp_t;

//HUITP_MSGID_uni_itf_i2c_report                   = 0x5381, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_i2c_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_i2c_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_i2c_report_t;

//HUITP_MSGID_uni_itf_i2c_confirm                      = 0x5301, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_i2c_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_i2c_confirm_t;

//HUITP_MSGID_uni_itf_i2c_max,

//PWM读取命令/返回结果
//HUITP_MSGID_uni_itf_pwm_min                      = 0x5400, 
//HUITP_MSGID_uni_itf_pwm_req                      = 0x5400,
typedef struct StrMsg_HUITP_MSGID_uni_itf_pwm_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_pwm_req_t;

//HUITP_MSGID_uni_itf_pwm_resp                     = 0x5480, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_pwm_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_pwm_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_pwm_resp_t;

//HUITP_MSGID_uni_itf_pwm_report                   = 0x5481, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_pwm_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_pwm_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_pwm_report_t;

//HUITP_MSGID_uni_itf_pwm_confirm                      = 0x5401, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_pwm_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_pwm_confirm_t;

//HUITP_MSGID_uni_itf_pwm_max,

//DI读取命令/返回结果
//HUITP_MSGID_uni_itf_di_min                       = 0x5500, 
//HUITP_MSGID_uni_itf_di_req                       = 0x5500,
typedef struct StrMsg_HUITP_MSGID_uni_itf_di_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_di_req_t;

//HUITP_MSGID_uni_itf_di_resp                      = 0x5580,
typedef struct StrMsg_HUITP_MSGID_uni_itf_di_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_di_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_di_resp_t;

//HUITP_MSGID_uni_itf_di_report                    = 0x5581,
typedef struct StrMsg_HUITP_MSGID_uni_itf_di_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_di_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_di_report_t;

//HUITP_MSGID_uni_itf_di_confirm                       = 0x5501, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_di_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_di_confirm_t;

//HUITP_MSGID_uni_itf_di_max,

//DO读取命令/返回结果
//HUITP_MSGID_uni_itf_do_min                       = 0x5600, 
//HUITP_MSGID_uni_itf_do_req                       = 0x5600, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_do_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_do_req_t;

//HUITP_MSGID_uni_itf_do_resp                      = 0x5680,
typedef struct StrMsg_HUITP_MSGID_uni_itf_do_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_do_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_do_resp_t;

//HUITP_MSGID_uni_itf_do_report                    = 0x5681,
typedef struct StrMsg_HUITP_MSGID_uni_itf_do_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_do_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_do_report_t;

//HUITP_MSGID_uni_itf_do_confirm                       = 0x5601,
typedef struct StrMsg_HUITP_MSGID_uni_itf_do_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_do_confirm_t;

//HUITP_MSGID_uni_itf_do_max,

//CAN读取命令/返回结果
//HUITP_MSGID_uni_itf_can_min                      = 0x5700, 
//HUITP_MSGID_uni_itf_can_req                      = 0x5700, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_can_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_can_req_t;

//HUITP_MSGID_uni_itf_can_resp                     = 0x5780, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_cam_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_can_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_cam_resp_t;

//HUITP_MSGID_uni_itf_can_report                   = 0x5781,
typedef struct StrMsg_HUITP_MSGID_uni_itf_can_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_can_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_can_report_t;

//HUITP_MSGID_uni_itf_can_confirm                      = 0x5701, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_can_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_can_confirm_t;

//HUITP_MSGID_uni_itf_can_max,

//SPI读取命令/返回结果
//HUITP_MSGID_uni_itf_spi_min                      = 0x5800, 
//HUITP_MSGID_uni_itf_spi_req                      = 0x5800, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_spi_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_spi_req_t;

//HUITP_MSGID_uni_itf_spi_resp                     = 0x5880, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_spi_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_spi_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_spi_resp_t;

//HUITP_MSGID_uni_itf_spi_report                   = 0x5881,
typedef struct StrMsg_HUITP_MSGID_uni_itf_spi_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_spi_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_spi_report_t;

//HUITP_MSGID_uni_itf_spi_confirm                      = 0x5801,
typedef struct StrMsg_HUITP_MSGID_uni_itf_spi_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_spi_confirm_t;

//HUITP_MSGID_uni_itf_spi_max,

//USB读取命令/返回结果
//HUITP_MSGID_uni_itf_usb_min                      = 0x5900, 
//HUITP_MSGID_uni_itf_usb_req                      = 0x5900, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_usb_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_usb_req_t;

//HUITP_MSGID_uni_itf_usb_resp                     = 0x5980,
typedef struct StrMsg_HUITP_MSGID_uni_itf_usb_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_usb_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_usb_resp_t;

//HUITP_MSGID_uni_itf_usb_report                   = 0x5981, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_usb_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_usb_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_usb_report_t;

//HUITP_MSGID_uni_itf_usb_confirm                      = 0x5901, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_usb_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_usb_confirm_t;

//HUITP_MSGID_uni_itf_usb_max,

//网口读取命令/返回结果
//HUITP_MSGID_uni_itf_eth_min                      = 0x5A00, 
//HUITP_MSGID_uni_itf_eth_req                      = 0x5A00, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_eth_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_eth_req_t;

//HUITP_MSGID_uni_itf_eth_resp                     = 0x5A80, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_eth_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_eth_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_eth_resp_t;

//HUITP_MSGID_uni_itf_eth_report                   = 0x5A81, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_eth_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_eth_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_eth_report_t;

//HUITP_MSGID_uni_itf_eth_confirm                      = 0x5A01, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_eth_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_eth_confirm_t;

//HUITP_MSGID_uni_itf_eth_max,

//485读取命令/返回结果 
//HUITP_MSGID_uni_itf_485_min                      = 0x5B00, 
//HUITP_MSGID_uni_itf_485_req                      = 0x5B00, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_485_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_itf_485_req_t;

//HUITP_MSGID_uni_itf_485_resp                     = 0x5B80,
typedef struct StrMsg_HUITP_MSGID_uni_itf_485_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_485_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_485_resp_t;

//HUITP_MSGID_uni_itf_485_report                   = 0x5B81,
typedef struct StrMsg_HUITP_MSGID_uni_itf_485_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_itf_485_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_itf_485_report_t;

//HUITP_MSGID_uni_itf_485_confirm                      = 0x5B01, 
typedef struct StrMsg_HUITP_MSGID_uni_itf_485_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_itf_485_confirm_t;

//HUITP_MSGID_uni_itf_485_max,

//软件清单
//HUITP_MSGID_uni_inventory_min                    = 0xA000,	
//HUITP_MSGID_uni_inventory_req                    = 0xA000,
typedef struct StrMsg_HUITP_MSGID_uni_inventory_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_inventory_req_t;

//HUITP_MSGID_uni_inventory_resp                   = 0xA080,
typedef struct StrMsg_HUITP_MSGID_uni_inventory_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_inventory_hw_type_t hwTypeRespValue;
	StrIe_HUITP_IEID_uni_inventory_hw_id_t hwIdRespValue;
	StrIe_HUITP_IEID_uni_inventory_sw_rel_t swRelRespValue;
	StrIe_HUITP_IEID_uni_inventory_sw_ver_t swVerRespValue;
}StrMsg_HUITP_MSGID_uni_inventory_resp_t;

//HUITP_MSGID_uni_inventory_report                 = 0xA081, 
typedef struct StrMsg_HUITP_MSGID_uni_inventory_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_inventory_hw_type_t hwTypeReportValue;
	StrIe_HUITP_IEID_uni_inventory_hw_id_t hwIdReportValue;
	StrIe_HUITP_IEID_uni_inventory_sw_rel_t swRelReportValue;
	StrIe_HUITP_IEID_uni_inventory_sw_ver_t swVerReportValue;
}StrMsg_HUITP_MSGID_uni_inventory_report_t;

//HUITP_MSGID_uni_inventory_confirm                    = 0xA001, 
typedef struct StrMsg_HUITP_MSGID_uni_inventory_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_inventory_confirm_t;

//HUITP_MSGID_uni_inventory_max,

//软件版本体
//HUITP_MSGID_uni_sw_package_min                   = 0xA100,	
//HUITP_MSGID_uni_sw_package_req                   = 0xA100,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_segment_total_t segTotal;
	StrIe_HUITP_IEID_uni_com_segment_index_t segIndex;
	StrIe_HUITP_IEID_uni_sw_package_body_t swPackageBody;
}StrMsg_HUITP_MSGID_uni_sw_package_req_t;

//HUITP_MSGID_uni_sw_package_resp                  = 0xA180,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_segment_total_t segTotal;
	StrIe_HUITP_IEID_uni_com_segment_index_t segIndex;
}StrMsg_HUITP_MSGID_uni_sw_package_resp_t;

//HUITP_MSGID_uni_sw_package_report                = 0xA181,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_com_segment_total_t segTotal;
	StrIe_HUITP_IEID_uni_com_segment_index_t segIndex;
}StrMsg_HUITP_MSGID_uni_sw_package_report_t;

//HUITP_MSGID_uni_sw_package_confirm                   = 0xA101,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_com_segment_total_t segTotal;
	StrIe_HUITP_IEID_uni_com_segment_index_t segIndex;
	StrIe_HUITP_IEID_uni_sw_package_body_t swPackageBody;
}StrMsg_HUITP_MSGID_uni_sw_package_confirm_t;

//HUITP_MSGID_uni_sw_package_max,

//ALARM REPORT
//HUITP_MSGID_uni_alarm_info_min                   = 0xB000, 
//HUITP_MSGID_uni_alarm_info_req                   = 0xB000,
typedef struct StrMsg_HUITP_MSGID_uni_alarm_info_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_alarm_info_req_t;

//HUITP_MSGID_uni_alarm_info_resp                  = 0xB080,
typedef struct StrMsg_HUITP_MSGID_uni_alarm_info_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_alarm_info_value_t respValue;
}StrMsg_HUITP_MSGID_uni_alarm_info_resp_t;

//HUITP_MSGID_uni_alarm_info_report                = 0xB081,
typedef struct StrMsg_HUITP_MSGID_uni_alarm_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_alarm_info_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_alarm_info_report_t;

//HUITP_MSGID_uni_alarm_info_confirm                   = 0xB001, 
typedef struct StrMsg_HUITP_MSGID_uni_alarm_info_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t;

//HUITP_MSGID_uni_alarm_info_max,

//PM Report
//HUITP_MSGID_uni_performance_info_min             = 0xB100, 
//HUITP_MSGID_uni_performance_info_req             = 0xB100,
typedef struct StrMsg_HUITP_MSGID_uni_performance_info_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_performance_info_req_t;

//HUITP_MSGID_uni_performance_info_resp            = 0xB180, 
typedef struct StrMsg_HUITP_MSGID_uni_performance_info_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_performance_info_value_t respValue;
}StrMsg_HUITP_MSGID_uni_performance_info_resp_t;

//HUITP_MSGID_uni_performance_info_report          = 0xB181, 
typedef struct StrMsg_HUITP_MSGID_uni_performance_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_performance_info_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_performance_info_report_t;

//HUITP_MSGID_uni_performance_info_confirm             = 0xB101,
typedef struct StrMsg_HUITP_MSGID_uni_performance_info_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_performance_info_confirm_t;

//HUITP_MSGID_uni_performance_info_max,

//设备基本信息
//HUITP_MSGID_uni_equipment_info_min               = 0xF000,	
//HUITP_MSGID_uni_equipment_info_req               = 0xF000,
typedef struct StrMsg_HUITP_MSGID_uni_equipment_info_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_equipment_info_req_t;

//HUITP_MSGID_uni_equipment_info_resp              = 0xF080,
typedef struct StrMsg_HUITP_MSGID_uni_equipment_info_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_equipment_info_value_t respValue;
}StrMsg_HUITP_MSGID_uni_equipment_info_resp_t;

//HUITP_MSGID_uni_equipment_info_report            = 0xF081, 
typedef struct StrMsg_HUITP_MSGID_uni_equipment_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_equipment_info_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_equipment_info_report_t;

//HUITP_MSGID_uni_equipment_info_confirm               = 0xF001, 
typedef struct StrMsg_HUITP_MSGID_uni_equipment_info_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_equipment_info_confirm_t;

//HUITP_MSGID_uni_equipment_info_max,

//个人基本信息
//HUITP_MSGID_uni_personal_info_min                = 0xF100,	
//HUITP_MSGID_uni_personal_info_req                = 0xF100,	
typedef struct StrMsg_HUITP_MSGID_uni_personal_info_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_personal_info_req_t;

//HUITP_MSGID_uni_personal_info_resp               = 0xF180, 
typedef struct StrMsg_HUITP_MSGID_uni_personal_info_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_personal_info_value_t respValue;
}StrMsg_HUITP_MSGID_uni_personal_info_resp_t;

//HUITP_MSGID_uni_personal_info_report             = 0xF181,
typedef struct StrMsg_HUITP_MSGID_uni_personal_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_personal_info_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_personal_info_report_t;

//HUITP_MSGID_uni_personal_info_confirm                = 0xF101,
typedef struct StrMsg_HUITP_MSGID_uni_personal_info_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_personal_info_confirm_t;

//HUITP_MSGID_uni_personal_info_max,

//时间同步
//HUITP_MSGID_uni_time_sync_min                    = 0xF200,
//HUITP_MSGID_uni_time_sync_req                    = 0xF200,
typedef struct StrMsg_HUITP_MSGID_uni_time_sync_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_time_sync_req_t;

//HUITP_MSGID_uni_time_sync_resp                   = 0xF280,
typedef struct StrMsg_HUITP_MSGID_uni_time_sync_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_time_sync_value_t respValue;
}StrMsg_HUITP_MSGID_uni_time_sync_resp_t;

//HUITP_MSGID_uni_time_sync_report                 = 0xF281,
typedef struct StrMsg_HUITP_MSGID_uni_time_sync_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_time_sync_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_time_sync_report_t;

//HUITP_MSGID_uni_time_sync_confirm                    = 0xF201,
typedef struct StrMsg_HUITP_MSGID_uni_time_sync_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_time_sync_confirm_t;

//HUITP_MSGID_uni_time_sync_max,

//读取数据
//HUITP_MSGID_uni_general_read_data_min            = 0xF300,	
//HUITP_MSGID_uni_general_read_data_req            = 0xF300,
typedef struct StrMsg_HUITP_MSGID_uni_general_read_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_general_read_data_req_t;

//HUITP_MSGID_uni_general_read_data_resp           = 0xF380,
typedef struct StrMsg_HUITP_MSGID_uni_general_read_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_general_read_data_value_t respValue;
}StrMsg_HUITP_MSGID_uni_general_read_data_resp_t;

//HUITP_MSGID_uni_general_read_data_report         = 0xF381,
typedef struct StrMsg_HUITP_MSGID_uni_general_read_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_general_read_data_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_general_read_data_report_t;

//HUITP_MSGID_uni_general_read_data_confirm            = 0xF301, 
typedef struct StrMsg_HUITP_MSGID_uni_general_read_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_general_read_data_confirm_t;

//HUITP_MSGID_uni_general_read_data_max,

//定时闹钟及久坐提醒
//HUITP_MSGID_uni_clock_timeout_min                = 0xF400,
//HUITP_MSGID_uni_clock_timeout_req                = 0xF400,	
typedef struct StrMsg_HUITP_MSGID_uni_clock_timeout_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_clock_timeout_req_t;

//HUITP_MSGID_uni_clock_timeout_resp               = 0xF480,
typedef struct StrMsg_HUITP_MSGID_uni_clock_timeout_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_clock_timeout_value_t respValue;
}StrMsg_HUITP_MSGID_uni_clock_timeout_resp_t;

//HUITP_MSGID_uni_clock_timeout_report             = 0xF481,
typedef struct StrMsg_HUITP_MSGID_uni_clock_timeout_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_clock_timeout_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_clock_timeout_report_t;

//HUITP_MSGID_uni_clock_timeout_confirm            = 0xF401,
typedef struct StrMsg_HUITP_MSGID_uni_clock_timeout_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_clock_timeout_confirm_t;

//HUITP_MSGID_uni_clock_timeout_max,

//同步充电，双击情况
//HUITP_MSGID_uni_sync_charging_min                = 0xF500,	
//HUITP_MSGID_uni_sync_charging_req                = 0xF500,
typedef struct StrMsg_HUITP_MSGID_uni_sync_charging_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_sync_charging_req_t;

//HUITP_MSGID_uni_sync_charging_resp               = 0xF580,
typedef struct StrMsg_HUITP_MSGID_uni_sync_charging_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_sync_charging_value_t respValue;
}StrMsg_HUITP_MSGID_uni_sync_charging_resp_t;

//HUITP_MSGID_uni_sync_charging_report             = 0xF581,
typedef struct StrMsg_HUITP_MSGID_uni_sync_charging_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_sync_charging_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_sync_charging_report_t;

//HUITP_MSGID_uni_sync_charging_confirm            = 0xF501,
typedef struct StrMsg_HUITP_MSGID_uni_sync_charging_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_sync_charging_confirm_t;

//HUITP_MSGID_uni_sync_charging_max,

//同步通知信息
//HUITP_MSGID_uni_sync_trigger_min                 = 0xF600,	
//HUITP_MSGID_uni_sync_trigger_req                 = 0xF600,
typedef struct StrMsg_HUITP_MSGID_uni_sync_trigger_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_sync_trigger_req_t;

//HUITP_MSGID_uni_sync_trigger_resp                = 0xF680,
typedef struct StrMsg_HUITP_MSGID_uni_sync_trigger_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_sync_trigger_value_t respValue;
}StrMsg_HUITP_MSGID_uni_sync_trigger_resp_t;

//HUITP_MSGID_uni_sync_trigger_report              = 0xF681,
typedef struct StrMsg_HUITP_MSGID_uni_sync_trigger_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_sync_trigger_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_sync_trigger_report_t;

//HUITP_MSGID_uni_sync_trigger_confirm             = 0xF601, 
typedef struct StrMsg_HUITP_MSGID_uni_sync_trigger_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_sync_trigger_confirm_t;

//HUITP_MSGID_uni_sync_trigger_max,

//CMD CONTROL
//HUITP_MSGID_uni_cmd_ctrl_min                     = 0xFD00,  
//HUITP_MSGID_uni_cmd_ctrl_req                     = 0xFD00,
typedef struct StrMsg_HUITP_MSGID_uni_cmd_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_cmd_ctrl_send_t cmdSend;
}StrMsg_HUITP_MSGID_uni_cmd_ctrl_req_t;

//HUITP_MSGID_uni_cmd_ctrl_resp                    = 0xFD80, 
typedef struct StrMsg_HUITP_MSGID_uni_cmd_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_cmd_ctrl_confirm_t cmdCfm;
}StrMsg_HUITP_MSGID_uni_cmd_ctrl_resp_t;

//HUITP_MSGID_uni_cmd_ctrl_report                  = 0xFD81,
typedef struct StrMsg_HUITP_MSGID_uni_cmd_ctrl_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_cmd_ctrl_send_t cmdSend;
}StrMsg_HUITP_MSGID_uni_cmd_ctrl_report_t;

//HUITP_MSGID_uni_cmd_ctrl_confirm                 = 0xFD01,
typedef struct StrMsg_HUITP_MSGID_uni_cmd_ctrl_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_cmd_ctrl_confirm_t cmdCfm;
}StrMsg_HUITP_MSGID_uni_cmd_ctrl_confirm_t;

//HUITP_MSGID_uni_cmd_ctrl_max,	

//心跳
//HUITP_MSGID_uni_heart_beat_min                   = 0xFE00, 
//HUITP_MSGID_uni_heart_beat_req                   = 0xFE00, 
typedef struct StrMsg_HUITP_MSGID_uni_heart_beat_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_heart_beat_ping_t ping;
}StrMsg_HUITP_MSGID_uni_heart_beat_req_t;

//HUITP_MSGID_uni_heart_beat_resp                  = 0xFE80, 
typedef struct StrMsg_HUITP_MSGID_uni_heart_beat_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_heart_beat_pong_t pong;
}StrMsg_HUITP_MSGID_uni_heart_beat_resp_t;

//HUITP_MSGID_uni_heart_beat_report                = 0xFE81,
typedef struct StrMsg_HUITP_MSGID_uni_heart_beat_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_heart_beat_ping_t ping;
}StrMsg_HUITP_MSGID_uni_heart_beat_report_t;

//HUITP_MSGID_uni_heart_beat_confirm               = 0xFE01, 
typedef struct StrMsg_HUITP_MSGID_uni_heart_beat_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_heart_beat_pong_t pong;
}StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t;

//HUITP_MSGID_uni_heart_beat_max,

//无效
//HUITP_MSGID_uni_null                             = 0xFF,	

#pragma pack () //取消字节对其
#endif /* _HUITP_H_ */
