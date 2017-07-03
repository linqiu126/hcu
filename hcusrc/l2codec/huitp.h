/*
 * huitp.h
 *
 *  Created on: 2016年12月25日
 *      Author: test
 */

#ifndef _HUITP_H_
#define _HUITP_H_
#include "../l0comvm/vmlayer.h"

//pack函数必须严格放在结构的里面，且不得包含任何其它包含文件，不然会发生意想不到的问题！！！
#pragma pack (1) //强制1字节对齐
/*
 *
 *  顺从并更新到技术规范《慧HUITP接口规范v2.8, LAST UPDATE@2007/7/3》
 *
 * 2017/06/10, MA Yuchu, modify  for BFSC, Weight Sensor parameters, based on v2.5, LAST UPDATE@2007/5/27
 * 2017/06/20 v2.6: 修改L3BFSC对应的结构，增加Heart-Beat过程
 * 2017/06/30 V2.7: 修改CANITF接口上SW INVENTORY/SW PACKAGE的消息定义与结构
 * 2017/07/03 V2.8: 完善SW INVENTORY/SW PACKAGE的消息定义与结构
 *
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//考虑到TCPIP中底层最长1500B的情形，这里先如此定义，不要超过这个限制，所以16进制的缓冲区最好不要超过500字节
#define HUITP_MSG_BUF_WITH_HEAD_MAX_LEN 				134
#define HUITP_MSG_BUF_BODY_ONLY_MAX_LEN 				HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - 4 //MSG头+长度域共4BYTE
#define HUITP_IE_BUF_MAX_LEN 							HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - 30 //MSG头4B，BASE_REQ 5B，IE头4B，为了安全，先减去30。在具体的消息定义中，如果独立IE较多，这个需要再检查
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN 			300  //人为定义，目前不包括具体内容在内的纯格式部分，已经达到210BYTE长度
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN 			200
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_VARIABLE_LEN 		HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN - HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN
//<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>
#define HUITP_MSG_HUIXML_TOTAL_BUF_IN_CHAR_MAX_LEN 		HUITP_MSG_XML_HEAD_IN_CHAR_MAX_LEN + 2 * HUITP_MSG_BUF_WITH_HEAD_MAX_LEN + 1 //CHAR方式，用于INTERNET传输，留下一个做为尾部缓冲
#define HUITP_MSG_HUIFRAME_HEAD_MAX_LEN 4 //固定长度，采用SUM(CONTENT)的方式进行校验
#define HUITP_MSG_HUIFRAME_TOTAL_BUF_MAX_LEN 			HUITP_MSG_HUIFRAME_HEAD_MAX_LEN + HUITP_MSG_BUF_WITH_HEAD_MAX_LEN //这个是16进制BYTE方式
//HUIXML固定头部常量定义
#define HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L  			"<xml>"
#define HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R  			"</xml>"
#define HUITP_MSG_HUIXML_CONSTANT_TO_USER_L  			"<ToUserName><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_TO_USER_R  			"]]></ToUserName>"
#define HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L  			"<FromUserName><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R  			"]]></FromUserName>"
#define HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L 		"<CreateTime>"
#define HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R 		"</CreateTime>"
#define HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L  			"<MsgType><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R  			"]]></MsgType>"
#define HUITP_MSG_HUIXML_CONSTANT_CONTENT_L  			"<Content><![CDATA["
#define HUITP_MSG_HUIXML_CONSTANT_CONTENT_R  			"]]></Content>"
#define HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L  			"<FuncFlag>"
#define HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R  			"</FuncFlag>"
#define HUITP_MSG_HUIXML_CONTSANT_CREATE_TIME_MAX_LEN	16 //以10进制存储的时间戳，最长长度
//HUIXML MsgType定义
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID 		1
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING  	"huitp_text"
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID 		2
#define HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING  "huitp_command"
#define HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID 			3
#define HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING 		"huitp_heart_beat" //心跳协议，里面的数据内容是空的
#define HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID 			4
#define HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING 		"huitp_biz_itg"  //业务智能 hcu_biz_inteligence
#define HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID 		5
#define HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING 	"huitp_alarm"  //for alarm report
#define HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID 			6
#define HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING 		"huitp_pm"  //for pm report

#define HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID 			7
#define HUITP_MSG_HUIXML_MSGTYPE_COMMON_STRING 		"hcu_huitp"  //use common string for the moment

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
	HUITP_CMDID_uni_hcho                  = 0x23, //甲醛HCHO
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
	HUITP_CMDID_uni_lightstr              = 0x35, //光照强度
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
	HUITP_MSGID_uni_emc_ctrl_req                     = 0x2002,
	HUITP_MSGID_uni_emc_ctrl_resp                    = 0x2082,
	HUITP_MSGID_uni_emc_data_max,

  //电磁辐射剂量
	HUITP_MSGID_uni_emc_accu_min                     = 0x2100, 
	HUITP_MSGID_uni_emc_accu_req                     = 0x2100, 
	HUITP_MSGID_uni_emc_accu_resp                    = 0x2180, 
	HUITP_MSGID_uni_emc_accu_report                  = 0x2181, 
	HUITP_MSGID_uni_emc_accu_confirm                 = 0x2101, 
	HUITP_MSGID_uni_emc_accu_max,

  //一氧化碳
	HUITP_MSGID_uni_co1_min                           = 0x2200,
	HUITP_MSGID_uni_co1_data_req                      = 0x2200,
	HUITP_MSGID_uni_co1_data_resp                     = 0x2280,
	HUITP_MSGID_uni_co1_data_report                   = 0x2281,
	HUITP_MSGID_uni_co1_data_confirm                  = 0x2201,
	HUITP_MSGID_uni_co1_max,

  //甲醛HCHO
	HUITP_MSGID_uni_hcho_min                         = 0x2300,
	HUITP_MSGID_uni_hcho_data_req                    = 0x2300,
	HUITP_MSGID_uni_hcho_data_resp                   = 0x2380,
	HUITP_MSGID_uni_hcho_data_report                 = 0x2381,
	HUITP_MSGID_uni_hcho_data_confirm                = 0x2301,
	HUITP_MSGID_uni_hcho_max,

  //酒精
	HUITP_MSGID_uni_alcohol_min                      = 0x2400, 
	HUITP_MSGID_uni_alcohol_data_req                 = 0x2400,
	HUITP_MSGID_uni_alcohol_data_resp                = 0x2480,
	HUITP_MSGID_uni_alcohol_data_report              = 0x2481,
	HUITP_MSGID_uni_alcohol_data_confirm             = 0x2401,
	HUITP_MSGID_uni_alcohol_max,
 
  //PM1/2.5/10
	HUITP_MSGID_uni_pm25_min                         = 0x2500,
	HUITP_MSGID_uni_pm25_data_req                    = 0x2500,
	HUITP_MSGID_uni_pm25_data_resp                   = 0x2580, 
	HUITP_MSGID_uni_pm25_data_report                 = 0x2581, 
	HUITP_MSGID_uni_pm25_data_confirm                = 0x2501, 
	HUITP_MSGID_uni_pm25_ctrl_req                    = 0x2502,
	HUITP_MSGID_uni_pm25_ctrl_resp                   = 0x2582,
	HUITP_MSGID_uni_pm25sp_data_req                  = 0x2503,
	HUITP_MSGID_uni_pm25sp_data_resp                 = 0x2583,
	HUITP_MSGID_uni_pm25sp_data_report               = 0x2504,
	HUITP_MSGID_uni_pm25sp_data_confirm              = 0x2584,

	HUITP_MSGID_uni_pm25_max,

  //风速Wind Speed
	HUITP_MSGID_uni_windspd_min                      = 0x2600, 
	HUITP_MSGID_uni_windspd_data_req                 = 0x2600, 
	HUITP_MSGID_uni_windspd_data_resp                = 0x2680, 
	HUITP_MSGID_uni_windspd_data_report              = 0x2681, 
	HUITP_MSGID_uni_windspd_data_confirm             = 0x2601,
	HUITP_MSGID_uni_windspd_ctrl_req                 = 0x2602,
	HUITP_MSGID_uni_windspd_ctrl_resp                = 0x2682,
	HUITP_MSGID_uni_windspd_max,

	//风向Wind Direction
	HUITP_MSGID_uni_winddir_min                      = 0x2700, 
	HUITP_MSGID_uni_winddir_data_req                 = 0x2700, 
	HUITP_MSGID_uni_winddir_data_resp                = 0x2780, 
	HUITP_MSGID_uni_winddir_data_report              = 0x2781, 
	HUITP_MSGID_uni_winddir_data_confirm             = 0x2701,
	HUITP_MSGID_uni_winddir_ctrl_req                 = 0x2702,
	HUITP_MSGID_uni_winddir_ctrl_resp                = 0x2782,
	HUITP_MSGID_uni_winddir_max,
	
	//温度Temperature
	HUITP_MSGID_uni_temp_min                         = 0x2800, 
	HUITP_MSGID_uni_temp_data_req                    = 0x2800, 
	HUITP_MSGID_uni_temp_data_resp                   = 0x2880, 
	HUITP_MSGID_uni_temp_data_report                 = 0x2881, 
	HUITP_MSGID_uni_temp_data_confirm                = 0x2801,
	HUITP_MSGID_uni_temp_ctrl_req                    = 0x2802,
	HUITP_MSGID_uni_temp_ctrl_resp                   = 0x2882,
	HUITP_MSGID_uni_temp_max,

  //湿度Humidity
	HUITP_MSGID_uni_humid_min                        = 0x2900, 
	HUITP_MSGID_uni_humid_data_req                   = 0x2900, 
	HUITP_MSGID_uni_humid_data_resp                  = 0x2980, 
	HUITP_MSGID_uni_humid_data_report                = 0x2981, 
	HUITP_MSGID_uni_humid_data_confirm               = 0x2901,
	HUITP_MSGID_uni_humid_ctrl_req                   = 0x2902,
	HUITP_MSGID_uni_humid_ctrl_resp                  = 0x2982,
	HUITP_MSGID_uni_humid_max,

  //气压Air pressure
	HUITP_MSGID_uni_airprs_min                       = 0x2A00, 
	HUITP_MSGID_uni_airprs_data_req                  = 0x2A00, 
	HUITP_MSGID_uni_airprs_data_resp                 = 0x2A80, 
	HUITP_MSGID_uni_airprs_data_report               = 0x2A81, 
	HUITP_MSGID_uni_airprs_data_confirm              = 0x2A01,
	HUITP_MSGID_uni_airprs_max,

  //噪声Noise
	HUITP_MSGID_uni_noise_min                        = 0x2B00, 
	HUITP_MSGID_uni_noise_data_req                   = 0x2B00, 
	HUITP_MSGID_uni_noise_data_resp                  = 0x2B80, 
	HUITP_MSGID_uni_noise_data_report                = 0x2B81, 
	HUITP_MSGID_uni_noise_data_confirm               = 0x2B01,
	HUITP_MSGID_uni_noise_ctrl_req                   = 0x2B02,
	HUITP_MSGID_uni_noise_ctrl_resp                  = 0x2B82,
	HUITP_MSGID_uni_noise_max,

  //相机Camer or audio high speed
	HUITP_MSGID_uni_hsmmp_min                        = 0x2C00, 
	HUITP_MSGID_uni_hsmmp_data_req                   = 0x2C00, 
	HUITP_MSGID_uni_hsmmp_data_resp                  = 0x2C80, 
	HUITP_MSGID_uni_hsmmp_data_report                = 0x2C81, 
	HUITP_MSGID_uni_hsmmp_data_confirm               = 0x2C01,
	HUITP_MSGID_uni_hsmmp_ctrl_req                   = 0x2C02,
	HUITP_MSGID_uni_hsmmp_ctrl_resp                  = 0x2C82,
	HUITP_MSGID_uni_hsmmp_max,

  //声音
	HUITP_MSGID_uni_audio_min                        = 0x2D00, 
	HUITP_MSGID_uni_audio_data_req                   = 0x2D00, 
	HUITP_MSGID_uni_audio_data_resp                  = 0x2D80, 
	HUITP_MSGID_uni_audio_data_report                = 0x2D81, 
	HUITP_MSGID_uni_audio_data_confirm               = 0x2D01,
	HUITP_MSGID_uni_audio_max,

  //视频
	HUITP_MSGID_uni_video_min                        = 0x2E00, 
	HUITP_MSGID_uni_video_data_req                   = 0x2E00, 
	HUITP_MSGID_uni_video_data_resp                  = 0x2E80, 
	HUITP_MSGID_uni_video_data_report                = 0x2E81, 
	HUITP_MSGID_uni_video_data_confirm               = 0x2E01,
	HUITP_MSGID_uni_video_max,

  //图片
	HUITP_MSGID_uni_picture_min                      = 0x2F00, 
	HUITP_MSGID_uni_picture_data_req                 = 0x2F00, 
	HUITP_MSGID_uni_picture_data_resp                = 0x2F80, 
	HUITP_MSGID_uni_picture_data_report              = 0x2F81, 
	HUITP_MSGID_uni_picture_data_confirm             = 0x2F01,
	HUITP_MSGID_uni_picture_max,

  //扬尘监控系统
	HUITP_MSGID_uni_ycjk_min                         = 0x3000, 
	HUITP_MSGID_uni_ycjk_data_req                    = 0x3000,
	HUITP_MSGID_uni_ycjk_data_resp                   = 0x3080,
	HUITP_MSGID_uni_ycjk_data_report                 = 0x3081,
	HUITP_MSGID_uni_ycjk_data_confirm                = 0x3001,
	HUITP_MSGID_uni_ycjk_ctrl_req                    = 0x3002,
	HUITP_MSGID_uni_ycjk_ctrl_resp                   = 0x3082,
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
	HUITP_MSGID_uni_lightstr_data_min                = 0x3500,
	HUITP_MSGID_uni_lightstr_data_req                = 0x3500,
	HUITP_MSGID_uni_lightstr_data_resp               = 0x3580,
	HUITP_MSGID_uni_lightstr_data_report             = 0x3581,
	HUITP_MSGID_uni_lightstr_data_confirm            = 0x3501,
	HUITP_MSGID_uni_lightstr_data_max,

  //有毒气体VOC
	HUITP_MSGID_uni_toxicgas_data_min                     = 0x3600,
	HUITP_MSGID_uni_toxicgas_data_req                     = 0x3600,
	HUITP_MSGID_uni_toxicgas_data_resp                    = 0x3680,
	HUITP_MSGID_uni_toxicgas_data_report                  = 0x3681,
	HUITP_MSGID_uni_toxicgas_data_confirm                 = 0x3601,
	HUITP_MSGID_uni_toxicgas_data_max,

  //海拔高度
	HUITP_MSGID_uni_altitude_data_min                     = 0x3700,
	HUITP_MSGID_uni_altitude_data_req                     = 0x3700,
	HUITP_MSGID_uni_altitude_data_resp                    = 0x3780,
	HUITP_MSGID_uni_altitude_data_report                  = 0x3781,
	HUITP_MSGID_uni_altitude_data_confirm                 = 0x3701,
	HUITP_MSGID_uni_altitude_data_max,

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
	HUITP_MSGID_uni_bfsc_comb_scale_data_req         = 0x3B00,
	HUITP_MSGID_uni_bfsc_comb_scale_data_resp        = 0x3B80,
	HUITP_MSGID_uni_bfsc_comb_scale_data_report      = 0x3B81,
	HUITP_MSGID_uni_bfsc_comb_scale_data_confirm     = 0x3B01,
	HUITP_MSGID_uni_bfsc_comb_scale_event_report     = 0x3B82,
	HUITP_MSGID_uni_bfsc_comb_scale_event_confirm    = 0x3B02,
	HUITP_MSGID_uni_bfsc_comb_scale_ctrl_req         = 0x3B03,
	HUITP_MSGID_uni_bfsc_comb_scale_ctrl_resp        = 0x3B83,
	HUITP_MSGID_uni_bfsc_statistic_report            = 0x3B84,
	HUITP_MSGID_uni_bfsc_statistic_confirm           = 0x3B04,
	//HCU-IHU SUI新增内容
	//上电过程
	HUITP_MSGID_sui_bfsc_startup_ind                 = 0x3B90,
	//配置过程
	HUITP_MSGID_sui_bfsc_set_config_req              = 0x3B11,
	HUITP_MSGID_sui_bfsc_set_config_resp             = 0x3B91,
	//启动过程
	HUITP_MSGID_sui_bfsc_start_req                   = 0x3B12,
	HUITP_MSGID_sui_bfsc_start_resp                  = 0x3B92,
	HUITP_MSGID_sui_bfsc_stop_req                    = 0x3B13,
	HUITP_MSGID_sui_bfsc_stop_resp                   = 0x3B93,
	//重量汇报过程
	HUITP_MSGID_sui_bfsc_new_ws_event                = 0x3B94,
	HUITP_MSGID_sui_bfsc_repeat_ws_event             = 0x3B95,
	//组合出料过程
	HUITP_MSGID_sui_bfsc_ws_comb_out_req             = 0x3B16,
	HUITP_MSGID_sui_bfsc_ws_comb_out_resp            = 0x3B96,
	//抛料过程：本协议暂时未使用
	HUITP_MSGID_sui_bfsc_ws_give_up_req              = 0x3B17,
	HUITP_MSGID_sui_bfsc_ws_give_up_resp             = 0x3B97,
	//特殊命令过程（测试等过程）
	HUITP_MSGID_sui_bfsc_command_req                 = 0x3B18,
	HUITP_MSGID_sui_bfsc_command_resp                = 0x3B98,
	//差错过程
	HUITP_MSGID_sui_bfsc_fault_ind                   = 0x3B99,
	HUITP_MSGID_sui_bfsc_err_inq_cmd_req             = 0x3B1A,
	HUITP_MSGID_sui_bfsc_err_inq_cmd_resp            = 0x3B9A,
	//心跳过程
	HUITP_MSGID_sui_bfsc_heart_beat_report           = 0x3BA0,
	HUITP_MSGID_sui_bfsc_heart_beat_confirm          = 0x3B20,

	//统一结束符
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
	//HCU-IHU SUI新增内容
	HUITP_MSGID_sui_inventory_report                 = 0xA090,
	HUITP_MSGID_sui_inventory_confirm                = 0xA010,
	HUITP_MSGID_uni_inventory_max,

  //软件版本体
	HUITP_MSGID_uni_sw_package_min                   = 0xA100,	
	HUITP_MSGID_uni_sw_package_req                   = 0xA100,	
	HUITP_MSGID_uni_sw_package_resp                  = 0xA180,	 
	HUITP_MSGID_uni_sw_package_report                = 0xA181,	 
	HUITP_MSGID_uni_sw_package_confirm               = 0xA101,
	//HCU-IHU SUI新增内容
	HUITP_MSGID_sui_sw_package_report                = 0xA190,
	HUITP_MSGID_sui_sw_package_confirm               = 0xA110,

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
	HUITP_IEID_uni_com_segment                      = 0x0032,
	HUITP_IEID_uni_com_snr_cmd_tag                  = 0x0033,


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
	HUITP_IEID_uni_co1_value                         = 0x2200,
	HUITP_IEID_uni_co_max,

  //甲醛HCHO
	HUITP_IEID_uni_hcho_min                         = 0x2300,
	HUITP_IEID_uni_hcho_value                       = 0x2300,
	HUITP_IEID_uni_hcho_max,

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
	HUITP_IEID_uni_picture_body                     = 0x2F03,
	HUITP_IEID_uni_picture_max,

  //扬尘监控系统
	HUITP_IEID_uni_ycjk_min                         = 0x3000, 
	HUITP_IEID_uni_ycjk_value                       = 0x3000, 
	HUITP_IEID_uni_ycjk_sensor_selection            = 0x3001,
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
	HUITP_IEID_uni_lightstr_data_min                = 0x3500,
	HUITP_IEID_uni_lightstr_data_value              = 0x3500,
	HUITP_IEID_uni_lightstr_data_max,

  //有毒气体VOC
	HUITP_IEID_uni_toxicgas_data_min                = 0x3600,
	HUITP_IEID_uni_toxicgas_data_value              = 0x3600,
	HUITP_IEID_uni_toxicgas_data_max,

  //海拔高度
	HUITP_IEID_uni_altitude_data_min                = 0x3700,
	HUITP_IEID_uni_altitude_data_value              = 0x3700,
	HUITP_IEID_uni_altitude_data_max,

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
	HUITP_IEID_uni_scale_weight_req		           	= 0x3B00,
	HUITP_IEID_uni_scale_weight_value               = 0x3B01,
	HUITP_IEID_uni_scale_weight_cmd                 = 0x3B02,
	HUITP_IEID_uni_scale_weight_cfg_par             = 0x3B03,
	HUITP_IEID_uni_scale_weight_event               = 0x3B04,
	HUITP_IEID_uni_scale_weight_statistic           = 0x3B05,
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
	HUITP_IEID_uni_ccl_lock_min                 	= 0x4D00,
	HUITP_IEID_uni_ccl_lock_state               	= 0x4D00,
	HUITP_IEID_uni_ccl_lock_auth_req            	= 0x4D01,
	HUITP_IEID_uni_ccl_lock_auth_resp           	= 0x4D02,
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
	HUITP_IEID_uni_inventory_element                = 0xA000,
	HUITP_IEID_sui_inventory_element                = 0xA010,
	HUITP_IEID_uni_inventory_max,

  //软件版本体
	HUITP_IEID_uni_sw_package_min                   = 0xA100,	
	HUITP_IEID_uni_sw_package_body                  = 0xA100, 
	HUITP_IEID_uni_sw_package_max,

  //ALARM REPORT
	HUITP_IEID_uni_alarm_info_min                   = 0xB000, 
	HUITP_IEID_uni_alarm_info_element               = 0xB000,
	HUITP_IEID_uni_alarm_info_max,

  //PM Report
	HUITP_IEID_uni_performance_info_min             = 0xB100, 
	HUITP_IEID_uni_performance_info_element         = 0xB100,
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
	UINT8  flag;
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
	UINT32 comEqupIdValue;
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
	UINT32 value;
}StrIe_HUITP_IEID_uni_com_work_cycle_t;

//HUITP_IEID_uni_com_sample_cycle                 = 0x0023,
typedef struct StrIe_HUITP_IEID_uni_com_sample_cycle
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 value;
}StrIe_HUITP_IEID_uni_com_sample_cycle_t;

//HUITP_IEID_uni_com_sample_number                = 0x0024,
typedef struct StrIe_HUITP_IEID_uni_com_sample_number
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 value;
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
	UINT32 comNtimesValue;
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
	UINT32 value;
}StrIe_HUITP_IEID_uni_com_grade_t;

//HUITP_IEID_uni_com_percentage                   = 0x002E,
typedef struct StrIe_HUITP_IEID_uni_com_percentage
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 value;
}StrIe_HUITP_IEID_uni_com_percentage_t;

//HUITP_IEID_uni_com_modbus_address               = 0x002F,
typedef struct StrIe_HUITP_IEID_uni_com_modbus_address
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 oldValue;
	UINT32 newValue;
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

//HUITP_IEID_uni_com_segment                     = 0x0032,
typedef struct StrIe_HUITP_IEID_uni_com_segment
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 swRelId;
	UINT16 swVerId;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
}StrIe_HUITP_IEID_uni_com_segment_t;

//HUITP_IEID_uni_com_snr_cmd_tag                  = 0x0033,
typedef struct StrIe_HUITP_IEID_uni_com_snr_cmd_tag
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  cmdTag;
}StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t;
#define HUITP_IEID_UNI_COM_CMD_TAG_NULL                   0
#define HUITP_IEID_UNI_COM_CMD_TAG_SET_SWITCH             1
#define HUITP_IEID_UNI_COM_CMD_TAG_SET_MODBUS_ADDR        2
#define HUITP_IEID_UNI_COM_CMD_TAG_SET_WORK_CYCLE         3
#define HUITP_IEID_UNI_COM_CMD_TAG_SET_SAMPLE_CYCLE      　4
#define HUITP_IEID_UNI_COM_CMD_TAG_SET_SAMPLE_NBR         5
#define HUITP_IEID_UNI_COM_CMD_TAG_READ_SWITCH            6
#define HUITP_IEID_UNI_COM_CMD_TAG_READ_MODBUS_ADDR       7
#define HUITP_IEID_UNI_COM_CMD_TAG_READ_WORK_CYCLE        8
#define HUITP_IEID_UNI_COM_CMD_TAG_READ_SAMPLE_CYCLE      9
#define HUITP_IEID_UNI_COM_CMD_TAG_READ_SAMPLE_NBR        10
#define HUITP_IEID_UNI_COM_CMD_TAG_INVALID                0xFF

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
	UINT32 emcDataValue;
	UINT32 timeStamp;
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
	UINT32 emcAccuValue;
}StrIe_HUITP_IEID_uni_emc_accu_value_t;

//HUITP_IEID_uni_emc_accu_max,

//一氧化碳
//HUITP_IEID_uni_co_min                           = 0x2200, 
//HUITP_IEID_uni_co1_value                         = 0x2200,
typedef struct StrIe_HUITP_IEID_uni_co1_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 co1Value;
}StrIe_HUITP_IEID_uni_co1_value_t;

//HUITP_IEID_uni_co_max,1

//甲醛HCHO
//HUITP_IEID_uni_hcho_min                 = 0x2300,
//HUITP_IEID_uni_hcho_value               = 0x2300,
typedef struct StrIe_HUITP_IEID_uni_hcho_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 hchoValue;
}StrIe_HUITP_IEID_uni_hcho_value_t;

//HUITP_IEID_uni_hcho_max,

//酒精
//HUITP_IEID_uni_alcohol_min                      = 0x2400, 
//HUITP_IEID_uni_alcohol_value                    = 0x2400, 
typedef struct StrIe_HUITP_IEID_uni_alcohol_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 alcoholValue;
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
	UINT32 pm01Value;
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_pm01_value_t;

//HUITP_IEID_uni_pm25_value                       = 0x2501,
typedef struct StrIe_HUITP_IEID_uni_pm25_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 pm25Value;
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_pm25_value_t;

//HUITP_IEID_uni_pm10_value                       = 0x2502,
typedef struct StrIe_HUITP_IEID_uni_pm10_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 pm10Value;
	UINT32 timeStamp;
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
	UINT32 windspdValue;
	UINT32 timeStamp;
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
	UINT32 winddirValue;
	UINT32 timeStamp;
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
	UINT32 tempValue;
	UINT32 timeStamp;
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
	UINT32 humidValue;
	UINT32 timeStamp;
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
	UINT32 airprsValue;
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
	UINT32 noiseValue;
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_noise_value_t;

//HUITP_IEID_uni_noise_max,

//相机Camer or audio high speed
//HUITP_IEID_uni_hsmmp_min                        = 0x2C00, 
//HUITP_IEID_uni_hsmmp_value                      = 0x2C00,
#define HUITP_IEID_UNI_HSMMP_LINK_FILE_LEN_MAX  80
typedef struct StrIe_HUITP_IEID_uni_hsmmp_value
{
	UINT16 ieId;
	UINT16 ieLen;
	char   linkName[HUITP_IEID_UNI_HSMMP_LINK_FILE_LEN_MAX];
	UINT32 timeStampStart;
	UINT32 timeStampEnd;
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
	UINT32 audioValue;
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
	UINT32 videoValue;
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
	UINT32 pictureValue;
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
	UINT8  picFormat;
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
	UINT32 emcValue;
	UINT32 tempValue;
	UINT32 humidValue;
	UINT32 winddirValue;
	UINT32 windspdValue;
	UINT32 noiseValue;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	UINT32 airprsValue;
}StrIe_HUITP_IEID_uni_ycjk_value_t;

//HUITP_IEID_uni_ycjk_sensor_selection            = 0x3001,
typedef struct StrIe_HUITP_IEID_uni_ycjk_sensor_selection
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  sensorType;
	UINT8  sensorId;
	UINT32 sensorBitmap;
}StrIe_HUITP_IEID_uni_ycjk_sensor_selection_t;

//HUITP_IEID_uni_ycjk_max,

//水表
//HUITP_IEID_uni_water_meter_min                  = 0x3100, 
//HUITP_IEID_uni_water_meter_value                = 0x3100,
typedef struct StrIe_HUITP_IEID_uni_water_meter_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  waterMeterFormat;
	UINT32 waterMeterValue;
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
	UINT32 heatMeterValue;
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
	UINT32 gasMeterValue;
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
	UINT32 powerMeterValue;
}StrIe_HUITP_IEID_uni_power_meter_value_t;

//HUITP_IEID_uni_power_meter_max,

//光照强度
//HUITP_IEID_uni_lightstr_data_min               = 0x3500,
//HUITP_IEID_uni_lightstr_data_value             = 0x3500,
typedef struct StrIe_HUITP_IEID_uni_lightstr_data_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 lightstrValue;
}StrIe_HUITP_IEID_uni_lightstr_data_value_t;

//HUITP_IEID_uni_lightstr_data_max,

//有毒气体VOC
//HUITP_IEID_uni_toxicgas_data_min                     = 0x3600,
//HUITP_IEID_uni_toxicgas_data_value                   = 0x3600,
typedef struct StrIe_HUITP_IEID_uni_toxicgas_data_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 toxicgasValue;
}StrIe_HUITP_IEID_uni_toxicgas_data_value_t;

//HUITP_IEID_uni_toxicgas_data_max,

//海拔高度
//HUITP_IEID_uni_altitude_data_min                     = 0x3700,
//HUITP_IEID_uni_altitude_data_value                   = 0x3700,
typedef struct StrIe_HUITP_IEID_uni_altitude_data_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 altitudeValue;
}StrIe_HUITP_IEID_uni_altitude_data_value_t;

//HUITP_IEID_uni_altitude_data_max,

//马达
//HUITP_IEID_uni_moto_min                         = 0x3800, 
//HUITP_IEID_uni_moto_value                       = 0x3800,
typedef struct StrIe_HUITP_IEID_uni_moto_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT32 motoValue;
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
	UINT32 switchResistorValue;
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
	UINT32 transporterValue;
}StrIe_HUITP_IEID_uni_transporter_value_t;

//HUITP_IEID_uni_transporter_max,

//组合秤BFSC
//HUITP_IEID_uni_bfsc_comb_scale_min              = 0x3B00, 
//HUITP_IEID_uni_scale_weight_req		           	= 0x3B00,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_req
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  type;
}StrIe_HUITP_IEID_uni_scale_weight_req_t;
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_NULL 0
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_CURRENT 1
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_INSTANT 2
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_1M 3
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_15M 4
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_60M 5
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_2H 6
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_8H 7
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_STA_REP_24H 8
#define HUITP_IEID_UNI_SCALE_WEIGHT_REQ_TYPE_INVALID 0xFF

//HUITP_IEID_uni_scale_weight_value               = 0x3B01,
#define HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX 20
typedef struct StrIe_HUITP_IEID_uni_scale_weight_sta_element
{
	UINT32 combTimes;		//组合总次数
	UINT32 tttTimes;		//组合出料次数
	UINT32 tgvTimes;		//组合放弃次数
	UINT32 combMatNbr;  	//组合的物料数量
	UINT32 tttMatNbr;		//组合出料的物料数量
	UINT32 tgvMatNbr;		//组合放弃的物料数量
	UINT32 combAvgSpeed;	//组合平均速度
	UINT32 totalWeight;		//总共处理的物料总重量
	UINT32 totalMatNbr;  	//总共处理的物料总数量
	UINT32 totalWorkMin;	//连续工作的总分钟数
	UINT32 timeInUnix;  	//报告生成的unix时间，网络可能报告滞留
	UINT32 errNbr;			//出错次数
}StrIe_HUITP_IEID_uni_scale_weight_sta_element_t;
typedef struct StrIe_HUITP_IEID_uni_scale_weight_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	UINT8  snrCfgNbrMax;  	//系统配置的称重传感器最大数
	UINT32 snrUsedBitmap;  	//汇报哪些称重传感器对应的数据
	UINT32 snrValue[HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX];
	StrIe_HUITP_IEID_uni_scale_weight_sta_element_t sta;
}StrIe_HUITP_IEID_uni_scale_weight_value_t;

//HUITP_IEID_uni_scale_weight_cmd                 = 0x3B02,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_cmd
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  scaleWeightCmd;
}StrIe_HUITP_IEID_uni_scale_weight_cmd_t;
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_NULL 0
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_START 1
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_STOP 2
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_REVERSE 3
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_SUSPEND 4
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_RESUME 5
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_CONTINUE 6
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_FORBIDEN 7
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_LOCK 8
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_UNLOCK 9
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_AUTH 10
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_UNAUTH 11
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_NORMAL_MODE 12
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_SLEEP_MODE 13
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_MAINT_MODE 14
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_RESTART 15
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_REBOOT 16
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_SHUT_DOWN 17
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_CFG_TO_EQU 18
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_CFG_REPORT 19
#define HUITP_IEID_UNI_SCALE_WEIGHT_CMD_INVALID 0xFF

//HUITP_IEID_uni_scale_weight_cfg_par             = 0x3B03,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_cfg_par
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  workMode;
	UINT8  scaleWeightNbr;  		//配置数量
	UINT32 staStartTime;  			//24小时之内，哪个时间作为统计的起点
	UINT32 staReportDuration;     	//统计报告周期
}StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t;
#define HUITP_IEID_UNI_SCALE_WEIGHT_CFG_PAR_WORK_MODE_NULL 0
#define HUITP_IEID_UNI_SCALE_WEIGHT_CFG_PAR_WORK_MODE_NORMAL 1
#define HUITP_IEID_UNI_SCALE_WEIGHT_CFG_PAR_WORK_MODE_TIMING 2
#define HUITP_IEID_UNI_SCALE_WEIGHT_CFG_PAR_WORK_MODE_INVALID 0xFF

//HUITP_IEID_uni_scale_weight_event               = 0x3B04,
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN 20
typedef struct StrIe_HUITP_IEID_uni_scale_weight_event
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  eventType;
	UINT8  sensorId;
	UINT16 eventValue;
	char   userName[HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN];
	UINT16 cause;
}StrIe_HUITP_IEID_uni_scale_weight_event_t;
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_NULL 				(0)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SENSOR_ERROR 		(1)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_ERROR 		(2)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_ONE_TIME 		(3)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_ONE_TIME 		(4)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_ONE_TIME 		(5)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_TIMES_IN_15M	(10)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_TIMES_IN_15M 	(11)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_TIMES_IN_15M 	(12)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_TIMES_IN_60M	(13)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_TIMES_IN_60M 	(14)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_TIMES_IN_60M 	(15)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_TIMES_IN_2H		(16)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_TIMES_IN_2H 		(17)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_TIMES_IN_2H 		(18)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_TIMES_IN_8H		(19)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_TIMES_IN_8H 		(20)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_TIMES_IN_8H 		(21)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_TIMES_IN_24H	(22)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TTT_TIMES_IN_24H 	(23)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TGV_TIMES_IN_24H 	(24)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_MATERIALS_NBR_IN_15M (30)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_MATERIALS_NBR_IN_60M (31)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_MATERIALS_NBR_IN_2H 	(32)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_MATERIALS_NBR_IN_8H 	(33)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_MATERIALS_NBR_IN_24M (34)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TOTAL_WEIGHT_IN_15M 	(35)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TOTAL_WEIGHT_IN_60M 	(36)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TOTAL_WEIGHT_IN_2H 	(37)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TOTAL_WEIGHT_IN_8H 	(38)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_TOTAL_WEIGHT_IN_24H 	(39)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_1M 	(40)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_15M 	(41)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_60M 	(42)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_2H 	(43)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_8H 	(44)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_COMB_AVG_SPD_IN_24H 	(45)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_POWER_ON		(50)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_USER_LOGIN			(51)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_USER_LOGOUT			(52)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_IDLE			(53)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_IN_OPR		(54)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_STAR		 	(55)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_STOP		 	(56)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_RESTART		(57)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_SUSPEND		(58)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_SYSTEM_RESUME		(59)
#define HUITP_IEID_UNI_SCALE_WEIGHT_EVENT_TYPE_INVALID 				(0xFF)
//新的错误类型定义
#define HUITP_VALID_ERR_CODE_SCALE_WEIGHT_SENSOR_OUT_OF_SERVICE		(0x3B00)
#define HUITP_VALID_ERR_CODE_SCALE_WEIGHT_SENSOR_RESTART			(0x3B01)
#define HUITP_VALID_ERR_CODE_SCALE_WEIGHT_SENSOR_RCV_ILLEGAL_DATA	(0x3B02)

//HUITP_IEID_uni_scale_weight_statistic           = 0x3B05,
typedef struct StrIe_HUITP_IEID_uni_scale_weight_statistic
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  staRepType;
	UINT8  dataFormat;
	StrIe_HUITP_IEID_uni_scale_weight_sta_element_t sta;
}StrIe_HUITP_IEID_uni_scale_weight_statistic_t;
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_NULL				(0)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_15M				(1)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_60M				(2)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_2H					(3)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_8H					(4)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_24H				(5)
#define HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_INVALID			(0xFF)

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
#define HUITP_IEID_UNI_INVENTORY_ELEMENT_DESC_LEN_MAX 50
typedef struct StrIe_HUITP_IEID_uni_inventory_element
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 	swCheckSum;
	UINT32  swTotalLengthInBytes;
	UINT8  upgradeFlag;
	char   desc[HUITP_IEID_UNI_INVENTORY_ELEMENT_DESC_LEN_MAX];
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_inventory_element_t;
//UpgradeFlag定义
#define HUITP_IEID_UNI_FW_UPGRADE_NONE 0
#define HUITP_IEID_UNI_FW_UPGRADE_NO 1
#define HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE 2
#define HUITP_IEID_UNI_FW_UPGRADE_YES_TRAIL 3
#define HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH 4
#define HUITP_IEID_UNI_FW_UPGRADE_YES_INVALID 0xFF
#define HUITP_IEID_SUI_FW_UPGRADE_NONE 0
#define HUITP_IEID_SUI_FW_UPGRADE_NO 1
#define HUITP_IEID_SUI_FW_UPGRADE_YES_STABLE 2
#define HUITP_IEID_SUI_FW_UPGRADE_YES_TRAIL 3
#define HUITP_IEID_SUI_FW_UPGRADE_YES_PATCH 4
#define HUITP_IEID_SUI_FW_UPGRADE_YES_INVALID 0xFF
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
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G1_AQYC_335D_01 	0x0101
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2001 	0x0201
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002 	0x0202
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003 	0x0203
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004 	0x0204
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100 	0x0210
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_SCY_WATER_01 0x0301
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_CB_01  0x0311
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_SB_01  0x0321
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G4_GQYB_01 0x0401
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5001 0x0501
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5002 0x0502
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5003 0x0503
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5004 0x0504
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5100 0x0510
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5101 0x0511
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5102 0x0512
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5103 0x0513
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5104 0x0514
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5105 0x0515
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5106 0x0516
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5107 0x0517
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5108 0x0518
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5109 0x0519
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5201 0x0521
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5202 0x0522
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G6_ZNLD_01 0x0601
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_RFID_MS01 0x0701
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_OTDR_01 0x0711
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8001 	0x0801
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8002 	0x0802
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8003	0x0803
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8004 	0x0804
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_IHU_01 0x0881
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_58_01 0x1101
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_68_01 0x1111
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E2_HJJK_01 0x1201  //手持式环境监控
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E3_DZGP_01 0x1301  //电子工牌
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E4_ZNSH_01 0x1401  //智能手环
//用于指示单个硬件具体的PEM信息，或者小版本的演进修改。
//如果没有独特信息，可采用0000/FFFF来表示无效不用。

//HUITP_IEID_sui_inventory_element                = 0xA010,
#define HUITP_IEID_SUI_INVENTORY_ELEMENT_DESC_LEN_MAX 50
typedef struct StrIe_HUITP_IEID_sui_inventory_element
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	char   desc[HUITP_IEID_SUI_INVENTORY_ELEMENT_DESC_LEN_MAX];
}StrIe_HUITP_IEID_sui_inventory_element_t;

//HUITP_IEID_uni_inventory_max,

//软件版本体
//HUITP_IEID_uni_sw_package_min                   = 0xA100,	
//HUITP_IEID_uni_sw_package_body                  = 0xA100,
#define HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN ((400 < HUITP_IE_BUF_MAX_LEN)? 400: HUITP_IE_BUF_MAX_LEN)
typedef struct StrIe_HUITP_IEID_uni_sw_package_body
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN];
}StrIe_HUITP_IEID_uni_sw_package_body_t;
#define HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN 232

//HUITP_IEID_uni_sw_package_max,

//ALARM REPORT
//HUITP_IEID_uni_alarm_info_min                   = 0xB000, 
//HUITP_IEID_uni_alarm_info_element               = 0xB000,
#define HUITP_IEID_UNI_ALARM_INFO_ELEMENT_DESC_LEN_MAX 100
typedef struct StrIe_HUITP_IEID_uni_alarm_info_element
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 alarmType;
	UINT8  alarmServerity;
	UINT8  alarmClearFlag;
	UINT32 equID;
	UINT32 causeId;
	UINT32 alarmContent;
	char   alarmDesc[HUITP_IEID_UNI_ALARM_INFO_ELEMENT_DESC_LEN_MAX];
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_alarm_info_element_t;

//HUITP_IEID_uni_alarm_info_max,

//PM Report
//HUITP_IEID_uni_performance_info_min             = 0xB100, 
//HUITP_IEID_uni_performance_info_element         = 0xB100,
typedef struct StrIe_HUITP_IEID_uni_performance_info_element
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 restartCnt;
	UINT32 networkConnCnt;
	UINT32 networkConnFailCnt;
	UINT32 networkDiscCnt;
	UINT32 socketDiscCnt;
	UINT32 cpuOccupy;
	UINT32 memOccupy;
	UINT32 diskOccupy;
	UINT32 cpuTemp;
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_performance_info_element_t;

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
	UINT16 randval;
}StrIe_HUITP_IEID_uni_heart_beat_ping_t;

//HUITP_IEID_uni_heart_beat_pong                  = 0xFE01, 
typedef struct StrIe_HUITP_IEID_uni_heart_beat_pong
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 randval;
}StrIe_HUITP_IEID_uni_heart_beat_pong_t;

//HUITP_IEID_uni_heart_beat_max,


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP ERR_CODE定义
//
//  #使用２BYTE
//  #第一个BYTE与对应的CMDID匹配，用于定义特定CMDID下出现的错误原因
//  #第二个BYTE定义与具体的错误
//  #第一个BYTE=0时，表示通用的错误类型
//  #0x0000, 0xFFFF都保留不用
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define HUITP_VALID_ERR_CODE_NULL 						(0x0000)
#define HUITP_VALID_ERR_CODE_UNKNOW 	 				(0x0001)
#define HUITP_VALID_ERR_CODE_GENERAL_REASON 			(0x0002)
#define HUITP_VALID_ERR_CODE_GENERAL_HARDWARE_FAULT 	(0x0003)
#define HUITP_VALID_ERR_CODE_GENERAL_SOFTWARE_FAULT 	(0x0003)
#define HUITP_VALID_ERR_CODE_INVALID 					(0xFFFF)


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

//HUITP_MSGID_uni_emc_ctrl_req                     = 0x2002,
typedef struct StrMsg_HUITP_MSGID_uni_emc_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t;

//HUITP_MSGID_uni_emc_ctrl_resp                    = 0x2082,
typedef struct StrMsg_HUITP_MSGID_uni_emc_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_emc_ctrl_resp_t;

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
//HUITP_MSGID_uni_co1_min                           = 0x2200,
//HUITP_MSGID_uni_co1_req                           = 0x2200,
typedef struct StrMsg_HUITP_MSGID_uni_co1_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_co1_data_req_t;

//HUITP_MSGID_uni_co1_data_resp                          = 0x2280,
typedef struct StrMsg_HUITP_MSGID_uni_co1_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_co1_value_t respValue;
}StrMsg_HUITP_MSGID_uni_co1_data_resp_t;

//HUITP_MSGID_uni_co1_data_report                        = 0x2281,
typedef struct StrMsg_HUITP_MSGID_uni_co1_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_co1_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_co1_data_report_t;
 
//HUITP_MSGID_uni_co1_co1_data_confirm                           = 0x2201,
typedef struct StrMsg_HUITP_MSGID_uni_co1_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_co1_data_confirm_t;


//HUITP_MSGID_uni_co1_max,

//甲醛HCHO
//HUITP_MSGID_uni_hcho_min                         = 0x2300,
//HUITP_MSGID_uni_hcho_data_req                 = 0x2300,
typedef struct StrMsg_HUITP_MSGID_uni_hcho_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_hcho_data_req_t;

//HUITP_MSGID_uni_hcho_data_resp                = 0x2380,
typedef struct StrMsg_HUITP_MSGID_uni_hcho_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_hcho_value_t respValue;
}StrMsg_HUITP_MSGID_uni_hcho_data_resp_t;
 
//HUITP_MSGID_uni_hcho_data_report              = 0x2381,
typedef struct StrMsg_HUITP_MSGID_uni_hcho_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_hcho_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_hcho_data_report_t;
 
//HUITP_MSGID_uni_hcho_data_confirm                 = 0x2301,
typedef struct StrMsg_HUITP_MSGID_uni_hcho_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_hcho_data_confirm_t;

//HUITP_MSGID_uni_hcho_max,

//酒精
//HUITP_MSGID_uni_alcohol_min                      = 0x2400, 
//HUITP_MSGID_uni_alcohol_data_req                      = 0x2400,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_alcohol_data_req_t;

//HUITP_MSGID_uni_alcohol_data_resp                     = 0x2480,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_alcohol_value_t respValue;
}StrMsg_HUITP_MSGID_uni_alcohol_data_resp_t;

//HUITP_MSGID_uni_alcohol_data_report                   = 0x2481,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_alcohol_value_t reportValue;	
}StrMsg_HUITP_MSGID_uni_alcohol_data_report_t;

//HUITP_MSGID_uni_alcohol_data_confirm                      = 0x2401,
typedef struct StrMsg_HUITP_MSGID_uni_alcohol_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t;

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

//HUITP_MSGID_uni_pm25_ctrl_req                    = 0x2502,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t;

//HUITP_MSGID_uni_pm25_ctrl_resp                   = 0x2582,
typedef struct StrMsg_HUITP_MSGID_uni_pm25_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_pm25_ctrl_resp_t;

//HUITP_MSGID_uni_pm25sp_data_req                  = 0x2503,
typedef struct StrMsg_HUITP_MSGID_uni_pm25sp_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t;

//HUITP_MSGID_uni_pm25sp_data_resp                 = 0x2583,
typedef struct StrMsg_HUITP_MSGID_uni_pm25sp_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_pm01_value_t respPm01Value;
	StrIe_HUITP_IEID_uni_pm25_value_t respPm25Value;
	StrIe_HUITP_IEID_uni_pm10_value_t respPm10Value;
}StrMsg_HUITP_MSGID_uni_pm25sp_data_resp_t;

//HUITP_MSGID_uni_pm25sp_data_report                  = 0x2504,
typedef struct StrMsg_HUITP_MSGID_uni_pm25sp_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_pm01_value_t reportPm01Value;
	StrIe_HUITP_IEID_uni_pm25_value_t reportPm25Value;
	StrIe_HUITP_IEID_uni_pm10_value_t reportPm10Value;
}StrMsg_HUITP_MSGID_uni_pm25sp_data_report_t;

//HUITP_MSGID_uni_pm25sp_data_confirm                 = 0x2584,
typedef struct StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t;

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

//HUITP_MSGID_uni_windspd_ctrl_req                 = 0x2602,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t;

//HUITP_MSGID_uni_windspd_ctrl_resp                = 0x2682,
typedef struct StrMsg_HUITP_MSGID_uni_windspd_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_windspd_ctrl_resp_t;

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

//HUITP_MSGID_uni_winddir_ctrl_req                 = 0x2702,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t;

//HUITP_MSGID_uni_winddir_ctrl_resp                = 0x2782,
typedef struct StrMsg_HUITP_MSGID_uni_winddir_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_winddir_ctrl_resp_t;

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

//HUITP_MSGID_uni_temp_ctrl_req                    = 0x2802,
typedef struct StrMsg_HUITP_MSGID_uni_temp_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t;

//HUITP_MSGID_uni_temp_ctrl_resp                   = 0x2882,
typedef struct StrMsg_HUITP_MSGID_uni_temp_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_temp_ctrl_resp_t;

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

//HUITP_MSGID_uni_humid_ctrl_req                   = 0x2902,
typedef struct StrMsg_HUITP_MSGID_uni_humid_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t;

//HUITP_MSGID_uni_humid_ctrl_resp                  = 0x2982,
typedef struct StrMsg_HUITP_MSGID_uni_humid_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_humid_ctrl_resp_t;

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

//HUITP_MSGID_uni_noise_ctrl_req                   = 0x2B02,
typedef struct StrMsg_HUITP_MSGID_uni_noise_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t;

//HUITP_MSGID_uni_noise_ctrl_resp                  = 0x2B82,
typedef struct StrMsg_HUITP_MSGID_uni_noise_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_noise_ctrl_resp_t;

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

//HUITP_MSGID_uni_hsmmp_ctrl_req                   = 0x2C02,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t;

//HUITP_MSGID_uni_hsmmp_ctrl_resp                  = 0x2C82,
typedef struct StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_resp_t;

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

//HUITP_MSGID_uni_audio_ctrl_req                   = 0x2D02,
typedef struct StrMsg_HUITP_MSGID_uni_audio_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_audio_ctrl_req_t;

//HUITP_MSGID_uni_audio_ctrl_resp                  = 0x2D82,
typedef struct StrMsg_HUITP_MSGID_uni_audio_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_audio_ctrl_resp_t;

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

//HUITP_MSGID_uni_video_ctrl_req                   = 0x2E02,
typedef struct StrMsg_HUITP_MSGID_uni_video_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_video_ctrl_req_t;

//HUITP_MSGID_uni_audio_ctrl_resp                  = 0x2E82,
typedef struct StrMsg_HUITP_MSGID_uni_video_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_video_ctrl_resp_t;

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

//HUITP_MSGID_uni_picture_ctrl_req                 = 0x2F02,
typedef struct StrMsg_HUITP_MSGID_uni_picture_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_picture_ctrl_req_t;

//HUITP_MSGID_uni_picture_ctrl_resp                = 0x2F82,
typedef struct StrMsg_HUITP_MSGID_uni_picture_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_picture_ctrl_resp_t;

//HUITP_MSGID_uni_picture_max,

//扬尘监控系统
//HUITP_MSGID_uni_ycjk_min                         = 0x3000, 
//HUITP_MSGID_uni_ycjk_data_req                    = 0x3000,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_ycjk_data_req_t;

//HUITP_MSGID_uni_ycjk_resp                        = 0x3080,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ycjk_value_t respValue;
}StrMsg_HUITP_MSGID_uni_ycjk_data_resp_t;

//HUITP_MSGID_uni_ycjk_data_report                      = 0x3081,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_ycjk_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_ycjk_data_report_t;

//HUITP_MSGID_uni_ycjk_data_confirm                         = 0x3001,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_ycjk_data_confirm_t;


//HUITP_MSGID_uni_ycjk_ctrl_req                    = 0x3002,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_ycjk_sensor_selection_t sensor;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdReq;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_ycjk_ctrl_req_t;

//HUITP_MSGID_uni_ycjk_ctrl_resp                   = 0x3082,
typedef struct StrMsg_HUITP_MSGID_uni_ycjk_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_ycjk_sensor_selection_t sensor;
	StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t cmdResp;
	StrIe_HUITP_IEID_uni_com_switch_onoff_t switchState;
	StrIe_HUITP_IEID_uni_com_work_cycle_t workCycle;
	StrIe_HUITP_IEID_uni_com_sample_cycle_t sampleCycle;
	StrIe_HUITP_IEID_uni_com_sample_number_t sampleNbr;
	StrIe_HUITP_IEID_uni_com_modbus_address_t modbusAddr;
}StrMsg_HUITP_MSGID_uni_ycjk_ctrl_resp_t;

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
//HUITP_MSGID_uni_lightstr_data_min               = 0x3500,
//HUITP_MSGID_uni_lightstr_data_req               = 0x3500,
typedef struct StrMsg_HUITP_MSGID_uni_lightstr_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_lightstr_data_req_t;

//HUITP_MSGID_uni_lightstr_data_resp              = 0x3580,
typedef struct StrMsg_HUITP_MSGID_uni_lightstr_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_lightstr_data_value_t respValue;
}StrMsg_HUITP_MSGID_uni_lightstr_data_resp_t;

//HUITP_MSGID_uni_lightstr_data_report            = 0x3581,
typedef struct StrMsg_HUITP_MSGID_uni_lightstr_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_lightstr_data_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_lightstr_data_report_t;

//HUITP_MSGID_uni_lightstr_data_confirm               = 0x3501,
typedef struct StrMsg_HUITP_MSGID_uni_lightstr_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t;
 
//HUITP_MSGID_uni_lightstr_data_max,

//有毒气体VOC
//HUITP_MSGID_uni_toxicgas_data_min                     = 0x3600,
//HUITP_MSGID_uni_toxicgas_data_req                     = 0x3600,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t;
 
//HUITP_MSGID_uni_toxicgas_data_resp                    = 0x3680,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_toxicgas_data_value_t respValue;
}StrMsg_HUITP_MSGID_uni_toxicgas_data_resp_t;
 
//HUITP_MSGID_uni_toxicgas_data_report                  = 0x3681,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_toxicgas_data_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_toxicgas_data_report_t;

//HUITP_MSGID_uni_toxicgas_data_confirm                     = 0x3601,
typedef struct StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t;
 
//HUITP_MSGID_uni_toxicgas_data_max,

//海拔高度
//HUITP_MSGID_uni_altitude_data_min                     = 0x3700,
//HUITP_MSGID_uni_altitude_data_req                     = 0x3700,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_altitude_data_req_t;
 
//HUITP_MSGID_uni_altitude_data_resp                    = 0x3780,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_altitude_data_value_t respValue;
}StrMsg_HUITP_MSGID_uni_altitude_data_resp_t;
 
//HUITP_MSGID_uni_altitude_data_report                  = 0x3781,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_altitude_data_value_t reportValue;
}StrMsg_HUITP_HUITP_MSGID_uni_altitude_data_report_t;

//HUITP_MSGID_uni_altitude_data_confirm                     = 0x3701,
typedef struct StrMsg_HUITP_MSGID_uni_altitude_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_altitude_data_confirm_t;

//HUITP_MSGID_uni_altitude_data_max,

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
}StrMsg_HUITP_MSGID_uni_transporter_report_t;

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
//HUITP_MSGID_uni_bfsc_comb_scale_data_req         = 0x3B00,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_req_t dataReq;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t;
 
//HUITP_MSGID_uni_bfsc_comb_scale_data_resp        = 0x3B80,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_value_t respValue;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_resp_t;

//HUITP_MSGID_uni_bfsc_comb_scale_data_report      = 0x3B81,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_report_t;

//HUITP_MSGID_uni_bfsc_comb_scale_data_confirm     = 0x3B01,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t;

//HUITP_MSGID_uni_bfsc_comb_scale_event_report     = 0x3B82,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_event_t reportEvent;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_report_t;

//HUITP_MSGID_uni_bfsc_comb_scale_event_confirm    = 0x3B02,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t;

//HUITP_MSGID_uni_bfsc_comb_scale_ctrl_req          = 0x3B03,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdReq;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgReq;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_ctrl_req_t;

//HUITP_MSGID_uni_bfsc_comb_scale_ctrl_resp         = 0x3B83,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdResp;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgResp;
}StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_ctrl_resp_t;

//HUITP_MSGID_uni_bfsc_statistic_report            = 0x3B84,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_statistic_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_statistic_t staRep;
}StrMsg_HUITP_MSGID_uni_bfsc_statistic_report_t;

//HUITP_MSGID_uni_bfsc_statistic_confirm           = 0x3B04,
typedef struct StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t;


//HCU-IHU SUI新增内容
//以下FRAME消息结构体，由于特殊原因，并没有采用TLV结构，以便节省消息长度

/*
** =============================================================================
** ============================ MYC START FOR BFSC =============================
** =============================================================================
*/

