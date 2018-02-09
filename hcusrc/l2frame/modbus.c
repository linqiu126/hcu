/*
 * modbus.c
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#include "../l2frame/modbus.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the MODBUS
*/
HcuFsmStateItem_t HcuFsmModbus[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_modbus_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_modbus_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_MODBUS_INITED,            	fsm_modbus_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_MODBUS_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_modbus_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Active working mode, no state transfer
    {MSG_ID_EMC_MODBUS_DATA_READ,       	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_emc_data_read},
	{MSG_ID_EMC_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_emc_control_cmd},
    {MSG_ID_PM25_MODBUS_DATA_READ,       	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_pm25_data_read},
	{MSG_ID_PM25_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_pm25_control_cmd},
    {MSG_ID_WINDDIR_MODBUS_DATA_READ,		FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_winddir_data_read},
	{MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,     FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_winddir_control_cmd},
    {MSG_ID_WINDSPD_MODBUS_DATA_READ,   	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_windspd_data_read},
	{MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,     FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_windspd_control_cmd},
    {MSG_ID_TEMP_MODBUS_DATA_READ,   		FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_temp_data_read},
	{MSG_ID_TEMP_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_temp_control_cmd},
    {MSG_ID_HUMID_MODBUS_DATA_READ,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_humid_data_read},
	{MSG_ID_HUMID_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_humid_control_cmd},
    {MSG_ID_NOISE_MODBUS_DATA_READ,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_noise_data_read},
	{MSG_ID_NOISE_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_noise_control_cmd},

	//Active-wait-for-feedback，该状态暂时只是为了状态协议的完整性，485还永不到
	//485接口采用了接收非阻塞式设计方式，232则采用了阻塞式。485模块的阻塞式任务模块只是装样子，由于MODBUS协议本身反馈的周期较短，这种方式暂时够用
	//232的RX程序任务模块框架采用了阻塞式，但由于RS232还没有距离应用，所以也没起作用
	//所有传感器接口（232/485/BLE/USB-CAMERA/I2S-AUDIO）均直接采用API发送，接受采用阻塞式任务框架
	//所有后台接口（ETHERNET/WIFI/USB-OTG/3G4G）也是发送TX直接API，但RX采用阻塞式任务框架
    {MSG_ID_COM_RESTART,        		FSM_STATE_MODBUS_ACTIVED_WFFB, 		fsm_modbus_restart},
    {MSG_ID_COM_INIT_FEEDBACK,			FSM_STATE_MODBUS_ACTIVED_WFFB,  	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     		FSM_STATE_MODBUS_ACTIVED_WFFB,   	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  		FSM_STATE_MODBUS_ACTIVED_WFFB,   	fsm_com_do_nothing},
	{MSG_ID_SPS232_MODBUS_DATA_RX,      FSM_STATE_MODBUS_ACTIVED_WFFB,  	fsm_modbus_sps232_data_rx},
	{MSG_ID_SPS485_MODBUS_DATA_RX,      FSM_STATE_MODBUS_ACTIVED_WFFB,  	fsm_modbus_sps485_data_rx},
	//BLE更有可能用在低速接口，用于被类似于MODBUS帧协议控制
	{MSG_ID_BLE_MODBUS_DATA_RX,      	FSM_STATE_MODBUS_ACTIVED_WFFB,  fsm_modbus_ble_data_rx},

	//added by Shanchun in case no response for the last command, to ensure not block the next command to sensor
	{MSG_ID_PM25_MODBUS_CONTROL_CMD,      	FSM_STATE_MODBUS_ACTIVED_WFFB,   fsm_modbus_pm25_control_cmd},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//extern global variables

//Task level global variables，该任务是单入的，所以两个传感器同时操作是不可以的
UINT32 currentSensorEqpId;  //当前正在工作的传感器
UINT32 InitForPMW; //控制是否需要初始化PMW粉尘仪（主动变被动式读取）
SerialModbusMsgBuf_t currentModbusBuf;
gTaskL3aqycq20Context_t		gTaskL3aqycq20Context;

gTaskModbusContext_t CurrentModusContext;



//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_modbus_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_IDLE);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_task_entry\n");
		return FAILURE;
	}
	return SUCCESS;
}

OPSTAT fsm_modbus_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_MODBUS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_MODBUS_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_init!\n");
		return FAILURE;
	}

	HCU_DEBUG_PRINT_FAT("MODBUS: Enter FSM_STATE_MODBUS_INITED status, everything goes well!\n");

	//Init global variables
	currentSensorEqpId = 0;
	InitForPMW = 0;
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS] = 0;

	memset(&CurrentModusContext, 0, sizeof(gTaskModbusContext_t));

	//基本上不设置状态机，所有操作均为同步式，这样就不需要状态机了
	ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_MODBUS_ACTIVED);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_init!\n");
		return FAILURE;
	}

	return SUCCESS;
}

OPSTAT fsm_modbus_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("MODBUS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_modbus_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_modbus_emc_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_emc_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_emc_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_emc_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_emc)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_emc_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus EMC req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);

	//串口不总是好的，而且也没有EMC外设，所以EMC保持为假数据，以便测试后台

	hcu_usleep(50); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断


	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_emc_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_emc_data_report_t));

	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	UINT8 sample[] = {0x05,0x03,0x02,0x12,0x34,0x44,0xF3};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	if (func_modbus_emc_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}
	/*
	currentModbusBuf.curLen = ret;
	if (func_modbus_emc_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}*/

	//检查下equipmentId，确保没重入
	if (snd.emc.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_emc_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.emc.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_emc_data_req:
		snd.useroptid = L3PO_emc_data_report;
		break;
	case L3PO_emc_set_switch:
		snd.useroptid = L3PO_emc_read_switch;
		break;
	case L3PO_emc_set_modbus_address:
		snd.useroptid = L3PO_emc_read_modbus_address;
		break;
	case L3PO_emc_set_work_cycle:
		snd.useroptid = L3PO_emc_read_work_cycle;
		break;
	case L3PO_emc_set_sample_cycle:
		snd.useroptid = L3PO_emc_read_sample_cycle;
		break;
	case L3PO_emc_set_sample_number:
		snd.useroptid = L3PO_emc_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.emc.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.emc.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	HCU_DEBUG_PRINT_INF("MODBUS: EW = %c, gpsx = %d, NS = %c, gpsy = %d, gpsz = %d\n",zHcuVmCtrTab.hwinv.gps.EW, zHcuVmCtrTab.hwinv.gps.gpsX, zHcuVmCtrTab.hwinv.gps.NS, zHcuVmCtrTab.hwinv.gps.gpsY, zHcuVmCtrTab.hwinv.gps.gpsZ);

	snd.emc.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.emc.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.emc.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.emc.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.emc.emcValue = snd.emc.emcValue + (rand()%1000);

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_EMC_DATA_REPORT, TASK_ID_EMC, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_EMC].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}

//需要一大堆标准化的函数：便解码，CRC16等等
//这个整体上可以做成一个大函数，供所有的不同传感器公用，而不用每一种单独搞一个不同的函数
OPSTAT fsm_modbus_pm25_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_pm25_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_pm25)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

//

	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008 || zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)//test for PMS
	{

		currentModbusBuf.curLen = 7;
		UINT8 sample1[] = {0x42,0x4D,0xE1,0x00,0x00,0x01,0x70};
		UINT8 sample2[] = {0x42,0x4D,0xE2,0x00,0x00,0x01,0x71};
		if(InitForPMW == 0)
		{
			memcpy(currentModbusBuf.curBuf, sample1, currentModbusBuf.curLen);
			InitForPMW = InitForPMW +1;
		}
		else
		{
			memcpy(currentModbusBuf.curBuf, sample2, currentModbusBuf.curLen);
		}


		HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus TSP req data to PMS = %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
		ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, currentModbusBuf.curLen);

		if (FAILURE == ret)
		{
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Error send TSP command to serials port!\n");
			gTaskL3aqycq20Context.eqtStatus.a34001_RS = OFF;

			return FAILURE;
		}

		else
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Send TSP data to PMS succeed: %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
		}
	}

	else
	{
		ret = func_modbus_pm25_msg_pack(&rcv, &currentModbusBuf);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Error pack message!\n");
			return FAILURE;
		}
		HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus TSP req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);

		ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

		if (FAILURE == ret)
		{
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Error send TSP command to serials port!\n");
			gTaskL3aqycq20Context.eqtStatus.a34001_RS = OFF;

			return FAILURE;
		}

		else
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Send TSP data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
		}

	}

