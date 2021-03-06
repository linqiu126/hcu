/*
 * sysdim.h
 *
 *  Created on: 2016年1月12日
 *      Author: hitpony
 */

#ifndef L0COMVM_SYSDIM_H_
#define L0COMVM_SYSDIM_H_

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



#endif /* L0COMVM_SYSDIM_H_ */
