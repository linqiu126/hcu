/*
 * huitp.h
 *
 *  Created on: 2016年12月25日
 *      Author: test
 */

//pack函数必须严格放在结构的里面，且不得包含任何其它包含文件，不然会发生意想不到的问题！！！


#ifndef _HUITP_H_
#define _HUITP_H_
typedef unsigned char             UINT8;
typedef signed char               INT8;
typedef char                      CHAR;
typedef unsigned short            UINT16;
typedef signed short              INT16;
typedef unsigned int              UINT32;
typedef	signed int                INT32;
typedef unsigned long int         UINT64;
typedef unsigned long long int    LP64;
typedef signed long int           INT64;
typedef unsigned long             ULONG;
typedef signed long               SLONG;
typedef UINT8                     BOOLEAN;
typedef void                      VOID;

#pragma pack (1) //强制1字节对齐
/*
 *
 *  顺从并更新到技术规范《慧HUITP接口规范v2.10, LAST UPDATE@2017/8/15》
 *
 * 2017/06/10, MA Yuchu, modify  for BFSC, Weight Sensor parameters, based on v2.5, LAST UPDATE@2017/5/27
 * 2017/06/20 v2.6: 修改L3BFSC对应的结构，增加Heart-Beat过程
 * 2017/06/30 V2.7: 修改CANITF接口上SW INVENTORY/SW PACKAGE的消息定义与结构
 * 2017/07/03 V2.8: 完善SW INVENTORY/SW PACKAGE的消息定义与结构
 * 2017/07/03 V2.9: StrMsg_HUITP_MSGID_sui_bfsc_command_req update
 *                  HUITP_SUI_IE长度改为233，为软件下载增加equEntry字段
 *                  为云控锁CCL增加gen_picid字段，为了方便后台存储图像文件以及遍连
 *					为了倾倒传感器的数据传输，增加新字段
 * 2017/8/15 V2.10: 去掉INVENTORY_xxx中有关desc的字段
 *                  增加标签管理消息
 * 2017/8/28 V2.11: 改善YCJK的字段IE内容体
 *                  增加TEST_COMMAND消息过程
 *                  将产品CODE全部改为十进制格式
 * 2017/10/8 V2.12: ZSC修改完善
 *                  为复旦医疗增加数据格式
 * 2017/10/16 V3.0: BFDF/BFHS新建消息
 * 2017/10/16 V3.1: FDWQ消息更新
 * 2017/11/24 V3.2: BFHS消息更新
 * 2017/11/24 V3.3: 合并公共消息, 以及BFDF消息
 * 2017/11/29 V3.4: 增加HUITP-JSON结构
 * 2018/02/20 V3.5: BFDF, ADD CALIBRATION REQ/RESP 
 * 2018/4/17  V3.6: BFSC, 增加分堆、校准、称重数量带符号等功能
 * 
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUITP公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define HUITP_USE_IN_HCU	1
#define HUITP_USE_IN_IHU	2
#define HUITP_USE_SET		HUITP_USE_IN_IHU

/*
 *
 *考虑到TCPIP中底层最长1500B的情形，这里先如此定义，不要超过这个限制，所以16进制的缓冲区最好不要超过500字节
 *注意后台的SwPkgBody_Buf数据结构，要以HUITP_IE_BUF_MAX_LEN长度为准，目前后台设置的长度
 *HCU -> 304=(344-40), IHU -> 94=(134-40)
 *
*/
#if (HUITP_USE_SET == HUITP_USE_IN_HCU)
	#define HUITP_MSG_BUF_WITH_HEAD_MAX_LEN 			344  //这个最大长度＝(1000-300)/2=350
#elif (HUITP_USE_SET == HUITP_USE_IN_IHU)
	#define HUITP_MSG_BUF_WITH_HEAD_MAX_LEN 			124  //这个主要受内部消息长度限制，(500-300)/2 = 100，所以100个有效长度就最大了
#else
	#error Error set HUITP_USE_SET!
#endif

//其它长度定义部分
#define HUITP_MSG_BUF_BODY_ONLY_MAX_LEN 				HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - 4 //MSG头+长度域共4BYTE
#define HUITP_IE_BUF_OVERHEAD							40
#define HUITP_IE_BUF_MAX_LEN 							HUITP_MSG_BUF_WITH_HEAD_MAX_LEN - HUITP_IE_BUF_OVERHEAD //MSG头4B，BASE_REQ 5B，IE头4B，为了安全，先减去30。在具体的消息定义中，如果独立IE较多，这个需要再检查
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN 			300  //人为定义，目前不包括具体内容在内的纯格式部分，已经达到210BYTE长度
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN 			200
#define HUITP_MSG_HUIXML_HEAD_IN_CHAR_VARIABLE_LEN 		HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN - HUITP_MSG_HUIXML_HEAD_IN_CHAR_FIX_LEN
//<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>
#define HUITP_MSG_HUIXML_TOTAL_BUF_IN_CHAR_MAX_LEN 		HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN + 2 * HUITP_MSG_BUF_WITH_HEAD_MAX_LEN + 1 //CHAR方式，用于INTERNET传输，留下一个做为尾部缓冲
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
#define HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID 				7
#define HUITP_MSG_HUIXML_MSGTYPE_COMMON_STRING 			"hcu_huitp"  //use common string for the moment
#define HUITP_MSG_HUIXML_MSGTYPE_HUIJSON_ID 			8
#define HUITP_MSG_HUIXML_MSGTYPE_HUIJSON_STRING 		"huitp_json"

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
	HUITP_MSGID_uni_bfdf_comb_scale_data_req         = 0x3B05,
	HUITP_MSGID_uni_bfdf_comb_scale_data_resp        = 0x3B85,
	HUITP_MSGID_uni_bfdf_comb_scale_data_report      = 0x3B86,
	HUITP_MSGID_uni_bfdf_comb_scale_data_confirm     = 0x3B06,
	HUITP_MSGID_uni_bfdf_comb_scale_event_report     = 0x3B87,
	HUITP_MSGID_uni_bfdf_comb_scale_event_confirm    = 0x3B07,
	HUITP_MSGID_uni_bfdf_comb_scale_ctrl_req         = 0x3B08,
	HUITP_MSGID_uni_bfdf_comb_scale_ctrl_resp        = 0x3B88,
	HUITP_MSGID_uni_bfdf_statistic_report            = 0x3B89,
	HUITP_MSGID_uni_bfdf_statistic_confirm           = 0x3B09,
	HUITP_MSGID_uni_bfhs_comb_scale_data_req         = 0x3B0A,
	HUITP_MSGID_uni_bfhs_comb_scale_data_resp        = 0x3B8A,
	HUITP_MSGID_uni_bfhs_comb_scale_data_report      = 0x3B8B,
	HUITP_MSGID_uni_bfhs_comb_scale_data_confirm     = 0x3B0B,
	HUITP_MSGID_uni_bfhs_comb_scale_event_report     = 0x3B8C,
	HUITP_MSGID_uni_bfhs_comb_scale_event_confirm    = 0x3B0C,
	HUITP_MSGID_uni_bfhs_comb_scale_ctrl_req         = 0x3B0D,
	HUITP_MSGID_uni_bfhs_comb_scale_ctrl_resp        = 0x3B8D,
	HUITP_MSGID_uni_bfhs_statistic_report            = 0x3B8E,
	HUITP_MSGID_uni_bfhs_statistic_confirm           = 0x3B0E,

	//HCU-IHU BFSC SUI新增内容
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
  // MYC: CALIBRATION REQUEST/RESPONSE
	HUITP_MSGID_sui_bfsc_calibration_req             = 0x3B1B, 
	HUITP_MSGID_sui_bfsc_calibration_resp            = 0x3B9B, 

	//心跳过程
	HUITP_MSGID_sui_bfsc_heart_beat_report           = 0x3BA0,
	HUITP_MSGID_sui_bfsc_heart_beat_confirm          = 0x3B20,
  

	//HCU-IHU BFDF SUI新增内容
	//配置过程
	HUITP_MSGID_sui_bfdf_set_config_req              = 0x3B30,
	HUITP_MSGID_sui_bfdf_set_config_resp             = 0x3BB0,
	//重量汇报过程
	HUITP_MSGID_sui_bfdf_new_ws_event                = 0x3BB1,
	//推杆及组合出料过程
	HUITP_MSGID_sui_bfdf_ws_comb_out_req             = 0x3B32,
	HUITP_MSGID_sui_bfdf_ws_comb_out_resp            = 0x3BB2,
	//篮筐清零报告
	HUITP_MSGID_sui_bfdf_basket_clean_ind            = 0x3BB3,
	
	// MYC: INFRA IND
	HUITP_MSGID_sui_bfdf_infra_ind                   = 0x3BB4,
  
  // MYC: CALIBRATION REQUEST/RESPONSE
	HUITP_MSGID_sui_bfdf_calibration_req             = 0x3B36, 
	HUITP_MSGID_sui_bfdf_calibration_resp            = 0x3BB6, 

	//HCU-IHU BFHS SUI新增内容
	//配置过程
	HUITP_MSGID_sui_bfhs_set_config_req              = 0x3B40,
	HUITP_MSGID_sui_bfhs_set_config_resp             = 0x3BC0,
	//重量汇报过程
	HUITP_MSGID_sui_bfhs_new_ws_event                = 0x3BC1,
	//校准过程
	HUITP_MSGID_sui_bfhs_calibration_zero_req        = 0x3B42,
	HUITP_MSGID_sui_bfhs_calibration_zero_resp       = 0x3BC2,
	HUITP_MSGID_sui_bfhs_calibration_full_req        = 0x3B43,
	HUITP_MSGID_sui_bfhs_calibration_full_resp       = 0x3BC3,
	HUITP_MSGID_sui_bfhs_dyn_calibration_zero_req    = 0x3B44,
	HUITP_MSGID_sui_bfhs_dyn_calibration_zero_resp   = 0x3BC4,
	HUITP_MSGID_sui_bfhs_dyn_calibration_full_req    = 0x3B45,
	HUITP_MSGID_sui_bfhs_dyn_calibration_full_resp   = 0x3BC5,

	//公共消息过程
	//传感器测试过程
	HUITP_MSGID_sui_com_test_command_req             = 0x3B70,
	HUITP_MSGID_sui_com_test_command_resp            = 0x3BF0,
	//心跳过程
	HUITP_MSGID_sui_com_heart_beat_report            = 0x3BF1,
	HUITP_MSGID_sui_com_heart_beat_confirm           = 0x3B71,
	//上电过程
	HUITP_MSGID_sui_com_startup_ind                  = 0x3BF2,
	//差错过程
	HUITP_MSGID_sui_com_fault_ind                    = 0x3BF3,
	//暂停过程
	HUITP_MSGID_sui_com_suspend_req                 = 0x3B74,
	HUITP_MSGID_sui_com_suspend_resp                = 0x3BF4,
	HUITP_MSGID_sui_com_resume_req                  = 0x3B75,
	HUITP_MSGID_sui_com_resume_resp                 = 0x3BF5,
	//STOP
	HUITP_MSGID_sui_com_stop_req                  	= 0x3B76,
	HUITP_MSGID_sui_com_stop_resp                 	= 0x3BF6,

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
	HUITP_MSGID_uni_ccl_state_pic_report             = 0x4E82,
	HUITP_MSGID_uni_ccl_state_pic_confirm        	 = 0x4E02,
	HUITP_MSGID_uni_ccl_state_max,

    //复旦项目
	HUITP_MSGID_uni_fdwq_min                         = 0x4F00,
	HUITP_MSGID_uni_fdwq_data_req                    = 0x4F00,
	HUITP_MSGID_uni_fdwq_data_resp                   = 0x4F80,
	HUITP_MSGID_uni_fdwq_data_report                 = 0x4F81,
	HUITP_MSGID_uni_fdwq_data_confirm                = 0x4F01,
	HUITP_MSGID_uni_fdwq_profile_report              = 0x4F82,
	HUITP_MSGID_uni_fdwq_profile_confirm             = 0x4F02,
	HUITP_MSGID_uni_fdwq_max,

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

	 //地震
	HUITP_JSON_MSGID_uni_earthquake_min              = 0x5C00,
	HUITP_JSON_MSGID_uni_earthquake_ctrl_req         = 0x5C00,
	HUITP_JSON_MSGID_uni_earthquake_ctrl_resp        = 0x5C80,
	HUITP_JSON_MSGID_uni_earthquake_data_report      = 0x5C81,
	HUITP_JSON_MSGID_uni_earthquake_data_confirm     = 0x5C01,
	HUITP_JSON_MSGID_uni_heart_beat_report           = 0x5CFF,
	HUITP_JSON_MSGID_uni_heart_beat_confirm     	 = 0x5C7F,
	HUITP_JSON_MSGID_uni_earthquake_max,

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

	//标签管理
	HUITP_MSGID_uni_equlable_min                     = 0xA200,
	HUITP_MSGID_uni_equlable_apply_req               = 0xA200,
	HUITP_MSGID_uni_equlable_apply_resp              = 0xA280,
	HUITP_MSGID_uni_equlable_apply_report            = 0xA281,
	HUITP_MSGID_uni_equlable_apply_confirm           = 0xA201,
	HUITP_MSGID_uni_equlable_inservice_req           = 0xA202,
	HUITP_MSGID_uni_equlable_inservice_resp          = 0xA282,
	HUITP_MSGID_uni_equlable_inservice_report        = 0xA283,
	HUITP_MSGID_uni_equlable_inservice_confirm       = 0xA203,
	HUITP_MSGID_uni_equlable_max,

	//HCU-IHU SUI新增内容 MYC 2017/09/10
	HUITP_MSGID_sui_flash_raw_cmd_req                = 0xA290,
	HUITP_MSGID_sui_flash_raw_cmd_rsp                = 0xA210,
	HUITP_MSGID_sui_flash_raw_cmd_max,

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

	//测试命令
	HUITP_MSGID_uni_test_command_min                 = 0xF700,
	HUITP_MSGID_uni_test_command_req                 = 0xF700,
	HUITP_MSGID_uni_test_command_resp                = 0xF780,
	HUITP_MSGID_uni_test_command_report              = 0xF781,
	HUITP_MSGID_uni_test_command_confirm             = 0xF701,
	HUITP_MSGID_uni_test_command_max,

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
	HUITP_IEID_uni_hsmmp_motive                     = 0x2C01,
	HUITP_IEID_uni_hsmmp_pos_status                 = 0x2C02,
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
	HUITP_IEID_uni_picture_ind                    = 0x2F00,
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
	HUITP_IEID_uni_ccl_fall_value                   = 0x4B01,
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
	HUITP_IEID_uni_ccl_gen_picid                    = 0x4E04,
	HUITP_IEID_uni_ccl_state_max,

    //复旦项目
	HUITP_IEID_uni_fdwq_min                         = 0x4F00,
	HUITP_IEID_uni_fdwq_sports_wrist_data           = 0x4F00,
	HUITP_IEID_uni_fdwq_sports_profile_simple_data  = 0x4F01,
	HUITP_IEID_uni_fdwq_sports_profile_detail_data  = 0x4F02,
	HUITP_IEID_uni_fdwq_max,

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
	HUITP_IEID_uni_inventory_max,

  //软件版本体
	HUITP_IEID_uni_sw_package_min                   = 0xA100,	
	HUITP_IEID_uni_sw_package_body                  = 0xA100, 
	HUITP_IEID_uni_sw_package_max,

	//标签管理
	HUITP_IEID_uni_equlable_min                   	= 0xA200,
	HUITP_IEID_uni_equlable_apply_user_info       	= 0xA200,
	HUITP_IEID_uni_equlable_apply_allocation       	= 0xA201,
	HUITP_IEID_uni_equlable_inservice_info       	= 0xA202,
	HUITP_IEID_uni_equlable_max,

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

	//测试命令
	HUITP_IEID_uni_test_command_min                 = 0xF700,
	HUITP_IEID_uni_test_command_value               = 0xF700,
	HUITP_IEID_uni_test_command_max,

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
	UINT16 hwType;
	UINT16 hwPem;
	UINT16 swRelId;
	UINT16 verId;
	UINT8  upgradeFlag;
	UINT8  equEntry;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
}StrIe_HUITP_IEID_uni_com_segment_t;
//equEntry定义
#define HUITP_IEID_UNI_EQU_ENTRY_NONE 		0
#define HUITP_IEID_UNI_EQU_ENTRY_HCU_SW 	1
#define HUITP_IEID_UNI_EQU_ENTRY_HCU_DB 	2
#define HUITP_IEID_UNI_EQU_ENTRY_IHU 		3
#define HUITP_IEID_SUI_EQU_ENTRY_INVALID 0xFF

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
	UINT32 timeStamp;
	UINT8  alarmFlag;
}StrIe_HUITP_IEID_uni_hsmmp_value_t;

