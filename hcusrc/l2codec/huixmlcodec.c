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

/*
//Task Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

//XML自定义标准的编码函数方式
//inputLen：这是包括MsgHead在内的所有缓冲区长度，正常情况下=sizeof(StrMsg_HUITP_MSGID_uni_general_message_t)，或者IE_BODY+4
OPSTAT func_cloud_standard_xml_pack(UINT8 msgType, char *funcFlag, UINT16 msgId, StrMsg_HUITP_MSGID_uni_general_message_t *inputPar, UINT16 inputLen, CloudDataSendBuf_t *output)
{
	//声明一个缓冲区长度，不能超越消息体内容的最长长度
	char s[MAX_HCU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN];
	
	//参数检查：特别要检查输入的数据长度，正常不能超过100，因为HUIXML的数据区= (500(最长消息长度)-300(XML头))/2=100，这在大多数情况下都够用的，但在
	//文件传输的条件下有可能不够。幸好，文件下载使用FFP模式，不用再担心这个了。
	if ((inputLen <4) || (inputPar == NULL) || (inputLen > (MAX_HCU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2) || \
		(inputLen > (sizeof(StrMsg_HUITP_MSGID_uni_general_message_t) - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)))
	{
		HcuDebugPrint("HUITPXML: InputLen=%d, InputPar=%d, MaxLen=%d, size2=%d\n", inputLen, inputPar, (MAX_HCU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
		HcuErrorPrint("HUITPXML: Error input pointer or message length!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	if (output == NULL)
	{
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		HcuErrorPrint("HUITPXML: Error CloudDataSendBuf_t pointer!\n");
		return HCU_FAILURE;
	}
	
	//准备输出缓冲区
	memset(output, 0, sizeof(CloudDataSendBuf_t));

	//格式固定区域
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	strcat(output->buf, zHcuSysEngPar.cloud.cloudBhServerName);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	strcat(output->buf, zHcuSysEngPar.hwBurnId.equLable);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	//time(0)：取得RTC时间的方式，通过跟BSP相连，得到解决了
	UINT32 timeStamp = hcu_l1hd_rtc_get_current_unix_time();
  sprintf(s, "%d", timeStamp);
	strcat(output->buf, s);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);
	
	//Message Type content
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if      (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING);
	else if (msgType == HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID) strcat(output->buf, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING);
	else {
		HcuErrorPrint("HUITPXML: Error Message Type input!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	
	//Content starting
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);

	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	//已经将CCL项目都改为了定长消息结构，所以原则上对于消息的挪动已经不需要了。
	//如果遇到确实需要改变长度的地方，则采用补0的方式即可，长度域表示实际长度，数据域中的实际长度有效，多余的则无效
	switch(msgId)
	{
		//这个消息已经改为定长了，空余位置补0
		case HUITP_MSGID_uni_ccl_lock_report:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return HCU_FAILURE;
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
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return HCU_FAILURE;
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
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return HCU_FAILURE;
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
	}
	
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
	if ((strlen(s) < 4) || (strlen(s) > (MAX_HCU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN))){
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		HcuErrorPrint("HUITPXML: No data to be pack or too long length of data content %d!\n", strlen(s));
		return HCU_FAILURE;
	}
	//消息BODY的长度已经在msgLen中，不需要再填入，已经由上层在生成消息的时候填好了，所以这里不再需要再行统计
	strcat(output->buf, s);	

	//Finish content part
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	
	//固定尾部分
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	if (funcFlag == NULL) strcat(output->buf, "0");
	else strcat(output->buf, funcFlag);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	strcat(output->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);

	//存入返回参量中：这个长度用于控制输出的字符串
	output->bufferLen = strlen(output->buf);

	//返回
	return HCU_SUCCESS;
}

//解码接收到的消息
//该消息以CHAR为单位，从纯CDATA模式修改为<xml>格式，所以需要加入这个内容
//expectMsgId是接收消息解码时带入的目标函数，如果设置为-1则意味着忽略这个判定条件
OPSTAT func_cloud_standard_xml_unpack(msg_struct_ccl_com_cloud_data_rx_t *rcv, int expectMsgId)
{
	UINT32 index=0, msgId=0, msgLen=0;
	char tmp[5] = "";
	int ret = 0;
	int dif = 0;
	char *pIndexT1, *pIndexT2;  //临时位置
	UINT8 msgType;
	UINT64 msgCreateTime;
	char msgToUser[HCU_FILE_NAME_LENGTH_MAX], msgFromUser[HCU_FILE_NAME_LENGTH_MAX], msgFuncFlag[HCU_FILE_NAME_LENGTH_MAX];
	char msgTmp[HCU_FILE_NAME_LENGTH_MAX];
	char msgContent[HUITP_MSG_BUF_WITH_HEAD_MAX_LEN];
	
	//检查参数
	if (rcv == NULL){
		HcuErrorPrint("HUITPXML: Received message error, invalid received data buffer!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	if ((rcv->length > MAX_HCU_MSG_BUF_LENGTH_CLOUD) || (rcv->length < 8)){
		HcuErrorPrint("HUITPXML: Received message error, invalid data length by XML content format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	
	//寻找<xml>的头部
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_XML_HEAD_L))>= pIndexT2) || (dif > MAX_HCU_MSG_BUF_LENGTH_CLOUD)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head xml format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	
	//寻找<ToUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_TO_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L))>= pIndexT2) || (dif > HCU_FILE_NAME_LENGTH_MAX)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head ToUserName format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	memset(msgToUser, 0, sizeof(msgToUser));
	strncpy(msgToUser, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_TO_USER_L), dif);
	if (strcmp(msgToUser, zHcuSysEngPar.hwBurnId.equLable) !=0){
		HcuErrorPrint("HUITPXML: Received message error, invalid toUser field!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	
	//寻找<fromUserName>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FROM_USER_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L))>= pIndexT2) || (dif > HCU_FILE_NAME_LENGTH_MAX)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head fromUser format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	memset(msgFromUser, 0, sizeof(msgFromUser));
	strncpy(msgFromUser, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_FROM_USER_L), dif);
	if (strcmp(msgFromUser, zHcuSysEngPar.cloud.cloudBhServerName) !=0){
		HcuErrorPrint("HUITPXML: Received message error, invalid fromUser field!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}	
	
	//寻找<CreateTime>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L))>= pIndexT2) || (dif > HUITP_MSG_HUIXML_CONTSANT_CREATE_TIME_MAX_LEN)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head CreateTime format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	memset(msgTmp, 0, sizeof(msgTmp));
	strncpy(msgTmp, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_CREATE_TIME_L), dif);
	msgCreateTime = strtoul(msgTmp, NULL, 10); //10进制，并非16进制
	//暂时不判定，存下即可，以后再完善
	if (msgCreateTime == 0) {}
	
	//寻找<msgType>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L))>= pIndexT2) || (dif > HCU_FILE_NAME_LENGTH_MAX)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head msgType format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	memset(msgTmp, 0, sizeof(msgTmp));
	strncpy(msgTmp, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_MSG_TYPE_L), dif);
	if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_CONTROL_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_HEAT_BEAT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_BIZ_ITG_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_ALARM_REPORT_ID;
	else if (strcmp(msgTmp, HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_STRING) ==0) msgType = HUITP_MSG_HUIXML_MSGTYPE_PM_REPORT_ID;
	else{
		HcuErrorPrint("HUITPXML: Received message error, invalid head msgType format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	//暂时不用，存下即可，以后再完善	
	if (msgType == HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID){}

	//寻找<funcFlag>
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L))>= pIndexT2) || (dif > HCU_FILE_NAME_LENGTH_MAX)){
		HcuErrorPrint("HUITPXML: Received message error, invalid head funcFlag format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	memset(msgFuncFlag, 0, sizeof(msgFuncFlag));
	strncpy(msgFuncFlag, pIndexT1+strlen(HUITP_MSG_HUIXML_CONSTANT_FUNC_FLAG_L), dif);
	//msgFuncFlag的用途未来待定
	if (msgFuncFlag[0] == '0') {}
	
	//寻找<Content>：长度必须是偶数
	pIndexT1 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	pIndexT2 = strstr(rcv->buf, HUITP_MSG_HUIXML_CONSTANT_CONTENT_R);
	dif = pIndexT2 - pIndexT1  - strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L);
	if ((pIndexT1 == NULL) || (pIndexT2 == NULL) || ((pIndexT1 +strlen(HUITP_MSG_HUIXML_CONSTANT_CONTENT_L))>= pIndexT2) ||\
		(dif > HUITP_MSG_BUF_WITH_HEAD_MAX_LEN) || (dif != ((dif/2)*2))){
		HcuErrorPrint("HUITPXML: Received message error, invalid content format!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
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
	msgLen = strtoul(tmp, NULL, 16);
	//如果接收到的消息不是目标消息，一样会放弃解码
	if ((msgId < HUITP_MSGID_uni_min) || (msgId > HUITP_MSGID_uni_max) || ((expectMsgId != -1) && (msgId != expectMsgId))){
		HcuErrorPrint("HUITPXML: Invalid received content data msgId info!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	if (msgLen > (MAX_HCU_MSG_BUF_LENGTH_CLOUD - HUITP_MSG_HUIXML_HEAD_IN_CHAR_MAX_LEN)/2){
		HcuErrorPrint("HUITPXML: Invalid received content data msgLen info!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return HCU_FAILURE;
	}
	index = index + 4;
	
	//解码到目标缓冲区
	StrMsg_HUITP_MSGID_uni_general_message_t pMsgBuf;
	memset(&pMsgBuf, 0, sizeof(StrMsg_HUITP_MSGID_uni_general_message_t));
	pMsgBuf.msgId.cmdId = (msgId>>8)&0xFF;
	pMsgBuf.msgId.optId = msgId&0xFF;
	pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(msgLen);  //回写时必须原封不动的按照原先的大小端顺序
	
	//转码，从CHAR进制转化为16进制，以1BYTE为单位，所以还未改变大小端的顺序
	for(index = 4; index < dif/2; index++){
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, &msgContent[index * 2], 2);
		pMsgBuf.data[index-4] = strtoul(tmp, NULL, 16)&0xFF;
	}
	
	//通过msgId将变长的消息结构进行独立处理
	//筛选出变长的消息结构，独立进行处理，剩下的统一处理
	//由于将所有变长消息都改善为定长消息，方式就是以系统定义的消息为基本格式，不够的地方补0
	//这样这部分挪动工作将不再需要。考虑未来可能还会需要，暂时保留这部分代码，但实际上并没有起作用
	switch(msgId)
	{
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_lock_resp:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_lock_report structure!\n");
				return HCU_FAILURE;
			}
			else if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER走
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return HCU_FAILURE;
//				}
//				pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_resp_t) - 4);
			}		
			break;
			
		//解码接收不可能收到这个消息，这里只是展示处理技巧
		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_door_resp:
			if (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_door_report structure!\n");
				return HCU_FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//由于UINT8  lockState[HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER]处于最后一块，所以还是不需要采取任何行动
				//注意跟着系统配置的HCU_CCL_SENSOR_LOCK_NUMBER_MAX走，而非跟着HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER走
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4 - (HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return HCU_FAILURE;
//				}
//				pMsgBuf.msgLen = HUITP_ENDIAN_EXG16(sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_resp_t) - 4);			
			}
			break;
			
		//该消息已经改善为定长消息了
		case HUITP_MSGID_uni_ccl_state_resp:
			if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX > HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error defination on max len of MSGID = HUITP_MSGID_uni_ccl_state_report structure!\n");
				return HCU_FAILURE;
			}
			else if ((HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER) || (HCU_CCL_SENSOR_LOCK_NUMBER_MAX < HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER)){
				//需要将缓冲区进行一定程度的移动
				//将剩下的移上来
//				UINT8 tt[HUITP_MSG_BUF_BODY_ONLY_MAX_LEN];
//				if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_resp_t) - 4 - (HUITP_IEID_UNI_CCL_LOCK_MAX_NUMBER + HUITP_IEID_UNI_CCL_DOOR_MAX_NUMBER - 2 * HCU_CCL_SENSOR_LOCK_NUMBER_MAX))){
//					zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
//					HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
//					return HCU_FAILURE;
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
	}
	
	//假设一切正常
	ret = HCU_SUCCESS;
	
	//再来进行消息的统一处理
	switch(msgId)
	{
#if (HCU_WORKING_PROJECT_NAME_UNIQUE_CURRENT_ID == HCU_WORKING_PROJECT_NAME_UNIQUE_STM32_CCL_ID)
		//心跳请求
		case HUITP_MSGID_uni_heart_beat_req:
		{
			StrMsg_HUITP_MSGID_uni_heart_beat_req_t *snd1;
			memset(snd1, 0, sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd1 = (StrMsg_HUITP_MSGID_uni_heart_beat_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_heart_beat_req_received_handle(snd1);
		}
			break;

		//心跳证实
		case HUITP_MSGID_uni_heart_beat_confirm:
		{
			StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t *snd2;
			memset(snd2, 0, sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd2 = (StrMsg_HUITP_MSGID_uni_heart_beat_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_heart_beat_confirm_received_handle(snd2);			
		}
			break;
		
		case HUITP_MSGID_uni_ccl_lock_req:	
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_req_t *snd3;
			memset(snd3, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd3 = (StrMsg_HUITP_MSGID_uni_ccl_lock_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_lock_req_received_handle(snd3);	
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t *snd4;
			memset(snd4, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd4 = (StrMsg_HUITP_MSGID_uni_ccl_lock_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_lock_confirm_received_handle(snd4);
		}
			break;
			
		case HUITP_MSGID_uni_ccl_lock_auth_resp:
		{
			StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t *snd5;
			memset(snd5, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd5 = (StrMsg_HUITP_MSGID_uni_ccl_lock_auth_resp_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_lock_auth_resp_received_handle(snd5);			
		}
			break;
			
		case HUITP_MSGID_uni_ccl_door_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_door_req_t *snd6;
			memset(snd6, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd6 = (StrMsg_HUITP_MSGID_uni_ccl_door_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_door_req_received_handle(snd6);			
		}
			break;
		
		case HUITP_MSGID_uni_ccl_door_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t *snd7;
			memset(snd7, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd7 = (StrMsg_HUITP_MSGID_uni_ccl_door_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_door_confirm_received_handle(snd7);						
		}
			break;
				
		case HUITP_MSGID_uni_ccl_rfid_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t *snd8;
			memset(snd8, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd8 = (StrMsg_HUITP_MSGID_uni_ccl_rfid_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_rfid_req_received_handle(snd8);						
		}
			break;

		case HUITP_MSGID_uni_ccl_rfid_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t *snd9;
			memset(snd9, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd9 = (StrMsg_HUITP_MSGID_uni_ccl_rfid_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_rfid_confirm_received_handle(snd9);						
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_ble_req_t *snd10;
			memset(snd10, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd10 = (StrMsg_HUITP_MSGID_uni_ccl_ble_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_ble_req_received_handle(snd10);					
		}
			break;

		case HUITP_MSGID_uni_ccl_ble_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t *snd11;
			memset(snd11, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd11 = (StrMsg_HUITP_MSGID_uni_ccl_ble_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_ble_confirm_received_handle(snd11);						
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t *snd12;
			memset(snd12, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd12 = (StrMsg_HUITP_MSGID_uni_ccl_gprs_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_gprs_req_received_handle(snd12);						
		}
			break;

		case HUITP_MSGID_uni_ccl_gprs_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t *snd13;
			memset(snd13, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd13 = (StrMsg_HUITP_MSGID_uni_ccl_gprs_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_gprs_confirm_received_handle(snd13);						
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_battery_req_t *snd14;
			memset(snd14, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd14 = (StrMsg_HUITP_MSGID_uni_ccl_battery_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_battery_req_received_handle(snd14);							
		}
			break;

		case HUITP_MSGID_uni_ccl_battery_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t *snd15;
			memset(snd15, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd15 = (StrMsg_HUITP_MSGID_uni_ccl_battery_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_battery_confirm_received_handle(snd15);								
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_shake_req_t *snd16;
			memset(snd16, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd16 = (StrMsg_HUITP_MSGID_uni_ccl_shake_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_shake_req_received_handle(snd16);					
		}
			break;

		case HUITP_MSGID_uni_ccl_shake_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t *snd17;
			memset(snd17, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd17 = (StrMsg_HUITP_MSGID_uni_ccl_shake_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_shake_confirm_received_handle(snd17);							
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t *snd18;
			memset(snd18, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd18 = (StrMsg_HUITP_MSGID_uni_ccl_smoke_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_smoke_req_received_handle(snd18);								
		}
			break;

		case HUITP_MSGID_uni_ccl_smoke_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t *snd19;
			memset(snd19, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd19 = (StrMsg_HUITP_MSGID_uni_ccl_smoke_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_smoke_confirm_received_handle(snd19);						
		}
			break;

		case HUITP_MSGID_uni_ccl_water_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_water_req_t *snd20;
			memset(snd20, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd20 = (StrMsg_HUITP_MSGID_uni_ccl_water_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_water_req_received_handle(snd20);				
		}
			break;

		case HUITP_MSGID_uni_ccl_water_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t *snd21;
			memset(snd21, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd21 = (StrMsg_HUITP_MSGID_uni_ccl_water_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_water_confirm_received_handle(snd21);							
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_temp_req_t *snd22;
			memset(snd22, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd22 = (StrMsg_HUITP_MSGID_uni_ccl_temp_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_temp_req_received_handle(snd22);				
		}
			break;

		case HUITP_MSGID_uni_ccl_temp_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t *snd23;
			memset(snd23, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd23 = (StrMsg_HUITP_MSGID_uni_ccl_temp_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_temp_confirm_received_handle(snd23);							
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_humid_req_t *snd24;
			memset(snd24, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd24 = (StrMsg_HUITP_MSGID_uni_ccl_humid_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_humid_req_received_handle(snd24);						
		}
			break;

		case HUITP_MSGID_uni_ccl_humid_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t *snd25;
			memset(snd25, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd25 = (StrMsg_HUITP_MSGID_uni_ccl_humid_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_humid_confirm_received_handle(snd25);								
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_fall_req_t *snd26;
			memset(snd26, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd26 = (StrMsg_HUITP_MSGID_uni_ccl_fall_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_fall_req_received_handle(snd26);						
		}
			break;

		case HUITP_MSGID_uni_ccl_fall_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t *snd27;
			memset(snd27, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd27 = (StrMsg_HUITP_MSGID_uni_ccl_fall_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_fall_confirm_received_handle(snd27);
		}
			break;

		case HUITP_MSGID_uni_ccl_state_req:
		{
			StrMsg_HUITP_MSGID_uni_ccl_state_req_t *snd28;
			memset(snd28, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd28 = (StrMsg_HUITP_MSGID_uni_ccl_state_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_state_req_received_handle(snd28);			
		}
			break;

		case HUITP_MSGID_uni_ccl_state_confirm:
		{
			StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t *snd29;
			memset(snd29, 0, sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd29 = (StrMsg_HUITP_MSGID_uni_ccl_state_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_ccl_state_confirm_received_handle(snd29);						
		}
			break;

		case HUITP_MSGID_uni_inventory_req:			
		{
			StrMsg_HUITP_MSGID_uni_inventory_req_t *snd30;
			memset(snd30, 0, sizeof(StrMsg_HUITP_MSGID_uni_inventory_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd30 = (StrMsg_HUITP_MSGID_uni_inventory_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_inventory_req_received_handle(snd30);			
		}
			break;

		case HUITP_MSGID_uni_inventory_confirm:
		{
			StrMsg_HUITP_MSGID_uni_inventory_confirm_t *snd31;
			memset(snd31, 0, sizeof(StrMsg_HUITP_MSGID_uni_inventory_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_inventory_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd31 = (StrMsg_HUITP_MSGID_uni_inventory_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_inventory_confirm_received_handle(snd31);					
		}
			break;		

		case HUITP_MSGID_uni_sw_package_req:
		{
			StrMsg_HUITP_MSGID_uni_sw_package_req_t *snd32;
			memset(snd32, 0, sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_req_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd32 = (StrMsg_HUITP_MSGID_uni_sw_package_req_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_sw_package_req_received_handle(snd32);							
		}
			break;		

		case HUITP_MSGID_uni_sw_package_confirm:
		{
			StrMsg_HUITP_MSGID_uni_sw_package_confirm_t *snd33;
			memset(snd33, 0, sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t));
			if (msgLen != (sizeof(StrMsg_HUITP_MSGID_uni_sw_package_confirm_t) - 4)){
				zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("HUITPXML: Error unpack message on length!\n");
				return HCU_FAILURE;
			}
			snd33 = (StrMsg_HUITP_MSGID_uni_sw_package_confirm_t*)(&pMsgBuf);
			ret = func_cloud_spsvirgo_ccl_msg_sw_package_confirm_received_handle(snd33);						
		}
			break;
		
#endif
		
		default:
		{
			zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
			HcuErrorPrint("HUITPXML: Receive unknown message id and not able to process!\n");
			return HCU_FAILURE;
		}
			//break;
	}
	
	//返回
	return ret;
}

void func_cloud_standard_xml_generate_message_test_data(void)
{
	int i = 0;
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
	HcuDebugPrint("HUITPXML: StateType=%d, StateConfirm=[%s]\n", pMsgProc4.reportType.event, pMsgOutput.buf);
	
}

*/