//

/*
	ret = func_modbus_pm25_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus TSP req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);

	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send TSP command to serials port!\n");
		gTaskL3aqycq20Context.eqtStatus.a34001_RS = OFF;

		return FAILURE;
	}

	else
	{
		HCU_DEBUG_PRINT_INF("MODBUS: Send TSP data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}
*/



	//hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008 || zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)//test for PMS

	{
		ret = hcu_sps485_serial_port_get(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中
	}
	else
	{
		ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中
	}


	if ((ret <= 0) && (CurrentModusContext.TspHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read TSP data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.TspHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_PM25_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.TspHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read TSP data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;

		return FAILURE;
	}

	else if ((ret > 0) && (CurrentModusContext.TspHW_AlarmFlag == OFF))
	{
		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received PSW TSP data length: %d  \n", ret);
		//UINT16 hwType = 0;
		//hwType = zHcuSysEngPar.hwBurnId.hwType & 0xFFFF;
		//HCU_DEBUG_PRINT_INF("MODBUS: zHcuSysEngPar.hwBurnId.hwType in Dec 0X%x  \n", hwType);
		//HCU_DEBUG_PRINT_INF("MODBUS: zHcuSysEngPar.hwBurnId.hwType in Hex %d  \n\n\n", hwType);
		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2002 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2003 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2006 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2004 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008 || zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)//test for PMS
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2008 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16], \
					currentModbusBuf.curBuf[17],currentModbusBuf.curBuf[18],currentModbusBuf.curBuf[19],currentModbusBuf.curBuf[20],currentModbusBuf.curBuf[21],currentModbusBuf.curBuf[22],currentModbusBuf.curBuf[23],currentModbusBuf.curBuf[24],currentModbusBuf.curBuf[25],currentModbusBuf.curBuf[26],currentModbusBuf.curBuf[27],currentModbusBuf.curBuf[28],currentModbusBuf.curBuf[29],currentModbusBuf.curBuf[30],currentModbusBuf.curBuf[31]);
		}
		else  //Default取MODBUS(sps232)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received other PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10]);
		}
	}

	else if ((ret > 0) && (CurrentModusContext.TspHW_AlarmFlag == ON))
	{
		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received PSW TSP data length: %d  \n", ret);
		CurrentModusContext.TspHW_AlarmFlag = 0xFF;
		//UINT16 hwType = 0;
		//hwType = zHcuSysEngPar.hwBurnId.hwType & 0xFFFF;
		//HCU_DEBUG_PRINT_INF("MODBUS: zHcuSysEngPar.hwBurnId.hwType in Dec 0X%x  \n", hwType);
		//HCU_DEBUG_PRINT_INF("MODBUS: zHcuSysEngPar.hwBurnId.hwType in Hex %d  \n\n\n", hwType);
		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2002 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2003 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2006 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2004 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10]);
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008 || zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2100)//test for PMS
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received 2008 PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16], \
					currentModbusBuf.curBuf[17],currentModbusBuf.curBuf[18],currentModbusBuf.curBuf[19],currentModbusBuf.curBuf[20],currentModbusBuf.curBuf[21],currentModbusBuf.curBuf[22],currentModbusBuf.curBuf[23],currentModbusBuf.curBuf[24],currentModbusBuf.curBuf[25],currentModbusBuf.curBuf[26],currentModbusBuf.curBuf[27],currentModbusBuf.curBuf[28],currentModbusBuf.curBuf[29],currentModbusBuf.curBuf[30],currentModbusBuf.curBuf[31]);
		}
		else  //Default取MODBUS(sps232)
		{
			HCU_DEBUG_PRINT_INF("MODBUS: Received other PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10]);
		}

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_PM25_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		//return FAILURE;

	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_pm25_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_pm25_data_report_t));