typedef struct WmcErrorCode
{
	UINT16 error_code;
	UINT16 spare1;
}WmcErrorCode_t;

typedef struct WmcId
{
	UINT8 wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare1;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare2;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare3;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
}WmcId_t;

typedef struct WmcInventory
{
	UINT32 hw_inventory_id;
	UINT32 sw_inventory_id;
	UINT32 stm32_cpu_id;
	UINT32 weight_sensor_type;
	UINT32 motor_type;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT32 spare1;
	UINT32 spare2;
}WmcInventory_t;

//==========================================================
/*
**	MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
*/
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcInventory_t wmc_inventory;
}StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
*/
typedef struct CombinationAlgorithmParamaters
{
	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
	UINT32	MinScaleNumberStartCombination;		//开始查找的最小个数，就是说大于这个个数就开始搜索
	UINT32	TargetCombinationWeight;				  //组合目标重量
	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
	UINT32	IsPriorityScaleEnabled;					  // 1: Enable, 0: Disable
	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
	UINT32	CombinationBias;						      //每个Scale要求放几个物品
	UINT32	IsRemainDetectionEnable;				  //Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	RemainScaleTreatment;					    // 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
	UINT32	CombinationSpeedMode;					    // 0：SpeedPriority，1: PrecisePriority
	UINT32	CombinationAutoMode;					    // 0: Auto, 1: Manual
	UINT32	MovingAvrageSpeedCount;					  //计算平均速度的时候使用最近多少个组合做统计
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}CombinationAlgorithmParamaters_t;

