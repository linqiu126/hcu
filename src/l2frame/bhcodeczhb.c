/*
 * bhcodeczhb.c
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "bhcodeczhb.h"
#include "cloudvela.h"

//Task Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表


//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
//INT8 tplFormat[] = "<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>"; //send data for debug
OPSTAT func_cloudvela_huanbao_heart_beat_msg_pack(CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", L3CI_heart_beat & 0xFF);
		strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HEART_BEAT Frame XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		//未来HEART_BEAT帧可能会定义独特的命令类型（char cn[8]; //CN=命令编号），现在暂时没有细节，所以采用数据长度=0表示心跳帧
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_HEART_BEAT);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: Heart Beat Frame ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//for cmd control by Shanchun
//rcv输入参数，buf输出参数
//INT8 tplFormat[] = "<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>"; //send data for debug
OPSTAT func_cloudvela_huanbao_cmd_control_msg_pack(CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", L3CI_cmd_control & 0xFF);
		strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: CMD CONTROL CHECK Frame XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}


	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){

		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		//未来HEART_BEAT帧可能会定义独特的命令类型（char cn[8]; //CN=命令编号），现在暂时没有细节，所以采用数据长度=0表示心跳帧
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_HEART_BEAT);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: Heart Beat Frame ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}

	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
//INT8 tplFormat[] = "<xml><ToUserName><![CDATA[%s]]></ToUserName><FromUserName><![CDATA[%s]]></FromUserName><CreateTime>%s</CreateTime><MsgType><![CDATA[%s]]></MsgType><Content><![CDATA[%s]]></Content><FuncFlag>0</FuncFlag></xml>"; //send data for debug
OPSTAT func_cloudvela_huanbao_emc_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 emcValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conEmc, "%04X", emcValue & 0xFFFF);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: EMC XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conEmc, "%04X", emcValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: EMC ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_pm25_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 pm1d0Value, UINT32 pm2d5Value, UINT32 pm10Value, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conPm1d0, "%08X", pm1d0Value);
		sprintf(xmlFormat.conPm2d5, "%08X", pm2d5Value);
		sprintf(xmlFormat.conPm10d, "%08X", pm10Value);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: PM25 XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conPm2d5, "%08X", pm2d5Value);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: PM25 ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_winddir_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 winddirValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conWinddir, "%04X", winddirValue  & 0xFFFF);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: WINDDIR XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conWinddir, "%04X", winddirValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: WINDDIR ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_windspd_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 windspdValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conWindspd, "%04X", windspdValue & 0xFFFF);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: WINDSPD XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conWindspd, "%04X", windspdValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: WINDSPD ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}


	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_temp_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 tempValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conTemp, "%04X", tempValue & 0xFFFF);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: TEMP XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conTemp, "%04X", tempValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: TEMP ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_humid_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 humidValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conHumid, "%04X", humidValue & 0xFFFF);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HUMID XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conHumid, "%04X", humidValue & 0xFFFF);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: Humid ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_noise_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 noiseValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conDataFormat, "%02X",  dataFormat & 0xFF);
		sprintf(xmlFormat.conNoise, "%08X", noiseValue);
		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: NOISE XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conNoise, "%08X", noiseValue);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: Noise ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数
OPSTAT func_cloudvela_huanbao_hsmmp_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, char *linkName, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);

		sprintf(xmlFormat.conEW, "%02X", ew & 0xFF);
		sprintf(xmlFormat.conNS, "%02X", ns & 0xFF);
		sprintf(xmlFormat.conGpsx, "%08X", gpsx);
		sprintf(xmlFormat.conGpsy, "%08X", gpsy);
		sprintf(xmlFormat.conGpsz, "%08X", gpsz);

		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (strlen(linkName) > CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH){
			HcuErrorPrint("CLOUDVELA: Error FuncFlag / linkName len!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		strcpy(xmlFormat.FuncFlag, linkName);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HSMMP XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量
		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		//sprintf(zhbFormat.data.linkLen, "%04X", linkLen & 0xFFFF);
		if (strlen(linkName) > CLOUDVELA_BH_ITF_STD_XML_HEAD_MAX_LENGTH){
			HcuErrorPrint("CLOUDVELA: Error linkName len!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		strcpy(zhbFormat.data.linkName, linkName);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HSMMP ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


OPSTAT func_cloudvela_standard_zhb_pack(CloudBhItfDevReportStdZhb_t *zhbFormat, CloudDataSendBuf_t *buf)
{
	int i=0;

	//参数检查，其它参数无所谓
	if (zhbFormat == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudBhItfDevReportStdXml_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//获取整体长度
	int len = 0;
	len = strlen(zhbFormat->data.conAirprs) + strlen(zhbFormat->data.conEmc) + strlen(zhbFormat->data.conHumid) + strlen(zhbFormat->data.conPm10d)\
			+ strlen(zhbFormat->data.conPm10d) + strlen(zhbFormat->data.conPm2d5) + strlen(zhbFormat->data.conTemp) + strlen(zhbFormat->data.conWinddir)\
			+ strlen(zhbFormat->data.conWindspd);
	if ((len < 0) || ((len % 2) != 0) || (len > MAX_HCU_MSG_BUF_LENGTH)){
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: No data to be pack or too long length of data content!!!\n");
		return FAILURE;
	}

	//Len =0 就表示HEART_BEAT消息

	//准备接龙字符串成为一整串
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char tmp[CLOUDVELA_BH_ITF_STD_ZHB_DATA_SEGMENT_MAX_LENGTH];

	//char qn[21]; //请求编号 QN, 精 确 到 毫 秒 的 时 间戳:QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求，用于请求命令或通知命令
	memset(tmp, 0, sizeof(tmp));
	time_t lt;
	struct tm *cu;
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon+1;
	sprintf(tmp,  "%04d%02d%02d%02d%02d%02d%03d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday,\
			(UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec, 0);
	strcat(tmp, "___"); //变成固定20个字符长度
	strcat(s, tmp);

	//char st[6]; //ST=系统编号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%05d", 11111);
	strcat(s, tmp);

	//char cn[8]; //CN=命令编号
	//命令编号，必然从上面带下来，故而这里不再做拷贝
	//先看理想情况
	if ((zhbFormat->data.cn[0] != '\0') && (strlen(zhbFormat->data.cn) == CLOUDVELA_BH_ITF_STD_ZHB_CN_FIX_LENGTH)){
		strcat(s, zhbFormat->data.cn);
	}
	//再看长度小了，补"_"符号
	else if ((zhbFormat->data.cn[0] != '\0') && (strlen(zhbFormat->data.cn) < CLOUDVELA_BH_ITF_STD_ZHB_CN_FIX_LENGTH)){
		int i=0, j=0;
		i = strlen(zhbFormat->data.cn);
		for (j=i; j<CLOUDVELA_BH_ITF_STD_ZHB_CN_FIX_LENGTH; j++){
			strcat(zhbFormat->data.cn, "_");
		}
		strcat(s, zhbFormat->data.cn);
	}
	//最后是空的
	else{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%07d", 2222222);
		strcat(s, tmp);
	}

	//char mn[13]; //MN=设备编号
	strcpy(tmp, zHcuSysEngPar.cloud.cloudBhHcuName);
	if (strlen(tmp) <12){
		for (i = strlen(tmp); i<12; i++)
			strcpy(&tmp[i], "_");
	}
	strcpy(&tmp[12], "\0");
	strcat(s, tmp);

	//char pw[7]; //PW=访问密码
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%06d", 444444);
	strcat(s, tmp);

	//char pnum[5]; //HCU->SAE only, PNUM 指示本次通讯总共包含的包数
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04d", 555);
	strcat(s, tmp);

	//char pno[5]; //HCU->SAE only, PNO 指示当前数据包的包号
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04d", 6666);
	strcat(s, tmp);

	char da[MAX_HCU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));

	//数据区域, 气压
	if (strlen(zhbFormat->data.conAirprs) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_AIRPRS);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conAirprs);
		strcat(da, ",");
	}

	//数据区域, EMC
	if (strlen(zhbFormat->data.conEmc) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_EMC);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conEmc);
		strcat(da, ",");
	}

	//数据区域, PM25 => 有关多种PMxy组合成总颗粒物数量，待完善，这个数据的长度为4/UINT32个字节，注意不要弄的不一样
	if (strlen(zhbFormat->data.conPm2d5) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_PM25);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conPm2d5);
		strcat(da, ",");
	}

	//数据区域, WINDDIR
	if (strlen(zhbFormat->data.conWinddir) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_WINDDIR);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conWinddir);
		strcat(da, ",");
	}

	//数据区域, WINDSPD
	if (strlen(zhbFormat->data.conWindspd) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_WINDSPD);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conWindspd);
		strcat(da, ",");
	}

	//数据区域, TEMP
	if (strlen(zhbFormat->data.conTemp) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_TEMP);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conTemp);
		strcat(da, ",");
	}

	//数据区域, HUMID
	if (strlen(zhbFormat->data.conHumid) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_HUMID);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conHumid);
		strcat(da, ",");
	}

	//数据区域, NOISE,   char conNoise[9];   //4B HCU->SAE only
	if (strlen(zhbFormat->data.conNoise) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_NOISE);
		strcat(da, "=");
		strcat(da, zhbFormat->data.conNoise);
		strcat(da, ",");
	}

	//数据区域, HSMMP
	if (strlen(zhbFormat->data.linkName) > 0){
		strcat(da, CLOUDVELA_BH_ITF_STD_ZHB_HSMMP);
		strcat(da, "=");
		strcat(da, zhbFormat->data.linkName);
		strcat(da, ",");
	}

	//纯粹项目内容的数据长度区域，暂时没有不同项目分界线，不然需要使用“;”进行分开表征
	i = strlen(da);
	if ((i<0) || (i> (MAX_HCU_MSG_BUF_LENGTH - strlen(s)) )){
		HcuErrorPrint("CLOUDVELA: Too much data to be packed, exceed system limitation!!!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//继续接上数据去
	strcat(s, da);
	if(strlen(s) > MAX_HCU_MSG_BUF_LENGTH-12){
		HcuErrorPrint("CLOUDVELA: Too much data to be packed, exceed system limitation!!!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	//固定头部分
	strcpy(zhbFormat->zhb_header, "##");
	strcpy(zhbFormat->zhb_tail, "\r\n");  //<CR><LF>, 	回车（CR 或 ASCII 字符 0x0D（13）） 换行（LF 或 ASCII 字符 0x0A（10））
	sprintf(zhbFormat->dataLen, "%04d", strlen(s));

	//CRC计算，字符串当UINT8做CRC计算，可能会有潜在的风险，未来待检查
	UINT16 crc16=0;
	CheckCRCModBus((UINT8*)s, strlen(s), &crc16);
	sprintf(zhbFormat->crc16, "%04X", crc16);

	//全部成帧
	strcpy(buf->curBuf, zhbFormat->zhb_header);
	strcat(buf->curBuf, zhbFormat->dataLen);
	strcat(buf->curBuf, s);
	strcat(buf->curBuf, zhbFormat->crc16);
	strcat(buf->curBuf, zhbFormat->zhb_tail);
	buf->curLen = strlen(buf->curBuf) + 4;
	if(buf->curLen > MAX_HCU_MSG_BUF_LENGTH){
		HcuErrorPrint("CLOUDVELA: Too much data to be packed, exceed system limitation!!!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	return SUCCESS;
}

//对于扬尘项目的环保国标进行协议编解码，暂时是自定义编码
//rcv输入参数，buf输出参数

//Adding by Shanchun for control cmd
OPSTAT func_cloudvela_huanbao_pm25_cmd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 powerOnOff, UINT32 interSample, UINT32 meausTimes, UINT32 newEquId, UINT32 workCycle,CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML || zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		//sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
        //Shanchun: need to change optId, response as ACK
		switch(optId)
		{
		    case L3PO_pm25_set_switch:
		    	optId = L3PO_pm25_set_switch_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conPowerOnOff, "%02X", powerOnOff & 0xFF);
			    break;

		    case L3PO_pm25_set_modbus_address:
		    	optId = L3PO_pm25_set_modbus_address_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conNewEquId, "%02X", newEquId & 0xFF);
			    break;

		    case L3PO_pm25_set_work_cycle:
		    	optId = L3PO_pm25_set_work_cycle_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conWorkCycle, "%02X", workCycle & 0xFF);
			    break;

		    case L3PO_pm25_set_sample_cycle:
		    	optId = L3PO_pm25_set_sample_cycle_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conInterSample, "%02X", interSample & 0xFF);
			    break;

		    case L3PO_pm25_set_sample_number:
		    	optId = L3PO_pm25_set_sample_number_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conMeausTimes, "%02X", meausTimes & 0xFF);
			    break;

		    case L3PO_pm25_read_switch:
		    	optId = L3PO_pm25_read_switch_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conPowerOnOff, "%02X", powerOnOff & 0xFF);
			    break;

		    case L3PO_pm25_read_modbus_address:
		    	optId = L3PO_pm25_read_modbus_address_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conNewEquId, "%02X", newEquId & 0xFF);
			    break;

		    case L3PO_pm25_read_work_cycle:
		    	optId = L3PO_pm25_read_work_cycle_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conWorkCycle, "%02X", workCycle & 0xFF);
			    break;

		    case L3PO_pm25_read_sample_cycle:
		    	optId = L3PO_pm25_read_sample_cycle_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conInterSample, "%02X", interSample & 0xFF);
			    break;

		    case L3PO_pm25_read_sample_number:
		    	optId = L3PO_pm25_read_sample_number_ack;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conMeausTimes, "%02X", meausTimes & 0xFF);
			    break;

		    default:
			    HcuErrorPrint("CLOUDVELA: Error operation code received!\n");
			    zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			    return FAILURE;
			    break;
		}

		//sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: PM25 XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	//Shanchun: No ZHB handling for Control Cmd
	/*
	else if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB){
		//初始化变量

		CloudBhItfDevReportStdZhb_t zhbFormat;
		memset(&zhbFormat, 0, sizeof(CloudBhItfDevReportStdZhb_t));

		//关键数据填入
		sprintf(zhbFormat.data.conPm2d5, "%08X", pm2d5Value);
		strcpy(zhbFormat.data.cn, CLOUDVELA_BH_ITF_STD_ZHB_CN_NORMAL_PKG);

		if (func_cloudvela_standard_zhb_pack(&zhbFormat, buf) == FAILURE){
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: PM25 ZHB Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}
	*/

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//Adding by Shanchun for hcu sw download response
OPSTAT func_cloudvela_huanbao_sw_download_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 swDownload, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML || zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		//sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
        //Shanchun: need to change optId, response as ACK
		switch(optId)
		{
		    case L3PO_swdownload_req:
		    	optId = L3PO_swdownload_report;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
		    	sprintf(xmlFormat.conSwDownload, "%02X", swDownload & 0xFF);
			    break;

		    default:
			    HcuErrorPrint("CLOUDVELA: Error operation code received!\n");
			    zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			    return FAILURE;
			    break;
		}

		//sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HCU sw download response XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}


