/**
 ****************************************************************************************
 *
 * @file huixmlcodec.h
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_ZHBHJT212CODEC_H_
#define L2FRAME_ZHBHJT212CODEC_H_

#include "../l2codec/zhbhjt212msg.h"
#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

//message pack/unpack
extern OPSTAT func_cloudvela_zhbhjt_msg_pack(UINT16 msgId, StrMsg_ZHBHJT_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output);
extern OPSTAT func_cloudvela_zhbhjt_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId);


//大小端变换宏定义
#define ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SMALL 1
#define ZHBHJT_CURRENT_PROCESSOR_ENDIAN_BIG 2
#define ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SET ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SMALL
#if (ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SET == ZHBHJT_CURRENT_PROCESSOR_ENDIAN_BIG)
   #define ZHBHJT_ENDIAN_EXG16(x) (x)
   #define ZHBHJT_ENDIAN_EXG32(x) (x)
#elif (ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SET == ZHBHJT_CURRENT_PROCESSOR_ENDIAN_SMALL)
   #define ZHBHJT_ENDIAN_EXG16(x) ((((x)&0xFF00)>>8) | (((x)&0x00FF)<<8))
   #define ZHBHJT_ENDIAN_EXG32(x) ((((x)&0xFF000000)>>24) | (((x)&0x00FF0000)>>8) | (((x)&0x0000FF00)<<8) | (((x)&0x000000FF)<<24))
#else
   #error Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both.
#endif


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_ZHBHJTCODEC(...)	do{zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//Test Data：纯粹是为了测试下位机，所以没有将ToUser/FromUser设置的非常规整


#endif /* L2FRAME_ZHBHJT212CODEC_H_ */