typedef struct WeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds

	UINT32	WeightSensorInitOrNot;							// NOT for GUI
	UINT32	WeightSensorAdcSampleFreq;
	UINT32	WeightSensorAdcGain;
	UINT32	WeightSensorAdcBitwidth;						// NOT for GUI
	UINT32  WeightSensorAdcValue;								// NOT for GUI
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorFilterCoeff[4];				// NOT for GUI
	UINT32  WeightSensorOutputValue[4];				// NOT for GUI
}WeightSensorParamaters_t;

typedef struct MotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}MotorControlParamaters_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfsc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t;

/*
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,
*/
//HUITP_MSGID_sui_bfsc_set_config_resp             = 0x3B91,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp
{
	UINT16 msgid;
	UINT16 length;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
*/
//HUITP_MSGID_sui_bfsc_start_req                   = 0x3B12,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_start_req
{
	UINT16 msgid;
	UINT16 length;
}StrMsg_HUITP_MSGID_sui_bfsc_start_req_t;

//HUITP_MSGID_sui_bfsc_start_resp                  = 0x3B92,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_start_resp
{
	UINT16 msgid;
	UINT16 length;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t;

//HUITP_MSGID_sui_bfsc_stop_req                    = 0x3B13,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t;

//HUITP_MSGID_sui_bfsc_stop_resp                   = 0x3B93,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_stop_resp
{
	UINT16 msgid;
	UINT16 length;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t;
/*
**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,	  	//	  	 = 0x3B95,
**
**	NOTE: These two are used for report Weight event, used same struct
**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT -> EMPTY -> LOAD
**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT -> LOAD -> LOAD. with repeat time, used for AWS to
**	**
**
*/
#define 	WEIGHT_EVENT_ID_LOAD						(0)
#define 	WEIGHT_EVENT_ID_EMPTY						(1)
#define 	WEIGHT_EVENT_ID_PICKUP						(2)

typedef struct CombineType
{
	UINT32	WeightCombineType;
	UINT32	ActionDelayMs;
}CombineType_t;
#define HUITP_IEID_SUI_BFSC_COMINETYPE_NULL 0
#define HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT 1
#define HUITP_IEID_SUI_BFSC_COMINETYPE_DROP 2
#define HUITP_IEID_SUI_BFSC_COMINETYPE_WARNING 3
#define HUITP_IEID_SUI_BFSC_COMINETYPE_ERROR 4
#define HUITP_IEID_SUI_BFSC_COMINETYPE_INVALID 0xFF

typedef struct WeightIndication
{
	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
	UINT32 average_weight;		//average value in the detect window  // <--- MUST
	UINT32 repeat_times;		// always = 0 for MSG_ID_L3BFSC_WMC_NEW_WS_EVENT, = n for MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT
}WeightIndication_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
	CombineType_t weight_combin_type;
}StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t;

//HUITP_MSGID_sui_bfsc_repeat_ws_event             = 0x3B95,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
	CombineType_t weight_combin_type;
}StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t;

/*
**	MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
*/
//#define 	LED_COMMNAD_ID_IGNORE					(0)//MUSR BE 0
//#define 	LED_COMMNAD_ID_ON						(1)
//#define 	LED_COMMNAD_ID_OFF						(2)
//#define 	LED_COMMNAD_ID_BINKING_HIGHSPEED		(3)
//#define 	LED_COMMNAD_ID_BINKING_LOWSPEED			(4)

//#define 	MOTOR_COMMAND_ID_IGORE					(0) //MUSR BE 0
//#define 	MOTOR_COMMAND_ID_START					(1)
//#define 	MOTOR_COMMAND_ID_STOP					(2)
//#define 	MOTOR_COMMAND_ID_ROLLONCE				(3)
//#define 	MOTOR_COMMAND_ID_SPEED_READ				(4)

// CombineType Definition
// COMNINETPYE_ROOLOUT_START
// COMNINETPYE_ROOLOUT_COMPLETE
// COMNINETPYE_DROP_START
// COMNINETPYE_DROP_COMPLETE
// COMNINETPYE_WARNING_START
// COMNINETPYE_WARNING_COMPLETE
// COMNINETPYE_ERROR_START
// COMNINETPYE_ERROR_COMPLETE

//HUITP_MSGID_sui_bfsc_ws_comb_out_req             = 0x3B16,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t;

//HUITP_MSGID_sui_bfsc_ws_comb_out_resp            = 0x3B96,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_FAULT_IND
*/
//HUITP_MSGID_sui_bfsc_fault_ind                   = 0x3B99,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t  wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
*/
#define SENSOR_COMMAND_ID_WEITGH_READ (0x0001)
#define MOTOR_COMMAND_ID (0x0002)
#define LED1_COMMAND_ID (0x0004)
#define LED2_COMMAND_ID (0x0008)
#define LED3_COMMAND_ID (0x0010)
#define LED4_COMMAND_ID (0x0020)

#define LED_COMMNAD_ON						(1)
#define LED_COMMNAD_OFF						(2)
#define LED_COMMNAD_BINKING_HIGHSPEED		(3)
#define LED_COMMNAD_BINKING_LOWSPEED		(4)

#define SESOR_COMMAND_ID_IGORE						(0) //MUSR BE 0
#define SESOR_COMMAND_ID_WEITGH_READ				(3)
#define SESOR_COMMAND_ID_CALIBRATION_ZERO			(4)
#define SESOR_COMMAND_ID_CALIBRATION_FULL			(5)

//特殊命令过程（测试等过程）
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_command_req
{
	UINT16 msgid;
	UINT16 length;
	UINT32 comand_flags;
	UINT8 led1_command;
	UINT8 led2_command;
	UINT8 led3_command;
	UINT8 led4_command;
	UINT32 motor_command;
	UINT32 sensor_command;
}StrMsg_HUITP_MSGID_sui_bfsc_command_req_t;

//HUITP_MSGID_sui_bfsc_command_resp                = 0x3B98,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_command_resp
{
	UINT16	msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
    WmcErrorCode_t result;
	UINT32 	motor_speed;
	UINT32 	sensor_weight;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16 spare2;
}StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_MSH_HEADER
*/
//HUITP_MSGID_sui_bfsc_err_inq_cmd_req             = 0x3B1A,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t;

//HUITP_MSGID_sui_bfsc_err_inq_cmd_resp            = 0x3B9A,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
	UINT32  average_weight;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16 spare2;
}StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t;

