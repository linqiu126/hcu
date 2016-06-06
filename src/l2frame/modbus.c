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

/*
** FSM of the MODBUS
*/
FsmStateItem_t FsmModbus[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_modbus_task_entry}, //Starting

	//System level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_modbus_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_modbus_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_MODBUS_INITED,            	fsm_modbus_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_MODBUS_INITED,            	fsm_modbus_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_MODBUS_INITED,            	fsm_com_do_nothing},

    //Active working mode, no state transfer
    {MSG_ID_COM_RESTART,        			FSM_STATE_MODBUS_ACTIVED,   fsm_modbus_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_MODBUS_ACTIVED,   fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     			FSM_STATE_MODBUS_ACTIVED,   fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  			FSM_STATE_MODBUS_ACTIVED,   fsm_com_do_nothing},
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
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

//Task level global variables，该任务是单入的，所以两个传感器同时操作是不可以的
UINT32 currentSensorEqpId;  //当前正在工作的传感器
SerialModbusMsgBuf_t currentModbusBuf;
//SerialPort_t gSerialPort = {zHcuSysEngPar.serialport.SeriesPortForModbus, zHcuSysEngPar.serialport.BautRateForMODBUSPort, 8, 'N', 1, HCU_INVALID_U16, 0, 1, 0};//initial config date for serial port
SerialPort_t gSerialPort;
//MYC 2015/12/08, For 335xD, RS485 is ttyO2 !!!

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
		hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_MODBUS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_MODBUS_INITED);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_init!\n");
		return FAILURE;
	}

	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("MODBUS: Enter FSM_STATE_MODBUS_INITED status, everything goes well!\n");
	}

	//Init global variables
	currentSensorEqpId = 0;
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	zHcuRunErrCnt[TASK_ID_MODBUS] = 0;
/*
	//基本上不设置状态机，所有操作均为同步式，这样就不需要状态机了
	ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_MODBUS_ACTIVED);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_init!\n");
		return FAILURE;
	}
*/

	gSerialPort.id = zHcuSysEngPar.serialport.SeriesPortForModbus;
	gSerialPort.nSpeed = 9600;
	gSerialPort.nBits = 8;
	gSerialPort.nEvent = 'N';
	gSerialPort.nStop = 1;
	gSerialPort.fd = HCU_INVALID_U16;
	gSerialPort.vTime = HCU_INVALID_U8;
	gSerialPort.vMin = HCU_INVALID_U8;
	gSerialPort.c_lflag = 0;

	//gSerialPort = {zHcuSysEngPar.serialport.SeriesPortForModbus, zHcuSysEngPar.serialport.BautRateForMODBUSPort, 8, 'N', 1, HCU_INVALID_U16, 0, 1, 0};//initial config date for serial port

	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
	}
	else
	{
		HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
		//基本上不设置状态机，所有操作均为同步式，这样就不需要状态机了
		ret = FsmSetState(TASK_ID_MODBUS, FSM_STATE_MODBUS_ACTIVED);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Error Set FSM State at fsm_modbus_init!\n");
			return FAILURE;
		}
	}

	SerialPortSetVtimeVmin(&gSerialPort, 1, 20);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);

	return SUCCESS;
}