//Adding by Shanchun for hcu sw inventory response
OPSTAT func_cloudvela_huanbao_hcu_inventory_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, HcuInventoryInfot *hcuInventoryInfo, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML || zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);

		int i;


		switch(optId)
		{
		    case L3PO_hcuinventory_report:
		    	//optId = L3PO_hcuinventory_report;
		    	sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);

		    	char temp[2];
		    	for(i=0;i<6;i++){
					//sprintf(xmlFormat.conHwUuid[i], "%02X", hcuInventoryInfo->hw_uuid[i]);
		    		sprintf(temp, "%02X", hcuInventoryInfo->hw_uuid[i] & 0xFF);
		    		strcat(xmlFormat.conHwUuid, temp);
		    	}

				sprintf(xmlFormat.conHwType, "%02X", hcuInventoryInfo->hw_type & 0xFF);
				sprintf(xmlFormat.conHwVersion, "%04X", hcuInventoryInfo->hw_version & 0xFFFF);
				sprintf(xmlFormat.conSwDelivery, "%02X", hcuInventoryInfo->sw_delivery & 0xFF);
				sprintf(xmlFormat.conSwRelease, "%04X", hcuInventoryInfo->sw_release & 0xFFFF);
			   break;

		    default:
			    HcuErrorPrint("CLOUDVELA: Error operation code received!\n");
			    zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			    return FAILURE;
			    break;

		}

		//sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HCU sw inventory response XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//Adding by Shanchun for hcu av upload response