//心跳过程
//HUITP_MSGID_sui_bfsc_heart_beat_report           = 0x3BA0,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
}StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t;

//HUITP_MSGID_sui_bfsc_heart_beat_confirm          = 0x3B20,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8   wmcState;
	UINT8   spare1;
	UINT16  spare2;
}StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t;
#define HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_NULL 		0
#define HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_OFFLINE 	1
#define HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_INIT 		2
#define HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_WORKING 	3
#define HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_INVALID 	0xFF

//Common head defintion
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header
{
	UINT16 msgid;
	UINT16 length;
}StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t;

/* Can ID for communication between AWS and WMC */
/* 1: AWS to WMC: 1 to n, n = 0 ... 15       */
/* Node ID bitmap: CAN ID = 0x0010 XXXX    */
/* CAN ID          direction => ^  ~~~~ <= 1111 1111 1111 1111, */
#define AWS_TO_WMC_CAN_ID_PREFIX		(0x00100000U)
//#define AWS_TO_WMC_CAN_ID_SUFFIX		(mwc_id_bitmap)

/* 2: AWS to WMC-S: 1 to 1  */
/* CAN ID: 0x0010 0000 */
#define AWS_TO_WMCS_CAN_ID				(0x00110000U)

/* 3: WMC to AWS: */
/* CAN ID: 0x0020 0000 (WMC Node ID 0) to 0x0020 000F (WMC Node ID 15) */
#define WMC_TO_AWS_CAN_ID_PREFIX		(0x00200000U)
//#define WMC_TO_AWS_CAN_ID_SUFFIX		(mwc_id)

