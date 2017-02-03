/*
 * spibusaries.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "spibusaries.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the SPIBUSARIES
*/
FsmStateItem_t HcuFsmSpibusaries[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_spibusaries_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_spibusaries_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_spibusaries_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_SPIBUSARIES_INITIED,            	fsm_spibusaries_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_SPIBUSARIES_INITIED,            	fsm_spibusaries_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SPIBUSARIES_INITIED,            	fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_SPIBUSARIES_ACTIVED,            	fsm_spibusaries_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SPIBUSARIES_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SPIBUSARIES_ACTIVED,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SPIBUSARIES_ACTIVED,       		fsm_com_do_nothing},


    //Active working mode, no state transfer
    {MSG_ID_TEMP_SPIBUSARIES_DATA_READ,   		FSM_STATE_SPIBUSARIES_ACTIVED,   fsm_spibusaries_temp_data_read},


    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

UINT32 currentSensorEqpId;  //当前正在工作的传感器
SPIBusariesMsgBuf_t currentSPIBuf;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spibusaries_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPIBUSARIES, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SPIBUSARIES: Error Set FSM State at fsm_spibusaries_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_spibusaries_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SPIBUSARIES, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SPIBUSARIES: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPIBUSARIES], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPIBUSARIES, FSM_STATE_SPIBUSARIES_INITED) == FAILURE){
		HcuErrorPrint("SPIBUSARIES: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_spibusaries_int_init() == FAILURE){
		HcuErrorPrint("SPIBUSARIES: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_SPIBUSARIES] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPIBUSARIES, FSM_STATE_SPIBUSARIES_ACTIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
		HcuErrorPrint("SPIBUSARIES: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SPIBUSARIES: Enter FSM_STATE_SPIBUSARIES_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_spibusaries_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_spibusaries_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_spibusaries_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_SPIBUSARIES_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_SPIBUSARIES, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
				HcuErrorPrint("SPIBUSARIES: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPIBUSARIES], zHcuTaskNameList[TASK_ID_HSMMP]);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_spibusaries_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SPIBUSARIES: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_spibusaries_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_spibusaries_int_init(void)
{
	return SUCCESS;
}



OPSTAT fsm_spibusaries_temp_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_temp_spibusaries_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_temp_spibusaries_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_temp_spibusaries_data_read_t))){
		HcuErrorPrint("SPIBUSARIES: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_temp)){
		zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentSPIBuf, 0, sizeof(SPIBusariesMsgBuf_t));
	ret = func_spibusaries_temp_msg_pack(&rcv, &currentSPIBuf);
	if (ret == FAILURE){
		HcuErrorPrint("SPIBUSARIES: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
		return FAILURE;
	}
	/*
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("SPIBUSARIES: Preparing send modbus temp req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}
	*/

    //send to STM1 with wireingPI
	/*
	ret = hcu_sps485_serial_port_send(&gSerialPortMobus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
	  HcuDebugPrint("MODBUS: Send temp req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断
    */

	//read data from STM1 with wireingPI ISR
    /*
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPortMobus, currentModbusBuf.curBuf, 9); //获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Received temp data length: %d \n ", ret);
	 HcuDebugPrint("MODBUS: Received temp data content: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
	}
	else
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d \n", ret);
		return FAILURE;
	}
    */

	//对信息进行协议的解码
	/*
	msg_struct_modbus_temp_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_temp_data_report_t));


	currentModbusBuf.curLen = ret;
	if (func_modbus_temp_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.temp.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_temp_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.temp.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_temp_data_req:
		snd.useroptid = L3PO_temp_data_report;
		break;
	case L3PO_temp_set_switch:
		snd.useroptid = L3PO_temp_read_switch;
		break;
	case L3PO_temp_set_modbus_address:
		snd.useroptid = L3PO_temp_read_modbus_address;
		break;
	case L3PO_temp_set_work_cycle:
		snd.useroptid = L3PO_temp_read_work_cycle;
		break;
	case L3PO_temp_set_sample_cycle:
		snd.useroptid = L3PO_temp_read_sample_cycle;
		break;
	case L3PO_temp_set_sample_number:
		snd.useroptid = L3PO_temp_read_sample_number;
		break;
	case L3PO_temp_data_report:
		snd.useroptid = L3PO_temp_data_report;
		break;
	case L3PO_temp_read_switch:
		snd.useroptid = L3PO_temp_read_switch;
		break;
	case L3PO_temp_read_modbus_address:
		snd.useroptid = L3PO_temp_read_modbus_address;
		break;
	case L3PO_temp_read_work_cycle:
		snd.useroptid = L3PO_temp_read_work_cycle;
		break;
	case L3PO_temp_read_sample_cycle:
		snd.useroptid = L3PO_temp_read_sample_cycle;
		break;
	case L3PO_temp_read_sample_number:
		snd.useroptid = L3PO_temp_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.temp.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.temp.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.temp.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.temp.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.temp.gps.ew = zHcuGpsPosInfo.EW;
	snd.temp.gps.ns = zHcuGpsPosInfo.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_TEMP_DATA_REPORT, TASK_ID_TEMP, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_TEMP]);
		return FAILURE;
	}
	*/

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}


OPSTAT func_spibusaries_temp_msg_pack(msg_struct_temp_spibusaries_data_read_t *inMsg, SPIBusariesMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	/*
	if (inMsg->equId > 0xFF){
		zHcuRunErrCnt[TASK_ID_SPIBUSARIES]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(TEMP_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_temp_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((TEMP_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(TEMP_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((TEMP_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(TEMP_LENGTH_OF_REG & 0x0FF); //长度低位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_temp_set_switch:
		break;

	case L3PO_temp_set_modbus_address:
		break;

	case L3PO_temp_set_work_cycle:
		break;

	case L3PO_temp_set_sample_cycle:
		break;

	case L3PO_temp_set_sample_number:
		break;

	case L3PO_temp_data_report:
		break;

	case L3PO_temp_read_switch:
		break;

	case L3PO_temp_read_modbus_address:
		break;

	case L3PO_temp_read_work_cycle:
		break;

	case L3PO_temp_read_sample_cycle:
		break;

	case L3PO_temp_read_sample_number:
		break;

	default:
		HcuErrorPrint("SPIBUSARIES: Error cmId par received during msg pack!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	CheckCRCModBus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	*/
	return SUCCESS;
}


