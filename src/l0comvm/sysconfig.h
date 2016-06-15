/*
 * sysconf.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L0COMVM_SYSCONFIG_H_
#define L0COMVM_SYSCONFIG_H_

#include "sysversion.h"
#include "sysengpar.h"

/*
 *
 *   全局性控制表，不可以被数据控制表临时改动，只能通过本文件进行修改，然后重新编译获得结果
 *
 */


//进程启动多少的全局控制参数
#define HCU_PROCESS_WORK_MODE_SINGLE 1
#define HCU_PROCESS_WORK_MODE_DOUBLE 2
#define HCU_PROCESS_WORK_MODE_TRIPPLE 3
#define HCU_PROCESS_WORK_MODE_MULTI 4
#define HCU_PROCESS_WORK_MODE_CURRENT 1 //1=单进程 2=双进程 3=三进程，4=多进程, OTHERS=无效

//控制是否存储本地硬盘/内存/数据库的选项
#define HCU_DB_SENSOR_SAVE_FLAG_YES 1
#define HCU_DB_SENSOR_SAVE_FLAG_NO 0 //and Others
#define HCU_DB_SENSOR_SAVE_FLAG HCU_DB_SENSOR_SAVE_FLAG_YES

#define HCU_MEM_SENSOR_SAVE_FLAG_YES 1
#define HCU_MEM_SENSOR_SAVE_FLAG_NO 0 //and Others
#define HCU_MEM_SENSOR_SAVE_FLAG HCU_MEM_SENSOR_SAVE_FLAG_YES

#define HCU_DISC_SENSOR_SAVE_FLAG_YES 1
#define HCU_DISC_SENSOR_SAVE_FLAG_NO 0 //and Others
#define HCU_DISC_SENSOR_SAVE_FLAG HCU_DISC_SENSOR_SAVE_FLAG_YES

//项目名称，每个项目均为唯一，编译生成项，用于本项目选择启动配置数据库中的工参信息
#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE "HCU_PRJ_AQYC"

//最少保留多久的数据，做成安全的全局变量，并不能随意通过工程参数改小
//部分保留数据可以改的更小，可以放在工参里另行定义
#define HCU_DATA_SAVE_DURATION_MIN_IN_DAYS 60
#define HCU_DATA_SAVE_DURATION_IN_DAYS 90
//月份的计算按照（[天数-1]/30）+1进行，还是很精妙的，这样就不用单独设置月份数据
//月份的意义是，定时扫描，确保多少个月以内，文件数据必须保留，因为文件数据目前是按照时间月份进行存储的。使用天数进行计算，容易出现错误
//#define HCU_DATA_SAVE_DURATION_IN_MONTHS 3

//使用了哪一种数据库
#define HCU_DB_SELECTION_MYSQL 1
#define HCU_DB_SELECTION_SQLITE3 2
#define HCU_DB_SELECTION HCU_DB_SELECTION_MYSQL
#define HCU_DB_SQLITE3_LOC_AND_FILE "/home/hitpony/workspace/hcu/hcudb.sqlite"

//定义底层传感器是否起作用的全局定义，由于比较底层，所以这里不考虑使用工参配置的方式
#define HCU_SENSOR_PRESENT_YES 1
#define HCU_SENSOR_PRESENT_NO 2
#define HCU_SENSOR_PRESENT_DHT11 HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_SHT20 HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_MQ135 HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_MTH01 HCU_SENSOR_PRESENT_NO
#define HCU_SENSOR_PRESENT_RHT03 HCU_SENSOR_PRESENT_NO
#define HCU_SENSOR_PRESENT_BH1750 HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_BMP180 HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_BMPD300 HCU_SENSOR_PRESENT_NO
#define HCU_SENSOR_PRESENT_MQ3ALCO HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_ZE08CH2O HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_ZP01VOC HCU_SENSOR_PRESENT_YES
#define HCU_SENSOR_PRESENT_SHARP HCU_SENSOR_PRESENT_NO