//HUITP_IEID_uni_hsmmp_motive                     = 0x2C01,
typedef struct StrIe_HUITP_IEID_uni_hsmmp_motive
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  motive;
	UINT32 value;
}StrIe_HUITP_IEID_uni_hsmmp_motive_t;
#define HUITP_IEID_UNI_HSMMP_MOTIVE_NULL                   0
#define HUITP_IEID_UNI_HSMMP_MOTIVE_MOVEUP                 1
#define HUITP_IEID_UNI_HSMMP_MOTIVE_MOVEDOWN               2
#define HUITP_IEID_UNI_HSMMP_MOTIVE_MOVELEFT               3
#define HUITP_IEID_UNI_HSMMP_MOTIVE_MOVERIGHT              4
#define HUITP_IEID_UNI_HSMMP_MOTIVE_ZOOMIN                 5
#define HUITP_IEID_UNI_HSMMP_MOTIVE_ZOOMOUT                6
#define HUITP_IEID_UNI_HSMMP_MOTIVE_INVALID                0xFF

//HUITP_IEID_uni_hsmmp_pos_status                 = 0x2C02,
typedef struct StrIe_HUITP_IEID_uni_hsmmp_pos_status
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	INT32 xPos;
	INT32 yPos;
	INT32 zoomPos;
}StrIe_HUITP_IEID_uni_hsmmp_pos_status_t;

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
//HUITP_IEID_uni_picture_ind                    = 0x2F00,
#define HUITP_IEID_UNI_PICTURE_NAME_MAX_LEN    	 100
typedef struct StrIe_HUITP_IEID_uni_picture_ind
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  flag;
	UINT32 eventId;
	char   picName[HUITP_IEID_UNI_PICTURE_NAME_MAX_LEN];
}StrIe_HUITP_IEID_uni_picture_ind_t;
#define HUITP_IEID_UNI_PICTURE_IND_FLAG_NULL                   0
#define HUITP_IEID_UNI_PICTURE_IND_FLAG_REMOTE_CURL_TRIGGER    1  //指示远程拍照
#define HUITP_IEID_UNI_PICTURE_IND_FLAG_NOT_PREPARED           2  //本地摄像头未准备好
#define HUITP_IEID_UNI_PICTURE_IND_FLAG_SEND_DATA_ONLY         3  //本地拍摄完成，将准备通过数据传送到后台
#define HUITP_IEID_UNI_PICTURE_IND_FLAG_INVALID                0xFF

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
	INT32  tempValue;
	INT32  humidValue;
	INT32  winddirValue;
	INT32  windspdValue;
	INT32  noiseValue;
	INT32  pm1d0Value;
	INT32  pm2d5Value;
	INT32  pm10Value;
	INT32  tspValue;
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
//HUITP_IEID_uni_ccl_lock_min                     = 0x4D00,  
//HUITP_IEID_uni_ccl_lock_state                   = 0x4D00,
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

//HUITP_IEID_uni_ccl_lock_auth_req                = 0x4D01,
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

//HUITP_IEID_uni_ccl_lock_auth_resp               = 0x4D02,
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

//HUITP_IEID_uni_ccl_fall_value                   = 0x4B01,
typedef struct StrIe_HUITP_IEID_uni_ccl_fall_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 fallValue;
}StrIe_HUITP_IEID_uni_ccl_fall_value_t;


//HUITP_IEID_uni_ccl_fall_max,

//云控锁-状态聚合
//HUITP_IEID_uni_ccl_state_min                    = 0x4E00, 
//HUITP_IEID_uni_ccl_general_value1               = 0x4E00,
typedef struct StrIe_HUITP_IEID_uni_ccl_general_value1
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 generalValue1;
}StrIe_HUITP_IEID_uni_ccl_general_value1_t;

//HUITP_IEID_uni_ccl_general_value2               = 0x4E01,
typedef struct StrIe_HUITP_IEID_uni_ccl_general_value2
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 generalValue2;
}StrIe_HUITP_IEID_uni_ccl_general_value2_t;

//HUITP_IEID_uni_ccl_dcmi_value                  = 0x4E02,
typedef struct StrIe_HUITP_IEID_uni_ccl_dcmi_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  dataFormat;
	UINT16 dcmiValue;
}StrIe_HUITP_IEID_uni_ccl_dcmi_value_t;

//HUITP_IEID_uni_ccl_report_type                 = 0x4E03,
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