/* 4: WMC-S to AWS: */
/* CAN ID: 0x0030 0010 (Node ID 0) */
#define WMCS_TO_AWS_CAN_ID				(0x00200010U)

/* CAN Msg Length */
//#define		MAX_WMC_CONTROL_MSG_LEN				(256U)
//#define		MAX_WMC_CONTROL_MSG_HEADER_LEN		(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
//#define		MAX_WMC_CONTROL_MSG_BODY_LEN		(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)

/* ERROR CODE */
typedef enum IHU_ERROR_CODE
{
	//ERROR CODE ID
	ERROR_CODE_NO_ERROR = 0, //Starting point

	//COMMON ERROR CODE
	ERROR_CODE_CALLING_ERROR,
	ERROR_CODE_INPUT_PARAMETER_KO,
	ERROR_CODE_WRONG_WMC_STATE,
	ERROR_CODE_UNKNOWN,

	/* TO BE ADDED FOR EACH OF THE MESSAGE */
	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_START_REQ,

	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,

	ERROR_CODE_MAX, //Ending point

}error_code_t; //end of IHU_INTER_TASK_MSG_ID

//统一结束符
//HUITP_MSGID_uni_bfsc_comb_scale_max,
/*
** =============================================================================
** ============================= MYC END FOR BFSC ==============================
** =============================================================================
*/



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
	StrIe_HUITP_IEID_uni_inventory_element_t reqValue;
}StrMsg_HUITP_MSGID_uni_inventory_req_t;

