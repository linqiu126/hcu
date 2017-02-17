/**
 ****************************************************************************************
 *
 * @file ZHBHJT212CODEC.c
 *
 * @brief ZHBHJT212CODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "zhbhjt212codec.h"
#include "../l0service/trace.h"

//Global variables

//Task Global variables
//extern gTaskCloudvelaContext_t gTaskCloudvelaContext;

//使用编译器的功能，检查ZHBHJT中设置的长度参数是否适合


//XML自定义标准的编码函数方式
//inputLen：这是包括MsgHead在内的所有缓冲区长度，正常情况下=sizeof(StrMsg_ZHBHJT_MSGID_uni_general_message_t)，或者IE_BODY+4
OPSTAT func_cloudvela_zhbhjt_msg_pack(UINT16 msgId, StrMsg_ZHBHJT_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output)
{

	//返回
	return SUCCESS;
}


OPSTAT func_cloudvela_zhbhjt_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId)
{
	//返回
	return SUCCESS;
}