///HUITP_IEID_uni_ccl_gen_picid                    = 0x4E04,
#define HUITP_IEID_UNI_CCL_GEN_PIC_ID_LEN_MAX		32
typedef struct StrIe_HUITP_IEID_uni_ccl_gen_picid
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  picid[HUITP_IEID_UNI_CCL_GEN_PIC_ID_LEN_MAX];
}StrIe_HUITP_IEID_uni_ccl_gen_picid_t;
//HUITP_IEID_uni_ccl_state_max,

//复旦项目
//HUITP_IEID_uni_fdwq_min                    = 0x4F00,
//HUITP_IEID_uni_fdwq_sports_wrist_data      = 0x4F00,
typedef struct StrIe_HUITP_IEID_uni_fdwq_sports_wrist_data
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 equId;
	UINT32 rfId;
	UINT32 reportTime;
	UINT32 sampleTime;
	UINT8  dataFormat;
	UINT32 temp;
	UINT32 miles;
	UINT16 curHbRate;
	UINT16 hbRateMax;
	UINT16 hbRateMin;
	UINT32 hbRateAvg;
	UINT32 bloodPress;
	UINT32 sleepLvl;
	UINT32 airPress;
	UINT32 energyLvl;
	UINT32 waterDrink;
	UINT8  skinAttached;
}StrIe_HUITP_IEID_uni_fdwq_sports_wrist_data_t;

//HUITP_IEID_uni_fdwq_profile_simple_data         = 0x4F01,
typedef struct StrIe_HUITP_IEID_uni_fdwq_profile_simple_data
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 rfId;
}StrIe_HUITP_IEID_uni_fdwq_profile_simple_data_t;

