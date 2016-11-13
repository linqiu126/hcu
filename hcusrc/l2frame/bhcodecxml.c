/*
 * bhcodecxml.c
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "bhcodecxml.h"
#include "cloudvela.h"

//Task Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
//Added by Shanchun for CMD command
extern UINT8 CMDShortTimerFlag;
extern UINT8 CMDLongTimerFlag;
extern UINT32 CMDPollingNoCommandNum;

//XML自定义标准的编码函数方式
//完全自己手动编码的方式，未来可以灵活的改动
//也可以考虑使用XML的标准函数来做，但其实最大的内容部分，也是靠手工编码的，并没有省略多少复杂度
OPSTAT func_cloudvela_standard_xml_pack(CloudBhItfDevReportStdXml_t *xmlFormat, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (xmlFormat == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudBhItfDevReportStdXml_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	if (buf == NULL){
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	//格式固定区域
	strcpy(xmlFormat->xml_l, "<xml>");
	strcpy(xmlFormat->xml_r, "</xml>");
	strcpy(xmlFormat->ToUserName_l, "<ToUserName><![CDATA[");
	strcpy(xmlFormat->ToUserName_r, "]]></ToUserName>");
	strcpy(xmlFormat->FromUserName_l, "<FromUserName><![CDATA[");
	strcpy(xmlFormat->FromUserName_r, "]]></FromUserName>");
	strcpy(xmlFormat->CreateTime_l, "<CreateTime>");
	strcpy(xmlFormat->CreateTime_r, "</CreateTime>");
	strcpy(xmlFormat->MsgType_l, "<MsgType><![CDATA[");
	strcpy(xmlFormat->MsgType_r, "]]></MsgType>");
	strcpy(xmlFormat->Content_l, "<Content><![CDATA[");
	strcpy(xmlFormat->Content_r, "]]></Content>");
	strcpy(xmlFormat->FuncFlag_l, "<FuncFlag>");
	strcpy(xmlFormat->FuncFlag_r, "</FuncFlag>");

	//对于目前来说，数值固定内容
	strcpy(xmlFormat->ToUserName, zHcuSysEngPar.cloud.cloudBhServerName);
	strcpy(xmlFormat->FromUserName, zHcuSysEngPar.cloud.cloudBhHcuName);


	UINT32 timeStamp = time(0);//by Shanchun: report local time to align with unpack method in Cloud
    sprintf(zCurTimeDate.sSec, "%d", timeStamp);
	//HcuDebugPrint("%s\n", zCurTimeDate.sSec);

	//func_hwinv_scan_date(); //更新时间戳
	strcpy(xmlFormat->CreateTime, zCurTimeDate.sSec);



	//MsgType参数，必须由调用函数填入，因为它才能知晓这是什么样的内容体
	//strcpy(xmlFormat->MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT);
	if (strlen(xmlFormat->FuncFlag) <=0 )	sprintf(xmlFormat->FuncFlag, "%1d", 0);

	//准备接龙字符串成为一整串
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char da[MAX_HCU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));
	char tmp[3] = "";

	//固定头部分
	strcat(s, xmlFormat->xml_l);
	strcat(s, xmlFormat->ToUserName_l);
	strcat(s, xmlFormat->ToUserName);
	strcat(s, xmlFormat->ToUserName_r);
	strcat(s, xmlFormat->FromUserName_l);
	strcat(s, xmlFormat->FromUserName);
	strcat(s, xmlFormat->FromUserName_r);
	strcat(s, xmlFormat->CreateTime_l);
	strcat(s, xmlFormat->CreateTime);
	strcat(s, xmlFormat->CreateTime_r);
	strcat(s, xmlFormat->MsgType_l);
	strcat(s, xmlFormat->MsgType);
	strcat(s, xmlFormat->MsgType_r);
	strcat(s, xmlFormat->Content_l);

	//顺序是编码的黄金规则，千万不能错，否则就无法解开了!!!
	//char conCmdId[3];
	strcat(s, xmlFormat->conCmdId);

	//所有变长部分
	//char conOptId[3]; //1B
	strcat(da, xmlFormat->conOptId);

	//char conEqpId[3];  //1B
	strcat(da, xmlFormat->conEqpId);

	//char conBackType[3]; //1B
	strcat(da, xmlFormat->conBackType);// by Shanchun: move it after EqpId to align with Pack sequence in Cloud

	//char conDataFormat[3]; //1B
	strcat(da, xmlFormat->conDataFormat);

	//char conEmc[5];   //2B
	strcat(da, xmlFormat->conEmc);

	//char conPm1d0[9];   //4B
	strcat(da, xmlFormat->conPm1d0);

	//char conPm2d5[9];   //4B
	strcat(da, xmlFormat->conPm2d5);

	//char conPm10d[9];   //4B
	strcat(da, xmlFormat->conPm10d);

	//char conWinddir[5];   //2B
	strcat(da, xmlFormat->conWinddir);

	//char conWindspd[5];   //2B
	strcat(da, xmlFormat->conWindspd);

	//char conTemp[5];   //2B
	strcat(da, xmlFormat->conTemp);

	//char conHumid[5];   //2B
	strcat(da, xmlFormat->conHumid);

	//char conNoise[9];   //4B
	strcat(da, xmlFormat->conNoise);

	//char conew; //1B
	strcat(da, xmlFormat->conEW);

	//char conGpsx[9];   //4B
	strcat(da, xmlFormat->conGpsx);

	//char conns; //1B
	strcat(da, xmlFormat->conNS);

	//char conGpsy[9];   //4B
	strcat(da, xmlFormat->conGpsy);

	//char conGpsz[9];   //4B
	strcat(da, xmlFormat->conGpsz);


	//Adding by Shanchun for alarm report
	//char conAlarmType[5]; //2B
	strcat(da, xmlFormat->conAlarmType);

	//char conAlarmContent[5]; //2B
	strcat(da, xmlFormat->conAlarmContent);


	//Adding by Shanchun for control cmd
	//char conPowerOnOff[3]; //1B
	strcat(da, xmlFormat->conPowerOnOff);

	//char conInterSample[3]; //1B
	strcat(da, xmlFormat->conInterSample);

	//char conMeausTimes[3]; //1B
	strcat(da, xmlFormat->conMeausTimes);

	//char conNewEquId[3]; //1B
	strcat(da, xmlFormat->conNewEquId);

	//char conWorkCycle[3]; //1B
	strcat(da, xmlFormat->conWorkCycle);

	//char conSwDownload[3]; //1B
	strcat(da, xmlFormat->conSwDownload);


	//strcat(da, xmlFormat->conHwUuid);
	strcat(da, xmlFormat->conHwType);
	strcat(da, xmlFormat->conHwVersion);
	strcat(da, xmlFormat->conSwDelivery);
	strcat(da, xmlFormat->conSwRelease);


	//char conSwInventory[3]; //1B
	strcat(da, xmlFormat->conAvUpload);
	//strcat(da, xmlFormat->conAvFileName);

	//char conTimeStamp[9]; //4B
	strcat(da, xmlFormat->conTimeStamp);

	//char conNtimes[5];   //2B
	strcat(da, xmlFormat->conNtimes);

	HcuDebugPrint("CLOUDVELA: da: %s!!!\n", da);

	//获取变长部分的长度, Len=0的情况存在，比如Heart_Beat消息，这里为了统一处理函数的简化，不做过分的区别对待和处理，尽量让处理函数通用化
	int len = 0;
	len = strlen(da);
	//if ((len < 0) || ((len % 2) != 0) || (len > MAX_HCU_MSG_BUF_LENGTH)){
	if ((len < 0) || (len > MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: No data to be pack or too long length of data content %d!!!\n", len);
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	len = len / 2;  //字节长度，而非字符串长度


	//如果长度=0,则正好，包含一个长度域=0的东东，非常好！省得底层收到的长度=1的HEART_BEAT消息
	//char conLen[3];  //1B
	sprintf(tmp, "%02X", len & 0xFF);
	strcat(s, tmp);

	//变长部分
	strcat(s, da);

	//Finish content part
	strcat(s, xmlFormat->Content_r);

	//固定尾部分
	strcat(s, xmlFormat->FuncFlag_l);
	strcat(s, xmlFormat->FuncFlag);
	strcat(s, xmlFormat->FuncFlag_r);
	strcat(s, xmlFormat->xml_r);

	//存入返回参量中
	strcpy(buf->curBuf, s);
	buf->curLen = strlen(s) + 4;
	return SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=0, cmdId=0;
	//int ret = 0;

	//检查参数
	if (rcv == NULL){
		HcuErrorPrint("CLOUDVELA: Invalid received data buffer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//控制命令不带XML格式头，接收的内容以裸控制命令，所以必须是偶数字节
	/*
	if ((rcv->length %2) != 0){
		HcuErrorPrint("CLOUDVELA: Received invalid data length by XML content format!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	*/

	//获取控制CMDID
	char tmp[3] = "";
	strncpy(tmp, &rcv->buf[index], 2);
	HcuDebugPrint("CLOUDVELA: received comId %s !\n", tmp);
	cmdId = strtoul(tmp, NULL, 16);

	if (cmdId ==L3CI_heart_beat)
	{
		if (func_cloudvela_standard_xml_heart_beat_msg_unpack(rcv) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error unpack receiving message!\n");
			return FAILURE;
		}

		return SUCCESS;
		//else{
		//	#ifdef TRACE_DEBUG_ON
		//	HcuDebugPrint("CLOUDVELA: Ack HEART_BEAT successful!\n");
		//	#endif //TRACE_DEBUG_ON
		//}
	}