OPSTAT func_cloudvela_huanbao_av_upload_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 avUpload, char *avFileName, CloudDataSendBuf_t *buf)
{
	//参数检查，其它参数无所谓
	if (buf == NULL){
		HcuErrorPrint("CLOUDVELA: Error CloudDataSendBuf_t pointer!\n");
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		return FAILURE;
	}

	if (zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_XML || zHcuSysEngPar.cloud.cloudBhItfFrameStd == CLOUDVELA_BH_INTERFACE_STANDARD_ZHB)
	{
		//初始化变量
		CloudBhItfDevReportStdXml_t xmlFormat;
		memset(&xmlFormat, 0, sizeof(CloudBhItfDevReportStdXml_t));

		//pack数据到临时字符串中, 将数据打印到关键的数值中
		sprintf(xmlFormat.conCmdId, "%02X", cmdId & 0xFF);
		sprintf(xmlFormat.conBackType, "%02X", backType & 0xFF);
		//sprintf(xmlFormat.conEqpId, "%02X", equipId & 0xFF);
		//Shanchun: need to change optId, response as ACK
		switch(optId)
		{
			case L3PO_hsmmp_upload_req:
				optId = L3PO_hsmmp_upload_report;
				sprintf(xmlFormat.conOptId, "%02X", optId & 0xFF);
				sprintf(xmlFormat.conAvUpload, "%02X", avUpload & 0xFF);
				strcpy(xmlFormat.conAvFileName, avFileName);
				break;

			default:
				HcuErrorPrint("CLOUDVELA: Error operation code received!\n");
				zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
				return FAILURE;
				break;
		}

		//sprintf(xmlFormat.conTimeStamp, "%08X", timeStamp);
		if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_REPORT_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_DEVICE_CONTROL_STRING);
		else if (msgType == CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_UINT8) strcpy(xmlFormat.MsgType, CLOUDVELA_BH_MSG_TYPE_HEAT_BEAT_STRING);
		else {
			HcuErrorPrint("CLOUDVELA: Error Message Type input!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}

		if (func_cloudvela_standard_xml_pack(&xmlFormat, buf) == FAILURE){
			HcuErrorPrint("CLOUDVELA: Pack message error!\n");
			zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
			return FAILURE;
		}
		if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("CLOUDVELA: HCU video response XML Send data len=%d, String= [%s]\n", buf->curLen, buf->curBuf);
		}
	}

	else{
		zHcuRunErrCnt[TASK_ID_CLOUDVELA]++;
		HcuErrorPrint("CLOUDVELA: Not set zHcuSysEngPar.cloud.cloudBhItfFrameStd rightly!\n");
		return FAILURE;
	}

	return SUCCESS;
}