//HUITP_IEID_uni_fdwq_profile_detail_data         = 0x4F02,
typedef struct StrIe_HUITP_IEID_uni_fdwq_profile_detail_data
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 rfId;
	char   name[20];
	UINT8  gender;
	UINT8  dataFormat;
	UINT32 high;
	UINT32 weight;
	UINT8  bloodType;
}StrIe_HUITP_IEID_uni_fdwq_profile_detail_data_t;
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_NULL 0
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_MALE 1
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_FEMALE 2
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_BOTH 3
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_UNDETERMIN 4
#define HUITP_IEID_UNI_FDWQ_PROFILE_GENDER_INVALID 0xFF
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_NULL 0
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_A 1
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_B 2
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_AB 3
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_O 4
#define HUITP_IEID_UNI_FDWQ_PROFILE_BLOOD_TYPE_INVALID 0xFF
//HUITP_IEID_uni_fdwq_max,

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
typedef struct StrIe_HUITP_IEID_uni_inventory_element
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 dbVer;
	UINT16 	swCheckSum;
	UINT32  swTotalLen;
	UINT16 	dbCheckSum;
	UINT32  dbTotalLen;
	UINT8  upgradeFlag;
	UINT8  equEntry;
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_inventory_element_t;
//UpgradeFlag定义
#define HUITP_IEID_UNI_FW_UPGRADE_NONE 0
#define HUITP_IEID_UNI_FW_UPGRADE_NO 1
#define HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE 2
#define HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL 3
#define HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH 4
#define HUITP_IEID_UNI_FW_UPGRADE_YES_INVALID 0xFF
#define HUITP_IEID_SUI_FW_UPGRADE_NONE 0
#define HUITP_IEID_SUI_FW_UPGRADE_NO 1
#define HUITP_IEID_SUI_FW_UPGRADE_YES_STABLE 2
#define HUITP_IEID_SUI_FW_UPGRADE_YES_TRIAL 3
#define HUITP_IEID_SUI_FW_UPGRADE_YES_PATCH 4
#define HUITP_IEID_SUI_FW_UPGRADE_YES_INVALID 0xFF
//HW_TYPE的高字节将表示HCU、IHU等不同类型
//HW_TYPE的低字节将表示产品的具体型号
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G1 		(1)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G2 		(2)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G3 		(3)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G4 		(4)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G5 		(5)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G6 		(6)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G7 		(7)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_G8 		(8)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E1 		(17)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E2 		(18)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E3 		(19)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDCAT_E4 		(20)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G1_AQYC_335D_01 	(1001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2001 	(2001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002 	(2002)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003 	(2003)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004 	(2004)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2005 	(2005)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006 	(2006)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2007 	(2007)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008 	(2008)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2009 	(2009)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_YCNOISE_2080 	(2080)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_YCPM25_2081 	(2081)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100 	(2100)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2101 	(2101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2102 	(2102)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2103 	(2103)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2200 	(2200)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2201 	(2201)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2202 	(2202)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2203 	(2203)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_SCY_WATER_01 	(3001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_CB_01  	(3101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G3_PLCCB_SB_01  	(3201)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G4_GQYB_01 			(4001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5001 		(5001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5002 		(5002)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5003 		(5003)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5004 		(5004)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5100 		(5100)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5101 		(5101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5102 		(5102)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5103 		(5103)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5104 		(5104)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5105		(5105)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5106 		(5106
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5107 		(5107)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5108 		(5108)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5109 		(5109)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5201 		(5201)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G5_FHYS_5202 		(5202)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G6_ZNLD_01 			(6001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_RFID_MS01 		(7001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G7_OTDR_01 			(7101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8001 		(8001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8002 		(8002)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8003		(8003)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFSC_8004 		(8004)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFDF_8101 		(8101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFDF_8102 		(8102)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFHS_8201 		(8201)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G8_BFHS_8202 		(8202)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G9_FDWQ_9000 		(9000)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G9_FDYP_9100 		(9100)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G9_FDJY_9200 		(9200)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G10_SCYWD_10000 		(10000)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G10_LCTBATH_10001 		(10001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G10_LCXGL_10002 		(10002)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_58_01 		(17001)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E1_EMC_68_01 		(17101)
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E2_HJJK_01 			(18001)  //手持式环境监控
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E3_DZGP_01 			(19001)  //电子工牌
#define HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_E4_ZNSH_01 			(20001)  //智能手环
//用于指示单个硬件具体的PEM信息，或者小版本的演进修改。
//如果没有独特信息，可采用0000/FFFF来表示无效不用。


//HUITP_IEID_uni_inventory_max,

//软件版本体
//HUITP_IEID_uni_sw_package_min                   = 0xA100,	
//HUITP_IEID_uni_sw_package_body                  = 0xA100,
//HCU内部设置为1000BYTE，这是尽力放大以提高效率
//IHU内部消息长度设置为500BYTE，这里想办法设置更大，无以为继
#if (HUITP_USE_SET == HUITP_USE_IN_HCU)
	#define HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN ((880 < HUITP_IE_BUF_MAX_LEN)? 880: HUITP_IE_BUF_MAX_LEN)
#elif (HUITP_USE_SET == HUITP_USE_IN_IHU)
	#define HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN ((450 < HUITP_IE_BUF_MAX_LEN)? 450: HUITP_IE_BUF_MAX_LEN)
#else
	#error Error set HUITP_USE_SET!
#endif
typedef struct StrIe_HUITP_IEID_uni_sw_package_body
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN];
}StrIe_HUITP_IEID_uni_sw_package_body_t;

#define HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN 233

//HUITP_IEID_uni_sw_package_max,

//标签管理
//HUITP_IEID_uni_equlable_min                   	= 0xA200,
//HUITP_IEID_uni_equlable_apply_user_info       	= 0xA200,
typedef struct StrIe_HUITP_IEID_uni_equlable_apply_user_info
{
	UINT16 ieId;
	UINT16 ieLen;
	char   facCode[20];  //比如FAC#123
	char   pdCode[5];    //比如G201
	char   pjCode[5];    //比如AQYC
	char   userCode[3];  //比如SH或TMP或者TUK
	char   uAccount[20];
	char   uPsd[20];
	UINT8  productType;  //HCU, IHU
	UINT8  formalFlag;   //Y, N
	UINT16 applyNbr;     //比如20，最小1，最大99
}StrIe_HUITP_IEID_uni_equlable_apply_user_info_t;
#define HUITP_IEID_UNI_EQULABLE_APPLY_USER_INFO_NONE 0
#define HUITP_IEID_UNI_EQULABLE_APPLY_USER_INFO_HCU 1
#define HUITP_IEID_UNI_EQULABLE_APPLY_USER_INFO_IHU 2
#define HUITP_IEID_SUI_EQULABLE_APPLY_USER_INFO_INVALID 0xFF

//HUITP_IEID_uni_equlable_apply_allocation       	= 0xA201,
typedef struct StrIe_HUITP_IEID_uni_equlable_apply_allocation
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT8  allocateRes;  //TRUE/FALSE
	UINT16 allocateNbr;     //比如20，最小1，最大99
	char   lableStart[20];
	char   lableEnd[20];
}StrIe_HUITP_IEID_uni_equlable_apply_allocation_t;

//HUITP_IEID_uni_equlable_inservice_info       	= 0xA202,
typedef struct StrIe_HUITP_IEID_uni_equlable_inservice_info
{
	UINT16 ieId;
	UINT16 ieLen;
	char   lable[20];  //IHU_G801_BFSC_TUK01
	UINT8  startStopFlag;
}StrIe_HUITP_IEID_uni_equlable_inservice_info_t;
#define HUITP_IEID_UNI_EQULABLE_INSERVICE_INFO_NONE 0
#define HUITP_IEID_UNI_EQULABLE_INSERVICE_INFO_START 1
#define HUITP_IEID_UNI_EQULABLE_INSERVICE_INFO_STOP 2
#define HUITP_IEID_SUI_EQULABLE_INSERVICE_INFO_INVALID 0xFF

//HUITP_IEID_uni_equlable_max,

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
	UINT32 timeStamp;
}StrIe_HUITP_IEID_uni_alarm_info_element_t;

#define HUITP_IEID_UNI_ALARM_TYPE_NONE           0
#define HUITP_IEID_UNI_ALARM_TYPE_SENSOR         1
#define HUITP_IEID_UNI_ALARM_TYPE_NETWORK        2
#define HUITP_IEID_UNI_ALARM_TYPE_TSP_VALUE      3
#define HUITP_IEID_UNI_ALARM_TYPE_NOISE_VALUE    4
#define HUITP_IEID_UNI_ALARM_TYPE_INVALID        0xFF

#define HUITP_IEID_UNI_ALARM_CONTENT_NONE                            0
#define HUITP_IEID_UNI_ALARM_CONTENT_PM25_NO_CONNECT                 1
#define HUITP_IEID_UNI_ALARM_CONTENT_TEMP_NO_CONNECT                 2
#define HUITP_IEID_UNI_ALARM_CONTENT_HUMID_NO_CONNECT                3
#define HUITP_IEID_UNI_ALARM_CONTENT_WINDDIR_NO_CONNECT              4
#define HUITP_IEID_UNI_ALARM_CONTENT_WINDSPD_NO_CONNECT              5
#define HUITP_IEID_UNI_ALARM_CONTENT_NOISE_NO_CONNECT                6
#define HUITP_IEID_UNI_ALARM_CONTENT_VIDEO_NO_CONNECT                7
#define HUITP_IEID_UNI_ALARM_CONTENT_TSP_VALUE_EXCEED_THRESHLOD      8
#define HUITP_IEID_UNI_ALARM_CONTENT_NOISE_VALUE_EXCEED_THRESHLOD    9
#define HUITP_IEID_UNI_ALARM_CONTENT_INVALID                		 0xFF

#define HUITP_IEID_UNI_ALARM_SEVERITY_NONE                   0
#define HUITP_IEID_UNI_ALARM_SEVERITY_HIGH                   1
#define HUITP_IEID_UNI_ALARM_SEVERITY_MEDIUM                 2
#define HUITP_IEID_UNI_ALARM_SEVERITY_MINOR                  3
#define HUITP_IEID_UNI_ALARM_SEVERITY_INVALID                0xFF

#define HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF                  0
#define HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON                   1

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

//测试命令
//HUITP_IEID_uni_test_command_min                 = 0xF700,
//HUITP_IEID_uni_test_command_value               = 0xF700,
#define HUITP_IEID_UNI_TEST_CMD_DESC_MAX_LEN	50
typedef struct StrIe_HUITP_IEID_uni_test_command_value
{
	UINT16 ieId;
	UINT16 ieLen;
	UINT32 testCmdId;
	UINT32 testCmdPar1;
	UINT32 testCmdPar2;
	UINT32 testCmdPar3;
	UINT32 testCmdPar4;
	char   testCmdDesc[HUITP_IEID_UNI_TEST_CMD_DESC_MAX_LEN];
}StrIe_HUITP_IEID_uni_test_command_value_t;
#define HUITP_IEID_UNI_TEST_CMD_ID_NULL			0
#define HUITP_IEID_UNI_TEST_CMD_ID_PING			1
#define HUITP_IEID_UNI_TEST_CMD_ID_READ_ALL		2
#define HUITP_IEID_UNI_TEST_CMD_ID_READ_LABLE	3
#define HUITP_IEID_UNI_TEST_CMD_ID_INVALID		0xFFFFFFFF


//HUITP_IEID_uni_test_command_max,


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
	StrIe_HUITP_IEID_uni_hsmmp_pos_status_t respPosStatus;
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
	StrIe_HUITP_IEID_uni_hsmmp_motive_t reqMotive;
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
	StrIe_HUITP_IEID_uni_hsmmp_motive_t respMotive;
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
	StrIe_HUITP_IEID_uni_picture_ind_t respInd;
}StrMsg_HUITP_MSGID_uni_picture_data_resp_t;
 
//HUITP_MSGID_uni_picture_data_report                  = 0x2F81,
typedef struct StrMsg_HUITP_MSGID_uni_picture_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_picture_ind_t reportInd;
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

//HUITP_MSGID_uni_bfdf_comb_scale_data_req         = 0x3B05,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_req_t dataReq;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_req_t;

//HUITP_MSGID_uni_bfdf_comb_scale_data_resp        = 0x3B85,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_value_t respValue;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_resp_t;

//HUITP_MSGID_uni_bfdf_comb_scale_data_report      = 0x3B86,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_report_t;

//HUITP_MSGID_uni_bfdf_comb_scale_data_confirm     = 0x3B06,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_data_confirm_t;

//HUITP_MSGID_uni_bfdf_comb_scale_event_report     = 0x3B87,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_event_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_event_t reportEvent;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_event_report_t;

//HUITP_MSGID_uni_bfdf_comb_scale_event_confirm    = 0x3B07,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_event_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_event_confirm_t;

//HUITP_MSGID_uni_bfdf_comb_scale_ctrl_req         = 0x3B08,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdReq;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgReq;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_ctrl_req_t;

//HUITP_MSGID_uni_bfdf_comb_scale_ctrl_resp        = 0x3B88,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdResp;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgResp;
}StrMsg_HUITP_MSGID_uni_bfdf_comb_scale_ctrl_resp_t;

//HUITP_MSGID_uni_bfdf_statistic_report            = 0x3B89,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_statistic_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_statistic_t staRep;
}StrMsg_HUITP_MSGID_uni_bfdf_statistic_report_t;

//HUITP_MSGID_uni_bfdf_statistic_confirm           = 0x3B09,
typedef struct StrMsg_HUITP_MSGID_uni_bfdf_statistic_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfdf_statistic_confirm_t;

//HUITP_MSGID_uni_bfhs_comb_scale_data_req         = 0x3B0A,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_req_t dataReq;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_req_t;

//HUITP_MSGID_uni_bfhs_comb_scale_data_resp        = 0x3B8A,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_value_t respValue;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_resp_t;

//HUITP_MSGID_uni_bfhs_comb_scale_data_report      = 0x3B8B,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_report_t;

//HUITP_MSGID_uni_bfhs_comb_scale_data_confirm     = 0x3B0B,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_data_confirm_t;

//HUITP_MSGID_uni_bfhs_comb_scale_event_report     = 0x3B8C,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_event_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_event_t reportEvent;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_event_report_t;

//HUITP_MSGID_uni_bfhs_comb_scale_event_confirm    = 0x3B0C,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_event_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_event_confirm_t;

//HUITP_MSGID_uni_bfhs_comb_scale_ctrl_req         = 0x3B0D,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_ctrl_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdReq;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgReq;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_ctrl_req_t;

//HUITP_MSGID_uni_bfhs_comb_scale_ctrl_resp        = 0x3B8D,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_ctrl_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_scale_weight_cmd_t cmdResp;
	StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t cfgResp;
}StrMsg_HUITP_MSGID_uni_bfhs_comb_scale_ctrl_resp_t;

//HUITP_MSGID_uni_bfhs_statistic_report            = 0x3B8E,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_statistic_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_scale_weight_statistic_t staRep;
}StrMsg_HUITP_MSGID_uni_bfhs_statistic_report_t;

//HUITP_MSGID_uni_bfhs_statistic_confirm           = 0x3B0E,
typedef struct StrMsg_HUITP_MSGID_uni_bfhs_statistic_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_bfhs_statistic_confirm_t;



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
#define 	WEIGHT_EVENT_ID_LOAD							(0)
#define 	WEIGHT_EVENT_ID_EMPTY							(1)
#define 	WEIGHT_EVENT_ID_PICKUP						(2)

typedef struct CombineType
{
	UINT32	WeightCombineType;
	UINT8   smallest_wmc_id;
	UINT8   spare1;
	UINT16  spare2;
	INT32	  ActionDelayMs;
  INT32   ActionArmOpenDurMs;
}CombineType_t;
#define HUITP_IEID_SUI_BFSC_COMINETYPE_NULL 			0
#define HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT 		1
#define HUITP_IEID_SUI_BFSC_COMINETYPE_DROP 			2
#define HUITP_IEID_SUI_BFSC_COMINETYPE_WARNING 		3
#define HUITP_IEID_SUI_BFSC_COMINETYPE_ERROR 			4
#define HUITP_IEID_SUI_BFSC_COMINETYPE_INVALID 		0xFF

typedef struct WeightIndication
{
	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
	INT32 average_weight;		//average value in the detect window  // <--- MUST
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
//待删除：不带固定头，容易重复，BFHS中重新定义了
#define CMDID_LED1_COMMNAD_ON							(1)
#define CMDID_LED1_COMMNAD_OFF							(2)
#define CMDID_LED1_COMMNAD_BINKING_HIGHSPEED			(3)
#define CMDID_LED1_COMMNAD_BINKING_LOWSPEED				(4)
#define CMDID_LED2_COMMNAD_ON							(5)
#define CMDID_LED2_COMMNAD_OFF							(6)
#define CMDID_LED2_COMMNAD_BINKING_HIGHSPEED			(7)
#define CMDID_LED2_COMMNAD_BINKING_LOWSPEED				(8)
#define CMDID_SENSOR_COMMAND_IGORE						(10) //MUSR BE 0
#define CMDID_SENSOR_COMMAND_WEITGH_READ				(11)
#define CMDID_SENSOR_COMMAND_CALIBRATION_ZERO			(12)
#define CMDID_SENSOR_COMMAND_CALIBRATION_FULL			(13)
#define CMDID_SENSOR_COMMAND_TARE_WEIGHT      			(14)
#define CMDID_MOTOR_COMMAND								(20)


//特殊命令过程（测试等过程）
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_command_req
{
	UINT16 msgid;
	UINT16 length;
	UINT32 cmdid;
	UINT32 cmdvalue;
}StrMsg_HUITP_MSGID_sui_bfsc_command_req_t;

//HUITP_MSGID_sui_bfsc_command_resp                = 0x3B98,
typedef struct StrMsg_HUITP_MSGID_sui_bfsc_command_resp
{
	UINT16	msgid;
	UINT16 length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WmcErrorCode_t result;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16 spare2;
	UINT32 cmdid;
	UINT32 cmdvalue1;
	UINT32 cmdvalue2;
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


//HUITP_MSGID_sui_bfsc_calibration_req             = 0x3B1B, 
#define BFSC_SENSOR_CALIBRATION_MODE_ZERO    1
#define BFSC_SENSOR_CALIBRATION_MODE_FULL    2
#define BFSC_SENSOR_CALIBRATION_MODE_READ    3
typedef struct StrHuiIe_bfsc_calibration_req //
{
    UINT8                    calibration_cmdvalue; /* 1 for ZERO, 2 for FULL */
    UINT8                    spare1;
    UINT8                    spare2;
    UINT8                    spare3;
    WeightSensorParamaters_t weight_sensor_param;
}StrHuiIe_bfsc_calibration_req_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfsc_calibration_req //
{
	UINT16           msgid;
	UINT16           length;
	StrHuiIe_bfsc_calibration_req_t cal_req;
}StrMsg_HUITP_MSGID_sui_bfsc_calibration_req_t;

//HUITP_MSGID_sui_bfsc_calibration_resp            = 0x3B9B,
typedef struct StrHuiIe_bfsc_calibration_resp //
{
    UINT8   calibration_cmdvalue;
    UINT8   calibration_result;  /* NOT USED FOR THE MOEMENT 2018/04/22 */
    UINT8   spare1;
    UINT8   spare2;
    WeightSensorParamaters_t weight_sensor_param;
}StrHuiIe_bfsc_calibration_resp_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfsc_calibration_resp //
{
	  UINT16           msgid;
	  UINT16           length;
  	UINT8            validFlag;  //是否执行成功
	  UINT8            spare1;
	  UINT16           errCode;
	  StrHuiIe_bfsc_calibration_resp_t cal_resp;
}StrMsg_HUITP_MSGID_sui_bfsc_calibration_resp_t;

//Common head defintion
typedef struct StrMsg_HUITP_MSGID_sui_common_msg_header
{
	UINT16 msgid;
	UINT16 length;
}StrMsg_HUITP_MSGID_sui_common_msg_header_t;

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

/* After add the new common error code, reserve the old one for BFSC */
typedef enum error_code
{
	//ERROR CODE ID
	ERROR_CODE_NO_ERROR = 0, //Starting point //兼容老旧系统
	//COMMON ERROR CODE
	ERROR_CODE_CALLING_ERROR,
	ERROR_CODE_INPUT_PARAMETER_KO,
	ERROR_CODE_WRONG_WMC_STATE,
    ERROR_CODE_SENSOR_NOT_CALIBRATED,
    ERROR_CODE_WRONG_CALIBRATION_SEQUENCE,
    ERROR_CODE_SENSOR_ADC_OVERLOADED,
    ERROR_CODE_FLASH_SAVING_KO,
    ERROR_CODE_MOTOR_FAILURE,
	ERROR_CODE_UNKNOWN,
	//SPECIFIC PART

	ERROR_CODE_MAX, //Ending point  //兼容老旧系统
}error_code_t; //end of IHU_INTER_TASK_MSG_ID


/* COMMON ERROR CODE */
typedef enum StrHuiIe_sui_com_error_code
{
	//ERROR CODE ID
	HUITP_IEID_SUI_COM_ERROR_CODE_STARING = 0, //Starting point
	HUITP_IEID_SUI_COM_ERROR_CODE_NO_ERROR = 0, //Starting point //兼容老旧系统
	//COMMON ERROR CODE
	HUITP_IEID_SUI_COM_ERROR_CODE_CALLING_ERROR,
	HUITP_IEID_SUI_COM_ERROR_CODE_INPUT_PARAMETER_KO,
	HUITP_IEID_SUI_COM_ERROR_CODE_WRONG_WMC_STATE,
    HUITP_IEID_SUI_COM_ERROR_CODE_SENSOR_NOT_CALIBRATED,
    HUITP_IEID_SUI_COM_ERROR_CODE_WRONG_CALIBRATION_SEQUENCE,
    HUITP_IEID_SUI_COM_ERROR_CODE_SENSOR_ADC_OVERLOADED,
    HUITP_IEID_SUI_COM_ERROR_CODE_FLASH_SAVING_KO,
    HUITP_IEID_SUI_COM_ERROR_CODE_MOTOR_FAILURE,
	HUITP_IEID_SUI_COM_ERROR_CODE_UNKNOWN,
	//SPECIFIC PART

	HUITP_IEID_SUI_COM_ERROR_CODE_MAX, //Ending point  //兼容老旧系统
}StrHuiIe_sui_com_error_code_t; //end of IHU_INTER_TASK_MSG_ID


//HCU-IHU BFHS SUI新增内容
//校准过程
//HUITP_MSGID_sui_bfhs_calibration_zero_req        = 0x3B42,
typedef struct StrHuiIe_WeightSensorBfhsCalibrationZeroReqParamaters
{
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
	UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;
    UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //location of decimal point for Preload Compensation, for example,6.25%, this value is 2.
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	//UINT8   WeightSensorFilterCoeff;  //NOT for GUI, object 0x2011, [0...255], default 10th LPF, calc cutoff freq according to this value
	UINT8   WeightSensorMeasurementRangeNo; //object 0x2040, Default is 0, set measurement range no(totally 3),which is displayed in 0x2041
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   spare1;
	UINT8   spare2;
}StrHuiIe_WeightSensorBfhsCalibrationZeroReqParamaters_t;
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_req
{
	UINT16 msgid;
	UINT16 length;
	StrHuiIe_WeightSensorBfhsCalibrationZeroReqParamaters_t weight_sensor_calibration_zero;
}StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_req_t;

//HUITP_MSGID_sui_bfhs_calibration_zero_resp       = 0x3BC2,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp
{
	UINT16  msgid;
	UINT16  length;
	UINT8   validFlag;  //是否执行成功
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t;

//HUITP_MSGID_sui_bfhs_calibration_full_req        = 0x3B43,
typedef struct StrHuiIe_WeightSensorBfhsCalibrationFullReqParamaters
{
	UINT32  WeightSensorAdjustingWeightGrams;      //object 0x2080, adjusting weight,first set this value, then combined with command 'C'
	UINT32  WeightSensorAdjustingTolerancePercent; //object0x2082, Current adjusting factor = 0.500000, adjusting tolerance = 1 %,The new factor must lie in the range 0.495000 ≤ Factornew ≤ 0.505000
}StrHuiIe_WeightSensorBfhsCalibrationFullReqParamaters_t;
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_req
{
	UINT16 msgid;
	UINT16 length;
	StrHuiIe_WeightSensorBfhsCalibrationFullReqParamaters_t weight_sensor_calibration_full;
}StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_req_t;

//HUITP_MSGID_sui_bfhs_calibration_full_resp       = 0x3BC3,
typedef struct StrHuiIe_WeightSensorBfhsCalibrationFullRespParamaters
{
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
    UINT32  WeightSensorCurrentZeroPointGrams; //object 0x2070, This value is displayed here as a weight. It contains information about how
																						 //far apart the zero point is from the lower limit of the A/D converter range
	UINT32  WeightSensorReferenceZeroPointGrams; /*object 0x2071, Zero setting can be performed for the Weigh Cell with the commands "T" or
																								"Z", but only if the new zero point lies within the permissible zero setting range.
																									Condition 1:
																										new zero point>=reference zero point - neg. zero setting range
																							  Condition 2:
																									new zero point <= reference zero point + pos. zero setting range */
	UINT32  WeightSensorNegativeZeroSettingRangeGrams; //object 0x2072;
	UINT32  WeightSensorPositiveZeroSettingRangeGrams; //object 0x2073;	
	UINT32  WeightSensorMeasurementRange; //object0x2041
	UINT32  WeightSensorScaleIntervalValue; //object0x2043
	UINT32  WeightSensorCalibrationValue; //object0x2044
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
  
	UINT32  WeightSensorAdjustingWeightGrams;      //object 0x2080, adjusting weight,first set this value, then combined with command 'C'
	UINT32  WeightSensorAdjustingFactor;      //object 0x2081, (Pnts ZeroPnt) AdjustingFactor Weight* 10^decimal point
	UINT32  WeightSensorAdjustingTolerancePercent; //object0x2082, Current adjusting factor = 0.500000, adjusting tolerance = 1 %,The new factor must lie in the range 0.495000 ≤ Factornew ≤ 0.505000	
	UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected	
	INT16  WeightSensorTemperatureInMagnetSystem; //object 0x2300, sub index1 format NF2
	//UINT16  TemperatureInMagnetSystemPlacesAfterDecimalPoint;//
	INT16  WeightSensorTemperatureAtMeasuringShunt; //object 0x2300, sub index2 format NF2
	//UINT16  TemperatureAtMeasuringShuntPlacesAfterDecimalPoint;//
	UINT16  WeightSensorSamplingFreqHz; //object 0x2049, sampling freq default is 1000Hz
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;

	UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //

	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT8   WeightSensorMeasurementRangeNo; //object0x2040
	UINT8   WeightSensorPlacesAfterDecimalPoint; //object0x2042
	UINT8   WeightSensorUintString[64]; //object 0x2045, The unit in which weight values are displayed.
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   WeightSensorCellAddress; //object 0x2098, node ID = cell address +48
	UINT8   WeightSensorTimeGrid;  //object 0x2222, send weight value in a fixed time grid.
	INT32   Weight; //format NF2;
    UINT8   spare2;
}StrHuiIe_WeightSensorBfhsCalibrationFullRespParamaters_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp
{
	UINT16 msgid;
	UINT16 length;
	UINT8  validFlag;  //是否执行成功
	UINT8  spare1;
	UINT16  errCode;
	StrHuiIe_WeightSensorBfhsCalibrationFullRespParamaters_t weight_sensor_calibration_full;
}StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t;

//HUITP_MSGID_sui_bfhs_set_config_req              = 0x3B41,
typedef struct StrHuiIe_WeightSensorBfhsParamaters
{
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
    UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT32	MaxAllowedWeight;				//称重物品的范围上限 NF2 format
	UINT32  MinAllowedWeight;        //称重物品的范围下限 NF2 format
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;
    UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //location of decimal point for Preload Compensation, for example,6.25%, this value is 2.
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT8   WeightSensorMeasurementRangeNo; //object 0x2040, Default is 0, set measurement range no(totally 3),which is displayed in 0x2041
	//UINT8   WeightSensorFilterCoeff;  //NOT for GUI, object 0x2011, [0...255], default 10th LPF, calc cutoff freq according to this value
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   WeightSensorTimeGrid;  //object 0x2222, send weight value in a fixed time grid.
	UINT8   WeightSensorAlgoSelect;  //weight algorithm select
	UINT32  WeightSensorReadStartMs;  //Weight sensor start sampling after infrared detector trigger
	UINT32  WeightSensorReadStopMs;   //Weight sensor stop sampling after infrared detector trigger
	UINT32  WeightSensorDynCaliCoeff; //NF5 format
	UINT32  WeightSensorTareWeight;			//NEW	
	UINT32  WeightSensorTargetThroughput;	//NEW	
	UINT32  WeightSensorAlgoAutoZeroSignal; //NEW
	UINT32	WeightSensorUpperLimit;
	UINT32	WeightSensorAlgoTu1Limit;  		//NEW	
	UINT32	WeightSensorAlgoTu2Limit;  		//NEW	
	UINT32	WeightSensorAlgoMaxTu1Ratio;  	//NEW, NF2, for example, 250 means 2.5%	
	UINT8	WeightSensorAlgoRejectOption;	//NEW
}StrHuiIe_WeightSensorBfhsParamaters_t;

#define HUITP_IEID_SUI_BFHS_ALGO_REJECTOR_OPTION_NULL  			0
#define HUITP_IEID_SUI_BFHS_ALGO_REJECTOR_OPTION_OVERWEIGHT  	1
#define HUITP_IEID_SUI_BFHS_ALGO_REJECTOR_OPTION_UNDERWEIGHT  	2
#define HUITP_IEID_SUI_BFHS_ALGO_REJECTOR_OPTION_BOTHSIDE  		3
#define HUITP_IEID_SUI_BFHS_ALGO_REJECTOR_OPTION_INVALID  		0xFF

typedef struct StrHuiIe_MotorControlBfhsParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}StrHuiIe_MotorControlBfhsParamaters_t;

typedef struct StrHuiIe_ArmControlBfhsParamaters
{
	UINT32	ArmRollingStartMs;						//how long do the arm rolling for start action
	UINT32	ArmRollingStopMs;							//how long do the arm rolling for stop action
	UINT32	ArmStartActionMs;					//The time delay of arm start action after infrared detector trigger
	UINT32	ArmRollingIntervalMs;				//If the arm is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	ArmFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	ArmFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}StrHuiIe_ArmControlBfhsParamaters_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfhs_set_config_req
{
	UINT16 	msgid;
	UINT16 	length;
	StrHuiIe_WeightSensorBfhsParamaters_t weight_sensor_param;
	StrHuiIe_MotorControlBfhsParamaters_t motor_control_param;
	StrHuiIe_ArmControlBfhsParamaters_t   arm_control_param;
}StrMsg_HUITP_MSGID_sui_bfhs_set_config_req_t;

//HUITP_MSGID_sui_bfhs_set_config_resp             = 0x3BC1,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   validFlag;
	UINT8  spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t;

//重量汇报过程
//HUITP_MSGID_sui_bfhs_new_ws_event                = 0x3BC4,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event
{
	UINT16 	msgid;
	UINT16 	length;
	INT32   Weight; //format NF2;
	UINT8   weight_state;  //1: normal; 2: overload; 3:underload;
	UINT8   spare1;
	UINT8   spare2;
	UINT8   spare3;
}StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t;
#define HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_NULL  				0
#define HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_NORMAL  			1
#define HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_OVERLOAD  			2
#define HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_UNDERLOAD  			3
#define HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_INVALID  			0xFF

//HUITP_MSGID_sui_bfhs_dyn_calibration_zero_req    = 0x3B44,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_zero_req
{
	UINT16 msgid;
	UINT16 length;
}StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_zero_req_t;

//HUITP_MSGID_sui_bfhs_dyn_calibration_zero_resp   = 0x3BC4,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_zero_resp
{
	UINT16  msgid;
	UINT16  length;
	UINT8   validFlag;  //是否执行成功
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_zero_resp_t;

//HUITP_MSGID_sui_bfhs_dyn_calibration_full_req    = 0x3B45,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_full_req
{
	UINT16 msgid;
	UINT16 length;
	StrHuiIe_WeightSensorBfhsParamaters_t weight_sensor_param;
    UINT32 adjustingWeight;
	UINT32 motorSpeed;
	UINT32 motorDirection;
}StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_full_req_t;

//HUITP_MSGID_sui_bfhs_dyn_calibration_full_resp   = 0x3BC5,
typedef struct StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_full_resp
{
	UINT16  msgid;
	UINT16  length;
	UINT8   validFlag;  //是否执行成功
	UINT8   iteration;
	UINT16  errCode;
	UINT32  DynCaliCoeff; //NF5 format
	INT32   Weight; //format NF2;
}StrMsg_HUITP_MSGID_sui_bfhs_dyn_calibration_full_resp_t;


//HCU-IHU BFDF SUI新增内容
#define     HUITP_IEID_SUI_BFDF_MAX_GLOBAL_AP_NUM				            (32)
#define     HUITP_IEID_SUI_BFDF_MAX_LOCAL_AP_NUM					          (8)

typedef struct StrHuiIe_BfdfErrorCode
{
	UINT16 error_code;
	UINT16 spare1;
}StrHuiIe_BfdfErrorCode_t;

//配置过程
//HUITP_MSGID_sui_bfdf_set_config_req              = 0x3B30,
/* FOR EXTERNAL INTERFACE AND INTERNAL VARIABLES  */
typedef struct StrHuiIe_BfdfWeightSensorParam
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//BFDF: 如果发现超过这个最大值，说明Sensor出错
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorMovingEverageSample;
}StrHuiIe_BfdfWeightSensorParam_t;

/* FOR EXTERNAL INTERFACE AND INTERNAL VARIABLES  */
typedef struct StrHuiIe_BfdfMotorCtrlParam
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}StrHuiIe_BfdfMotorControlParam_t;

/* FOR EXTERNAL INTERFACE AND INTERNAL VARIABLES  */
typedef struct StrHuiIe_BfdfActionCtrlParam
{
	UINT16  TWeightInd;                     /* After INFRA INT, Wait for how long to send WEIGHT_IND, unit is 10ms Tick */
	UINT16  T0bis;                          /* After INFRA INT, INFRA INT sent to Node 2 to 5, unit is 10ms Tick */
	UINT16  TA0;                            /* After INFRA INT, Deay to AP01 */
	UINT16  TActCmd;
	UINT16  TArmStart;
	UINT16  TArmStop;
	UINT16  TDoorCloseLightOn;
	UINT16  TApIntervalMin;
	UINT16  TApInterval[HUITP_IEID_SUI_BFDF_MAX_GLOBAL_AP_NUM];
	UINT16  TLocalAp[HUITP_IEID_SUI_BFDF_MAX_LOCAL_AP_NUM];
	UINT16  DelayNode1ToX;
	UINT16  DelayUpHcuAlgoDl;
}StrHuiIe_BfdfActionCtrlParam_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfdf_set_config_req
{
	UINT16                  msgid;
	UINT16                  length;
	StrHuiIe_BfdfWeightSensorParam_t bfdf_wsp;
	StrHuiIe_BfdfMotorControlParam_t bfdf_mcp_main;
	StrHuiIe_BfdfMotorControlParam_t bfdf_mcp_secondary;
	StrHuiIe_BfdfActionCtrlParam_t   bfdf_acp;
}StrMsg_HUITP_MSGID_sui_bfdf_set_config_req_t;

//HUITP_MSGID_sui_bfdf_set_config_resp             = 0x3BB0,
typedef struct StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp
{
	UINT16	msgid;
	UINT16  length;
	UINT8   validFlag;  //是否执行成功
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t;

//重量汇报过程
//HUITP_MSGID_sui_bfdf_new_ws_event                = 0x3BB1,
typedef struct StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event
{
	UINT16 	msgid;
	UINT16 	length;
	INT32   weight;
}StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t;

//推杆及组合出料过程
//HUITP_MSGID_sui_bfdf_ws_comb_out_req             = 0x3B32,
typedef struct StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_req
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   apHopperId;  //1-32 for Line1, 33-64 for line2
	UINT8   basketFullStatus;    //0-FALSE, 1-TRUE
	UINT8   bufferFullStatus;	   //0-FALSE, 1-TRUE
}StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_req_t;

//HUITP_MSGID_sui_bfdf_ws_comb_out_resp            = 0x3BB2,
typedef struct StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   apHopperId;  //1-32 for all lines
}StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t;

//篮筐清零报告
//HUITP_MSGID_sui_bfdf_basket_clean_ind            = 0x3BB3,
typedef struct StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   apHopperId;  //1-32 for all lines
}StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t;
	
//HUITP_MSGID_sui_bfdf_infra_ind                   = 0x3BB4, 
typedef struct StrHuiIe_bfdf_infra_ind //
{
	UINT32 infra_counters;
	UINT32 infra_int_last_interval_ms;
	UINT32 is_infra_int_too_close;
}StrHuiIe_bfdf_infra_ind_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfdf_infra_ind //
{
	UINT16           msgid;
	UINT16           length;
	StrHuiIe_bfdf_infra_ind_t bfdf_infra_ind;
}StrMsg_HUITP_MSGID_sui_bfdf_infra_ind_t;

//HUITP_MSGID_sui_bfdf_calibration_req             = 0x3B36, 
#define BFDF_SENSOR_CALIBRATION_MODE_ZERO    1
#define BFDF_SENSOR_CALIBRATION_MODE_FULL    2
typedef struct StrHuiIe_bfdf_calibration_req //
{
    UINT8            calibration_zero_or_full; /* 1 for ZERO, 2 for FULL */
    UINT8            calibration_iteration;    /* 8 for ZERO, 4 for FULL */ 
    UINT16           TWeightInd;
    UINT32           full_weight; /* in 0.01g */
    UINT32           motor_speed;
    UINT32           adc_sample_freq;
    UINT32           adc_gain;
    UINT32           noise_floor_filter_factor;   /* 0 - 100: 0.00 to 1.00, y = factor * x(n) + (1-factor) * x(n-1), 0 means disable */
    UINT32           max_allowed_weight; /* in 0.01g */
    UINT32           WeightSensorTailorValue;
}StrHuiIe_bfdf_calibration_req_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfdf_calibration_req //
{
	UINT16           msgid;
	UINT16           length;
	StrHuiIe_bfdf_calibration_req_t cal_req;
}StrMsg_HUITP_MSGID_sui_bfdf_calibration_req_t;

//HUITP_MSGID_sui_bfdf_calibration_resp            = 0x3BB6,
typedef struct StrHuiIe_bfdf_calibration_resp //
{
    UINT8   calibration_zero_or_full;
    UINT8   calibration_cur_iteration;
    UINT8   calibration_result;
    UINT8   spare2;
    UINT32  full_weight; /* in 0.01g */
    UINT32  noise_floor_period_10ms;
    UINT32  noise_floor_period_10ms_max;
    UINT32  weight_report_offset_wrt_infra;
    UINT32  noise_floor_sd_mean;
    UINT32  noise_floor_sd_max;
    UINT32  noise_floor_sd_min;
    UINT32  noise_floor_sd_sd;
    UINT32  noise_floor_mean_mean;
    UINT32  noise_floor_mean_max;
    UINT32  noise_floor_mean_min;
    UINT32  noise_floor_mean_sd;
    UINT32  full_offset_peak_wrt_infra[4];
    UINT32  full_coefficiency_average;            /* in 0.0001, MIN: 0.0001, MAX: 429496.7295 */
    UINT32  estimated_error_iteration[4];
    UINT32  estimated_error_max_possible;
    UINT32  estimated_error_average;
}StrHuiIe_bfdf_calibration_resp_t;

typedef struct StrMsg_HUITP_MSGID_sui_bfdf_calibration_resp //
{
	UINT16  msgid;
	UINT16  length;
	UINT8   validFlag;  //是否执行成功
	UINT8   spare1;
	UINT16  errCode;
	StrHuiIe_bfdf_calibration_resp_t cal_resp;
}StrMsg_HUITP_MSGID_sui_bfdf_calibration_resp_t;
/*
**	BFDF_ACTION_REQ
*/

#define     HUITP_IEID_SUI_BFDF_ACTION_ARM_DEFAULT                    (1)   /* Default means, open then close */
#define     HUITP_IEID_SUI_BFDF_ACTION_ARM_OPEN                       (2)
#define     HUITP_IEID_SUI_BFDF_ACTION_ARM_CLOSE                      (3)
#define     HUITP_IEID_SUI_BFDF_ACTION_ARM_OPEN_AUTO_CLOSE            (4)
#define     HUITP_IEID_SUI_BFDF_ACTION_ARM_CLOSE_AUTO_OPEN            (5)

#define     HUITP_IEID_SUI_BFDF_ACTION_DOOR_DEFAULT                   (2)   /* Default means, = close */
#define     HUITP_IEID_SUI_BFDF_ACTION_DOOR_OPEN                      (1)
#define     HUITP_IEID_SUI_BFDF_ACTION_DOOR_CLOSE                     (2)
#define     HUITP_IEID_SUI_BFDF_ACTION_DOOR_OPEN_AUTO_CLOSE           (3)
#define     HUITP_IEID_SUI_BFDF_ACTION_DOOR_CLOSE_AUTO_OPEN           (4)

#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_DEFAULT                  (1)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_ON                       (1)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_OFF                      (2)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_ON_AUTO_OFF              (3)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_OFF_AUTO_ON              (4)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_BLINK_HS_ON              (5)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_BLINK_HS_OFF             (6)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_BLINK_LS_ON              (7)
#define     HUITP_IEID_SUI_BFDF_ACTION_LIGHT_BLINK_LS_OFF             (8)

typedef struct StrHuiIe_bfdf_ap_action_single
{
	UINT8  ap_id;
	UINT8  spare;
	UINT8  arm_action;
	UINT8  arm_delay_tick;    /* active when xx_AUTO_xx command, uint is 10ms */
	UINT8  door_action;
	UINT8  door_delay_tick;   /* active when xx_AUTO_xx command, uint is 10ms */
	UINT8  light_action;
	UINT8  light_delay_tick;  /* active when xx_AUTO_xx command, uint is 10ms */
}StrHuiIe_bfdf_ap_action_single_t;

typedef struct StrHuiIe_BfdfBoardId
{
	UINT8 board_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare1;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare2;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare3;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
}StrHuiIe_BfdfBoardId_t;

typedef struct StrHuiIe_bfdf_ap_action_req
{
	StrHuiIe_BfdfBoardId_t board_id;
  StrHuiIe_bfdf_ap_action_single_t ap_action[HUITP_IEID_SUI_BFDF_MAX_LOCAL_AP_NUM];
}StrHuiIe_bfdf_ap_action_req_t;

typedef struct StrMsg_HUITP_COMM_sui_bfdf_ap_action_req
{
	UINT16 	msgid;
	UINT16 	length;
	StrHuiIe_bfdf_ap_action_req_t ap_act_cmd; 
}StrMsg_HUITP_COMM_sui_bfdf_ap_action_req_t;


/*
**	BFDF_COMMAND_REQ
*/
typedef struct StrHuiIe_bfdf_sensor_action_req
{
	  UINT32  cmdid;
	  UINT32  cmdvalue;	
}StrHuiIe_bfdf_sensor_action_req_t;

typedef struct StrHuiIe_bfdf_motor_action_req
{
    UINT32	speed;
    UINT32	direction;
	  UINT32  delay_tick;
}StrHuiIe_bfdf_motor_action_req_t;

typedef struct StrMsg_HUITP_COMM_sui_bfdf_command_req
{
	UINT16 msgid;
	UINT16 length;
	StrHuiIe_bfdf_motor_action_req_t     motor_action;
	StrHuiIe_bfdf_sensor_action_req_t    sensor_action;
  StrHuiIe_bfdf_ap_action_single_t     ap_action[HUITP_IEID_SUI_BFDF_MAX_LOCAL_AP_NUM];
}StrMsg_HUITP_COMM_sui_bfdf_command_req_t;

//公共消息过程
//传感器测试过程
//HUITP_MSGID_sui_com_test_command_req             = 0x3B70,
#define HUITP_IEID_SUI_COM_TEST_CMD_BUF_LEN_MAX  200  //MYC Change from 256 to 200, 2017/11/28
typedef struct StrMsg_HUITP_MSGID_sui_com_test_command_req
{
	UINT16 	msgid;
	UINT16 	length;
	UINT32 	cmdid;
	UINT32 	cmdValue1;
	UINT32 	cmdValue2;
	UINT32 	cmdValue3;
	UINT32 	cmdValue4;
	UINT8   cmdBuf[HUITP_IEID_SUI_COM_TEST_CMD_BUF_LEN_MAX];
}StrMsg_HUITP_MSGID_sui_com_test_command_req_t;
typedef enum StrHuiIe_sui_com_test_cmdid
{
	//COMMON PART
	HUITP_IEID_SUI_COM_TEST_CMDID_NULL = 0, //Starting
	HUITP_IEID_SUI_COM_TEST_CMDID_LED1_COMMNAD_ON = 1, //Starting point
	HUITP_IEID_SUI_COM_TEST_CMDID_LED1_COMMNAD_OFF,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED1_COMMNAD_BINKING_HIGHSPEED,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED1_COMMNAD_BINKING_LOWSPEED,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED2_COMMNAD_ON,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED2_COMMNAD_OFF,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED2_COMMNAD_BINKING_HIGHSPEED,
	HUITP_IEID_SUI_COM_TEST_CMDID_LED2_COMMNAD_BINKING_LOWSPEED,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_IGORE = 10,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_WEITGH_READ,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_CALIBRATION_ZERO,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_CALIBRATION_FULL,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_TARE_WEIGHT,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_OBJECT_READ,
	HUITP_IEID_SUI_COM_TEST_CMDID_SENSOR_COMMAND_OBJECT_WRITE,
	HUITP_IEID_SUI_COM_TEST_CMDID_MOTOR_COMMAND = 20,
	//BFDF SPECIFIC PART
	HUITP_IEID_SUI_BFDF_TEST_CMDID_SPEC1 = 0x0100,
	HUITP_IEID_SUI_BFDF_TEST_CMDID_BLINK_PATTERN1 = 0x0101,
	//BFHS SPECIFIC PART
	HUITP_IEID_SUI_BFHS_TEST_CMDID_SPEC1 = 0x0200,

	HUITP_IEID_SUI_COM_TEST_CMDID_MAX, //Ending
	HUITP_IEID_SUI_COM_TEST_CMDID_INVALID = 0xFFFF,
}StrHuiIe_sui_com_test_cmdid_t;

//HUITP_MSGID_sui_com_test_command_resp            = 0x3BF0,
typedef struct StrMsg_HUITP_MSGID_sui_com_test_command_resp
{
	UINT16 msgid;
	UINT16 length;
	UINT8  wmc_id;     /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  validFlag;  /*是否执行成功 0-FALSE, 1-TRUE */
	UINT16 error_code;
	UINT32 cmdid;
	UINT32 cmdValue1;
	UINT32 cmdValue2;
	UINT32 cmdValue3;
	UINT32 cmdValue4;
}StrMsg_HUITP_MSGID_sui_com_test_command_resp_t;

//心跳过程
//HUITP_MSGID_sui_com_heart_beat_report            = 0x3BF1,
typedef struct StrMsg_HUITP_MSGID_sui_com_heart_beat_report
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8   spare1;
	UINT16  spare2;
	UINT32  timeStamp;
}StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t;

//HUITP_MSGID_sui_com_heart_beat_confirm           = 0x3B71,
typedef struct StrMsg_HUITP_MSGID_sui_com_heart_beat_confirm
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8   wmcState;
	UINT16  spare1;
	UINT32  timeStamp;
}StrMsg_HUITP_MSGID_sui_com_heart_beat_confirm_t;
#define HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_NULL 		0
#define HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_OFFLINE 	1
#define HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INIT 		2
#define HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_WORKING 	3
#define HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INVALID 	0xFF

//上电过程
//HUITP_MSGID_sui_com_startup_ind                  = 0x3BF2,
typedef struct StrHuiIe_wmc_inventory
{
	UINT32 hw_inventory_id;
	UINT32 sw_inventory_id;
	UINT32 stm32_cpu_id;
	UINT32 weight_sensor_type;
	UINT32 motor_type;
	UINT8  wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  spare1;
	UINT16 spare2;
	UINT32 spare3;
	UINT32 spare4;
}StrHuiIe_wmc_inventory_t;
typedef struct StrMsg_HUITP_MSGID_sui_com_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	StrHuiIe_wmc_inventory_t wmc_inventory;
}StrMsg_HUITP_MSGID_sui_com_startup_ind_t;

//差错过程
//HUITP_MSGID_sui_com_fault_ind                    = 0x3BF3,
typedef struct StrMsg_HUITP_MSGID_sui_com_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8   spare1;
	UINT16	error_code;
}StrMsg_HUITP_MSGID_sui_com_fault_ind_t;

//暂停过程
//HUITP_MSGID_sui_com_suspend_req                 = 0x3B74,
typedef struct StrMsg_HUITP_MSGID_sui_com_suspend_req
{
	UINT16 	msgid;
	UINT16 	length;
}StrMsg_HUITP_MSGID_sui_com_suspend_req_t;

//HUITP_MSGID_sui_com_suspend_resp                 = 0x3BF4,
typedef struct StrMsg_HUITP_MSGID_sui_com_suspend_resp
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   validFlag;
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_com_suspend_resp_t;

//HUITP_MSGID_sui_com_resume_req                  = 0x3B75,
typedef struct StrMsg_HUITP_MSGID_sui_com_resume_req
{
	UINT16 	msgid;
	UINT16 	length;
}StrMsg_HUITP_MSGID_sui_com_resume_req_t;

//HUITP_MSGID_sui_com_resume_resp                 = 0x3BF5,
typedef struct StrMsg_HUITP_MSGID_sui_com_resume_resp
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   validFlag;
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_com_resume_resp_t;

//HUITP_MSGID_sui_com_stop_req                 	 = 0x3B76,
typedef struct StrMsg_HUITP_MSGID_sui_com_stop_req
{
	UINT16 	msgid;
	UINT16 	length;
}StrMsg_HUITP_MSGID_sui_com_stop_req_t;

//HUITP_MSGID_sui_com_stop_resp                 = 0x3BF5,
typedef struct StrMsg_HUITP_MSGID_sui_com_stop_resp
{
	UINT16 	msgid;
	UINT16 	length;
	UINT8   validFlag;
	UINT8   spare1;
	UINT16  errCode;
}StrMsg_HUITP_MSGID_sui_com_stop_resp_t;

//统一结束符
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
	StrIe_HUITP_IEID_uni_ccl_gen_picid_t  respPic;
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
	StrIe_HUITP_IEID_uni_ccl_fall_value_t fallValue;
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
	StrIe_HUITP_IEID_uni_ccl_fall_value_t fallValue;
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

//复旦项目
//HUITP_MSGID_uni_fdwq_min                    = 0x4F00,
//HUITP_MSGID_uni_fdwq_data_req               = 0x4F00,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_data_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
}StrMsg_HUITP_MSGID_uni_fdwq_data_req_t;

//HUITP_MSGID_uni_fdwq_data_resp              = 0x4F80,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_data_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_fdwq_sports_wrist_data_t wrist;
}StrMsg_HUITP_MSGID_uni_fdwq_data_resp_t;