OPSTAT fsm_modbus_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("MODBUS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_emc)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_emc_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus EMC req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//串口不总是好的，而且也没有EMC外设，所以EMC保持为假数据，以便测试后台
	/*
	//Init Serial Port
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
	zHcuRunErrCnt[TASK_ID_MODBUS]++;
	HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
	return ret;
	}
	else
	{
	HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 7);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
	  HcuDebugPrint("MODBUS: Send EMC req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}
	*/

	hcu_usleep(50); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	/*
	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Len %d \n ", ret);
	 HcuDebugPrint("MODBUS: Received EMC data succeed: %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
	}
	else
	{
	  HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d", ret);
	  zHcuRunErrCnt[TASK_ID_MODBUS]++;
	  return FAILURE;
	}
	*/

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_emc_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_emc_data_report_t));

	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	UINT8 sample[] = {0x05,0x03,0x02,0x12,0x34,0x44,0xF3};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	if (func_modbus_emc_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.emc.gps.ew = zHcuGpsPosInfo.EW;
	snd.emc.gps.ns = zHcuGpsPosInfo.NS;

	HcuDebugPrint("MODBUS: EW = %c, gpsx = %d, NS = %c, gpsy = %d, gpsz = %d\n",zHcuGpsPosInfo.EW, zHcuGpsPosInfo.gpsX, zHcuGpsPosInfo.NS, zHcuGpsPosInfo.gpsY, zHcuGpsPosInfo.gpsZ);

	snd.emc.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.emc.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.emc.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.emc.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.emc.emcValue = snd.emc.emcValue + (rand()%1000);

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_EMC_DATA_REPORT, TASK_ID_EMC, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_EMC]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_pm25)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_pm25_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus PM25 req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 17);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
    */

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
		HcuDebugPrint("MODBUS: Send PM25 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Len %d  \n", ret);
	 HcuDebugPrint("MODBUS: Received PM2.5 data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8],currentModbusBuf.curBuf[9],currentModbusBuf.curBuf[10],currentModbusBuf.curBuf[11],currentModbusBuf.curBuf[12],currentModbusBuf.curBuf[13],currentModbusBuf.curBuf[14],currentModbusBuf.curBuf[15],currentModbusBuf.curBuf[16]);
	}
	else
	{
	  HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d\n", ret);
	  zHcuRunErrCnt[TASK_ID_MODBUS]++;
	  return FAILURE;
	}
	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_pm25_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_pm25_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 17;
	UINT8 sample[] = {0x01,0x03,0x0C,0x01,0x02,0x03,0x04,0x11,0x12,0x13,0x14,
			0x21,0x22,0x23,0x24,0xD4,0xB8};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

	currentModbusBuf.curLen = ret;
	if (func_modbus_pm25_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.pm25.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//zPM25ConfigData.powerOnOff = 1;
	//将读取的数据回送给传感器控制器
	snd.length = sizeof(msg_struct_modbus_pm25_data_report_t);
	snd.usercmdid = rcv.cmdId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.pm25.timeStamp = time(0);

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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.pm25.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.pm25.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.pm25.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.pm25.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.pm25.gps.ew = zHcuGpsPosInfo.EW;
	snd.pm25.gps.ns = zHcuGpsPosInfo.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_PM25_DATA_REPORT, TASK_ID_PM25, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_PM25]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_winddir)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_winddir_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Prepareing send modbus winddir req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
	HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 7);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
    */

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
	  HcuDebugPrint("MODBUS: Send  winddir req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH); //获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Len %d  \n", ret);
	 HcuDebugPrint("MODBUS: Received windir data succeed: %02X %02X %02X %02X %02X %02X %02X \n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
	}
	else
	{
	  HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d \n", ret);
	  zHcuRunErrCnt[TASK_ID_MODBUS]++;
	  return FAILURE;
	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_winddir_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_winddir_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	UINT8 sample[] = {0x03,0x03,0x02,0x12,0x34,0xCC,0xF3};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

	currentModbusBuf.curLen =ret;
	if (func_modbus_winddir_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.winddir.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.winddir.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.winddir.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.winddir.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.winddir.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.winddir.gps.ew = zHcuGpsPosInfo.EW;
	snd.winddir.gps.ns = zHcuGpsPosInfo.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_WINDDIR_DATA_REPORT, TASK_ID_WINDDIR, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_WINDDIR]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_windspd)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_windspd_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus windspd req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 7);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
    */

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}
	else
	{
	  HcuDebugPrint("MODBUS: Send windspd req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Len %d  \n", ret);
	 HcuDebugPrint("MODBUS: Received windspd data succeed %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6]);
	}
	else
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d \n",  ret);
		return FAILURE;
	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_windspd_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_windspd_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 7;
	UINT8 sample[] = {0x02,0x03,0x02,0x12,0x34,0xF1,0x33};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

	currentModbusBuf.curLen = ret;
	if (func_modbus_windspd_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.windspd.equipid != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.windspd.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.windspd.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.windspd.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.windspd.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.windspd.gps.ew = zHcuGpsPosInfo.EW;
	snd.windspd.gps.ns = zHcuGpsPosInfo.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_WINDSPD_DATA_REPORT, TASK_ID_WINDSPD, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_WINDSPD]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_temp)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_temp_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus temp req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
	HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 9);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