//HUITP_MSGID_uni_inventory_resp                   = 0xA080,
typedef struct StrMsg_HUITP_MSGID_uni_inventory_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_inventory_element_t respValue;
}StrMsg_HUITP_MSGID_uni_inventory_resp_t;

//HUITP_MSGID_uni_inventory_report                 = 0xA081, 
typedef struct StrMsg_HUITP_MSGID_uni_inventory_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_inventory_element_t reportValue;
}StrMsg_HUITP_MSGID_uni_inventory_report_t;

//HUITP_MSGID_uni_inventory_confirm                    = 0xA001, 
typedef struct StrMsg_HUITP_MSGID_uni_inventory_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_inventory_element_t confirmValue;
}StrMsg_HUITP_MSGID_uni_inventory_confirm_t;

//HCU-IHU SUI新增内容
//HUITP_MSGID_sui_inventory_report                 = 0xA090,
typedef struct StrMsg_HUITP_MSGID_sui_inventory_report
{
	UINT16 msgid;
	UINT16 length;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
}StrMsg_HUITP_MSGID_sui_inventory_report_t;

//HUITP_MSGID_sui_inventory_confirm                = 0xA010,
typedef struct StrMsg_HUITP_MSGID_sui_inventory_confirm
{
	UINT16 msgid;
	UINT16 length;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	UINT16 	swCheckSum;
	UINT32  swTotalLengthInBytes;
}StrMsg_HUITP_MSGID_sui_inventory_confirm_t;


