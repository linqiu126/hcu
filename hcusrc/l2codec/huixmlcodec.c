/**
 ****************************************************************************************
 *
 * @file huixmlcodec.c
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */
 
#include "huixmlcodec.h"
#include "../l0service/trace.h"

//Task Global variables

//Task Global variables
extern gTaskCloudvelaContext_t gTaskCloudvelaContext;

//使用编译器的功能，检查HUITP中设置的长度参数是否适合
#if ((HUITP_MSG_BUF_WITH_HEAD_MAX_LEN > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX) || (HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX) ||\
		(HUITP_MSG_HUIXML_HEAD_IN_CHAR_VARIABLE_LEN <= 0) || (HUITP_MSG_HUIXML_TOTAL_BUF_IN_CHAR_MAX_LEN > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX) ||\
		(HUITP_MSG_HUIFRAME_TOTAL_BUF_MAX_LEN > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX))
	#error HUITP protocal parameter setting error!
#endif

//XML自定义标准的编码函数方式
//inputLen：这是包括MsgHead在内的所有缓冲区长度，正常情况下=sizeof(StrMsg_HUITP_MSGID_uni_general_message_t)，或者IE_BODY+4
OPSTAT func_cloudvela_huitpxml_msg_pack(UINT16 msgId, StrMsg_HUITP_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output)
{
	//声明一个缓冲区长度，不能超越消息体内容的最长长度
	char s[HCU_SYSMSG_BH_BUF_BODY_LEN_MAX - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN];
	
	//参数检查：特别要检查输入的数据长度，正常不能超过100，因为HUIXML的数据区= (500(最长消息长度)-300(XML头))/2=100，这在大多数情况下都够用的，但在
	//文件传输的条件下有可能不够。幸好，文件下载使用FFP模式，不用再担心这个了。
	if ((inputLen <4) || (inputPar == NULL) || (inputLen > (HCU_SYSMSG_BH_BUF_BODY_LEN_MAX - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2) || \
		(inputLen > (sizeof(StrMsg_HUITP_MSGID_uni_general_message_t) - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)))
	{
		HCU_DEBUG_PRINT_INF("HUITPXML: InputLen=%d, InputPar=%d, MaxLen=%d, size2=%d\n", inputLen, inputPar, (HCU_SYSMSG_BH_BUF_BODY_LEN_MAX - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error input pointer or message length!\n");
	}
	if (output == NULL) HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error CloudDataSendBuf_t pointer!\n");
	
	//准备输出缓冲区
	memset(output, 0, sizeof(CloudDataSendBuf_t));

	//格式固定区域
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	strcat(output->curBuf, gTaskCloudvelaContext.L2Link.destUser); //zHcuSysEngPar.cloud.svrNameDefault
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	strcat(output->curBuf, gTaskCloudvelaContext.L2Link.srcUser); //zHcuSysEngPar.hwBurnId.equLable
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	sprintf(s, "%d", gTaskCloudvelaContext.L2Link.timeStamp);
	strcat(output->curBuf, s);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);

	//根据目的服务器地址，填入LINKID信息
	if ((strcmp(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameDefault) == 0) && (strcmp(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameHome) == 0))
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DUAL_SAME;
	else if (strcmp(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameDefault) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;
	else if (strcmp(gTaskCloudvelaContext.L2Link.destUser, zHcuSysEngPar.cloud.svrNameHome) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_HOME;
	else
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_INVALID;
	
	//Message Type content
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if      (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING);
	else if (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING);
	else if (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING);
	else if (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING);
	else if (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING);
	else if (gTaskCloudvelaContext.L2Link.msgType == HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID) strcat(output->curBuf, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING);
	else {
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error Message Type input!\n");
	}
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	
	//Content starting
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);

	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	//已经将CCL项目都改为了定长消息结构，所以原则上对于消息的挪动已经不需要了。
	//如果遇到确实需要改变长度的地方，则采用补0的方式即可，长度域表示实际长度，数据域中的实际长度有效，多余的则无效
	//变长的处理技巧，这里只是一个从IHU拿来的例子。本规范暂时不考虑变长，都采用系统固定定义的长度来进行编解码，以简化整个过程
/*	switch(msgId)
	{
		//这个消息已经改为定长了，空余位置补0
		case HUITP_MSGID_uni_ccl_lock_report:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return FAILURE;
			}
			else if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意inputLen跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER走

				//以上是之前的讲法，现在已经将该消息认定为定长，长度就是HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER。如果HCU_CCL_SENSOR_LOCK_NUMBER_MAX
				//更小，则后续的参数填写0或者FF均可
			}
			break;

		//这个消息同上已经改为定长了，空余位置补0
		case HUITP_MSGID_uni_ccl_door_report:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意inputLen跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER走
			}
			break;

		//这个消息由于上述IE改为了定长，所以这里也改为了定长，。不需要再挪动，空余位置补0
		case HUITP_MSGID_uni_ccl_state_report:
			if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
				//将StrIe_HUITP_IEID_uni_ccl_door_state_t移上来
				//memset(tt, 0, sizeof(tt));
				//memcpy(tt, inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t), HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
				//memcpy(inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+HCU_CCL_SENSOR_LOCK_NUMBER_MAX, tt, HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
				////将剩下的移上来
				//memset(tt, 0, sizeof(tt));
				//memcpy(tt, inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t), \
				//	inputLen-4-sizeof(StrIe_HUITP_IEID_uni_com_report_t)-2*HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
				//memcpy(inputPar+4+sizeof(StrIe_HUITP_IEID_uni_com_report_t)+ 2*HCU_CCL_SENSOR_LOCK_NUMBER_MAX, tt, \
				//	inputLen-4-sizeof(StrIe_HUITP_IEID_uni_com_report_t)-2*HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
				////移动之后，将末尾清0
				//memset(inputPar+inputLen, 0, (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER + HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER - 2*HCU_CCL_SENSOR_LOCK_NUMBER_MAX));
			}
			break;

		default:
			break;
	}*/
	
	//准备接龙字符串成为一整串
	memset(s, 0, sizeof(s));
	int i = 0;
	char tmp[3]="";
	UINT8 tmpU8 = 0;
	UINT8 *pIncoming;
	pIncoming = (UINT8*)inputPar;
	for (i=0; i<inputLen; i++){
		memset(tmp, 0, sizeof(tmp));
		memcpy(&tmpU8, pIncoming+i, 1);
		sprintf(tmp, "%02X", tmpU8);
		strcat(s, tmp);
	}
	if ((strlen(s) < 4) || (strlen(s) > (HCU_SYSMSG_BH_BUF_BODY_LEN_MAX - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: No data to be pack or too long length of data content %d!\n", strlen(s));

	//消息BODY的长度已经在msgLen中，不需要再填入，已经由上层在生成消息的时候填好了，所以这里不再需要再行统计
	strcat(output->curBuf, s);

	//Finish content part
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	
	//固定尾部分
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	strcat(output->curBuf, gTaskCloudvelaContext.L2Link.funcFlag);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	strcat(output->curBuf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);

	//存入返回参量中：这个长度用于控制输出的字符串
	output->curLen = strlen(output->curBuf);

	//返回
	return SUCCESS;
}

//解码接收到的消息
//该消息以CHAR为单位，从纯CDATA模式修改为<xml>格式，所以需要加入这个内容
//expectMsgId是接收消息解码时带入的目标函数，如果设置为-1则意味着忽略这个判定条件
OPSTAT func_cloudvela_huitpxml_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv, int expectMsgId)
{
	UINT32 index=0, msgId=0, msgLen=0;
	char tmp[5] = "";
	int ret = 0;
	int dif = 0;
	char *pIndexT1, *pIndexT2;  //临时位置
	UINT8 msgType;
	UINT64 msgCreateTime;
	char msgToUser[HCU_SYSDIM_FILE_NAME_LEN_MAX], msgFromUser[HCU_SYSDIM_FILE_NAME_LEN_MAX], msgFuncFlag[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char msgTmp[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char msgContent[HUITP_MSG_BUF_WITH_HEAD_MAX_LEN];
	
	//检查参数
	if (rcv == NULL) HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid received data buffer!\n");

	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if ((rcv->length > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX) || (rcv->length < 8))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid data length by XML content format!\n");
	
	//寻找<xml>的头部
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L))>= pIndexT2) || (dif > HCU_SYSMSG_BH_BUF_BODY_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head xml format!\n");
	
	//初始化接收的L2Link参数
	memset(&(gTaskCloudvelaContext.L2Link), 0, sizeof(msgie_struct_bh_com_head_t));

	//寻找<ToUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L))>= pIndexT2) || (dif > HCU_SYSDIM_FILE_NAME_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head ToUserName format!\n");
	memset(msgToUser, 0, sizeof(msgToUser));
	strncpy(msgToUser, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L), dif);
	//是否要如此对比，待定
	if ((strcmp(msgToUser, zHcuSysEngPar.hwBurnId.equLable) !=0) && (strcmp(msgToUser, zHcuSysEngPar.cloud.hcuName) !=0))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid toUser field!\n");
	strncpy(gTaskCloudvelaContext.L2Link.destUser, msgToUser, strlen(msgToUser)<sizeof(gTaskCloudvelaContext.L2Link.destUser)?strlen(msgToUser):sizeof(gTaskCloudvelaContext.L2Link.destUser));
	
	//寻找<fromUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L))>= pIndexT2) || (dif > HCU_SYSDIM_FILE_NAME_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head fromUser format!\n");
	memset(msgFromUser, 0, sizeof(msgFromUser));
	strncpy(msgFromUser, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L), dif);
	//是否要如此对比，待定
	if ((strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameDefault) !=0) &&(strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameHome) !=0))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid fromUser field!\n");
	strncpy(gTaskCloudvelaContext.L2Link.srcUser, msgFromUser, strlen(msgFromUser)<sizeof(gTaskCloudvelaContext.L2Link.srcUser)?strlen(msgFromUser):sizeof(gTaskCloudvelaContext.L2Link.srcUser));
	
	//根据目的服务器地址，填入LINKID信息
	if ((strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameDefault) == 0) && (strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameHome) == 0))
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DUAL_SAME;
	else if (strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameDefault) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;
	else if (strcmp(msgFromUser, zHcuSysEngPar.cloud.svrNameHome) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_HOME;
	else
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_INVALID;

	//寻找<CreateTime>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L))>= pIndexT2) || (dif > HUITP_MSG_HUIXML_CONTSANT_CREATE_TIME_MAX_LEN))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head CreateTime format!\n");
	memset(msgTmp, 0, sizeof(msgTmp));
	strncpy(msgTmp, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L), dif);
	msgCreateTime = strtoul(msgTmp, NULL, 10); //10进制，并非16进制
	//暂时不判定，存下即可，以后再完善
	gTaskCloudvelaContext.L2Link.timeStamp = msgCreateTime;
	
	//寻找<msgType>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L))>= pIndexT2) || (dif > HCU_SYSDIM_FILE_NAME_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head msgType format!\n");
	memset(msgTmp, 0, sizeof(msgTmp));
	strncpy(msgTmp, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L), dif);
	if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID;
	else{
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head msgType format!\n");
	}
	//暂时不用，存下即可，以后再完善	
	gTaskCloudvelaContext.L2Link.msgType = msgType;

	//寻找<funcFlag>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L))>= pIndexT2) || (dif > HCU_SYSDIM_FILE_NAME_LEN_MAX))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid head funcFlag format!\n");
	memset(msgFuncFlag, 0, sizeof(msgFuncFlag));
	strncpy(msgFuncFlag, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L), dif);
	//msgFuncFlag的用途未来待定
	strncpy(gTaskCloudvelaContext.L2Link.funcFlag, msgFuncFlag, strlen(msgFuncFlag)<sizeof(gTaskCloudvelaContext.L2Link.funcFlag)?strlen(msgFuncFlag):sizeof(gTaskCloudvelaContext.L2Link.funcFlag));
	
	//寻找<Content>：长度必须是偶数
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L))>= pIndexT2) ||\
		(dif > HUITP_MSG_BUF_WITH_HEAD_MAX_LEN) || (dif != ((dif/2)*2)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Received message error, invalid content format!\n");
	memset(msgContent, 0, sizeof(msgContent));
	strncpy(msgContent, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L), dif);
		
	//msgContent中解出msgId/msgLen
	index = 0;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &msgContent[index], 4);
	msgId = strtoul(tmp, NULL, 16);
	index = index + 4;
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, &msgContent[index], 4);
	msgLen = strtoul(tmp, NULL, 16); //这种方式转码，就是按照大端进行的转码，已经不需要再调整大小端了
	//如果接收到的消息不是目标消息，一样会放弃解码
	if ((msgId < HUITP_MSGID_uni_min) || (msgId > HUITP_MSGID_uni_max) || ((expectMsgId != -1) && (msgId != expectMsgId)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Invalid received content data msgId info!\n");
	if (msgLen > (HCU_SYSMSG_BH_BUF_BODY_LEN_MAX - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2)
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Invalid received content data msgLen info!\n");
	index = index + 4;
	
	//解码到目标缓冲区
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgBuf;
	memset(&pMsgBuf, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	pMsgBuf.msgId.cmdId = (msgId>>8)&0xFF;
	pMsgBuf.msgId.optId = msgId&0xFF;
	pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(msgLen);  //回写时必须原封不动的按照原先的大小端顺序
	
	//转码，从CHAR进制转化为16进制，以1BYTE为单位，所以还未改变大小端的顺序。回写的目的是为了对完整的数据做结构强制转换
	for(index = 4; index < dif/2; index++){
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, &msgContent[index * 2], 2);
		pMsgBuf.data[index-4] = strtoul(tmp, NULL, 16)&0xFF;
	}
	
	//通过msgId将变长的消息结构进行独立处理
	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	//由于将所有变长消息都改善为定长消息，方式就是以系统定义的消息为基本格式，不够的地方补0
	//这样这部分挪动工作将不再需要。考虑未来可能还会需要，暂时保留这部分代码，但实际上并没有起作用
/*	switch(msgId)
	{
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_lock_resp:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return FAILURE;
			}
			else if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER走
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return FAILURE;
//				}
//				pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4);
			}
			break;

		//解码接收不可能收到这个消息，这里只是展示处理技巧
		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_door_resp:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER走
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return FAILURE;
//				}
//				pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4);
			}
			break;

		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_state_resp:
			if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//将剩下的移上来
//				UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t) - 4 - (HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER + HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - 2 * HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return FAILURE;
//				}
//				//通过tt过度，不然有可能拷贝会自己覆盖自己
//				memset(tt, 0, sizeof(tt));
//				memcpy(tt, &pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+ 2*HCU_CCL_SENSOR_LOCK_NUMBER_MAX,\
//					sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t)-4-sizeof(StrIe_HUITP_IEID_uni_com_resp_t)-sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t) - sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t));
//				memcpy(&pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t), tt, \
//					sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t)-4-sizeof(StrIe_HUITP_IEID_uni_com_resp_t)-sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t) - sizeof(StrIe_HUITP_IEID_uni_ccl_door_state_t));
//
//				//将StrIe_HUITP_IEID_uni_ccl_door_state_t移下去
//				memset(tt, 0, sizeof(tt));
//				memcpy(tt, &pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+HCU_CCL_SENSOR_LOCK_NUMBER_MAX, HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
//				memcpy(&pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t), tt, HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
//				//将LOCK_IE/DOOR_IE空余部分清0
//				memset(&pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+HCU_CCL_SENSOR_LOCK_NUMBER_MAX, 0, \
//					HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
//				memset(&pMsgBuf+4+sizeof(StrIe_HUITP_IEID_uni_com_resp_t)+sizeof(StrIe_HUITP_IEID_uni_ccl_lock_state_t)+HCU_CCL_SENSOR_LOCK_NUMBER_MAX, 0, \
//					HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX);
//				pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t) - 4);
			}
			break;

		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_sw_package_req:
			//因为只有一个边长IE，且IE正好处于最后一个结构部分，所以不需要干啥
			//将消息长度恢复到消息结构长度，以便下面统一处理
//			pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t) - 4);
			break;

		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_sw_package_confirm:
			//因为只有一个边长IE，且IE正好处于最后一个结构部分，所以不需要干啥
			//将消息长度恢复到消息结构长度，以便下面统一处理
//			pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t) - 4);

			break;

		default:
			break;
	}*/
	
	//假设一切正常
	ret = SUCCESS;
	
	//再来进行消息的统一处理
	switch(msgId){

	//心跳请求
	case HUITP_MSGID_uni_heart_beat_req:
	{
		StrMsg_HUITP_MSGID_uni_heart_beat_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_heart_beat_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_heart_beat_req_received_handle(snd);
	}
	break;

	//心跳证实
	case HUITP_MSGID_uni_heart_beat_confirm:
	{
		StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_heart_beat_confirm_received_handle(snd);
	}
	break;

	//AMARM
	case HUITP_MSGID_uni_alarm_info_req:
	{
		StrMsg_HUITP_MSGID_uni_alarm_info_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_alarm_info_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_alarm_info_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_alarm_info_req_received_handle(snd);
	}
	break;

	//AMARM
	case HUITP_MSGID_uni_alarm_info_confirm:
	{
		StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_alarm_info_confirm_received_handle(snd);
	}
	break;

	//PM
	case HUITP_MSGID_uni_performance_info_req:
	{
		StrMsg_HUITP_MSGID_uni_performance_info_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_performance_info_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_performance_info_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm_info_req_received_handle(snd);
	}
	break;

	//PM
	case HUITP_MSGID_uni_performance_info_confirm:
	{
		StrMsg_HUITP_MSGID_uni_performance_info_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_performance_info_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_performance_info_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm_info_confirm_received_handle(snd);
	}
	break;

	//清单
	case HUITP_MSGID_uni_inventory_req:
	{
		StrMsg_HUITP_MSGID_uni_inventory_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_inventory_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_inventory_req_received_handle(snd);
	}
	break;

	//清单
	case HUITP_MSGID_uni_inventory_confirm:
	{
		StrMsg_HUITP_MSGID_uni_inventory_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_inventory_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_inventory_confirm_received_handle(snd);
	}
	break;

	//软件包
	case HUITP_MSGID_uni_sw_package_req:
	{
		StrMsg_HUITP_MSGID_uni_sw_package_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_sw_package_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_sw_package_req_received_handle(snd);
	}
	break;

	//软件包
	case HUITP_MSGID_uni_sw_package_confirm:
	{
		StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_sw_package_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_sw_package_confirm_received_handle(snd);
	}
	break;

	//BFSC业务
	case HUITP_MSGID_uni_bfsc_comb_scale_data_req:
	{
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_req_received_handle(snd);
	}
	break;

	//BFSC业务
	case HUITP_MSGID_uni_bfsc_comb_scale_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_confirm_received_handle(snd);
	}
	break;

	//BFSC业务
	case HUITP_MSGID_uni_bfsc_comb_scale_event_confirm:
	{
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_bfsc_comb_scale_event_confirm_received_handle(snd);
	}
	break;

	//BFSC业务
	case HUITP_MSGID_uni_bfsc_comb_scale_cmd_req:
	{
		StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_bfsc_comb_scale_cmd_req_received_handle(snd);
	}
	break;

	//BFSC业务
	case HUITP_MSGID_uni_bfsc_statistic_confirm:
	{
		StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_bfsc_statistic_confirm_received_handle(snd);
	}
	break;

	//EMC
	case HUITP_MSGID_uni_emc_data_req:
	{
		StrMsg_HUITP_MSGID_uni_emc_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_emc_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_emc_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_emc_data_req_received_handle(snd);
	}
	break;

	//EMC
	case HUITP_MSGID_uni_emc_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_emc_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_emc_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_emc_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_emc_data_confirm_received_handle(snd);
	}
	break;

	//EMC
	case HUITP_MSGID_uni_emc_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_emc_ctrl_req_received_handle(snd);
	}
	break;

	//CO1
	case HUITP_MSGID_uni_co1_data_req:
	{
		StrMsg_HUITP_MSGID_uni_co1_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_co1_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_co1_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_co1_data_req_received_handle(snd);
	}
	break;

	//CO1
	case HUITP_MSGID_uni_co1_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_co1_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_co1_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_co1_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_co1_data_confirm_received_handle(snd);
	}
	break;

	//HCHO
	case HUITP_MSGID_uni_hcho_data_req:
	{
		StrMsg_HUITP_MSGID_uni_hcho_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_hcho_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_hcho_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_hcho_data_req_received_handle(snd);
	}
	break;

	//HCHO
	case HUITP_MSGID_uni_hcho_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_hcho_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_hcho_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_hcho_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_hcho_data_confirm_received_handle(snd);
	}
	break;

	//酒精
	case HUITP_MSGID_uni_alcohol_data_req:
	{
		StrMsg_HUITP_MSGID_uni_alcohol_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_alcohol_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_alcohol_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_alcohol_data_req_received_handle(snd);
	}
	break;

	//酒精
	case HUITP_MSGID_uni_alcohol_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_alcohol_data_confirm_received_handle(snd);
	}
	break;

	//PM25
	case HUITP_MSGID_uni_pm25_data_req:
	{
		StrMsg_HUITP_MSGID_uni_pm25_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_pm25_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_pm25_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm25_data_req_received_handle(snd);
	}
	break;

	//PM25
	case HUITP_MSGID_uni_pm25_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm25_data_confirm_received_handle(snd);
	}
	break;

	//PM25
	case HUITP_MSGID_uni_pm25_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm25_ctrl_req_received_handle(snd);
	}
	break;

	//PM25SP
	case HUITP_MSGID_uni_pm25sp_data_req:
	{
		StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm25sp_data_req_received_handle(snd);
	}
	break;

	//PM25SP
	case HUITP_MSGID_uni_pm25sp_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_pm25sp_data_confirm_received_handle(snd);
	}
	break;

	//WINDSPD
	case HUITP_MSGID_uni_windspd_data_req:
	{
		StrMsg_HUITP_MSGID_uni_windspd_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_windspd_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_windspd_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_windspd_data_req_received_handle(snd);
	}
	break;

	//WINDSPD
	case HUITP_MSGID_uni_windspd_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_windspd_data_confirm_received_handle(snd);
	}
	break;

	//WINDSPD
	case HUITP_MSGID_uni_windspd_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_windspd_ctrl_req_received_handle(snd);
	}
	break;

	//WINDDIR
	case HUITP_MSGID_uni_winddir_data_req:
	{
		StrMsg_HUITP_MSGID_uni_winddir_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_winddir_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_winddir_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_winddir_data_req_received_handle(snd);
	}
	break;

	//WINDDIR
	case HUITP_MSGID_uni_winddir_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_winddir_data_confirm_received_handle(snd);
	}
	break;

	//WINDDIR
	case HUITP_MSGID_uni_winddir_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_winddir_ctrl_req_received_handle(snd);
	}
	break;

	//TEMP
	case HUITP_MSGID_uni_temp_data_req:
	{
		StrMsg_HUITP_MSGID_uni_temp_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_temp_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_temp_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_temp_data_req_received_handle(snd);
	}
	break;

	//TEMP
	case HUITP_MSGID_uni_temp_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_temp_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_temp_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_temp_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_temp_data_confirm_received_handle(snd);
	}
	break;

	//TEMP
	case HUITP_MSGID_uni_temp_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_temp_ctrl_req_received_handle(snd);
	}
	break;

	//HUMID
	case HUITP_MSGID_uni_humid_data_req:
	{
		StrMsg_HUITP_MSGID_uni_humid_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_humid_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_humid_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_humid_data_req_received_handle(snd);
	}
	break;

	//HUMID
	case HUITP_MSGID_uni_humid_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_humid_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_humid_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_humid_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_humid_data_confirm_received_handle(snd);
	}
	break;

	//HUMID
	case HUITP_MSGID_uni_humid_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_humid_ctrl_req_received_handle(snd);
	}
	break;

	//AIRPRS
	case HUITP_MSGID_uni_airprs_data_req:
	{
		StrMsg_HUITP_MSGID_uni_airprs_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_airprs_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_airprs_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_airprs_data_req_received_handle(snd);
	}
	break;

	//AIRPRS
	case HUITP_MSGID_uni_airprs_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_airprs_data_confirm_received_handle(snd);
	}
	break;

	//NOISE
	case HUITP_MSGID_uni_noise_data_req:
	{
		StrMsg_HUITP_MSGID_uni_noise_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_noise_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_noise_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_noise_data_req_received_handle(snd);
	}
	break;

	//NOISE
	case HUITP_MSGID_uni_noise_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_noise_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_noise_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_noise_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_noise_data_confirm_received_handle(snd);
	}
	break;

	//NOISE
	case HUITP_MSGID_uni_noise_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_noise_ctrl_req_received_handle(snd);
	}
	break;

	//HSMMP
	case HUITP_MSGID_uni_hsmmp_data_req:
	{
		StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_hsmmp_data_req_received_handle(snd);
	}
	break;

	//HSMMP
	case HUITP_MSGID_uni_hsmmp_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_hsmmp_data_confirm_received_handle(snd);
	}
	break;

	//HSMMP
	case HUITP_MSGID_uni_hsmmp_ctrl_req:
	{
		StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_hsmmp_ctrl_req_received_handle(snd);
	}
	break;

	//LIGHTSTR
	case HUITP_MSGID_uni_lightstr_data_req:
	{
		StrMsg_HUITP_MSGID_uni_lightstr_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_lightstr_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_lightstr_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_lightstr_data_req_received_handle(snd);
	}
	break;

	//LIGHTSTR
	case HUITP_MSGID_uni_lightstr_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_lightstr_data_confirm_received_handle(snd);
	}
	break;

	//TOXICGAS
	case HUITP_MSGID_uni_toxicgas_data_req:
	{
		StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_toxicgas_data_req_received_handle(snd);
	}
	break;

	//TOXICGAS
	case HUITP_MSGID_uni_toxicgas_data_confirm:
	{
		StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t) - 4))
			HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t*)(&pMsgBuf);
		ret = func_cloudvela_huitpxml_msg_toxicgas_data_confirm_received_handle(snd);
	}
	break;

	default:
	{
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Receive unknown message id and not able to process!\n");
	}//break;
	}//SWITCH
	
	//返回
	return ret;
}