*/
	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

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

	//从相应的从设备中读取数据

	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, 9); //获得的数据存在currentModbusBuf中
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

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_temp_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_temp_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 9;
	UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_humid)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_humid_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus humid req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 9);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
    */

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}
	else
	{
		HcuDebugPrint("MODBUS: Send humuid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
		 HcuDebugPrint("MODBUS: Len %d\n", ret);
		 HcuDebugPrint("MODBUS: Received humuid req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
	}
	else
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Can not read data from serial port, return of read %d \n", ret);
		return FAILURE;
	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_humid_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_humid_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 9;
	UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

	currentModbusBuf.curLen = ret;
	if (func_modbus_humid_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.humid.equipid != currentSensorEqpId){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		return FAILURE;
	}
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.humid.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
	snd.humid.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.humid.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.humid.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.humid.gps.ew = zHcuGpsPosInfo.EW;
	snd.humid.gps.ns = zHcuGpsPosInfo.NS;


	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_HUMID_DATA_REPORT, TASK_ID_HUMID, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_HUMID]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_noise)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_noise_msg_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		HcuErrorPrint("MODBUS: Error pack message!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send modbus noise req data = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	//Init Serial Port
	/*
	ret = hcu_sps485_serial_init(&gSerialPort);
	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Init Serial Port Failure, Exit.\n");
		return ret;
	}
	else
	{
		HcuDebugPrint("MODBUS: Init Serial Port Success ...\n");
	}

	SerialPortSetVtimeVmin(&gSerialPort, 0, 9);
	HcuDebugPrint("MODBUS: COM port flags: VTIME = 0x%d, TMIN = 0x%d\n",  gSerialPort.vTime, gSerialPort.vMin);
    */

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}
	else
	{
		HcuDebugPrint("MODBUS: Send noise req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
		 HcuDebugPrint("MODBUS: Len %d\n", ret);
		 HcuDebugPrint("MODBUS: Received noise req data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7],currentModbusBuf.curBuf[8]);
	}
	else
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Can not read data from serial port, return of read %d \n", ret);
		return FAILURE;
	}

	//对信息进行MODBUS协议的解码，包括CRC16的判断
	msg_struct_modbus_noise_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_modbus_noise_data_report_t));

	/*
	//放点假数据进行测试
	currentModbusBuf.curLen = 9;
	UINT8 sample[] = {0x06,0x03,0x04,0x12,0x34,0x56,0x78,0xF7,0xC7};
	memcpy(currentModbusBuf.curBuf, sample, currentModbusBuf.curLen);
	*/

	currentModbusBuf.curLen = ret;
	if (func_modbus_noise_msg_unpack(&currentModbusBuf, &rcv, &snd) == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error unpack message!\n");
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.noise.equipid != currentSensorEqpId){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	snd.noise.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
	snd.noise.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.noise.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.noise.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	snd.noise.gps.ew = zHcuGpsPosInfo.EW;
	snd.noise.gps.ns = zHcuGpsPosInfo.NS;

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_NOISE_DATA_REPORT, TASK_ID_NOISE, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_NOISE]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
	return SUCCESS;
}

OPSTAT func_modbus_emc_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_emc_modbus_data_read_t *rcv, msg_struct_modbus_emc_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->emc.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != EMC_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_emc_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != EMC_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_REG_DATA_PM1D0  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_REG_DATA_PM1D0 & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)((PM25_LENGTH_OF_REG >> 8) & 0x0FF) ; //长度高位 = 3个寄存器，6B长度
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(PM25_LENGTH_OF_REG & 0x0FF); //长度低位 = 3个寄存器，6B长度
		outMsg->curLen = outMsg->curLen + 1;
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

	return SUCCESS;
}

OPSTAT func_modbus_pm25_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_data_read_t *rcv, msg_struct_modbus_pm25_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0, t2=0, t3=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->pm25.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_pm25_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != PM25_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t2 = buf->curBuf[index++];
		t3 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		t2 = (t2 <<24) & 0xFF000000;
		t3 = (t3 << 16) & 0xFF0000;
		snd->pm25.pm1d0Value = t0 + t1 + t2 + t3;
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t2 = buf->curBuf[index++];
		t3 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		t2 = (t2 <<24) & 0xFF000000;
		t3 = (t3 << 16) & 0xFF0000;
		snd->pm25.pm2d5Value = t0 + t1 + t2 + t3;
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t2 = buf->curBuf[index++];
		t3 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		t2 = (t2 <<24) & 0xFF000000;
		t3 = (t3 << 16) & 0xFF0000;
		snd->pm25.pm10Value = t0 + t1 + t2 + t3;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_winddir_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDDIR_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDDIR_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
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
	return SUCCESS;
}