/*
	// updated by Shanchun for CMD control polling
	else if (cmdId ==L3CI_cmd_control)
	{//CMD polling response from Clound: 0x00FD

		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: CMD polling Ack from Cloud: cmdID =%d\n", cmdId);
		}

		if(CMDShortTimerFlag == HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_ON)
		{
			CMDPollingNoCommandNum++;

			if(CMDPollingNoCommandNum >= HCU_CLOUDVELA_CMD_POLLING_NO_COMMAND_MAX)
			{
				CMDPollingNoCommandNum = 0;

				ret = hcu_timer_stop(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_SHORT, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Error stop timer!\n");
					return FAILURE;
				}

				CMDShortTimerFlag = HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_OFF;

				ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_LONG, zHcuSysEngPar.timer.cmdcontrolLongTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Error start timer!\n");
					return FAILURE;
				}

				CMDLongTimerFlag = HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_ON;

			}
			return SUCCESS;
		}

		return SUCCESS;

	}

	else if((cmdId==L3CI_emc)||(cmdId==L3CI_pm25)||(cmdId==L3CI_winddir)||(cmdId ==L3CI_windspd)||(cmdId ==L3CI_temp)||(cmdId ==L3CI_humid)||(cmdId ==L3CI_noise)||(cmdId ==L3CI_hsmmp)||(cmdId ==L3CI_hcu_inventory)||(cmdId ==L3CI_sw_package))
	{
		CMDPollingNoCommandNum = 0;

		if(CMDLongTimerFlag == HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_ON)
		{
			ret = hcu_timer_stop(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_LONG, TIMER_RESOLUTION_1S);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error stop timer!\n");
				return FAILURE;
			}

			CMDLongTimerFlag = HCU_CLOUDVELA_CMD_POLLING_LONG_TIMER_START_OFF;
		}

		if(CMDShortTimerFlag == HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_OFF)
		{
			ret = hcu_timer_start(TASK_ID_CLOUDVELA, TIMER_ID_1S_CLOUDVELA_PERIOD_CMD_CONTROL_SHORT, zHcuSysEngPar.timer.cmdcontrolShortTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error start timer!\n");
				return FAILURE;
			}

			CMDShortTimerFlag = HCU_CLOUDVELA_CMD_POLLING_SHORT_TIMER_START_ON;
		}
	}
*/

	switch(cmdId)
	{
		case L3CI_emc:

			if (func_cloudvela_standard_xml_emc_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of EMC!\n");
				return FAILURE;
			}
			break;

		case L3CI_pm25:
			if (func_cloudvela_standard_xml_pm25_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of PM25!\n");
				return FAILURE;
			}
			break;

		case L3CI_winddir:
			if (func_cloudvela_standard_xml_winddir_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of WINDDIR!\n");
				return FAILURE;
			}
			break;

		case L3CI_windspd:
			if (func_cloudvela_standard_xml_windspd_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of WINDSPD!\n");
				return FAILURE;
			}
			break;

		case L3CI_temp:
			if (func_cloudvela_standard_xml_temp_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of TEMP!\n");
				return FAILURE;
			}
			break;

		case L3CI_humid:
			if (func_cloudvela_standard_xml_humid_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message HUMID!\n");
				return FAILURE;
			}
			break;

		case L3CI_noise:
			if (func_cloudvela_standard_xml_noise_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of NOISE!\n");
				return FAILURE;
			}
			break;

		case L3CI_hsmmp:
			if (func_cloudvela_standard_xml_hsmmp_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of HSMMP!\n");
				return FAILURE;
			}
			break;

		case L3CI_hcu_inventory:
			if (func_cloudvela_standard_xml_hcuinventory_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of SW INVENTORY!\n");
				return FAILURE;
			}
			break;

		case L3CI_sw_package:
			if (func_cloudvela_standard_xml_swpackage_msg_unpack(rcv) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error unpack receiving message of SW PACKAGE!\n");
				return FAILURE;
			}
			break;

		default:
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Receive cloud data error with CmdId = %d\n", cmdId);
			return FAILURE;

			break;

	}

	return SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_heart_beat_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0;  //, cmdId=0
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	//cmdId = L3CI_heart_beat;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<0) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length, received length != embeded length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//没有了其它的字段，所以长度=0就对了
	if (len != 0){
		HcuErrorPrint("CLOUDVELA: Error unpack on length, HearT_beat frame, Len!=0!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//暂时没有层2的帧号，所以不用对帧号进行处理

	//也许会有其它潜在的状态转移困惑，所以这里的ONLINE状态只是为了做一定的检查，防止出现各种奇怪现象，而不是为了设置状态
	if (func_cloudvela_heart_beat_received_handle() == FAILURE){
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Heart_beat processing error!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_emc_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_emc;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_emc_min) || (it >= L3PO_emc_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_emc_data_req) || (optId == L3PO_emc_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_emc_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_emc_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		//如果想测试，可以先把这里的严格检查放开，以方便测试
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给EMC任务
		snd.length = sizeof(msg_struct_cloudvela_emc_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_EMC_DATA_REQ, TASK_ID_EMC, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_EMC]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_emc_set_switch) || (optId == L3PO_emc_set_modbus_address) || (optId == L3PO_emc_set_work_cycle) || \
			(optId == L3PO_emc_set_sample_cycle) || (optId == L3PO_emc_set_sample_number) || (optId == L3PO_emc_read_switch) || \
			(optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) || (optId == L3PO_emc_read_sample_cycle)\
			|| (optId == L3PO_emc_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_emc_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_emc_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_emc_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_emc_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_emc_read_switch) || (optId == L3PO_emc_read_modbus_address) || (optId == L3PO_emc_read_work_cycle) ||\
				(optId == L3PO_emc_read_sample_cycle) || (optId == L3PO_emc_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给EMC任务
		snd1.length = sizeof(msg_struct_cloudvela_emc_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_EMC_CONTROL_CMD, TASK_ID_EMC, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("EMC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_EMC]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_pm25_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_pm25;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_pm25_min) || (it >= L3PO_pm25_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_pm25_data_req) || (optId == L3PO_pm25_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_pm25_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_pm25_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给PM25任务
		snd.length = sizeof(msg_struct_cloudvela_pm25_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_PM25_DATA_REQ, TASK_ID_PM25, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_PM25]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_pm25_set_switch) || (optId == L3PO_pm25_set_modbus_address) || (optId == L3PO_pm25_set_work_cycle) || \
			(optId == L3PO_pm25_set_sample_cycle) || (optId == L3PO_pm25_set_sample_number) || (optId == L3PO_pm25_read_switch) || \
			(optId == L3PO_pm25_read_modbus_address) || (optId == L3PO_pm25_read_work_cycle) || (optId == L3PO_pm25_read_sample_cycle)\
			|| (optId == L3PO_pm25_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_pm25_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_pm25_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_pm25_set_switch){
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_modbus_address){
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_work_cycle){
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_sample_cycle){
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_pm25_set_sample_number){
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}


		else if ((optId == L3PO_pm25_read_switch) || (optId == L3PO_pm25_read_modbus_address) || (optId == L3PO_pm25_read_work_cycle) ||\
				(optId == L3PO_pm25_read_sample_cycle) || (optId == L3PO_pm25_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给PM25任务
		snd1.length = sizeof(msg_struct_cloudvela_pm25_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_PM25_CONTROL_CMD, TASK_ID_PM25, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_PM25]);
			return FAILURE;
		}

	}


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_winddir_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_winddir;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_winddir_min) || (it >= L3PO_winddir_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_winddir_data_req) || (optId == L3PO_winddir_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_winddir_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_winddir_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给WINDDIR任务
		snd.length = sizeof(msg_struct_cloudvela_winddir_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ, TASK_ID_WINDDIR, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_WINDDIR]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_winddir_set_switch) || (optId == L3PO_winddir_set_modbus_address) || (optId == L3PO_winddir_set_work_cycle) || \
			(optId == L3PO_winddir_set_sample_cycle) || (optId == L3PO_winddir_set_sample_number) || (optId == L3PO_winddir_read_switch) || \
			(optId == L3PO_winddir_read_modbus_address) || (optId == L3PO_winddir_read_work_cycle) || (optId == L3PO_winddir_read_sample_cycle)\
			|| (optId == L3PO_winddir_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_winddir_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_winddir_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_winddir_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_winddir_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_winddir_read_switch) || (optId == L3PO_winddir_read_modbus_address) || (optId == L3PO_winddir_read_work_cycle) ||\
				(optId == L3PO_winddir_read_sample_cycle) || (optId == L3PO_winddir_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给WINDDIR任务
		snd1.length = sizeof(msg_struct_cloudvela_winddir_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_WINDDIR_CONTROL_CMD, TASK_ID_WINDDIR, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_WINDDIR]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_windspd_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_windspd;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_windspd_min) || (it >= L3PO_windspd_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_windspd_data_req) || (optId == L3PO_windspd_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_windspd_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_windspd_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给WINDSPD任务
		snd.length = sizeof(msg_struct_cloudvela_windspd_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ, TASK_ID_WINDSPD, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_WINDSPD]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_windspd_set_switch) || (optId == L3PO_windspd_set_modbus_address) || (optId == L3PO_windspd_set_work_cycle) || \
			(optId == L3PO_windspd_set_sample_cycle) || (optId == L3PO_windspd_set_sample_number) || (optId == L3PO_windspd_read_switch) || \
			(optId == L3PO_windspd_read_modbus_address) || (optId == L3PO_windspd_read_work_cycle) || (optId == L3PO_windspd_read_sample_cycle)\
			|| (optId == L3PO_windspd_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_windspd_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_windspd_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_windspd_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_windspd_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_windspd_read_switch) || (optId == L3PO_windspd_read_modbus_address) || (optId == L3PO_windspd_read_work_cycle) ||\
				(optId == L3PO_windspd_read_sample_cycle) || (optId == L3PO_windspd_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给WINDSPD任务
		snd1.length = sizeof(msg_struct_cloudvela_windspd_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_WINDSPD_CONTROL_CMD, TASK_ID_WINDSPD, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_WINDSPD]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}


OPSTAT func_cloudvela_standard_xml_temp_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_temp;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_temp_min) || (it >= L3PO_temp_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_temp_data_req) || (optId == L3PO_temp_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_temp_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_temp_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给TEMP任务
		snd.length = sizeof(msg_struct_cloudvela_temp_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_TEMP_DATA_REQ, TASK_ID_TEMP, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_TEMP]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_temp_set_switch) || (optId == L3PO_temp_set_modbus_address) || (optId == L3PO_temp_set_work_cycle) || \
			(optId == L3PO_temp_set_sample_cycle) || (optId == L3PO_temp_set_sample_number) || (optId == L3PO_temp_read_switch) || \
			(optId == L3PO_temp_read_modbus_address) || (optId == L3PO_temp_read_work_cycle) || (optId == L3PO_temp_read_sample_cycle)\
			|| (optId == L3PO_temp_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_temp_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_temp_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_temp_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_temp_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_temp_read_switch) || (optId == L3PO_temp_read_modbus_address) || (optId == L3PO_temp_read_work_cycle) ||\
				(optId == L3PO_temp_read_sample_cycle) || (optId == L3PO_temp_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给TEMP任务
		snd1.length = sizeof(msg_struct_cloudvela_temp_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_TEMP_CONTROL_CMD, TASK_ID_TEMP, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_TEMP]);
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_humid_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_humid;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_humid_min) || (it >= L3PO_humid_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_humid_data_req) || (optId == L3PO_humid_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_humid_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_humid_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给HUMID任务
		snd.length = sizeof(msg_struct_cloudvela_humid_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_HUMID_DATA_REQ, TASK_ID_HUMID, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_HUMID]);
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
	}

	else if ((optId == L3PO_humid_set_switch) || (optId == L3PO_humid_set_modbus_address) || (optId == L3PO_humid_set_work_cycle) || \
			(optId == L3PO_humid_set_sample_cycle) || (optId == L3PO_humid_set_sample_number) || (optId == L3PO_humid_read_switch) || \
			(optId == L3PO_humid_read_modbus_address) || (optId == L3PO_humid_read_work_cycle) || (optId == L3PO_humid_read_sample_cycle)\
			|| (optId == L3PO_humid_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_humid_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_humid_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_humid_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_humid_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_humid_read_switch) || (optId == L3PO_humid_read_modbus_address) || (optId == L3PO_humid_read_work_cycle) ||\
				(optId == L3PO_humid_read_sample_cycle) || (optId == L3PO_humid_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给HUMID任务
		snd1.length = sizeof(msg_struct_cloudvela_humid_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_HUMID_CONTROL_CMD, TASK_ID_HUMID, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_HUMID]);
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_noise_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, it=0, len=0, ret=0, cmdId=0, optId=0, equId=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_noise;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((it <= L3PO_noise_min) || (it >= L3PO_noise_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	optId = it;

	//设备号，1BYTE
	if (len <=0){
		HcuErrorPrint("CLOUDVELA: Error unpack on equipment Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	it = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	equId = it;

	//根据OptId操作字分别进行进一步的分解
	if ((optId == L3PO_noise_data_req) || (optId == L3PO_noise_data_report)){

		//初始化发送函数
		msg_struct_cloudvela_noise_data_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_cloudvela_noise_data_req_t));

		snd.cmdId = cmdId;
		snd.optId = optId;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.equId = equId;

		//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
		//这里的格式是，CLOUD->HCU读取PM3:  25 02(len) 01(opt) 01(equ)
		if (len != 0 ){
			HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		//将消息命令发送给NOISE任务
		snd.length = sizeof(msg_struct_cloudvela_noise_data_req_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_NOISE_DATA_REQ, TASK_ID_NOISE, TASK_ID_CLOUDVELA, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_NOISE]);
			return FAILURE;
		}
	}

	else if ((optId == L3PO_noise_set_switch) || (optId == L3PO_noise_set_modbus_address) || (optId == L3PO_noise_set_work_cycle) || \
			(optId == L3PO_noise_set_sample_cycle) || (optId == L3PO_noise_set_sample_number) || (optId == L3PO_noise_read_switch) || \
			(optId == L3PO_noise_read_modbus_address) || (optId == L3PO_noise_read_work_cycle) || (optId == L3PO_noise_read_sample_cycle)\
			|| (optId == L3PO_noise_read_sample_number) ){

		//初始化发送函数
		msg_struct_cloudvela_noise_control_cmd_t snd1;
		memset(&snd1, 0, sizeof(msg_struct_cloudvela_noise_control_cmd_t));

		snd1.cmdId = cmdId;
		snd1.optId = optId;
		snd1.backType = L3CI_cmdid_back_type_control;
		snd1.opt.equId = equId;

		if (optId == L3PO_noise_set_switch){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器开关: 25 03(len) 02(opt) 01(equ) 00(on/off)

			//控制开关命令，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.powerOnOff = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_noise_set_modbus_address){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器地址: 25 03(len) 03(opt) 01(equ) 02(new Equipment)

			//新设备标识地址，1BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 2);
			it = strtoul(st, NULL, 16);
			index = index + 2;
			len = len-1;
			snd1.opt.newEquId = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_noise_set_work_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器工作周期: 25 05(len) 04(opt) 01(equ) 00 32(new cycle)

			//新工作周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.workCycle = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_noise_set_sample_cycle){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器采样间隔: 25 05(len) 05(opt) 01(equ) 00 32(new sample)

			//新采样周期，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.interSample = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if (optId == L3PO_noise_set_sample_number){
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//这里的格式是，CLOUD->HCU 设置传感器测量次数: 25 05(len) 06(opt) 01(equ) 00 32(new measurement)

			//新测量次数，2BYTE
			memset(st, 0, sizeof(st));
			strncpy(st, &rcv->buf[index], 4);
			it = strtoul(st, NULL, 16);
			index = index + 4;
			len = len-2;
			snd1.opt.meausTimes = it;

			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		else if ((optId == L3PO_noise_read_switch) || (optId == L3PO_noise_read_modbus_address) || (optId == L3PO_noise_read_work_cycle) ||\
				(optId == L3PO_noise_read_sample_cycle) || (optId == L3PO_noise_read_sample_number)) {
			//消息结构还需要进一步细化，这里是按照最严格的检查方式进行，一旦有错，全部拒绝，以保护HCU的安全执行
			//读取命令，不应该再跟任何字节了
			if (len != 0 ){
				HcuErrorPrint("CLOUDVELA: Error unpack on message length!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
			}
		}

		//将消息命令发送给NOISE任务
		snd1.length = sizeof(msg_struct_cloudvela_noise_control_cmd_t);
		ret = hcu_message_send(MSG_ID_CLOUDVELA_NOISE_CONTROL_CMD, TASK_ID_NOISE, TASK_ID_CLOUDVELA, &snd1, snd1.length);
		if (ret == FAILURE){
			HcuErrorPrint("CLOUDVELA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CLOUDVELA], zHcuTaskNameList[TASK_ID_NOISE]);
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

	} //end of if complex ||||||


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT func_cloudvela_standard_xml_hsmmp_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, len=0, optId=0, cmdId=0, backType=0, avUpload=0, ret=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_hsmmp;
	backType = L3CI_cmdid_back_type_control;

	//长度域，1BYTE
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	strncpy(st, &rcv->buf[index], 2);
	optId = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((optId <= L3PO_hsmmp_min) || (optId >= L3PO_hsmmp_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//AV file name to be uploaded
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	strncpy(st, &rcv->buf[index], len);
	HcuDebugPrint("CLOUDVELA: AV file name to be uploaded: %s!\n", st);

	//根据OptId操作字进行进一步的分解
	if ((optId == L3PO_hsmmp_upload_req)){
		if (func_cloudvela_av_upload(st) == SUCCESS){
			HcuDebugPrint("CLOUDVELA: AV file upload successful.\n");
			avUpload = TRUE;
		}
		else{
			HcuErrorPrint("CLOUDVELA: AV file Upload failed.\n");
			avUpload = FALSE;
		}


		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: control command cmdId= %d\n", cmdId);
			HcuDebugPrint("CLOUDVELA: control command optId= %d\n", optId);
			HcuDebugPrint("CLOUDVELA: control command backType = %d\n", backType);
			HcuDebugPrint("CLOUDVELA: control command avUpload= %d\n", avUpload);
			HcuDebugPrint("CLOUDVELA: control command avFileName= %s\n", st);
		}

	    // send resp msg to cloud: 01 successful 00: failure
		if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
			//初始化变量
			CloudDataSendBuf_t buf;
			memset(&buf, 0, sizeof(CloudDataSendBuf_t));
			//打包数据
			if (FAILURE == func_cloudvela_huanbao_av_upload_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, avUpload, st, &buf))
			{
				HcuErrorPrint("CLOUDVELA: Package message error!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				//return FAILURE;
			}
			//Send out
			ret = func_cloudvela_send_data_to_cloud(&buf);
			if ( ret == SUCCESS){
				HcuDebugPrint("CLOUDVELA: Online state, send AV Upload Resp to cloud success!\n");
				//return FAILURE;
			}
			else{
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Online state, send AV Upload Resp to cloud failure!\n");
				return FAILURE;
			}
		}
	}


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;

}


//for hcu sw invertory by shanchun
OPSTAT func_cloudvela_standard_xml_hcuinventory_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, len=0, optId=0, cmdId=0, backType=0, ret=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";
	HcuInventoryInfot hcuInventoryInfo;

	memset(&hcuInventoryInfo, 0, sizeof(HcuInventoryInfot));


	//命令字
	cmdId = L3CI_hcu_inventory;
	backType = L3CI_cmdid_back_type_control;

	//长度域，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//操作字，1BYTE
	memset(st, 0, sizeof(st));
	strncpy(st, &rcv->buf[index], 2);
	optId = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((optId <= L3PO_hcuinventory_min) || (optId >= L3PO_hcuinventory_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//根据OptId操作字进行进一步的分解
	if ((optId == L3PO_hcuinventory_req)){
		    optId = L3PO_hcuinventory_report;
			hcuInventoryInfo.hw_type = CURRENT_HW_TYPE;
			hcuInventoryInfo.hw_version = CURRENT_HW_MODULE;
			hcuInventoryInfo.sw_delivery = CURRENT_SW_RELEASE;
			hcuInventoryInfo.sw_release = CURRENT_SW_DELIVERY;

			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("CLOUDVELA: control command cmdId= %d\n", cmdId);
				HcuDebugPrint("CLOUDVELA: control command optId= %d\n", optId);
				HcuDebugPrint("CLOUDVELA: control command backType = %d\n", backType);
				HcuDebugPrint("CLOUDVELA: control command hw_type = %d\n", hcuInventoryInfo.hw_type);
				HcuDebugPrint("CLOUDVELA: control command hw_version= %d\n", hcuInventoryInfo.hw_version);
				HcuDebugPrint("CLOUDVELA: control command sw_delivery = %d\n", hcuInventoryInfo.sw_delivery);
				HcuDebugPrint("CLOUDVELA: control command sw_release= %d\n", hcuInventoryInfo.sw_release);

			}

		    // send resp msg to cloud
			if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
				//初始化变量
				CloudDataSendBuf_t buf;
				memset(&buf, 0, sizeof(CloudDataSendBuf_t));
				//打包数据
				if (FAILURE == func_cloudvela_huanbao_hcu_inventory_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, &hcuInventoryInfo, &buf))
				{
					HcuErrorPrint("CLOUDVELA: Package message error!\n");
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					//return FAILURE;
				}
				//Send out
				ret = func_cloudvela_send_data_to_cloud(&buf);
				if ( ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Online state, send HCU Inventory Resp to cloud failure!\n");
					return FAILURE;
				}

				else{
					HcuDebugPrint("CLOUDVELA: Online state, send HCU Inventory Resp to cloud success!\n");
				}
			}
	}


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//for hcu sw package by shanchun
OPSTAT func_cloudvela_standard_xml_swpackage_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv)
{
	UINT32 index=2, len=0, optId=0, cmdId=0, backType=0, swDownload=0, ret=0;
	char st[HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH] = "";

	//命令字
	cmdId = L3CI_sw_package;
	backType = L3CI_cmdid_back_type_control;

	//长度域，1BYTE
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	strncpy(st, &rcv->buf[index], 2);
	len = strtoul(st, NULL, 16);
	index = index + 2;
	if ((len<1) ||(len>MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	//确保长度域是完全一致的
	/*
	it = len*2 + 4;
	if (it != rcv->length){
		HcuErrorPrint("CLOUDVELA: Error unpack on length!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	*/

	//操作字，1BYTE
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	strncpy(st, &rcv->buf[index], 2);
	optId = strtoul(st, NULL, 16);
	index = index + 2;
	len = len-1;
	if ((optId <= L3PO_swdownload_min) || (optId >= L3PO_swdownload_max)){
		HcuErrorPrint("CLOUDVELA: Error unpack on operation Id!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//HCU file name to be downloaded
	memset(st, 0, HCU_CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH);
	//To check Len

	strncpy(st, &rcv->buf[index], len);
	HcuDebugPrint("CLOUDVELA: SW download HCU file name: %s!\n", st);

	//根据OptId操作字进行进一步的分解
	if ((optId == L3PO_swdownload_req)){

		if (func_cloudvela_sw_download(st) == SUCCESS){
			HcuDebugPrint("CLOUDVELA: HCU SW Download successful.\n");
			swDownload = TRUE;

			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("CLOUDVELA: control command cmdId= %d\n", cmdId);
				HcuDebugPrint("CLOUDVELA: control command optId= %d\n", optId);
				HcuDebugPrint("CLOUDVELA: control command backType = %d\n", backType);
				HcuDebugPrint("CLOUDVELA: control command swDownload= %d\n", swDownload);

			}

		    // send resp msg to cloud: 01 successful 00: failure

			//发送数据给后台
			if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
				//初始化变量
				CloudDataSendBuf_t buf;
				memset(&buf, 0, sizeof(CloudDataSendBuf_t));
				//打包数据
				if (FAILURE == func_cloudvela_huanbao_sw_download_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, swDownload, &buf))
				{
					HcuErrorPrint("CLOUDVELA: Package message error!\n");
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					//return FAILURE;
				}
				//Send out
				ret = func_cloudvela_send_data_to_cloud(&buf);
				if ( ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Package message error!\n");
					//return FAILURE;
				}

			}

			else{
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				HcuErrorPrint("CLOUDVELA: Error send HCU SW Download Success Resp to cloud!");
				return FAILURE;
			}

			//结束
			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
				HcuDebugPrint("CLOUDVELA: Online state, send HCU SW Download Success Resp to cloud success!\n");
			}

		    //system("reboot");

/*
		    FILE   *stream;
		    char   buf[1024];

		    memset(buf, '\0', sizeof(buf) );//初始化buf,以免后面写如乱码到文件中
		    stream = popen( "reboot", "r" ); //将“ls －l”命令的输出 通过管道读取（“r”参数）到FILE* stream

		    fread( buf, sizeof(char), sizeof(buf),  stream);  //将刚刚FILE* stream的数据流读取到buf中
<<<<<<< HEAD
		    HcuDebugPrint("CLOUDVELA: Return of popen-reboot!\n\n\n\n\n\n\n\n\n", buf);
=======
		    //fwrite( buf, 1, sizeof(buf), wstream );//将buf中的数据写到FILE    *wstream对应的流中，也是写到文件中
		    HcuDebugPrint("CLOUDVELA: Return of popen-reboot!\n", buf);
>>>>>>> dev
		    pclose( stream );
*/
			sync();
			reboot(RB_AUTOBOOT);

		}


		else{

				HcuErrorPrint("CLOUDVELA: HCU SW Download failed.\n");

				swDownload = FALSE;

				if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
					HcuDebugPrint("CLOUDVELA: control command cmdId= %d\n", cmdId);
					HcuDebugPrint("CLOUDVELA: control command optId= %d\n", optId);
					HcuDebugPrint("CLOUDVELA: control command backType = %d\n", backType);
					HcuDebugPrint("CLOUDVELA: control command swDownload= %d\n", swDownload);

				}
				// send resp msg to cloud: 00 failure

				//发送数据给后台
				if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
					//初始化变量
					CloudDataSendBuf_t buf;
					memset(&buf, 0, sizeof(CloudDataSendBuf_t));
					//打包数据
					if (func_cloudvela_huanbao_sw_download_pack(CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8, cmdId, optId, backType, swDownload, &buf) == FAILURE)
					{
						HcuErrorPrint("CLOUDVELA: Package message error!\n");
						zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
						//return FAILURE;
					}
					//Send out
					ret = func_cloudvela_send_data_to_cloud(&buf);
					if ( ret == FAILURE){
						zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
						HcuErrorPrint("CLOUDVELA: Package message error!\n");
						//return FAILURE;
					}

				}

				else{
					zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
					HcuErrorPrint("CLOUDVELA: Error send HCU SW Download Failure Resp to cloud!");
					return FAILURE;
				}

				//结束
				if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
					HcuDebugPrint("CLOUDVELA: Online state, send HCU SW Download Failure Resp to cloud success!\n");
				}

		}
	}


	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error unpack on operational Id!\n");
		return FAILURE;
	}

	return SUCCESS;
}