OPSTAT func_cloudvela_huitpxml_msg_heart_beat_req_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->ping.ieId = HUITP_ENDIAN_EXG16(rcv->ping.ieId);
	rcv->ping.ieLen = HUITP_ENDIAN_EXG16(rcv->ping.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->ping.ieId != HUITP_IEID_uni_heart_beat_ping) || (rcv->ping.ieLen != (sizeof(StrIe_HUITP_IEID_uni_heart_beat_ping_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//这个消息属于L2/CLOUDVELA的处理过程
	UINT16 randval = HUITP_ENDIAN_EXG16(rcv->ping.randval);

	//根据目的服务器地址，填入LINKID信息
	if ((strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameDefault) == 0) && (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameHome) == 0))
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DUAL_SAME;
	else if (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameDefault) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;
	else if (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameHome) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_HOME;
	else
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_INVALID;

	//干活
	if (func_cloudvela_hb_link_passive_rcv_signal_for_react(randval) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Heart_beat processing error!\n");

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_heart_beat_confirm_received_handle(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);
	rcv->pong.ieId = HUITP_ENDIAN_EXG16(rcv->pong.ieId);
	rcv->pong.ieLen = HUITP_ENDIAN_EXG16(rcv->pong.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->pong.ieId != HUITP_IEID_uni_heart_beat_pong) || (rcv->pong.ieLen != (sizeof(StrIe_HUITP_IEID_uni_heart_beat_pong_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//这个消息属于L2/CLOUDVELA的处理过程
	UINT16 randval = HUITP_ENDIAN_EXG16(rcv->pong.randval);

	//根据目的服务器地址，填入LINKID信息
	if ((strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameDefault) == 0) && (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameHome) == 0))
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DUAL_SAME;
	else if (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameDefault) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_DEFAULT;
	else if (strcmp(gTaskCloudvelaContext.L2Link.srcUser, zHcuSysEngPar.cloud.svrNameHome) == 0)
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_HOME;
	else
		gTaskCloudvelaContext.linkId = HCU_SYSCFG_CLOUD_BH_LINK_INVALID;

	//干活
	if (func_cloudvela_hb_link_active_rcv_signal_check(randval) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Heart_beat processing error!\n");

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_alarm_info_req_received_handle(StrMsg_HUITP_MSGID_uni_alarm_info_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_alarm_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_alarm_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_spspm_alarm_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ, TASK_ID_SYSPM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_alarm_info_confirm_received_handle(StrMsg_HUITP_MSGID_uni_alarm_info_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_alarm_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_alarm_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_spspm_alarm_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM, TASK_ID_SYSPM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm_info_req_received_handle(StrMsg_HUITP_MSGID_uni_performance_info_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_perfm_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_perfm_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_spspm_perfm_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ, TASK_ID_SYSPM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm_info_confirm_received_handle(StrMsg_HUITP_MSGID_uni_performance_info_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_perfm_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_perfm_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_spspm_perfm_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM, TASK_ID_SYSPM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_inventory_req_received_handle(StrMsg_HUITP_MSGID_uni_inventory_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->reqValue.ieId = HUITP_ENDIAN_EXG16(rcv->reqValue.ieId);
	rcv->reqValue.ieLen = HUITP_ENDIAN_EXG16(rcv->reqValue.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->reqValue.ieId != HUITP_IEID_uni_inventory_element) || (rcv->reqValue.ieLen != (sizeof(StrIe_HUITP_IEID_uni_inventory_element_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_inventory_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_inventory_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = HUITP_ENDIAN_EXG16(rcv->baseReq.comReq);
	snd.hwType = HUITP_ENDIAN_EXG16(rcv->reqValue.hwType);
	snd.hwId = HUITP_ENDIAN_EXG16(rcv->reqValue.hwId);
	snd.swRel = HUITP_ENDIAN_EXG16(rcv->reqValue.swRel);
	snd.swVer = HUITP_ENDIAN_EXG16(rcv->reqValue.swVer);
	snd.upgradeFlag = rcv->reqValue.upgradeFlag;
	strncpy(snd.desc, rcv->reqValue.desc, strlen(rcv->reqValue.desc)<sizeof(snd.desc)?strlen(rcv->reqValue.desc):sizeof(snd.desc));
	snd.length = sizeof(msg_struct_cloudvela_spspm_inventory_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ, TASK_ID_SYSSWM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_inventory_confirm_received_handle(StrMsg_HUITP_MSGID_uni_inventory_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);
	rcv->confirmValue.ieId = HUITP_ENDIAN_EXG16(rcv->confirmValue.ieId);
	rcv->confirmValue.ieLen = HUITP_ENDIAN_EXG16(rcv->confirmValue.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->confirmValue.ieId != HUITP_IEID_uni_inventory_element) || (rcv->confirmValue.ieLen != (sizeof(StrIe_HUITP_IEID_uni_inventory_element_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_inventory_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_inventory_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.hwType = HUITP_ENDIAN_EXG16(rcv->confirmValue.hwType);
	snd.hwId = HUITP_ENDIAN_EXG16(rcv->confirmValue.hwId);
	snd.swRel = HUITP_ENDIAN_EXG16(rcv->confirmValue.swRel);
	snd.swVer = HUITP_ENDIAN_EXG16(rcv->confirmValue.swVer);
	snd.upgradeFlag = rcv->confirmValue.upgradeFlag;
	strncpy(snd.desc, rcv->confirmValue.desc, strlen(rcv->confirmValue.desc)<sizeof(snd.desc)?strlen(rcv->confirmValue.desc):sizeof(snd.desc));
	snd.length = sizeof(msg_struct_cloudvela_spspm_inventory_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM, TASK_ID_SYSSWM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_sw_package_req_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->segValue.ieId = HUITP_ENDIAN_EXG16(rcv->segValue.ieId);
	rcv->segValue.ieLen = HUITP_ENDIAN_EXG16(rcv->segValue.ieLen);
	rcv->body.ieId = HUITP_ENDIAN_EXG16(rcv->body.ieId);
	rcv->body.ieLen = HUITP_ENDIAN_EXG16(rcv->body.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->segValue.ieId != HUITP_IEID_uni_com_segment) || (rcv->segValue.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_segment_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->body.ieId != HUITP_IEID_uni_sw_package_body) || (rcv->body.ieLen != (sizeof(StrIe_HUITP_IEID_uni_sw_package_body_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_sw_package_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_sw_package_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = HUITP_ENDIAN_EXG16(rcv->baseReq.comReq);
	snd.segIndex = HUITP_ENDIAN_EXG16(rcv->segValue.segIndex);
	snd.segTotal = HUITP_ENDIAN_EXG16(rcv->segValue.segTotal);
	snd.segLen = HUITP_ENDIAN_EXG16(rcv->segValue.segLen);
	snd.validLen = HUITP_ENDIAN_EXG16(rcv->body.validLen);
	if (HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN > HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN)
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: HUITP and COMMSG parameter set error!\n");
	memcpy(snd.body, rcv->body.swPkgBody, sizeof(rcv->body.swPkgBody)<sizeof(snd.body)?sizeof(rcv->body.swPkgBody):sizeof(snd.body));
	snd.length = sizeof(msg_struct_cloudvela_spspm_inventory_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ, TASK_ID_SYSSWM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_sw_package_confirm_received_handle(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);
	rcv->segValue.ieId = HUITP_ENDIAN_EXG16(rcv->segValue.ieId);
	rcv->segValue.ieLen = HUITP_ENDIAN_EXG16(rcv->segValue.ieLen);
	rcv->body.ieId = HUITP_ENDIAN_EXG16(rcv->body.ieId);
	rcv->body.ieLen = HUITP_ENDIAN_EXG16(rcv->body.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->segValue.ieId != HUITP_IEID_uni_com_segment) || (rcv->segValue.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_segment_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->body.ieId != HUITP_IEID_uni_sw_package_body) || (rcv->body.ieLen != (sizeof(StrIe_HUITP_IEID_uni_sw_package_body_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_spspm_sw_packag_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_spspm_sw_packag_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.segIndex = HUITP_ENDIAN_EXG16(rcv->segValue.segIndex);
	snd.segTotal = HUITP_ENDIAN_EXG16(rcv->segValue.segTotal);
	snd.segLen = HUITP_ENDIAN_EXG16(rcv->segValue.segLen);
	snd.validLen = HUITP_ENDIAN_EXG16(rcv->body.validLen);
	if (HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN > HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN)
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: HUITP and COMMSG parameter set error!\n");
	memcpy(snd.body, rcv->body.swPkgBody, sizeof(rcv->body.swPkgBody)<sizeof(snd.body)?sizeof(rcv->body.swPkgBody):sizeof(snd.body));
	snd.length = sizeof(msg_struct_cloudvela_spspm_inventory_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM, TASK_ID_SYSSWM, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_req_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->dataReq.ieId = HUITP_ENDIAN_EXG16(rcv->dataReq.ieId);
	rcv->dataReq.ieLen = HUITP_ENDIAN_EXG16(rcv->dataReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->dataReq.ieId != HUITP_IEID_uni_scale_weight_req) || (rcv->dataReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_scale_weight_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_l3bfsc_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_l3bfsc_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.reqType = rcv->dataReq.type;
	snd.length = sizeof(msg_struct_cloudvela_l3bfsc_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ, TASK_ID_L3BFSC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_l3bfsc_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_l3bfsc_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_l3bfsc_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM, TASK_ID_L3BFSC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_event_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_event_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_l3bfsc_event_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_l3bfsc_event_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_l3bfsc_event_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM, TASK_ID_L3BFSC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;}

OPSTAT func_cloudvela_huitpxml_msg_bfsc_comb_scale_cmd_req_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_comb_scale_cmd_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->cfgReq.ieId = HUITP_ENDIAN_EXG16(rcv->cfgReq.ieId);
	rcv->cfgReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cfgReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->cmdReq.ieId != HUITP_IEID_uni_scale_weight_cmd) || (rcv->cmdReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_scale_weight_cmd_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->cfgReq.ieId != HUITP_IEID_uni_scale_weight_cfg_par) || (rcv->cfgReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_l3bfsc_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_l3bfsc_cmd_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.scaleWeightCmd = rcv->cmdReq.scaleWeightCmd;
	snd.workMode = rcv->cfgReq.workMode;
	snd.scaleWeightNbr = rcv->cfgReq.scaleWeightNbr;
	snd.staStartTime = HUITP_ENDIAN_EXG32(rcv->cfgReq.staStartTime);
	snd.staReportDuration = HUITP_ENDIAN_EXG32(rcv->cfgReq.staReportDuration);
	snd.length = sizeof(msg_struct_cloudvela_l3bfsc_cmd_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_bfsc_statistic_confirm_received_handle(StrMsg_HUITP_MSGID_uni_bfsc_statistic_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_l3bfsc_statistic_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_l3bfsc_statistic_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_l3bfsc_statistic_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM, TASK_ID_L3BFSC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}



void func_cloudvela_huitpxml_msg_generate_test_data(void)
{
	/*	int i = 0;
	UINT16 msgProcLen = 0;
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgInput;
	CloudDataSendBuf_t pMsgOutput;

	//Auth Ind
	StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_t pMsgProc1;
	msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_inq_t);
	memset(&pMsgProc1, 0, msgProcLen);
	pMsgProc1.msgId.cmdId = (HUITP_MSGID_uni_ccl_lock_auth_inq>>8)&0xFF;
	pMsgProc1.msgId.optId = HUITP_MSGID_uni_ccl_lock_auth_inq&0xFF;
	pMsgProc1.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_req_t
	pMsgProc1.baseReq.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_req);
	pMsgProc1.baseReq.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4);
	pMsgProc1.baseReq.comReq = HUITP_IEID_UNI_COM_REQUEST_YES;
	//StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t
	pMsgProc1.authReq.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_auth_req);
	pMsgProc1.authReq.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_lock_auth_req_t) - 4);
	pMsgProc1.authReq.authReqType = HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_TYPE_LOCK;
	for (i = 0; i < HUITP_IEID_UNI_CCL_LOCK_AUTH_REQ_MAX_LEN; i++){
		pMsgProc1.authReq.bleMacAddr[i] = 0xFF;
		pMsgProc1.authReq.rfidAddr[i] = 0xFF;
	}
	//Pack message
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc1, msgProcLen);
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_lock_auth_inq, &pMsgInput, msgProcLen, &pMsgOutput);
	HcuDebugPrint("HUITPXML: AuthInq=[%s]\n", pMsgOutput.buf);

	//Auth Resp
	StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t pMsgProc2;
	msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t);
	memset(&pMsgProc2, 0, msgProcLen);
	pMsgProc2.msgId.cmdId = (HUITP_MSGID_uni_ccl_lock_auth_resp>>8)&0xFF;
	pMsgProc2.msgId.optId = HUITP_MSGID_uni_ccl_lock_auth_resp&0xFF;
	pMsgProc2.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_resp_t
	pMsgProc2.baseResp.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_resp);
	pMsgProc2.baseResp.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_resp_t) - 4);
	pMsgProc2.baseResp.comResp = HUITP_IEID_UNI_COM_RESPONSE_YES;
	//StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t
	pMsgProc2.respState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_auth_resp);
	pMsgProc2.respState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_lock_auth_resp_t) - 4);
	pMsgProc2.respState.authResp = HUITP_IEID_UNI_CCL_LOCK_AUTH_RESP_NO;
	//Pack message
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc2, msgProcLen);
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_lock_auth_resp, &pMsgInput, msgProcLen, &pMsgOutput);
	HcuDebugPrint("HUITPXML: AuthResp=[%s]\n", pMsgOutput.buf);

	//State Report
	StrMsg_HUITP_MSGID_uni_ccl_state_report_t pMsgProc3;
	msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_report_t);
	memset(&pMsgProc3, 0, msgProcLen);
	pMsgProc3.msgId.cmdId = (HUITP_MSGID_uni_ccl_state_report>>8)&0xFF;
	pMsgProc3.msgId.optId = HUITP_MSGID_uni_ccl_state_report&0xFF;
	pMsgProc3.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_report_t
	pMsgProc3.baseReport.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_report);
	pMsgProc3.baseReport.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_report_t) - 4);
	pMsgProc3.baseReport.comReport = HUITP_IEID_UNI_COM_REPORT_YES;
	//StrIe_HUITP_IEID_uni_ccl_report_type_t
	pMsgProc3.reportType.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_report_type);
	pMsgProc3.reportType.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4);
	//生成三种不同的报告
	i = rand()%4;
	if (i==1) pMsgProc3.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT;
	else if (i==2) pMsgProc3.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT;
	else pMsgProc3.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT;	
	//StrIe_HUITP_IEID_uni_ccl_lock_state_t
	pMsgProc3.lockState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc3.lockState.ieLen = HUITP_ENDIAN_EXG16(2 + HCU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的锁的多少
	pMsgProc3.lockState.maxLockNo = HCU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc3.lockState.lockId = HCU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < HCU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc3.lockState.lockState[i] = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_lock_open_state(i) == TRUE)?HUITP_IEID_UNI_LOCK_STATE_OPEN:HUITP_IEID_UNI_LOCK_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_door_state_t
	pMsgProc3.doorState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_lock_state);
	pMsgProc3.doorState.ieLen = HUITP_ENDIAN_EXG16(2 + HCU_CCL_SENSOR_LOCK_NUMBER_MAX); //实际上配置的门的多少
	pMsgProc3.doorState.maxDoorNo = HCU_CCL_SENSOR_LOCK_NUMBER_MAX;
	pMsgProc3.doorState.doorId = HCU_CCL_SENSOR_LOCK_NUMBER_MAX; //这个表示全部
	for (i = 0; i < HCU_CCL_SENSOR_LOCK_NUMBER_MAX; i++){
		pMsgProc3.doorState.doorState[i] = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_door_open_state(i) == TRUE)?HUITP_IEID_UNI_DOOR_STATE_OPEN:HUITP_IEID_UNI_DOOR_STATE_CLOSE);
	}
	//StrIe_HUITP_IEID_uni_ccl_water_state_t
	pMsgProc3.waterState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_water_state);
	pMsgProc3.waterState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_water_state_t) - 4);
	pMsgProc3.waterState.waterState = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_water_state() == TRUE)?HUITP_IEID_UNI_WATER_STATE_ACTIVE:HUITP_IEID_UNI_WATER_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_fall_state_t
	pMsgProc3.fallState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_fall_state);
	pMsgProc3.fallState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_fall_state_t) - 4);
	pMsgProc3.fallState.fallState = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_fall_state() == TRUE)?HUITP_IEID_UNI_FALL_STATE_ACTIVE:HUITP_IEID_UNI_FALL_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_shake_state_t
	pMsgProc3.shakeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_shake_state);
	pMsgProc3.shakeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_shake_state_t) - 4);
	pMsgProc3.shakeState.shakeState = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_shake_state() == TRUE)?HUITP_IEID_UNI_SHAKE_STATE_ACTIVE:HUITP_IEID_UNI_SHAKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_smoke_state_t
	pMsgProc3.smokeState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_smoke_state);
	pMsgProc3.smokeState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_smoke_state_t) - 4);
	pMsgProc3.smokeState.smokeState = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_smoke_state() == TRUE)?HUITP_IEID_UNI_SMOKE_STATE_ACTIVE:HUITP_IEID_UNI_SMOKE_STATE_DEACTIVE);
	//StrIe_HUITP_IEID_uni_ccl_bat_state_t
	pMsgProc3.batState.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_state);
	pMsgProc3.batState.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_state_t) - 4);
	pMsgProc3.batState.batState = ((hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_state() == TRUE)?HUITP_IEID_UNI_BAT_STATE_WARNING:HUITP_IEID_UNI_BAT_STATE_NORMAL);
	//StrIe_HUITP_IEID_uni_ccl_temp_value_t
	pMsgProc3.tempValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_temp_value);
	pMsgProc3.tempValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_temp_value_t) - 4);
	pMsgProc3.tempValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.tempValue.tempValue = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_temp_value());
	//StrIe_HUITP_IEID_uni_ccl_humid_value_t
	pMsgProc3.humidValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_humid_value);
	pMsgProc3.humidValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_humid_value_t) - 4);
	pMsgProc3.humidValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.humidValue.humidValue = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_humid_value());
	//StrIe_HUITP_IEID_uni_ccl_bat_value_t
	pMsgProc3.batValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_bat_value);
	pMsgProc3.batValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_bat_value_t) - 4);
	pMsgProc3.batValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.batValue.batValue = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_bat_value());
	//StrIe_HUITP_IEID_uni_ccl_general_value1_t
	pMsgProc3.general1Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value1);
	pMsgProc3.general1Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value1_t) - 4);
	pMsgProc3.general1Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.general1Value.generalValue1 = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv1_value());
	//StrIe_HUITP_IEID_uni_ccl_general_value2_t
	pMsgProc3.general2Value.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_general_value2);
	pMsgProc3.general2Value.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_general_value2_t) - 4);
	pMsgProc3.general2Value.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.general2Value.generalValue2 = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rsv2_value());
	//StrIe_HUITP_IEID_uni_ccl_rssi_value_t
	pMsgProc3.rssiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_rssi_value);
	pMsgProc3.rssiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_rssi_value_t) - 4);
	pMsgProc3.rssiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.rssiValue.rssiValue = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_rssi_value());
	//StrIe_HUITP_IEID_uni_ccl_dcmi_value_t
	pMsgProc3.dcmiValue.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_dcmi_value);
	pMsgProc3.dcmiValue.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_dcmi_value_t) - 4);
	pMsgProc3.dcmiValue.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;  //100倍放大
	pMsgProc3.dcmiValue.dcmiValue = HUITP_ENDIAN_EXG16(hcu_didocap_ccl_sleep_and_fault_mode_ul_scan_illegal_dcmi_value());
	//Pack message
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc3, msgProcLen);
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));	
	func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_state_report, &pMsgInput, msgProcLen, &pMsgOutput);
	HcuDebugPrint("HUITPXML: StateType=%d, StateReport=[%s]\n", pMsgProc3.reportType.event, pMsgOutput.buf);

	//State Confirm
	StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t pMsgProc4;
	msgProcLen = sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t);
	memset(&pMsgProc4, 0, msgProcLen);
	pMsgProc4.msgId.cmdId = (HUITP_MSGID_uni_ccl_state_confirm>>8)&0xFF;
	pMsgProc4.msgId.optId = HUITP_MSGID_uni_ccl_state_confirm&0xFF;
	pMsgProc4.msgLen = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	//StrIe_HUITP_IEID_uni_com_confirm_t
	pMsgProc4.baseConfirm.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_com_confirm);
	pMsgProc4.baseConfirm.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4);
	pMsgProc4.baseConfirm.comConfirm = HUITP_IEID_UNI_COM_CONFIRM_POSITIVE;
	//StrIe_HUITP_IEID_uni_ccl_report_type_t
	pMsgProc4.reportType.ieId = HUITP_ENDIAN_EXG16(HUITP_IEID_uni_ccl_report_type);
	pMsgProc4.reportType.ieLen = HUITP_ENDIAN_EXG16(sizeof(StrIe_HUITP_IEID_uni_ccl_report_type_t) - 4);
	//生成三种不同的报告
	i = rand()%4;
	if (i==1) pMsgProc4.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_PERIOD_EVENT;
	else if (i==2) pMsgProc4.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_CLOSE_EVENT;
	else pMsgProc4.reportType.event = HUITP_IEID_UNI_CCL_REPORT_TYPE_FAULT_EVENT;
	//Pack message
	memset(&pMsgInput, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	memcpy(&pMsgInput, &pMsgProc4, msgProcLen);
	memset(&pMsgOutput, 0, sizeof(CloudDataSendBuf_t));
	func_cloud_standard_xml_pack(HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID, NULL, HUITP_MSGID_uni_ccl_state_confirm, &pMsgInput, msgProcLen, &pMsgOutput);
	HcuDebugPrint("HUITPXML: StateType=%d, StateConfirm=[%s]\n", pMsgProc4.reportType.event, pMsgOutput.buf);*/
	
}

OPSTAT func_cloudvela_huitpxml_msg_emc_data_req_received_handle(StrMsg_HUITP_MSGID_uni_emc_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_emc_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_emc_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_emc_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_EMC_DATA_REQ, TASK_ID_EMC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_EMC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_emc_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_emc_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_emc_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_emc_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_emc_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM, TASK_ID_EMC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_EMC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_emc_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_emc_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_emc_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_emc_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_emc_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_EMC_CTRL_REQ, TASK_ID_EMC, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_EMC].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_co1_data_req_received_handle(StrMsg_HUITP_MSGID_uni_co1_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_co1_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_co1_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_co1_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_CO1_DATA_REQ, TASK_ID_CO1, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_CO1].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_co1_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_co1_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_co1_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_co1_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_co1_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM, TASK_ID_CO1, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_CO1].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_hcho_data_req_received_handle(StrMsg_HUITP_MSGID_uni_hcho_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_hcho_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_hcho_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_hcho_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HCHO_DATA_REQ, TASK_ID_HCHO, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HCHO].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_hcho_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_hcho_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_hcho_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_hcho_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_hcho_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM, TASK_ID_HCHO, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HCHO].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_alcohol_data_req_received_handle(StrMsg_HUITP_MSGID_uni_alcohol_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_alcohol_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_alcohol_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_alcohol_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ, TASK_ID_ALCOHOL, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_ALCOHOL].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_alcohol_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_alcohol_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_alcohol_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_alcohol_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_alcohol_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM, TASK_ID_ALCOHOL, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_ALCOHOL].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm25_data_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_pm25_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_pm25_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_PM25_DATA_REQ, TASK_ID_PM25, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm25_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_pm25_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_pm25_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_pm25_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM, TASK_ID_PM25, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm25_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_pm25_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_pm25_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_PM25_CTRL_REQ, TASK_ID_PM25, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm25sp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_pm25sp_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_pm25sp_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25sp_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_pm25sp_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_PM25SP_DATA_REQ, TASK_ID_PM25SHARP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_PM25SHARP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_pm25sp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_pm25sp_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_pm25sp_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25sp_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_pm25sp_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM, TASK_ID_PM25SHARP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_PM25SHARP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_windspd_data_req_received_handle(StrMsg_HUITP_MSGID_uni_windspd_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_windspd_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_windspd_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_windspd_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ, TASK_ID_WINDSPD, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_windspd_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_windspd_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_windspd_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_windspd_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_windspd_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM, TASK_ID_WINDSPD, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_windspd_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_windspd_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_windspd_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_windspd_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_windspd_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ, TASK_ID_WINDSPD, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_winddir_data_req_received_handle(StrMsg_HUITP_MSGID_uni_winddir_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_winddir_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_winddir_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_winddir_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ, TASK_ID_WINDDIR, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDDIR].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_winddir_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_winddir_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_winddir_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_winddir_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_winddir_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM, TASK_ID_WINDDIR, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDDIR].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_winddir_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_winddir_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_winddir_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_winddir_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_winddir_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ, TASK_ID_WINDDIR, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_WINDDIR].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_temp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_temp_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_temp_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_temp_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_temp_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_TEMP_DATA_REQ, TASK_ID_TEMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_TEMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_temp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_temp_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_temp_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_temp_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_temp_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM, TASK_ID_TEMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_TEMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_temp_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_temp_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_temp_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_temp_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_temp_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_TEMP_CTRL_REQ, TASK_ID_TEMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_TEMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_humid_data_req_received_handle(StrMsg_HUITP_MSGID_uni_humid_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_humid_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_humid_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_humid_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HUMID_DATA_REQ, TASK_ID_HUMID, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HUMID].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_humid_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_humid_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_humid_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_humid_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_humid_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM, TASK_ID_HUMID, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HUMID].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_humid_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_humid_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_humid_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_humid_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_humid_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HUMID_CTRL_REQ, TASK_ID_HUMID, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HUMID].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_airprs_data_req_received_handle(StrMsg_HUITP_MSGID_uni_airprs_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_airprs_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_airprs_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_airprs_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ, TASK_ID_AIRPRS, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_AIRPRS].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_airprs_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_airprs_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_airprs_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_airprs_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_airprs_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM, TASK_ID_AIRPRS, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_AIRPRS].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_noise_data_req_received_handle(StrMsg_HUITP_MSGID_uni_noise_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_noise_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_noise_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_noise_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_NOISE_DATA_REQ, TASK_ID_NOISE, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_NOISE].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_noise_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_noise_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_noise_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_noise_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_noise_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM, TASK_ID_NOISE, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_NOISE].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_noise_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_noise_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_noise_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_noise_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_noise_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_NOISE_CTRL_REQ, TASK_ID_NOISE, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_NOISE].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_hsmmp_data_req_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_hsmmp_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_hsmmp_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_hsmmp_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HSMMP_DATA_REQ, TASK_ID_HSMMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_hsmmp_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_hsmmp_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_hsmmp_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_hsmmp_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM, TASK_ID_HSMMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_hsmmp_ctrl_req_received_handle(StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);
	rcv->cmdReq.ieId = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieId);
	rcv->cmdReq.ieLen = HUITP_ENDIAN_EXG16(rcv->cmdReq.ieLen);
	rcv->switchState.ieId = HUITP_ENDIAN_EXG16(rcv->switchState.ieId);
	rcv->switchState.ieLen = HUITP_ENDIAN_EXG16(rcv->switchState.ieLen);
	rcv->workCycle.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->workCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->workCycle.ieLen);
	rcv->sampleCycle.ieId = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieId);
	rcv->sampleCycle.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleCycle.ieLen);
	rcv->sampleNbr.ieId = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieId);
	rcv->sampleNbr.ieLen = HUITP_ENDIAN_EXG16(rcv->sampleNbr.ieLen);
	rcv->modbusAddr.ieId = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieId);
	rcv->modbusAddr.ieLen = HUITP_ENDIAN_EXG16(rcv->modbusAddr.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_snr_cmd_tag) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_switch_onoff) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_switch_onoff_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_work_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_work_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_cycle) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_cycle_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_sample_number) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_sample_number_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_modbus_address) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_modbus_address_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_hsmmp_ctrl_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_hsmmp_ctrl_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.cmdTag = rcv->cmdReq.cmdTag;
	snd.opt.equId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.newValue);
	snd.opt.newEquId = HUITP_ENDIAN_EXG32(rcv->modbusAddr.oldValue);
	snd.opt.powerOnOff = rcv->switchState.flag;
	snd.opt.workCycle = HUITP_ENDIAN_EXG32(rcv->workCycle.value);
	snd.opt.interSample = HUITP_ENDIAN_EXG32(rcv->sampleNbr.value);
	snd.opt.meausTimes = HUITP_ENDIAN_EXG32(rcv->sampleCycle.value);
	snd.length = sizeof(msg_struct_cloudvela_hsmmp_ctrl_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ, TASK_ID_HSMMP, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_HSMMP].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_lightstr_data_req_received_handle(StrMsg_HUITP_MSGID_uni_lightstr_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_lightstr_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_lightstr_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_lightstr_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ, TASK_ID_LIGHTSTR, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_LIGHTSTR].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_lightstr_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_lightstr_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_lightstr_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_lightstr_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_lightstr_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM, TASK_ID_LIGHTSTR, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_LIGHTSTR].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_toxicgas_data_req_received_handle(StrMsg_HUITP_MSGID_uni_toxicgas_data_req_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseReq.ieId = HUITP_ENDIAN_EXG16(rcv->baseReq.ieId);
	rcv->baseReq.ieLen = HUITP_ENDIAN_EXG16(rcv->baseReq.ieLen);

	//IE参数检查
	if ((rcv->baseReq.ieId != HUITP_IEID_uni_com_req) || (rcv->baseReq.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_req_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_toxicgas_data_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_toxicgas_data_req_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseReq = rcv->baseReq.comReq;
	snd.length = sizeof(msg_struct_cloudvela_toxicgas_data_req_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ, TASK_ID_TOXICGAS, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_TOXICGAS].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_cloudvela_huitpxml_msg_toxicgas_data_confirm_received_handle(StrMsg_HUITP_MSGID_uni_toxicgas_data_confirm_t *rcv)
{
	//int ret = 0;

	//先处理大小端问题
	rcv->msgLen = HUITP_ENDIAN_EXG16(rcv->msgLen);
	rcv->baseConfirm.ieId = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieId);
	rcv->baseConfirm.ieLen = HUITP_ENDIAN_EXG16(rcv->baseConfirm.ieLen);

	//IE参数检查
	if ((rcv->baseConfirm.ieId != HUITP_IEID_uni_com_confirm) || (rcv->baseConfirm.ieLen != (sizeof(StrIe_HUITP_IEID_uni_com_confirm_t) - 4)))
		HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Cloud raw message content unpack error!\n");

	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_cloudvela_toxicgas_data_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_cloudvela_toxicgas_data_confirm_t));
	memcpy(&(snd.comHead), &(gTaskCloudvelaContext.L2Link), sizeof(msgie_struct_bh_com_head_t));
	snd.baseConfirm = rcv->baseConfirm.comConfirm;
	snd.length = sizeof(msg_struct_cloudvela_toxicgas_data_confirm_t);
	if (hcu_message_send(MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM, TASK_ID_TOXICGAS, TASK_ID_CLOUDVELA, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CLOUDVELA("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName, zHcuVmCtrTab.task[TASK_ID_TOXICGAS].taskName);

	//返回
	return SUCCESS;
}


//HCU_ERROR_PRINT_CLOUDVELA("HUITPXML: Un-supported message but known message StrMsg_HUITP_MSGID_uni_hsmmp_ctrl_req_t received!\n");