OPSTAT func_modbus_winddir_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_winddir_modbus_data_read_t *rcv, msg_struct_modbus_winddir_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->winddir.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != WINDDIR_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_winddir_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != WINDDIR_LENGTH_OF_REG *2){
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			return FAILURE;
		}
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t0 = (t0 <<8) & 0xFF00;
		t1 = t1 & 0xFF;
		snd->winddir.winddirValue = t0 + t1;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	outMsg->curBuf[outMsg->curLen] = (UINT8)(inMsg->equId & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//取得功能码字，目前这是唯一支持的操作命令码字
	outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY & 0x0FF);
	outMsg->curLen = outMsg->curLen + 1;

	//根据不同的操作码字OPT，进行分支操作
	switch(inMsg->optId){
	case L3PO_windspd_data_req:
		//取得寄存器地址
		outMsg->curBuf[outMsg->curLen] = (UINT8)((WINDSPD_REG_DATA_READ  >> 8) & 0x0FF); //高位
		outMsg->curLen = outMsg->curLen + 1;
		outMsg->curBuf[outMsg->curLen] = (UINT8)(WINDSPD_REG_DATA_READ & 0x0FF); //低位
		outMsg->curLen = outMsg->curLen + 1;
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
	return SUCCESS;
}

OPSTAT func_modbus_windspd_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_windspd_modbus_data_read_t *rcv, msg_struct_modbus_windspd_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->windspd.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != WINDSPD_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_windspd_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != WINDSPD_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
	return SUCCESS;
}

OPSTAT func_modbus_temp_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_temp_modbus_data_read_t *rcv, msg_struct_modbus_temp_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->temp.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != TEMP_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_temp_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != TEMP_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
	return SUCCESS;
}

OPSTAT func_modbus_humid_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_humid_modbus_data_read_t *rcv, msg_struct_modbus_humid_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->humid.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != HUMID_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_humid_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != HUMID_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
	return SUCCESS;
}