/*
 *
 *   全局性工程参数控制表，可以被中途修改，更可以事先更新好，重启系统让其起到作用
 *   这里的配置项，只是作为缺省参数，一旦工程参数出错或者不可用，这里的缺省参数将会被起作用
 *   这里的参数起到出厂参数的作用，随着工厂版本一起到达现场
 *
 */

//可选项通信端口的全局定义，未来需要通过ConfigXml进一步优化
#define COMM_HW_BOARD_ON 1
#define COMM_HW_BOARD_OFF 0
#define COMM_BACK_HAWL_CON 1 //0=NULL, 1=ETHERNET, 2=WIFI, 3=3G4G, 4=USBNET, 5/OTHERS=INVALID
#define COMM_HW_BOARD_ETHERNET 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_USBNET 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_WIFI 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_3G4G 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_GPS 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_LCD 1 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_ZEEGBE 0 //1=ON, 0/OTHERS=OFF
#define COMM_HW_BOARD_FLASH 1 //1=ON, 0/OTHERS=OFF  TF card
#define COMM_FRONT_MODBUS 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SPSVIRGO 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_AVORION 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_CLOUDVELA 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_I2CBUSLIBRA 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SPIBUSARIES 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SPS485 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SPS232 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_MICROPHONE 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_CAMERA 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_BLE 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_GPIO 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_I2C 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SPI 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_PWM 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_ADC 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SWITCH 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_RELAY 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_MOTOR 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_EMC 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_PM25 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_TEMP 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_HUMID 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_WINDDIR 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_WINDSPD 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_NOISE 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_HSMMP 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_AIRPRS 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_CO1 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_LIGHTSTR 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_ALCOHOL 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_HCHO 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_TOXICGAS 1 //1=ON, 0/OTHERS=OFF
#define COMM_FRONT_SENSOR_PM25SHARP 1 //1=ON, 0/OTHERS=OFF

//定义后台连接网络的接口
//现在采用这种互斥的方式进行定义，以后需要等待HWINV进行PnP，确保多种接口即插即用，随时切换
//多种接口之间的优先级关系，则有程序任务自动决定：ETHERNET > WIFI > USB-OTG2 > 3G4G
//当前版本的做法是，多种接口均启动起来，但如何使用，由CLOUDCONT按照优先级和COMM_BACK_HAWL_CON定义唯一决定

//本地数据库HCUDB
#define HCU_DB_HOST_DEFAULT "localhost"		//连接的服务器地址
#define HCU_DB_USER_DEFAULT "root"     	//连接数据库的用户名
#define HCU_DB_PSW_DEFAULT  "123456"        //连接数据库的密码
#define HCU_DB_NAME_DEFAULT "hcudb"         //连接的数据库名称HCU
#define HCU_DB_PORT_DEFAULT 0           	//缺省设置

//Timer setting by Shanhun
#define EMC_TIMER_DURATION_PERIOD_READ 600
#define EMC_TIMER_DURATION_MODBUS_FB 3
#define HUMID_TIMER_DURATION_PERIOD_READ 600
#define HUMID_TIMER_DURATION_MODBUS_FB 3
#define NOISE_TIMER_DURATION_PERIOD_READ 600
#define NOISE_TIMER_DURATION_MODBUS_FB 3
#define PM25_TIMER_DURATION_PERIOD_READ 600
#define PM25_TIMER_DURATION_MODBUS_FB 3
#define TEMP_TIMER_DURATION_PERIOD_READ 600
#define TEMP_TIMER_DURATION_MODBUS_FB 3
#define WINDDIR_TIMER_DURATION_PERIOD_READ 600
#define WINDDIR_TIMER_DURATION_MODBUS_FB 3
#define WINDSPD_TIMER_DURATION_PERIOD_READ 600
#define WINDSPD_TIMER_DURATION_MODBUS_FB 3
#define TIMER_DURATION_PERIOD_HEART_BEAT 600  //in second
#define TIMER_DURATION_PERIOD_SEND_DATA_BACK 80 //in second
#define TIMER_DURATION_PERIOD_CMD_CONTROL_LONG 600  //for cmd control long timer by Shanchun
#define TIMER_DURATION_PERIOD_CMD_CONTROL_SHORT 5  //for cmd control short timer by Shanchun
#define AIRPRS_TIMER_DURATION_PERIOD_READ 600
#define CO1_TIMER_DURATION_PERIOD_READ 600
#define LIGHTSTR_TIMER_DURATION_PERIOD_READ 600
#define ALCOHOL_TIMER_DURATION_PERIOD_READ 600
#define HCHO_TIMER_DURATION_PERIOD_READ 600
#define TOXICGAS_TIMER_DURATION_PERIOD_READ 600
#define PM25SHARP_TIMER_DURATION_PERIOD_READ 60
#define SYSPM_TIMER_DURATION_PERIOD_WORKING 600

