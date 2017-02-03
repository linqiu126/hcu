/*
 * sysdim.h
 *
 *  Created on: 2016年1月12日
 *      Author: hitpony
 */

#ifndef L0COMVM_SYSDIM_H_
#define L0COMVM_SYSDIM_H_

//项目名称，每个项目均为唯一，编译生成项，用于本项目选择启动配置数据库中的工参信息
#define HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE 			"HCU_PRJ_AQYC_OBSOLETE"   	//老旧的AQYC项目
#define HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID 		1
#define HCU_WORKING_PROJECT_NAME_TEST_MODE				"HCU_PRJ_TEST_MODE"       	//打开所有模块，为了测试性能
#define HCU_WORKING_PROJECT_NAME_TEST_MODE_ID 			2
#define HCU_WORKING_PROJECT_NAME_AQYCG10_335D 			"HCU_PRJ_AQYCG10_335D"    	//基于G1/335D的AQYC
#define HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID 		3
#define HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY		"HCU_PRJ_AQYCG20_RASBERRY"  //基于G2/树莓派的AQYC
#define HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID 	4
#define HCU_WORKING_PROJECT_NAME_TBSWRG30				"HCU_PRJ_TBSWRG30"         	//数采仪
#define HCU_WORKING_PROJECT_NAME_TBSWRG30_ID 			5
#define HCU_WORKING_PROJECT_NAME_GQYBG40				"HCU_PRJ_GQYBG40"          	//挂墙仪表
#define HCU_WORKING_PROJECT_NAME_GQYBG40_ID 			6
#define HCU_WORKING_PROJECT_NAME_CXILC					"HCU_PRJ_CXILC"            	//CHUNXU智能路灯
#define HCU_WORKING_PROJECT_NAME_CXILC_ID 				7
#define HCU_WORKING_PROJECT_NAME_CXGLACM				"HCU_PRJ_CXGLACM"          	//CHUNXU格力空调模块
#define HCU_WORKING_PROJECT_NAME_CXGLACM_ID 			8
#define HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ			"HCU_PRJ_NBIOT_LPM_CJ"     	//CHUNXU低功耗抄表
#define HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID 		9
#define HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG			"HCU_PRJ_NBIOT_HPM_QG"     	//CHUNXU中等功耗抄表
#define HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID 		10
#define HCU_WORKING_PROJECT_NAME_BFSC_CBU				"HCU_PRJ_BFSC_CBU"         	//波峰组合秤上位机
#define HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID 			11
#define HCU_WORKING_PROJECT_NAME_OPWL_OTDR				"HCU_PRJ_OPWL_OTDR"        	//奥普维尔公司OTDR仪表
#define HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID 			12

//项目选择，这里需要选择唯一编译的项目ID
//由于采用ECLIPSE项目工程工具，所有文件均采用目录控制法
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_TEST_MODE
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_TEST_MODE_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_AQYCG10_335D
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID
#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY
#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_TBSWRG30
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_TBSWRG30_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_GQYBG40
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_GQYBG40_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_CXILC
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_CXILC_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_CXGLACM
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_CXGLACM_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_BFSC_CBU
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID
//#define HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE     HCU_WORKING_PROJECT_NAME_OPWL_OTDR
//#define HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE       HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID

//全局统一定义
//系统任务级
#define MAX_TASK_NUM_IN_ONE_HCU 100  //需要根据系统中实际多少任务模块来决定，从70改为100，不然随着新模块的加入，已经不够了
#define TASK_NAME_MAX_LENGTH 15  //从12改为15
//系统消息级
//maxmum state number and msg number in one task, no instance concept
#define HCU_TASK_QUEUE_ID_START 1024
#define MAX_MSGID_NUM_IN_ONE_TASK 400 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多：从200改为400个
#define MASK_MSGID_NUM_IN_ONE_TASK 511 //消息号段在同一个任务中必须连续到这个范围内：从255改为511个
#define MAX_STATE_NUM_IN_ONE_TASK 12  //一个任务之中最多定义的状态数量：从8个改为12个
#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
//消息长度
#define MSG_NAME_MAX_LENGTH 70
#define MAX_HCU_MSG_BODY_LENGTH 512 //MYC改为1500，就是为了ETHERNET数据段的最大长度。考虑到实时性，应该缩短。考虑到LINUX最大9K的单个任务QUEUE，应该缩短。
#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC


/*

//分项目设置不同项目的全局性能参数
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
	//系统任务级
	#define MAX_TASK_NUM_IN_ONE_HCU 70  //需要根据系统中实际多少任务模块来决定
	#define TASK_NAME_MAX_LENGTH 12
	//系统消息级
	//maxmum state number and msg number in one task, no instance concept
	#define HCU_TASK_QUEUE_ID_START 1024
	#define MAX_MSGID_NUM_IN_ONE_TASK 200 //一个任务中最多定义的消息数量，之前放254个，优化后更省内存，暂时用不了这么多
	#define MASK_MSGID_NUM_IN_ONE_TASK 255 //消息号段在同一个任务中必须连续到这个范围内
	#define MAX_STATE_NUM_IN_ONE_TASK 8  //一个任务之中最多定义的状态数量
	#define MAX_FSM_STATE_ENTRY_NUM_IN_ONE_TASK 64   //一个任务之中，STATE-MSGID成对处理函数最多数量
	//消息长度
	#define MSG_NAME_MAX_LENGTH 70
	#define MAX_HCU_MSG_BODY_LENGTH 1500 //MYC
	#define HCU_FILE_NAME_LENGTH_MAX 256 //MYC

//小技巧，不要这部分，以便加强编译检查
#else
#endif


//用于别的程序直接拷贝
//不通过数据库配置的参数区域
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)

//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif
*/

//#define HCU_WORKING_PROJECT_NAME_TBSWR "HCU_PRJ_TBSWR"
//#define HCU_WORKING_PROJECT_NAME_NBIOT_CJ188 "HCU_PRJ_NBIOT_CJ188" //Intelligence Meter
//#define HCU_WORKING_PROJECT_NAME_NBIOT_QG376 "HCU_PRJ_NBIOT_QG376"
//#define HCU_WORKING_PROJECT_NAME_TEST_MODE "HCU_PRJ_TESTMODE"


#endif /* L0COMVM_SYSDIM_H_ */