//HUITP_MSGID_uni_fdwq_data_report            = 0x4F81,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_data_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_fdwq_sports_wrist_data_t wrist;
}StrMsg_HUITP_MSGID_uni_fdwq_data_report_t;

//HUITP_MSGID_uni_fdwq_data_confirm           = 0x4F01,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_data_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
}StrMsg_HUITP_MSGID_uni_fdwq_data_confirm_t;

//HUITP_MSGID_uni_fdwq_profile_report              = 0x4F82,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_profile_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_fdwq_profile_simple_data_t wrist;
}StrMsg_HUITP_MSGID_uni_fdwq_profile_report_t;

//HUITP_MSGID_uni_fdwq_profile_confirm             = 0x4F02,
typedef struct StrMsg_HUITP_MSGID_uni_fdwq_profile_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_fdwq_profile_detail_data_t wrist;
}StrMsg_HUITP_MSGID_uni_fdwq_profile_confirm_t;

//HUITP_MSGID_uni_fdwq_max,

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
typedef struct StrMsg_HUITP_MSGID_uni_itf_can_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_itf_can_value_t respValue;
}StrMsg_HUITP_MSGID_uni_itf_can_resp_t;

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
	UINT8  upgradeFlag;
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
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN];
}StrMsg_HUITP_MSGID_sui_sw_package_confirm_t;