OPSTAT func_modbus_noise_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_noise_modbus_data_read_t *rcv, msg_struct_modbus_noise_data_report_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	UINT32 len=0, t0=0, t1=0, t2=0, t3=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive Modbus data error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->equId){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->noise.equipid = buf->curBuf[index];
	index++;

	//检查功能码=03
	if (buf->curBuf[index] != NOISE_MODBUS_GENERIC_FUNC_DATA_INQUERY){
		HcuErrorPrint("MODBUS: Receive Modbus data error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive Modbus data error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//根据不同的操作码字OPT，进行解码分支操作
	switch(rcv->optId){
	case L3PO_noise_data_req:
		len = buf->curBuf[index];
		index++;
		if (len != NOISE_LENGTH_OF_REG *2){
			HcuErrorPrint("MODBUS: Receive Modbus data error with data length!\n");
			zHcuRunErrCnt[TASK_ID_MODBUS]++;
			return FAILURE;
		}
		//T0-T4的四个寄存器数据顺序是串行的，跟PM25不太一样，故而这里只是顺序加总
		t0 = buf->curBuf[index++];
		t1 = buf->curBuf[index++];
		t2 = buf->curBuf[index++];
		t3 = buf->curBuf[index++];
		t0 = (t0 <<24) & 0xFF000000;
		t1 = (t1 << 16) & 0xFF0000;
		t2 = (t2 << 8) & 0xFF00;
		t3 = t3 & 0xFF;
		snd->noise.noiseValue = t0 + t1 + t2 + t3;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}


//CRC16计算方法
static UINT16 wCRC16Table[256] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

//////////////////////////////////////////////////////////////////////////
// CRC MODBUS 效验
// 输入参数: pDataIn: 数据地址
//           iLenIn: 数据长度
// 输出参数: pCRCOut: 2字节校验值
void CheckCRCModBus(UINT8* pDataIn, UINT32 iLenIn, UINT16* pCRCOut)
{
	UINT16 wHi = 0;
	UINT16 wLo = 0;
	UINT16 wCRC;
    wCRC = 0xFFFF;
    int i=0;

    for (i=0;i<iLenIn;i++)
    {
        wCRC = CalcCRCModBus(*pDataIn, wCRC);
        pDataIn++;
    }

    wHi = wCRC / 256;
    wLo = wCRC % 256;
    wCRC = (wHi << 8) | wLo;

    *pCRCOut = wCRC;
}

UINT16 CalcCRCModBus(UINT8 cDataIn, UINT16 wCRCIn)
{
    UINT16 wCheck = 0;
    wCRCIn = wCRCIn ^ cDataIn;
    int i=0;

    for(i=0;i<8;i++)
    {
        wCheck = wCRCIn & 1;
        wCRCIn = wCRCIn >> 1;
        wCRCIn = wCRCIn & 0x7fff;

        if(wCheck == 1)
        {
            wCRCIn = wCRCIn ^ 0xa001;
        }
        wCRCIn = wCRCIn & 0xffff;
    }

    return wCRCIn;
}

UINT16 crc_ccitt(unsigned char *q, int len)
{
	unsigned short crc = 0;
	while (len-- > 0)
	crc = wCRC16Table[(crc >> 8 ^ *q++) & 0xff] ^ (crc << 8);
	return crc;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.opt.equId;
	HcuDebugPrint("MODBUS: current sensor ID  = %d\n", currentSensorEqpId);

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_pm25)){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//只从MODBUS语法角度检查收到的命令的正确性，逻辑正确性由上层任务保证

	//对信息进行MODBUS协议的编码，包括CRC16的生成
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));
	ret = func_modbus_pm25_cmd_pack(&rcv, &currentModbusBuf);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error pack message!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("MODBUS: Preparing send PM25 control cmd = %02X %02x %02X %02X %02X %02X %02X %02X\n", currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	ret = hcu_sps485_serial_port_send(&gSerialPort, currentModbusBuf.curBuf, currentModbusBuf.curLen);

	if (FAILURE == ret)
	{
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Error send command to serials port!\n");
	}

	else
	{
		HcuDebugPrint("MODBUS: Send PM25 control cmd succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}

	hcu_usleep(10); //经典的操作，需要50ms的延迟，确保安全，该休眠不会被打断

	//从相应的从设备中读取数据
	memset(&currentModbusBuf, 0, sizeof(SerialModbusMsgBuf_t));

	ret = hcu_sps485_serial_port_get(&gSerialPort, currentModbusBuf.curBuf, MAX_HCU_MSG_BODY_LENGTH);//获得的数据存在currentModbusBuf中
	if (ret > 0)
	{
	 HcuDebugPrint("MODBUS: Len %d  \n", ret);
	 HcuDebugPrint("MODBUS: Received PM25 control cmd feedback succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",currentModbusBuf.curBuf[0],currentModbusBuf.curBuf[1],currentModbusBuf.curBuf[2],currentModbusBuf.curBuf[3],currentModbusBuf.curBuf[4],currentModbusBuf.curBuf[5],currentModbusBuf.curBuf[6],currentModbusBuf.curBuf[7]);
	}
	else
	{
	  HcuDebugPrint("MODBUS: Can not read data from serial port, return of read %d", ret);
	  zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查下equipmentId，确保没重入
	if (snd.opt.equId != currentSensorEqpId){
		HcuErrorPrint("MODBUS: Re-enter modbus operation by equpId used!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_INF_ON) != FALSE){

		HcuDebugPrint("modbus: control command--switch= %d\n", snd.opt.powerOnOff);
		HcuDebugPrint("modbus: control command--address= %d\n", snd.opt.newEquId);
		HcuDebugPrint("modbus: control command--workCycle= %d\n", snd.opt.workCycle);
		HcuDebugPrint("modbus: control command--interSample= %d\n", snd.opt.interSample);
		HcuDebugPrint("modbus: control command--meausTimes= %d\n", snd.opt.meausTimes);
	}

	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_MODBUS_PM25_CONTROL_FB, TASK_ID_PM25, TASK_ID_MODBUS, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		HcuErrorPrint("MODBUS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_MODBUS], zHcuTaskNameList[TASK_ID_PM25]);
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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

	return SUCCESS;
}
//Added by Shanchun for Control command
OPSTAT func_modbus_pm25_cmd_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_control_cmd_t *rcv, msg_struct_modbus_pm25_control_fb_t *snd)
{
	UINT32 index=0;
	UINT16 crc16_orin=0, crc16_gen=0;
	//UINT32 t0=0, t1=0, t2=0, t3=0;

	//检查长度
	if ((buf->curLen<=0) || (buf->curLen>MAX_HCU_MSG_BODY_LENGTH)){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with length = %d\n", buf->curLen);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	//检查地址码
	if (buf->curBuf[index] != rcv->opt.equId){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with EquId = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}
	snd->opt.equId = buf->curBuf[index];
	index++;

	//检查功能码=06

	if ((buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_SET) && (buf->curBuf[index] != PM25_MODBUS_GENERIC_FUNC_DATA_INQUERY)){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with func code = %d\n", buf->curBuf[index]);
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
	}

	index++;

	//检查CRC16
	crc16_orin = buf->curBuf[buf->curLen-1];
	crc16_orin = (crc16_orin <<8)+ buf->curBuf[buf->curLen-2];
	CheckCRCModBus(buf->curBuf, buf->curLen-2, &crc16_gen);
	if (crc16_orin != crc16_gen){
		HcuErrorPrint("MODBUS: Receive PM25 control command feedback error with CRC16 check!\n");
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
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
		zHcuRunErrCnt[TASK_ID_MODBUS]++;
		return FAILURE;
		break;
	}

	return SUCCESS;
}