/*/
	//放点假数据进行测试
	currentModbusBuf.curLen = 17;
	UINT8 sample[] = {0x01,0x03,0x0C,0x00,0x00,0x01,0x16,0x00,0x00,0x00,0xBA,
			0x00,0x00,0x00,0xF7,0xBC,0x88};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Len %d  \n", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
/*/
	if (func_modbus_pm25_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	HCU_DEBUG_PRINT_INF("MODBUS: rcv.equId = %d  \n", rcv.equId);
	HCU_DEBUG_PRINT_INF("MODBUS: currentSensorEqpId = %d  \n", currentSensorEqpId);
	HCU_DEBUG_PRINT_INF("MODBUS: snd.pm25.equipid = %d  \n\n\n", snd.pm25.equipid);//debug by shanchun

	//检查下equipmentId，确保没重入
	if (snd.pm25.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//zPM25ConfigData.powerOnOff = 1;
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_pm25_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.pm25.timeStamp = time(0);

	//snd.pm25.equipid = rcv.equId;//debug by shanchun,需要查为何收到的消息解出来的equId和currentSensorEqpId为何不等，是收到字节错误，还是确实重入了。

	switch(rcv.optId){
	case L3PO_pm25_data_req:
		snd.useroptid = L3PO_pm25_data_report;
		break;
	case L3PO_pm25_set_switch:
		snd.useroptid = L3PO_pm25_read_switch;
		break;
	case L3PO_pm25_set_modbus_address:
		snd.useroptid = L3PO_pm25_read_modbus_address;
		break;
	case L3PO_pm25_set_work_cycle:
		snd.useroptid = L3PO_pm25_read_work_cycle;
		break;
	case L3PO_pm25_set_sample_cycle:
		snd.useroptid = L3PO_pm25_read_sample_cycle;
		break;
	case L3PO_pm25_set_sample_number:
		snd.useroptid = L3PO_pm25_read_sample_number;
		break;
	case L3PO_pm25_data_report:
		snd.useroptid = L3PO_pm25_data_report;
		break;
	case L3PO_pm25_read_switch:
		snd.useroptid = L3PO_pm25_read_switch;
		break;
	case L3PO_pm25_read_modbus_address:
		snd.useroptid = L3PO_pm25_read_modbus_address;
		break;
	case L3PO_pm25_read_work_cycle:
		snd.useroptid = L3PO_pm25_read_work_cycle;
		break;
	case L3PO_pm25_read_sample_cycle:
		snd.useroptid = L3PO_pm25_read_sample_cycle;
		break;
	case L3PO_pm25_read_sample_number:
		snd.useroptid = L3PO_pm25_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
	{
		snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
	{
		snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006)
	{
		snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
	{
		snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	}
	else //DEFAULT取标准高配置传感器
	{
		snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	}

	snd.pm25.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.pm25.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.pm25.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.pm25.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.pm25.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_PM25_DATA_REPORT, TASK_ID_PM25, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}
OPSTAT fsm_modbus_winddir_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_winddir_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_winddir_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_winddir_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_winddir)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_winddir_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");//route to L3 or direct to cloudvela, TBD
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus WindDir req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
//
	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{

		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send WindDir command to serials port!\n");

		gTaskL3aqycq20Context.eqtStatus.a01008_RS = OFF;
		return FAILURE;
	}

	else
	{
		HCU_DEBUG_PRINT_INF("MODBUS: Send  WindDir req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX); //获得的数据存在currentModbusBuf中

	if ((ret <= 0) && (CurrentModusContext.WindDirHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read WindDir data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.WindDirHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_WINDDIR_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.WindDirHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read WindDir data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	else if ((ret > 0) && (CurrentModusContext.WindDirHW_AlarmFlag == OFF))
	{
	    currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindDir data length: %d  \n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindDir data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
	}

	else if ((ret > 0) && (CurrentModusContext.WindDirHW_AlarmFlag == ON))
	{

		CurrentModusContext.WindDirHW_AlarmFlag = OFF;
	    currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindDir data length: %d  \n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindDir data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_WINDDIR_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);


	}

//
	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_winddir_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_winddir_data_report_t));
/*//
	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	//UINT8 sample[] = {0x03,0x03,0x02,0x12,0x34,0xCC,0xF3};
	UINT8 sample[] = {0x23,0x04,0x02,0x0A,0xF0,0x47,0xD3};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Len %d  \n", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received windir data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
*///
	if (func_modbus_winddir_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.winddir.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_winddir_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.winddir.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_winddir_data_req:
		snd.useroptid = L3PO_winddir_data_report;
		break;
	case L3PO_winddir_set_switch:
		snd.useroptid = L3PO_winddir_read_switch;
		break;
	case L3PO_winddir_set_modbus_address:
		snd.useroptid = L3PO_winddir_read_modbus_address;
		break;
	case L3PO_winddir_set_work_cycle:
		snd.useroptid = L3PO_winddir_read_work_cycle;
		break;
	case L3PO_winddir_set_sample_cycle:
		snd.useroptid = L3PO_winddir_read_sample_cycle;
		break;
	case L3PO_winddir_set_sample_number:
		snd.useroptid = L3PO_winddir_read_sample_number;
		break;
	case L3PO_winddir_data_report:
		snd.useroptid = L3PO_winddir_data_report;
		break;
	case L3PO_winddir_read_switch:
		snd.useroptid = L3PO_winddir_read_switch;
		break;
	case L3PO_winddir_read_modbus_address:
		snd.useroptid = L3PO_winddir_read_modbus_address;
		break;
	case L3PO_winddir_read_work_cycle:
		snd.useroptid = L3PO_winddir_read_work_cycle;
		break;
	case L3PO_winddir_read_sample_cycle:
		snd.useroptid = L3PO_winddir_read_sample_cycle;
		break;
	case L3PO_winddir_read_sample_number:
		snd.useroptid = L3PO_winddir_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.winddir.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	snd.winddir.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.winddir.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.winddir.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.winddir.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.winddir.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_WINDDIR_DATA_REPORT, TASK_ID_WINDDIR, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_WINDDIR].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}
OPSTAT fsm_modbus_windspd_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_windspd_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_windspd_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_windspd_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_windspd)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_windspd_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus WindSpd req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
//
	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		gTaskL3aqycq20Context.eqtStatus.a01007_RS = OFF;
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send WindSpd command to serials port!\n");
		return FAILURE;

	}
	else
	{
		//CurrentModusContext.WindSpdHW_AlarmFlag = OFF;
		HCU_DEBUG_PRINT_INF("MODBUS: Send WindSpd req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

	if ((ret <= 0) && (CurrentModusContext.WindSpdHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read WindSpd data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.WindSpdHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_WINDSPD_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.WindSpdHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read WindSpd data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}


	else if ((ret > 0) && (CurrentModusContext.WindSpdHW_AlarmFlag == OFF))
	{
		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindSpd data length: %d  \n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindSpd data succeed %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
	}

	else if ((ret > 0) && (CurrentModusContext.WindSpdHW_AlarmFlag == ON))
	{

		CurrentModusContext.WindSpdHW_AlarmFlag = OFF;
		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindSpd data length: %d  \n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received WindSpd data succeed %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_WINDSPD_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);


	}

//
	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_windspd_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_windspd_data_report_t));
/*//
	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	//UINT8 sample[] = {0x02,0x03,0x02,0x12,0x34,0xF1,0x33};
	UINT8 sample[] = {0x21,0x04,0x02,0x01,0x31,0xF8,0xB3};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Len %d  \n", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received windspd data succeed %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
*///
	if (func_modbus_windspd_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.windspd.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_windspd_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.windspd.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_windspd_data_req:
		snd.useroptid = L3PO_windspd_data_report;
		break;
	case L3PO_windspd_set_switch:
		snd.useroptid = L3PO_windspd_read_switch;
		break;
	case L3PO_windspd_set_modbus_address:
		snd.useroptid = L3PO_windspd_read_modbus_address;
		break;
	case L3PO_windspd_set_work_cycle:
		snd.useroptid = L3PO_windspd_read_work_cycle;
		break;
	case L3PO_windspd_set_sample_cycle:
		snd.useroptid = L3PO_windspd_read_sample_cycle;
		break;
	case L3PO_windspd_set_sample_number:
		snd.useroptid = L3PO_windspd_read_sample_number;
		break;
	case L3PO_windspd_data_report:
		snd.useroptid = L3PO_windspd_data_report;
		break;
	case L3PO_windspd_read_switch:
		snd.useroptid = L3PO_windspd_read_switch;
		break;
	case L3PO_windspd_read_modbus_address:
		snd.useroptid = L3PO_windspd_read_modbus_address;
		break;
	case L3PO_windspd_read_work_cycle:
		snd.useroptid = L3PO_windspd_read_work_cycle;
		break;
	case L3PO_windspd_read_sample_cycle:
		snd.useroptid = L3PO_windspd_read_sample_cycle;
		break;
	case L3PO_windspd_read_sample_number:
		snd.useroptid = L3PO_windspd_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.windspd.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.windspd.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.windspd.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.windspd.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.windspd.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.windspd.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_WINDSPD_DATA_REPORT, TASK_ID_WINDSPD, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_WINDSPD].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}