//HUITP_MSGID_sui_flash_raw_cmd_req = 0xA290,
#define HUITP_IEID_SUI_FLASH_RAW_CMD_MAX_LEN 236
typedef struct StrMsg_HUITP_MSGID_flash_raw_command_req
{
	UINT16  msgid;
	UINT16  length;
  UINT8   flashRawCommandMode;
	UINT8   flashRawCommand;
	UINT8   flashSectorIdToErase;
	UINT8   flashSectorNumberToErase;
	UINT32  flashAddressToAccess;
	UINT32  flashValidLengthToAccess;
  UINT8   data[HUITP_IEID_SUI_FLASH_RAW_CMD_MAX_LEN];
}StrMsg_HUITP_MSGID_flash_raw_command_req_t;

//HUITP_MSGID_sui_flash_raw_cmd_rsp = 0xA210,
typedef struct StrMsg_HUITP_MSGID_flash_raw_command_resp
{
	UINT16  msgid;
	UINT16  length;
    UINT8   flashRawCommandModeResp;
	UINT8   flashRawCommandResp;
	UINT8   flashSectorIdToErase;
	UINT8   flashSectorNumberToErase;
	UINT32  flashAddressToAccess;
	UINT32  flashValidLengthToAccess;
  UINT8   data[HUITP_IEID_SUI_FLASH_RAW_CMD_MAX_LEN];
}StrMsg_HUITP_MSGID_flash_raw_command_resp_t;