#ifdef TARGET_LINUX_ARM
#define DURATION_OF_INIT_FB_WAIT_MAX 2000 //in us iso ms, caculate by task_id * this value, to wait for period for SVRCON too much INIT_WAIT_FOR_FB coming at the same time.
#endif

#ifdef TARGET_LINUX_X86
#define DURATION_OF_INIT_FB_WAIT_MAX 20000 //in us iso ms, caculate by task_id * this value, to wait for period for SVRCON too much INIT_WAIT_FOR_FB coming at the same time.
#endif

#ifdef TARGET_RASPBERRY_PI3B
#define DURATION_OF_INIT_FB_WAIT_MAX 20000 //in us iso ms, caculate by task_id * this value, to wait for period for SVRCON too much INIT_WAIT_FOR_FB coming at the same time.
#endif

//定时器，控制摄像头工作周期及时长
#define HSMMP_TIMER_DURATION_PERIOD_AVORION_READ 60 //should be 600second = 10分钟, in second
#define HSMMP_AVORION_CAPTURE_DURATION_DEFAULT 2 //10 second
//下面的时间设置，必须是2.5%精度以上，不能设置为固定TIMEOUT时长，否则会出现逻辑错误
//比如，采样时间为1000秒，则超时 >= 1000*1.025 = 1025
#define HSMMP_TIMER_DURATION_AVORION_FB HSMMP_AVORION_CAPTURE_DURATION_DEFAULT + 10    //通过AVORION的访问，得到结果的时长。这个时间必须大于摄像头干活的时长
#define HSMMP_AVORION_REFRESH_RATE_DEFAULT 25 //10 second

//Series Port config by Shanchun
#define HCU_SERIESPORT_NUM_FOR_MODBUS_DEFAULT 2
#define HCU_SERIESPORT_NUM_FOR_GPS_DEFAULT  1
#define HCU_SERIESPORT_NUM_FOR_PM25SHARP_DEFAULT  0
#define HCU_SERIESPORT_BAUTRATE_DEFAULT 9600

//定义后台CLOUD连接到本地及SAE的地址
#define CLOUDVELA_HTTP_ADDRESS_LOCAL "http://127.0.0.1/test.php"
#define CLOUDVELA_HTTP_ADDRESS_TEST "http://aaa.bbb/"
#define CLOUDVELA_HTTP_ADDRESS_SAE "http://mfunhcu.sinaapp.com/wechat/main/cloud_callback.php"
#define CLOUDVELA_HTTP_ADDRESS_JD "http://mfunhcu.sinaapp.com/jd/cloud_callback.php"
#define CLOUDVELA_HTTP_ADDRESS_WECHAT "http://mfunhcu.sinaapp.com/wechat/cloud_callback.php"

//定义后台CLOUD  FTP 的地址
#define  CLOUDVELA_FTP_ADDRESS "ftp://121.40.185.177" //for HCU SW FTP by shanchun
#define  CLOUDVELA_FTP_USER "anonymous"    //for HCU SW FTP by shanchun
#define  CLOUDVELA_FTP_PWD "anonymous" //for HCU SW FTP by shanchun