//HUITP_MSGID_uni_inventory_max,

//软件版本体
//HUITP_MSGID_uni_sw_package_min                   = 0xA100,	
//HUITP_MSGID_uni_sw_package_req                   = 0xA100,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_com_segment_t segValue;
	StrIe_HUITP_IEID_uni_sw_package_body_t body;
}StrMsg_HUITP_MSGID_uni_sw_package_req_t;

//HUITP_MSGID_uni_sw_package_resp                  = 0xA180,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_com_segment_t segValue;
}StrMsg_HUITP_MSGID_uni_sw_package_resp_t;

//HUITP_MSGID_uni_sw_package_report                = 0xA181,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_com_segment_t segValue;
}StrMsg_HUITP_MSGID_uni_sw_package_report_t;

//HUITP_MSGID_uni_sw_package_confirm                   = 0xA101,
typedef struct StrMsg_HUITP_MSGID_uni_sw_package_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_com_segment_t segValue;
	StrIe_HUITP_IEID_uni_sw_package_body_t body;
}StrMsg_HUITP_MSGID_uni_sw_package_confirm_t;


//HCU-IHU SUI新增内容
//HUITP_MSGID_sui_sw_package_report                   = 0xA190,
typedef struct StrMsg_HUITP_MSGID_sui_sw_package_report
{
	UINT16 msgid;
	UINT16 length;
	UINT16 swRelId;
	UINT16 swVerId;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
}StrMsg_HUITP_MSGID_sui_sw_package_report_t;

//HUITP_MSGID_sui_sw_package_confirm                  = 0xA110,
typedef struct StrMsg_HUITP_MSGID_sui_sw_package_confirm
{
	UINT16 msgid;
	UINT16 length;
	UINT16 swRelId;
	UINT16 swVerId;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN];
}StrMsg_HUITP_MSGID_sui_sw_package_confirm_t;


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
	StrIe_HUITP_IEID_uni_alarm_info_element_t respValue;
}StrMsg_HUITP_MSGID_uni_alarm_info_resp_t;

//HUITP_MSGID_uni_alarm_info_report                = 0xB081,
typedef struct StrMsg_HUITP_MSGID_uni_alarm_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_alarm_info_element_t reportValue;
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
	StrIe_HUITP_IEID_uni_performance_info_element_t respValue;
}StrMsg_HUITP_MSGID_uni_performance_info_resp_t;

//HUITP_MSGID_uni_performance_info_report          = 0xB181, 
typedef struct StrMsg_HUITP_MSGID_uni_performance_info_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_performance_info_element_t reportValue;
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

#pragma pack () //取消字节对齐
#endif /* _HUITP_H_ */