//HUITP_MSGID_uni_sw_package_max,
//标签管理
//HUITP_MSGID_uni_equlable_min                     = 0xA200,
//HUITP_MSGID_uni_equlable_apply_req               = 0xA200,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_apply_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_equlable_apply_user_info_t userInfo;
}StrMsg_HUITP_MSGID_uni_equlable_apply_req_t;

//HUITP_MSGID_uni_equlable_apply_resp              = 0xA280,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_apply_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_equlable_apply_allocation_t allocatInfo;
}StrMsg_HUITP_MSGID_uni_equlable_apply_resp_t;

//HUITP_MSGID_uni_equlable_apply_report            = 0xA281,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_apply_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_equlable_apply_user_info_t userInfo;
}StrMsg_HUITP_MSGID_uni_equlable_apply_report_t;

//HUITP_MSGID_uni_equlable_apply_confirm           = 0xA201,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_apply_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_equlable_apply_allocation_t allocatInfo;
}StrMsg_HUITP_MSGID_uni_equlable_apply_confirm_t;

//HUITP_MSGID_uni_equlable_inservice_req           = 0xA202,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_inservice_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_equlable_inservice_info_t insvInfo;
}StrMsg_HUITP_MSGID_uni_equlable_inservice_req_t;

//HUITP_MSGID_uni_equlable_inservice_resp          = 0xA282,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_inservice_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_equlable_inservice_info_t insvInfo;
}StrMsg_HUITP_MSGID_uni_equlable_inservice_resp_t;