OPSTAT fsm_modbus_temp_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_temp_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_temp_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_temp_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_temp)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_temp_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus Temp req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
//
	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{

		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send Temp command to serials port!\n");

		gTaskL3aqycq20Context.eqtStatus.a01001_RS = OFF;
		return FAILURE;
	}

	else
	{
	  HCU_DEBUG_PRINT_INF("MODBUS: Send Temp req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据

	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, 9); //获得的数据存在currentModbusBuf中

	if ((ret <= 0) && (CurrentModusContext.TempHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read Temp data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.TempHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_TEMP_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.TempHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read Temp data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}


	else if ((ret > 0) && (CurrentModusContext.TempHW_AlarmFlag == OFF))
	{
	 	currentModbusBuf.curLen =ret;
	 	HCU_DEBUG_PRINT_INF("MODBUS: Received Temp data length: %d \n ", ret);
	 	HCU_DEBUG_PRINT_INF("MODBUS: Received Temp data content: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
	}

	else if ((ret > 0) && (CurrentModusContext.TempHW_AlarmFlag == ON))
	{

		CurrentModusContext.TempHW_AlarmFlag = OFF;
	 	currentModbusBuf.curLen =ret;
	 	HCU_DEBUG_PRINT_INF("MODBUS: Received Temp data length: %d \n ", ret);
	 	HCU_DEBUG_PRINT_INF("MODBUS: Received Temp data content: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_TEMP_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);


	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_temp_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_temp_data_report_t));

/*//
	//放点假数据进行测试
	currentModbusBuf.curLen = 9;
	UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received temp data length: %d \n ", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received temp data content: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
*///


	if (func_modbus_temp_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.temp.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.temp.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.temp.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.temp.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.temp.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.temp.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.temp.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_TEMP_DATA_REPORT, TASK_ID_TEMP, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_TEMP].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}

OPSTAT fsm_modbus_humid_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_humid_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_humid_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_humid_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_humid)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_humid_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus Humid req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
//
	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{

		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send Humid command to serials port!\n");

		gTaskL3aqycq20Context.eqtStatus.a01002_RS = OFF;
		return FAILURE;
	}

	else
	{
		HCU_DEBUG_PRINT_INF("MODBUS: Send Humid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中

	if ((ret <= 0) && (CurrentModusContext.HumidHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read Humid data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.HumidHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_HUMID_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.HumidHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read Humid data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}


	else if ((ret > 0) && (CurrentModusContext.HumidHW_AlarmFlag == OFF))
	{
		 currentModbusBuf.curLen =ret;
		 HCU_DEBUG_PRINT_INF("MODBUS: Received Humid data length: %d\n", ret);
		 HCU_DEBUG_PRINT_INF("MODBUS: Received Humid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);

	}

	else if ((ret > 0) && (CurrentModusContext.HumidHW_AlarmFlag == ON))
	{

		CurrentModusContext.HumidHW_AlarmFlag = OFF;
		 currentModbusBuf.curLen =ret;
		 HCU_DEBUG_PRINT_INF("MODBUS: Received Humid data length: %d\n", ret);
		 HCU_DEBUG_PRINT_INF("MODBUS: Received Humid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_HUMID_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_humid_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_humid_data_report_t));
/*//
	//放点假数据进行测试
	currentModbusBuf.curLen = 9;
	UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Len %d\n", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received humuid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
*///
	if (func_modbus_humid_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.humid.equipid != currentSensorEqpId){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		return FAILURE;
	}

//////////////////////////////////////////////////////////////
//朗亿温控器测试 01 06 00 00 01 F4 89 DD （01F4:500==> 50度）
	//对信息进行MODBUS协议的编码，包括CRC16的生成
	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2009)
	{
		if (snd.humid.humidValue>=HCU_SENSOR_HUMID_VALUE_ALARM_THRESHOLD_UP)//for 温控测试
		{
			memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

			currentModbusBuf.curLen = 8;
			//UINT8 sample[] = {0x01,0x06,0x00,0x00,0x01,0xF5,0x48,0x1D};//501=50.1度
			UINT8 sample[] = {0x01,0x06,0x00,0x00,0x02,0x58,0x89,0x50};//600=60.0度
			memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);

			HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus Temp setting data to Temprature control = %02x %02X %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, currentModbusBuf.curLen);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				HcuErrorPrint("MODBUS: Error send Temparature control setting data to serials port!\n");
				//return FAILURE;
			}

			else
			{
				HCU_DEBUG_PRINT_INF("MODBUS: Send modbus Temp setting data to Temprature control succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n\n\n\n\n\n\n\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			}

			//从相应的从设备中读取数据
			memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

			ret = hcu_sps485_serial_port_get(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中



			//if ((ret <= 0) && (CurrentModusContext.TspHW_AlarmFlag == OFF))
			if (ret <= 0)
			{

				HcuErrorPrint("MODBUS: Can not read Temp setting data from serial port, return of read %d\n", ret);
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			}

			else
			{
				currentModbusBuf.curLen =ret;
				HCU_DEBUG_PRINT_INF("MODBUS: Received Temp setting data length: %d  \n", ret);
				HCU_DEBUG_PRINT_INF("MODBUS: Received Temp setting data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n\n\n\n\n\n\n\n\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			}

		}

		else if(snd.humid.humidValue<=HCU_SENSOR_HUMID_VALUE_ALARM_THRESHOLD_DOWN)
		{
			memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

			currentModbusBuf.curLen = 8;
			UINT8 sample[] = {0x01,0x06,0x00,0x00,0x00,0x00,0x89,0xCA};
			memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);

			HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus Temp setting data 0 to Temprature control = %02x %02X %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, currentModbusBuf.curLen);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				HcuErrorPrint("MODBUS: Error send Temparature control setting data to serials port!\n");
				//return FAILURE;
			}

			else
			{
				HCU_DEBUG_PRINT_INF("MODBUS: Send modbus Temp setting data to Temprature control 0 succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n\n\n\n\n\n\n\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			}

			//从相应的从设备中读取数据
			memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

			ret = hcu_sps485_serial_port_get(&(zHcuVmCtrTab.hwinv.sps232.sp), currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中



			//if ((ret <= 0) && (CurrentModusContext.TspHW_AlarmFlag == OFF))
			if (ret <= 0)
			{

				HcuErrorPrint("MODBUS: Can not read Temp setting data from serial port, return of read %d\n", ret);
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			}

			else
			{
				currentModbusBuf.curLen =ret;
				HCU_DEBUG_PRINT_INF("MODBUS: Received Temp setting data 0 length: %d  \n", ret);
				HCU_DEBUG_PRINT_INF("MODBUS: Received Temp setting data 0 succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n\n\n\n\n\n\n\n\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
			}

		}
	}
/////////////////////////////////////////////////////////////


	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_humid_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.humid.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_humid_data_req:
		snd.useroptid = L3PO_humid_data_report;
		break;
	case L3PO_humid_set_switch:
		snd.useroptid = L3PO_humid_read_switch;
		break;
	case L3PO_humid_set_modbus_address:
		snd.useroptid = L3PO_humid_read_modbus_address;
		break;
	case L3PO_humid_set_work_cycle:
		snd.useroptid = L3PO_humid_read_work_cycle;
		break;
	case L3PO_humid_set_sample_cycle:
		snd.useroptid = L3PO_humid_read_sample_cycle;
		break;
	case L3PO_humid_set_sample_number:
		snd.useroptid = L3PO_humid_read_sample_number;
		break;
	case L3PO_humid_data_report:
		snd.useroptid = L3PO_humid_data_report;
		break;
	case L3PO_humid_read_switch:
		snd.useroptid = L3PO_humid_read_switch;
		break;
	case L3PO_humid_read_modbus_address:
		snd.useroptid = L3PO_humid_read_modbus_address;
		break;
	case L3PO_humid_read_work_cycle:
		snd.useroptid = L3PO_humid_read_work_cycle;
		break;
	case L3PO_humid_read_sample_cycle:
		snd.useroptid = L3PO_humid_read_sample_cycle;
		break;
	case L3PO_humid_read_sample_number:
		snd.useroptid = L3PO_humid_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.humid.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	snd.humid.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.humid.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.humid.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.humid.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.humid.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_HUMID_DATA_REPORT, TASK_ID_HUMID, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_HUMID].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}

OPSTAT fsm_modbus_noise_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//消息参数检查
	msg_struct_noise_modbus_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_noise_modbus_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_noise_modbus_data_read_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_noise)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证
	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

	ret = func_modbus_noise_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send modbus noise req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);


    //对于LC版本，此处需要改为sps232或sps485带测试（与所有传感器共用一个端口）
	//HCU_DEBUG_PRINT_INF("MODBUS: SPS232 series port sp =%d \n", zHcuVmCtrTab.hwinv.sps232.sp.fd);
	//HCU_DEBUG_PRINT_INF("MODBUS: SPS485 series port sp =%d \n\n\n\n", zHcuVmCtrTab.hwinv.sps485.modbus.fd);
    //ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps232.sp, currentModbusBuf.curBuf, currentModbusBuf.curLen); //use the separate serial port avoid the interferace with other sensors(noise read period is 1s)

    ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send Noise command to serials port!\n");

		return FAILURE;

	}

	else
	{
		HCU_DEBUG_PRINT_INF("MODBUS: Send noise req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(MODBUS_TIMER_FOR_SERIAL_PORT_READ); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	//ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中
	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中

	if ((ret <= 0) && (CurrentModusContext.NoiseHW_AlarmFlag == OFF))
	{

		HcuErrorPrint("MODBUS: Can not read Noise data from serial port, return of read %d\n", ret);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		CurrentModusContext.NoiseHW_AlarmFlag = ON;

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_NOISE_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

		return FAILURE;

	}

	else if ((ret <= 0) && (CurrentModusContext.NoiseHW_AlarmFlag == ON))
	{
		HcuErrorPrint("MODBUS: Can not read Noise data from serial port, return of read %d\n", ret);
		return FAILURE;
	}


	else if ((ret > 0) && (CurrentModusContext.NoiseHW_AlarmFlag == OFF))
	{
		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received Noise data length %d\n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received Noise req data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);

	}

	else if ((ret > 0) && (CurrentModusContext.NoiseHW_AlarmFlag == ON))
	{

		CurrentModusContext.NoiseHW_AlarmFlag = OFF;

		currentModbusBuf.curLen =ret;
		HCU_DEBUG_PRINT_INF("MODBUS: Received Noise data length %d\n", ret);
		HCU_DEBUG_PRINT_INF("MODBUS: Received Noise req data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);

		msg_struct_com_alarm_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

		snd.length = sizeof(msg_struct_com_alarm_report_t);
		snd.usercmdid = L3CI_alarm;
		snd.useroptid = L3PO_hcualarm_report;
		snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
		snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
		snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
		snd.timeStamp = time(0);
		snd.equID = rcv.equId;
		snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
		snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_NOISE_NO_CONNECT;

		if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_MODBUS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_MODBUS, "MODBUS: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_noise_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_noise_data_report_t));

/*//
	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	//UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	UINT8 sample[] = {0x07,0x04,0x02,0x01,0x16,0xB1,0x6E};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Len %d\n", currentModbusBuf.curLen);
	HCU_DEBUG_PRINT_INF("MODBUS: Received noise req data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);

/*/

	if (func_modbus_noise_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.noise.equipid != currentSensorEqpId){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_noise_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.noise.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_noise_data_req:
		snd.useroptid = L3PO_noise_data_report;
		break;
	case L3PO_noise_set_switch:
		snd.useroptid = L3PO_noise_read_switch;
		break;
	case L3PO_noise_set_modbus_address:
		snd.useroptid = L3PO_noise_read_modbus_address;
		break;
	case L3PO_noise_set_work_cycle:
		snd.useroptid = L3PO_noise_read_work_cycle;
		break;
	case L3PO_noise_set_sample_cycle:
		snd.useroptid = L3PO_noise_read_sample_cycle;
		break;
	case L3PO_noise_set_sample_number:
		snd.useroptid = L3PO_noise_read_sample_number;
		break;
	case L3PO_noise_data_report:
		snd.useroptid = L3PO_noise_data_report;
		break;
	case L3PO_noise_read_switch:
		snd.useroptid = L3PO_noise_read_switch;
		break;
	case L3PO_noise_read_modbus_address:
		snd.useroptid = L3PO_noise_read_modbus_address;
		break;
	case L3PO_noise_read_work_cycle:
		snd.useroptid = L3PO_noise_read_work_cycle;
		break;
	case L3PO_noise_read_sample_cycle:
		snd.useroptid = L3PO_noise_read_sample_cycle;
		break;
	case L3PO_noise_read_sample_number:
		snd.useroptid = L3PO_noise_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.noise.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	snd.noise.gps.gpsx = zHcuVmCtrTab.hwinv.gps.gpsX;
	snd.noise.gps.gpsy = zHcuVmCtrTab.hwinv.gps.gpsY;
	snd.noise.gps.gpsz = zHcuVmCtrTab.hwinv.gps.gpsZ;
	snd.noise.gps.ew = zHcuVmCtrTab.hwinv.gps.EW;
	snd.noise.gps.ns = zHcuVmCtrTab.hwinv.gps.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_NOISE_DATA_REPORT, TASK_ID_NOISE, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_NOISE].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;
}

OPSTAT func_modbus_emc_msg_pack(msg_struct_emc_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(EMC_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_emc_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((EMC_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(EMC_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((EMC_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(EMC_LENGTH_OF_REG & 0x0FF); //长度低位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_emc_set_switch:
		break;

	case L3PO_emc_set_modbus_address:
		break;

	case L3PO_emc_set_work_cycle:
		break;

	case L3PO_emc_set_sample_cycle:
		break;

	case L3PO_emc_set_sample_number:
		break;

	case L3PO_emc_data_report:
		break;

	case L3PO_emc_read_switch:
		break;

	case L3PO_emc_read_modbus_address:
		break;

	case L3PO_emc_read_work_cycle:
		break;

	case L3PO_emc_read_sample_cycle:
		break;

	case L3PO_emc_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_emc_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_emc_modbus_data_read_t *rcv, msg_struct_modbus_emc_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->emc.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != EMC_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_emc_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != EMC_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->emc.emcValue = t0 + t1;
		break;

	case L3PO_emc_set_switch:
		break;

	case L3PO_emc_set_modbus_address:
		break;

	case L3PO_emc_set_work_cycle:
		break;

	case L3PO_emc_set_sample_cycle:
		break;

	case L3PO_emc_set_sample_number:
		break;

	case L3PO_emc_data_report:
		break;

	case L3PO_emc_read_switch:
		break;

	case L3PO_emc_read_modbus_address:
		break;

	case L3PO_emc_read_work_cycle:
		break;

	case L3PO_emc_read_sample_cycle:
		break;

	case L3PO_emc_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_pm25_msg_pack(msg_struct_pm25_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_pm25_data_req:
		//取得寄存器地址
		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_LOW & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_LOW & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_LOW & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG_NEW >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG_NEW & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2009)//测试朗亿LPM1051
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG_NEW_LY >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG_NEW_LY & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}
		else  //Default阿尔森4-20MA/RS485
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PMTSP_NEW_HIGH & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG_NEW >> 8) & 0x0FF) ;
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG_NEW & 0x0FF);
			outMsg->curLen = outMsg->curLen + 1;
		}

		break;

	case L3PO_pm25_set_switch:
		break;

	case L3PO_pm25_set_modbus_address:
		break;

	case L3PO_pm25_set_work_cycle:
		break;

	case L3PO_pm25_set_sample_cycle:
		break;

	case L3PO_pm25_set_sample_number:
		break;

	case L3PO_pm25_data_report:
		break;

	case L3PO_pm25_read_switch:
		break;

	case L3PO_pm25_read_modbus_address:
		break;

	case L3PO_pm25_read_work_cycle:
		break;

	case L3PO_pm25_read_sample_cycle:
		break;

	case L3PO_pm25_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;

	return SUCCESS;
}

OPSTAT func_modbus_pm25_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_data_read_t *rcv, msg_struct_modbus_pm25_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0, t2=0, t3=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->pm25.equipid = buf->curBuf[index];
	index++;

	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008)//for PMS
	{
		//检查功能码=4D
		if (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY_PMS){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n\n\n\n\n\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		index++;
/*
		if (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY_PMS_CMD){
			HcuErrorPrint("MODBUS: Receive Modbus data error with CMD = %d\n\n\n\n\n\n", buf->curBuf[index]);
		}
*/

	}

	else
	{
		//检查功能码=03
		if (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}

		//检查CRC16
		crc16_orin = buf->curBuf[buf->curLen-1];
		crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
		hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
		if (crc16_orin != crc16_gen){
			HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	index++;



	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_pm25_data_req:
		len = buf->curBuf[index];
		index++;

		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			if (len != PM25_LENGTH_OF_REG *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pmTSPValue = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm10Value = t0 + t1 + t2 + t3;
		}

		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			if (len != PM25_LENGTH_OF_REG *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pmTSPValue = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm10Value = t0 + t1 + t2 + t3;
		}

		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2006)
		{
			if (len != PM25_LENGTH_OF_REG *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pmTSPValue = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1 + t2 + t3;
			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t2 = buf->curBuf[index++];
			t3 = buf->curBuf[index++];
			t0 = (t0 <<24) & 0xFF000000;
			t1 = (t1 << 16) & 0xFF0000;
			t2 = (t2 << 8) & 0xFF00;
			t3 = t3 & 0xFF;
			snd->pm25.pm10Value = t0 + t1 + t2 + t3;
		}


		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)//阿尔森4-20MA/RS485
		{
			if (len != PM25_LENGTH_OF_REG_NEW *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm10Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pmTSPValue = t0 + t1;
		}


		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2008)//PMS
		{
			//len = buf->curBuf[index];
			index = index + 6;
			if (len != PM25_LENGTH_OF_REG_NEW_PMS *2){
				HcuErrorPrint("MODBUS: Receive Modbus data from PMS error with data length: %d!\n", len);
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm1d0Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm10Value = t0 + t1;

			snd->pm25.pmTSPValue = snd->pm25.pm1d0Value + snd->pm25.pm2d5Value + snd->pm25.pm10Value;
			HCU_DEBUG_PRINT_INF("MODBUS: PMS  PM1.0 = %d, PM2.5 = %d, PM10 = %d, TSP = %d\n\n\n\n", snd->pm25.pm1d0Value,snd->pm25.pm2d5Value,snd->pm25.pm10Value,snd->pm25.pmTSPValue);

		}

		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2009)//测试朗亿LPM1051
		{
			if (len != PM25_LENGTH_OF_REG_NEW_LY *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			//unsigned char a[ ] = {0x3D, 0x44, 0x3A, 0xDA, 0x00, 0x00, 0x00, 0x00};
			unsigned char a[ ] = {buf->curBuf[index++], buf->curBuf[index++], buf->curBuf[index++], buf->curBuf[index++], 0x00, 0x00, 0x00, 0x00};

			int i;
			for (i=0;i<8;i++) printf("%2X ",0xff & a[i]);

			float TSP = hcu_hex2float(a);
			printf("%f\n\n\n",TSP);
			snd->pm25.pm2d5Value = 1000*TSP;
			snd->pm25.pm10Value = 1000*TSP;
			snd->pm25.pmTSPValue = 1000*TSP;

		}

		else  //Default阿尔森4-20MA/RS485
		{
			if (len != PM25_LENGTH_OF_REG_NEW *2){
				HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
				return FAILURE;
			}

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm2d5Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pm10Value = t0 + t1;

			t0 = buf->curBuf[index++];
			t1 = buf->curBuf[index++];
			t0 = (t0 <<8) & 0xFF00;
			t1 = t1 & 0xFF;
			snd->pm25.pmTSPValue = t0 + t1;
		}
		break;

	case L3PO_pm25_set_switch:
		break;

	case L3PO_pm25_set_modbus_address:
		break;

	case L3PO_pm25_set_work_cycle:
		break;

	case L3PO_pm25_set_sample_cycle:
		break;

	case L3PO_pm25_set_sample_number:
		break;

	case L3PO_pm25_data_report:
		break;

	case L3PO_pm25_read_switch:
		break;

	case L3PO_pm25_read_modbus_address:
		break;

	case L3PO_pm25_read_work_cycle:
		break;

	case L3PO_pm25_read_sample_cycle:
		break;

	case L3PO_pm25_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_winddir_msg_pack(msg_struct_winddir_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU & 0x0FF);
	}
	else
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU & 0x0FF);
	}

	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_winddir_data_req:
		//取得寄存器地址

		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_REG_DATA_READ  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_REG_DATA_READ & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_REG_DATA_READ  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_REG_DATA_READ & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}
		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_REG_DATA_READ_YIGU  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_REG_DATA_READ_YIGU & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}
		else  //Default取易谷格式
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_REG_DATA_READ_YIGU  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_REG_DATA_READ_YIGU & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}

		outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_LENGTH_OF_REG & 0x0FF); //长度低位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_winddir_set_switch:
		break;

	case L3PO_winddir_set_modbus_address:
		break;

	case L3PO_winddir_set_work_cycle:
		break;

	case L3PO_winddir_set_sample_cycle:
		break;

	case L3PO_winddir_set_sample_number:
		break;

	case L3PO_winddir_data_report:
		break;

	case L3PO_winddir_read_switch:
		break;

	case L3PO_winddir_read_modbus_address:
		break;

	case L3PO_winddir_read_work_cycle:
		break;

	case L3PO_winddir_read_sample_cycle:
		break;

	case L3PO_winddir_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_winddir_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_winddir_modbus_data_read_t *rcv, msg_struct_modbus_winddir_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->winddir.equipid = buf->curBuf[index];
	index++;


	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
	{
		//检查功能码=04
		if (buf->curBuf[index] != WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
	{
		//检查功能码=04
		if (buf->curBuf[index] != WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
	{
		//检查功能码=03
		if (buf->curBuf[index] != WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	else
	{
		//检查功能码=03
		if (buf->curBuf[index] != WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_winddir_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != WINDDIR_LENGTH_OF_REG *2){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			return FAILURE;
		}
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->winddir.winddirValue = t0 + t1;
		snd->winddir.winddirValue = zHcuSysEngPar.winddircalibration;//风向校准
		if(snd->winddir.winddirValue > MODBUS_WINDDIR_CALIBRATION_MAX)
		{
			snd->winddir.winddirValue = snd->winddir.winddirValue - MODBUS_WINDDIR_CALIBRATION_MAX;
		}
		break;

	case L3PO_winddir_set_switch:
		break;

	case L3PO_winddir_set_modbus_address:
		break;

	case L3PO_winddir_set_work_cycle:
		break;

	case L3PO_winddir_set_sample_cycle:
		break;

	case L3PO_winddir_set_sample_number:
		break;

	case L3PO_winddir_data_report:
		break;

	case L3PO_winddir_read_switch:
		break;

	case L3PO_winddir_read_modbus_address:
		break;

	case L3PO_winddir_read_work_cycle:
		break;

	case L3PO_winddir_read_sample_cycle:
		break;

	case L3PO_winddir_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_windspd_msg_pack(msg_struct_windspd_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU & 0x0FF);
	}
	else
	{
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU & 0x0FF);
	}

	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_windspd_data_req:
		//取得寄存器地址

		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_REG_DATA_READ  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_REG_DATA_READ & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}

		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_REG_DATA_READ  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_REG_DATA_READ & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}

		else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_REG_DATA_READ_YIGU  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_REG_DATA_READ_YIGU & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}

		else
		{
			outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_REG_DATA_READ_YIGU  >> 8) & 0x0FF); //高位
			outMsg->curLen = outMsg->curLen + 1;
			outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_REG_DATA_READ_YIGU & 0x0FF); //低位
			outMsg->curLen = outMsg->curLen + 1;
		}

		outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_LENGTH_OF_REG & 0x0FF); //长度低位 = 1个寄存器，2B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_windspd_set_switch:
		break;

	case L3PO_windspd_set_modbus_address:
		break;

	case L3PO_windspd_set_work_cycle:
		break;

	case L3PO_windspd_set_sample_cycle:
		break;

	case L3PO_windspd_set_sample_number:
		break;

	case L3PO_windspd_data_report:
		break;

	case L3PO_windspd_read_switch:
		break;

	case L3PO_windspd_read_modbus_address:
		break;

	case L3PO_windspd_read_work_cycle:
		break;

	case L3PO_windspd_read_sample_cycle:
		break;

	case L3PO_windspd_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_windspd_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_windspd_modbus_data_read_t *rcv, msg_struct_modbus_windspd_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->windspd.equipid = buf->curBuf[index];
	index++;

	if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002)
	{
		//检查功能码=04
		if (buf->curBuf[index] != WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2003)
	{
		//检查功能码=04
		if (buf->curBuf[index] != WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}
	else if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2004)
	{
		//检查功能码=03
		if (buf->curBuf[index] != WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU){
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}
	else
	{
		//检查功能码=03
		if (buf->curBuf[index] != WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY_YIGU){
			HcuErrorPrint("MODBUS: HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002 = %d\n", HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2002);
			HcuErrorPrint("MODBUS: zHcuSysEngPar.hwBurnId.hwType = %d\n", zHcuSysEngPar.hwBurnId.hwType);
			HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
	}

	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_windspd_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != WINDSPD_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->windspd.windspdValue = t0 + t1;
		break;

	case L3PO_windspd_set_switch:
		break;

	case L3PO_windspd_set_modbus_address:
		break;

	case L3PO_windspd_set_work_cycle:
		break;

	case L3PO_windspd_set_sample_cycle:
		break;

	case L3PO_windspd_set_sample_number:
		break;

	case L3PO_windspd_data_report:
		break;

	case L3PO_windspd_read_switch:
		break;

	case L3PO_windspd_read_modbus_address:
		break;

	case L3PO_windspd_read_work_cycle:
		break;

	case L3PO_windspd_read_sample_cycle:
		break;

	case L3PO_windspd_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_temp_msg_pack(msg_struct_temp_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
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
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_temp_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_temp_modbus_data_read_t *rcv, msg_struct_modbus_temp_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->temp.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != TEMP_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_temp_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != TEMP_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		t0 = buf->curBuf[index++];  //前两个数据是温度，后面的2个数据是湿度
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->temp.tempValue = t0 + t1;
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
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_humid_msg_pack(msg_struct_humid_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(HUMID_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_humid_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((HUMID_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(HUMID_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((HUMID_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(HUMID_LENGTH_OF_REG & 0x0FF); //长度低位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_humid_set_switch:
		break;

	case L3PO_humid_set_modbus_address:
		break;

	case L3PO_humid_set_work_cycle:
		break;

	case L3PO_humid_set_sample_cycle:
		break;

	case L3PO_humid_set_sample_number:
		break;

	case L3PO_humid_data_report:
		break;

	case L3PO_humid_read_switch:
		break;

	case L3PO_humid_read_modbus_address:
		break;

	case L3PO_humid_read_work_cycle:
		break;

	case L3PO_humid_read_sample_cycle:
		break;

	case L3PO_humid_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_humid_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_humid_modbus_data_read_t *rcv, msg_struct_modbus_humid_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->humid.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != HUMID_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_humid_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != HUMID_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		index = index+2;  //前两个数据是温度，后面的2个数据是湿度
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->humid.humidValue = t0 + t1;
		break;

	case L3PO_humid_set_switch:
		break;

	case L3PO_humid_set_modbus_address:
		break;

	case L3PO_humid_set_work_cycle:
		break;

	case L3PO_humid_set_sample_cycle:
		break;

	case L3PO_humid_set_sample_number:
		break;

	case L3PO_humid_data_report:
		break;

	case L3PO_humid_read_switch:
		break;

	case L3PO_humid_read_modbus_address:
		break;

	case L3PO_humid_read_work_cycle:
		break;

	case L3PO_humid_read_sample_cycle:
		break;

	case L3PO_humid_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

OPSTAT func_modbus_noise_msg_pack(msg_struct_noise_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(NOISE_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_noise_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((NOISE_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(NOISE_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((NOISE_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(NOISE_LENGTH_OF_REG & 0x0FF); //长度低位 = 2个寄存器，4B长度
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_noise_set_switch:
		break;

	case L3PO_noise_set_modbus_address:
		break;

	case L3PO_noise_set_work_cycle:
		break;

	case L3PO_noise_set_sample_cycle:
		break;

	case L3PO_noise_set_sample_number:
		break;

	case L3PO_noise_data_report:
		break;

	case L3PO_noise_read_switch:
		break;

	case L3PO_noise_read_modbus_address:
		break;

	case L3PO_noise_read_work_cycle:
		break;

	case L3PO_noise_read_sample_cycle:
		break;

	case L3PO_noise_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;
	return SUCCESS;
}

OPSTAT func_modbus_noise_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_noise_modbus_data_read_t *rcv, msg_struct_modbus_noise_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->noise.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != NOISE_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with FuncCode = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_noise_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != NOISE_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		//T0-T4的四个寄存器数据顺序是串行的，跟PM25不太一样，故而这里只是顺序加总
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;

		snd->noise.noiseValue = t0 + t1;

		break;

	case L3PO_noise_set_switch:
		break;

	case L3PO_noise_set_modbus_address:
		break;

	case L3PO_noise_set_work_cycle:
		break;

	case L3PO_noise_set_sample_cycle:
		break;

	case L3PO_noise_set_sample_number:
		break;

	case L3PO_noise_data_report:
		break;

	case L3PO_noise_read_switch:
		break;

	case L3PO_noise_read_modbus_address:
		break;

	case L3PO_noise_read_work_cycle:
		break;

	case L3PO_noise_read_sample_cycle:
		break;

	case L3PO_noise_read_sample_number:
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}


OPSTAT fsm_modbus_sps232_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_sps485_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_ble_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_emc_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_pm25_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Edited by Shanchun for Control cmd
	int ret=0;

	msg_struct_pm25_modbus_control_cmd_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_pm25_modbus_control_cmd_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_pm25_modbus_control_cmd_t))){
		HcuErrorPrint("MODBUS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.opt.equId;
	HCU_DEBUG_PRINT_INF("MODBUS: current sensor ID  = %d\n", currentSensorEqpId);

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_pm25)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_pm25_cmd_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("MODBUS: Preparing send PM25 control cmd = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);

	ret = hcu_sps485_serial_port_send(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
		HCU_DEBUG_PRINT_INF("MODBUS: Send PM25 control cmd succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

	ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps485.modbus, currentModbusBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HCU_DEBUG_PRINT_INF("MODBUS: Len %d  \n", ret);
	 HCU_DEBUG_PRINT_INF("MODBUS: Received PM25 control cmd feedback succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}
	else
	{
	  HCU_DEBUG_PRINT_INF("MODBUS: Can not read data from serial port, return of read %d \n", ret);
	  zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
	  return FAILURE;
	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_pm25_control_fb_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_pm25_control_fb_t));
	currentModbusBuf.curLen = ret;


	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 8;
	UINT8 sample[] = {0x01,0x06,0x00,0x21,0x00,0x01,0x18,0x00}; //0x01,0x06,0x00,0x21,0x00,0x00,0xD9,0xC0
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
    */

	//currentModbusBuf.curLen = ret;
	//if (func_modbus_pm25_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
	if (func_modbus_pm25_cmd_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.opt.equId != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_pm25_control_fb_t);
	snd.cmdId = rcv.cmdId;
	snd.backType = rcv.backType;
	//snd.pm25.timeStamp = time(0);

	switch(rcv.optId){
	case L3PO_pm25_set_switch:
		snd.optId = L3PO_pm25_set_switch;
		//snd.opt.powerOnOff = rcv.opt.powerOnOff;
		break;
	case L3PO_pm25_set_modbus_address:
		snd.optId = L3PO_pm25_set_modbus_address;
		//snd.opt.newEquId = rcv.opt.newEquId;
		break;
	case L3PO_pm25_set_work_cycle:
		snd.optId = L3PO_pm25_set_work_cycle;
		//snd.opt.workCycle = rcv.opt.workCycle;
		break;
	case L3PO_pm25_set_sample_cycle:
		snd.optId = L3PO_pm25_set_sample_cycle;
		//snd.opt.interSample = rcv.opt.interSample;
		break;
	case L3PO_pm25_set_sample_number:
		snd.optId = L3PO_pm25_set_sample_number;
		//snd.opt.meausTimes = rcv.opt.meausTimes;
		break;
	case L3PO_pm25_read_switch:
		snd.optId = L3PO_pm25_read_switch;
		break;
	case L3PO_pm25_read_modbus_address:
		snd.optId = L3PO_pm25_read_modbus_address;
		break;
	case L3PO_pm25_read_work_cycle:
		snd.optId = L3PO_pm25_read_work_cycle;
		break;
	case L3PO_pm25_read_sample_cycle:
		snd.optId = L3PO_pm25_read_sample_cycle;
		break;
	case L3PO_pm25_read_sample_number:
		snd.optId = L3PO_pm25_read_sample_number;
		break;
	default:
		HcuErrorPrint("MODBUS: Error operation code received!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	HCU_DEBUG_PRINT_INF("modbus: control command--switch= %d\n", snd.opt.powerOnOff);
	HCU_DEBUG_PRINT_INF("modbus: control command--address= %d\n", snd.opt.newEquId);
	HCU_DEBUG_PRINT_INF("modbus: control command--workCycle= %d\n", snd.opt.workCycle);
	HCU_DEBUG_PRINT_INF("modbus: control command--interSample= %d\n", snd.opt.interSample);
	HCU_DEBUG_PRINT_INF("modbus: control command--meausTimes= %d\n", snd.opt.meausTimes);

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_PM25_CONTROL_FB, TASK_ID_PM25, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_PM25].taskName);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;

	return SUCCESS;
}

OPSTAT fsm_modbus_winddir_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_windspd_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_temp_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_humid_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_modbus_noise_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


//Added by Shanchun for Control command
OPSTAT func_modbus_pm25_cmd_pack(msg_struct_pm25_modbus_control_cmd_t *inMsg, SerialModbusMsgBuf_t *outMsg)
{
	UINT32 RespByteNum = 1;
	//参数不再做详细的检查，因为上层调用者已经做过严格的检查了
	//取得设备地址
	if (inMsg->opt.equId > 0xFF){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;


	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){

	case L3PO_pm25_set_switch://REG: 33	//off: 01 06 00 21 00 00 09 C0  //on: 01 06 00 21 00 01 18 00
		//取得功能码字，目前这是唯一支持的操作命令码字
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_SET & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;

		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_STOP_START  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_STOP_START & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((inMsg->opt.powerOnOff >> 8) & 0x0FF) ; //switch高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.powerOnOff & 0x0FF); //switch低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_set_modbus_address://60 //01 06 00 3C 00 09 89 C0
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_SET & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;

		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_CUR_EQU_ID  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_CUR_EQU_ID & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((inMsg->opt.newEquId >> 8) & 0x0FF) ; //address高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.newEquId & 0x0FF); //address低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_set_work_cycle://31  //01 06 00 1F 00 05 78 0F
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_SET & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;

		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_CYCLE_SUM  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_CYCLE_SUM & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((inMsg->opt.workCycle >> 8) & 0x0FF) ; //cycle高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.workCycle & 0x0FF); //cycle低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_set_sample_cycle://37 //01 06 00 26 00 06 E8 03
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_SET & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;

		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_INTERVAL  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_INTERVAL & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((inMsg->opt.interSample >> 8) & 0x0FF) ; //measure cnt高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.interSample & 0x0FF); //measure cnt低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_set_sample_number://38 //01 06 00 25 00 08 99 C7
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_SET & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;

		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_MEASURE_CNT  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_MEASURE_CNT & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((inMsg->opt.meausTimes >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->opt.meausTimes & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	//need to update
	case L3PO_pm25_read_switch: //Req: 01 03 00 21 00 01 D4 00  Resp: 01 03 02 00 01 79 84
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_STOP_START  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_STOP_START & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((RespByteNum >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(RespByteNum & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_read_modbus_address: //01 03 00 3C 00 01
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_CUR_EQU_ID  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_CUR_EQU_ID & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((RespByteNum >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(RespByteNum & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_read_work_cycle: //01 03 00 1F 00 01
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_CYCLE_SUM  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_CYCLE_SUM & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((RespByteNum >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(RespByteNum & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_read_sample_cycle: //01 03 00 26 00 01
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_INTERVAL  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_INTERVAL & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((RespByteNum >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(RespByteNum & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	case L3PO_pm25_read_sample_number: //01 03 00 25 00 01
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_MEASURE_CNT  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_MEASURE_CNT & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((RespByteNum >> 8) & 0x0FF) ; //measure times高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(RespByteNum & 0x0FF); //measure times低位
		outMsg->curLen = outMsg->curLen + 1;
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg pack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
	UINT16 crc16=0;
	hcu_vm_calculate_crc_modbus(outMsg->curBuf, outMsg->curLen, &crc16);
	outMsg->curBuf[outMsg->curLen] = (UINT8)(crc16 & 0x0FF); //高位字节
	outMsg->curLen = outMsg->curLen + 1;
	outMsg->curBuf[outMsg->curLen] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
	outMsg->curLen = outMsg->curLen + 1;

	return SUCCESS;
}
//Added by Shanchun for Control command
OPSTAT func_modbus_pm25_cmd_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_control_cmd_t *rcv, msg_struct_modbus_pm25_control_fb_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	//UINT32 t0=0, t1=0, t2=0, t3=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>HCU_SYSDIM_MSG_BODY_LEN_MAX)){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with length = %d\n", buf->curLen);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->opt.equId){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with EquId = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->opt.equId = buf->curBuf[index];
	index++;

	//检查功能码=06

	if ((buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_SET) && (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY)){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with func code = %d\n", buf->curBuf[index]);
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	hcu_vm_calculate_crc_modbus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with CRC16 check!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}


	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){

	case L3PO_pm25_set_switch:
		index=index+3;
		snd->opt.powerOnOff = buf->curBuf[index];//01 06 00 21 00 01 18 00
		break;

	case L3PO_pm25_set_modbus_address: //01 06 00 3C 00 09 89 C0
		index=index+3;
		snd->opt.newEquId = buf->curBuf[index];
		break;

	case L3PO_pm25_set_work_cycle: //01 06 00 1F 00 09
		index=index+3;
		snd->opt.workCycle = buf->curBuf[index];
		break;

	case L3PO_pm25_set_sample_cycle://01 06 00 26 00 09
		index=index+3;
		snd->opt.interSample = buf->curBuf[index];
		break;

	case L3PO_pm25_set_sample_number://01 06 00 25 00 09
		index=index+3;
		snd->opt.meausTimes = buf->curBuf[index];
		break;

	case L3PO_pm25_read_switch:
		index=index+2;
		snd->opt.powerOnOff = buf->curBuf[index];//01 03 02 00 01 79 84
		break;

	case L3PO_pm25_read_modbus_address://01 03 02 00 01 79 84
		index=index+2;
		snd->opt.newEquId = buf->curBuf[index];
		break;

	case L3PO_pm25_read_work_cycle://01 03 02 00 03 F8 45
		index=index+2;
		snd->opt.workCycle = buf->curBuf[index];
		break;

	case L3PO_pm25_read_sample_cycle://01 03 02 00 08 B9 82
		index=index+2;
		snd->opt.interSample = buf->curBuf[index];
		break;

	case L3PO_pm25_read_sample_number: //01 03 02 00 09 78 42
		index=index+2;
		snd->opt.meausTimes = buf->curBuf[index];
		/*index=index+2;
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t2 = buf->curBuf[index++];
		t3 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		t2 = (t2 <<24) & 0xFF000000;
		t3 = (t3 << 16) & 0xFF0000;
		snd->opt.meausTimes = t0 + t1 + t2 + t3;*/
		break;

	default:
		HcuErrorPrint("MODBUS: Error cmId par received during msg unpack!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}




long long int hcu_getS(int e,int m)
{
    long long int s=e;
    int i;
    for (i=1;i<m;i++)
    {
        s*=e;
    }
    return s;
}

float hcu_hex2float(unsigned char *p)
{
    long long int a=0x00000000;

    a=a|p[0];
    a=(a<<8)|p[1];
    a=(a<<8)|p[2];
    a=(a<<8)|p[3];

    //获得符号位，1表示负数，0表示正数
    int s=(a>>31)&0xFF;
    int e=(a>>23)&0x0FF;
    //获得指数
    e=e-127;
    //获得底数
    long long int m = (a&0x7FFFFF)|0x800000;
    long long int c=0;
    float v = 0.0f, y = 1.0f;
    //向右移动
    if (e>0)
    {
        //获得整数的二进制
        c=(m>>(23-e))&0xFFFFFFF;
        long int b=0;
        int i;
        for (i=0;i<23-e;i++)
        {
            b=(b<<1)|0x01;
        }
        //获得小数的二进制
        b=b&m;
        int j=0;
        //int i;
        for (i=23-e-1;i>=0;i--)
        {
            j++;
            y=(double)(((b>>i)&0x01)*hcu_getS(2,j));
            if (y>0.0)
            {
                v+=1.0/y;
            }
        }
        v=c+v;
        if (s>0)
        {
            v=-v;
        }
    }
    else
    {
        //向左移动
        e=-e;
        c=m;
        int j=0;
        int i;
        for (i=23+e-1;i>=0;i--)
        {
            j++;
            y=(float)(((c>>i)&0x01)*hcu_getS(2,j));
            if (y>0.0)
            {
                v+=1.0/y;
            }
        }
        if (s>0)
        {
            v=-v;
        }
    }

    //result = v;
    return v;
}