//local SW storage address for HCU SW upgrade by shanchun
#define  HCU_SW_DOWNLOAD_DIR_DEFAULT "/home/pi/hcu_sw_download/"
#define  HCU_SW_ACTIVE_DIR_DEFAULT "/home/pi/hcu_sw_active/"
#define  HCU_SW_BACKUP_DIR_DEFAULT "/home/pi/hcu_sw_backup/"

//系统定义的服务器以及本机名称，用于HCU与服务器之间的通信
#define CLOUDVELA_BH_SERVER_NAME "SAE_MFUNHCU"  //SERVER NAME
#define CLOUDVELA_BH_HCU_NAME "HCU_SH_0301"     //DEVICE NAME
#define CLOUDVELA_BH_INTERFACE_STANDARD_XML 1
#define CLOUDVELA_BH_INTERFACE_STANDARD_ZHB 2  //中环保

//后台CLOUVELA选用的帧协议格式
//系统级参数，非常重要，一旦改变，可能导致后台不通！！！
#define CLOUDVELA_BH_INTERFACE_STANDARD CLOUDVELA_BH_INTERFACE_STANDARD_XML

//定义本地视频服务器
#define HCU_VIDEO_STREAM_SERVER_DIR_DEFAULT  "/var/www/html/avorion/"
#define HCU_VIDEO_STREAM_SERVER_HTTP_DEFAULT "http://192.168.1.232/avorion/"

//DEBUG开关，缺省打开
#define TRACE_DEBUG_ALL_OFF 0 //全关
#define TRACE_DEBUG_INF_ON 1  //全开
#define TRACE_DEBUG_NOR_ON 2  //普通级
#define TRACE_DEBUG_IPT_ON 4  //重要级
#define TRACE_DEBUG_CRT_ON 8  //关键级
#define TRACE_DEBUG_FAT_ON 16 //致命级
#define TRACE_DEBUG_ON TRACE_DEBUG_INF_ON //利用以上标示位，确定TRACE的级别

//TRACE开关，缺省打开
#define TRACE_MSG_MODE_OFF 0 //全关
#define TRACE_MSG_MODE_INVALID 0xFF //无效
#define TRACE_MSG_MODE_ALL 1 //放开所有的TRACE模块和消息，该模式将忽略模块和消息的设置
#define TRACE_MSG_MODE_ALL_BUT_TIME_OUT 2  //除了所有的TIME_OUT消息
#define TRACE_MSG_MODE_ALL_BUT_HEART_BEAT 3 //除了所有的HEART_BEAT消息
#define TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT 4  //除了所有的TIME_OUT消息
#define TRACE_MSG_MODE_MOUDLE_TO_ALLOW 10  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MOUDLE_TO_RESTRICT 11  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MOUDLE_FROM_ALLOW 12  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MOUDLE_FROM_RESTRICT 13  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MOUDLE_DOUBLE_ALLOW 14  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MOUDLE_DOUBLE_RESTRICT 15  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MSGID_ALLOW 20  //只通过模块号过滤消息
#define TRACE_MSG_MODE_MSGID_RESTRICT 21  //只通过模块号过滤消息
#define TRACE_MSG_MODE_COMBINE_TO_ALLOW 30  //通过模块和消息枚举
#define TRACE_MSG_MODE_COMBINE_TO_RESTRICT 31  //通过模块和消息枚举
#define TRACE_MSG_MODE_COMBINE_FROM_ALLOW 32  //通过模块和消息枚举
#define TRACE_MSG_MODE_COMBINE_FROM_RESTRICT 33  //通过模块和消息枚举
#define TRACE_MSG_MODE_COMBINE_DOUBLE_ALLOW 34  //通过模块和消息枚举
#define TRACE_MSG_MODE_COMBINE_DOUBLE_RESTRICT 35  //通过模块和消息枚举
#define TRACE_MSG_ON TRACE_MSG_MODE_ALL_BUT_TIME_OUT_AND_HEART_BEAT

#endif /* L0COMVM_SYSCONFIG_H_ */