//HUITP_MSGID_uni_equlable_inservice_report        = 0xA283,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_inservice_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_equlable_inservice_info_t insvInfo;
}StrMsg_HUITP_MSGID_uni_equlable_inservice_report_t;

//HUITP_MSGID_uni_equlable_inservice_confirm       = 0xA203,
typedef struct StrMsg_HUITP_MSGID_uni_equlable_inservice_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_equlable_inservice_info_t insvInfo;
}StrMsg_HUITP_MSGID_uni_equlable_inservice_confirm_t;

//HUITP_MSGID_uni_equlable_max,

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

//测试命令
//HUITP_MSGID_uni_test_command_min                 = 0xF700,
//HUITP_MSGID_uni_test_command_req                 = 0xF700,
typedef struct StrMsg_HUITP_MSGID_uni_test_command_req
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_req_t baseReq;
	StrIe_HUITP_IEID_uni_test_command_value_t reqValue;
}StrMsg_HUITP_MSGID_uni_test_command_req_t;

//HUITP_MSGID_uni_test_command_resp                = 0xF780,
typedef struct StrMsg_HUITP_MSGID_uni_test_command_resp
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_resp_t baseResp;
	StrIe_HUITP_IEID_uni_test_command_value_t respValue;
}StrMsg_HUITP_MSGID_uni_test_command_resp_t;

//HUITP_MSGID_uni_test_command_report              = 0xF781,
typedef struct StrMsg_HUITP_MSGID_uni_test_command_report
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_report_t baseReport;
	StrIe_HUITP_IEID_uni_test_command_value_t reportValue;
}StrMsg_HUITP_MSGID_uni_test_command_report_t;

//HUITP_MSGID_uni_test_command_confirm             = 0xF701,
typedef struct StrMsg_HUITP_MSGID_uni_test_command_confirm
{
	StrMsg_HUITP_MSGID_uni_general_head_msgid_t msgId;
	UINT16 msgLen;
	StrIe_HUITP_IEID_uni_com_confirm_t baseConfirm;
	StrIe_HUITP_IEID_uni_test_command_value_t confirmValue;
}StrMsg_HUITP_MSGID_uni_test_command_confirm_t;

//HUITP_MSGID_uni_test_command_max,


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



/////////////////////////////////////////////////////////
////HUITP-JSON///////////////////////////////////////////
/////////////////////////////////////////////////////////
//增加HUITP-JSON结构
#define HUITP_MSG_HUIJSON_CONSTANT_TO_USER  		"ToUsr"
#define HUITP_MSG_HUIJSON_CONSTANT_FROM_USER  		"FrUsr"
#define HUITP_MSG_HUIJSON_CONSTANT_CREATE_TIME  	"CrTim"
#define HUITP_MSG_HUIJSON_CONSTANT_MSG_TYPE  		"MsgTp"
#define HUITP_MSG_HUIJSON_CONSTANT_MSG_ID  			"MsgId"
#define HUITP_MSG_HUIJSON_CONSTANT_MSG_LEN  		"MsgLn"
#define HUITP_MSG_HUIJSON_CONSTANT_IE_CONTENT  		"IeCnt"
#define HUITP_MSG_HUIJSON_CONSTANT_FUNC_FLAG  		"FnFlg"

/*
{
	“ToUsr”:”XHZN”,
	“FrUsr”:”HCU_GTJYG5203_RND04”,
	“CrTim”:4335667,
	“MsgTp”:"huitp-json",
	“MsgId”:4345,
	“MsgLn”:15,
	“IeCnt”:
	{
		“snrId”: 12,
		“validFlag”: 1
		“errCode”: 0,
		“cmdTestValue1”: 1,
		“cmdTestValue2”: 0,
		“cmdTestValue3”: 0,
		“cmdTestValue4”: 0
	},
	“FnFlg”:0
}

*/

//公共消息
#define HUITP_JSON_IE_BUF_MAX_LEN	300
typedef struct StrMsg_HUITP_JSON_MSGID_general_message
{
	UINT32 ToUsr;
	UINT32 FrUsr;
	UINT32 CrTim;
	UINT32 MsgTp;
	UINT32 MsgId;
	UINT32 MsgLn;
	UINT32 IeCnt;
	UINT8  ieBuf[HUITP_JSON_IE_BUF_MAX_LEN];
	UINT32 FnFlg;
}StrMsg_HUITP_JSON_MSGID_general_message;

//地震
//HUITP_JSON_MSGID_uni_earthquake_min                   = 0x5C00,
//HUITP_JSON_MSGID_uni_earthquake_ctrl_req              = 0x5C00,
typedef struct StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_req
{
	UINT8	cmdId;
}StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_req_t;

//HUITP_JSON_MSGID_uni_earthquake_ctrl_resp             = 0x5C80,
typedef struct StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp
{
	UINT8	cmdId;
	UINT8   result;
}StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp_t;

//HUITP_JSON_MSGID_uni_earthquake_data_report           = 0x5C81,
typedef struct StrIe_HUITP_JSON_MSGID_uni_earthquake_data_report
{
	UINT8	cmdId;
	UINT32  xData;
	UINT32  yData;
	UINT32  zData;
}StrIe_HUITP_JSON_MSGID_uni_earthquake_data_report_t;

//HUITP_JSON_MSGID_uni_earthquake_data_confirm          = 0x5C01,
typedef struct StrIe_HUITP_JSON_MSGID_uni_earthquake_data_confirm
{
	UINT8	cmdId;
	UINT8   result;
}StrIe_HUITP_JSON_MSGID_uni_earthquake_data_confirm_t;

//HUITP_JSON_MSGID_uni_heart_beat_report           = 0x5CFF,
typedef struct StrIe_HUITP_JSON_MSGID_uni_heart_beat_report
{
	UINT32 rand;
}StrIe_HUITP_JSON_MSGID_uni_heart_beat_report_t;

//HUITP_JSON_MSGID_uni_heart_beat_confirm     	 = 0x5C7F,
typedef struct StrIe_HUITP_JSON_MSGID_uni_heart_beat_confirm
{
	UINT32 rand;
}StrIe_HUITP_JSON_MSGID_uni_heart_beat_confirm_t;

//HUITP_JSON_MSGID_uni_earthquake_max,



#endif /* _HUITP_H_ */
