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

//Task level definition
#define ZHBHJT_PROTOCOL_FRAME_FIX_HEAD  "##"
#define ZHBHJT_PROTOCOL_FRAME_FIX_TAIL  "\r\n"
#define ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX 30


//ZHBHJT212协议配置静态表
typedef struct ZHBHJT212MsgIeEleStaticCfg
{
	const char  keyLable[20];
	const UINT8 dataType;
	const UINT8 maxLen;
	const UINT8 backwardMatchFlag;  //是否需要采用前向匹配1-7个字符串，配置用true/false表示。另外，前向匹配的长度由PolId定义
}ZHBHJT212MsgIeEleStaticCfg_t;
//PFDT = PROTOCAL_FRAME_DATA_TYPE
#define ZHBHJT_PFDT_CHAR    1
#define ZHBHJT_PFDT_UINT8   2
#define ZHBHJT_PFDT_INT16   3
#define ZHBHJT_PFDT_INT32   4
#define ZHBHJT_PFDT_INT64   5
#define ZHBHJT_PFDT_FLOAT   6  //浮点
#define ZHBHJT_PFDT_DBFLT   7  //双精度浮点
//组合IE的定义：目前考虑最多4次，如果更多的话，需要更加复杂的定义
typedef struct ZHBHJT212MsgIeCmbBasicStaticCfg
{
	const UINT8 ieId;
	const UINT8 ieRptNbr;
}ZHBHJT212MsgIeCmbBasicStaticCfg_t;
#define ZHBHJT_PROTOCOL_FRAME_IE_TO_IECMB_NBR_MAX 4
typedef struct ZHBHJT212MsgIeCmbStaticCfg
{
	const UINT8 cmbIeId;
	ZHBHJT212MsgIeCmbBasicStaticCfg_t ie[ZHBHJT_PROTOCOL_FRAME_IE_TO_IECMB_NBR_MAX];
}ZHBHJT212MsgIeCmbStaticCfg_t;

//消息定义
typedef struct ZHBHJT212MsgMsgBasicStaticCfg
{
	const UINT8 ieCmbId;
	const UINT8 ieCmbRptNbr;
}ZHBHJT212MsgMsgBasicStaticCfg_t;
#define ZHBHJT_PROTOCOL_FRAME_IECMB_TO_MSG_NBR_MAX 4
typedef struct ZHBHJT212MsgFormatStaticCfg
{
	const UINT16 cnCode;
	ZHBHJT212MsgMsgBasicStaticCfg_t IeCmb[ZHBHJT_PROTOCOL_FRAME_IECMB_TO_MSG_NBR_MAX];
}ZHBHJT212MsgFormatStaticCfg_t;
//ieCmbRptNbr取值的逻辑：0表示没有，1-254表示多少，255/0xFF表示可能存在也可以不存在





//message pack/unpack
extern OPSTAT func_cloudvela_zhbhjt212_msg_pack(msg_struct_llczhb_cloudvela_frame_resp_t *inputPar, CloudDataSendBuf_t *output);
extern OPSTAT func_cloudvela_zhbhjt212_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId);


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
#define HCU_ERROR_PRINT_ZHBHJTCODEC(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_ZHBHJT212CODEC_H_ */
