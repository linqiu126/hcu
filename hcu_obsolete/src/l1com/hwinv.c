/*
 * hwinv.c
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#include "../l1com/hwinv.h"

#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
/*
** FSM of the HWINV
*/
HcuFsmStateItem_t FsmHwinv[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_hwinv_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_hwinv_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_hwinv_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
	//在激活状态下，已经进入死循环，这些消息，暂时没啥用，也接收不到，所以也不能由其它任务发送而来，小心处理
    {MSG_ID_COM_RESTART,        FSM_STATE_HWINV_ACTIVED,            	fsm_hwinv_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_HWINV_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_HWINV_ACTIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_HWINV_ACTIVED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

/*
 *  设备的标识符定义，统一放在了L1COMDEF.H文件中，需要根据不同设备进行同意修改
 *
 */

//Global variables
HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表
HcuHwinvCtrlTable_t zHcuVmCtrTab.hwinv; //硬件列表
HcuTimeDateTable_t zHcuVmCtrTab.clock;   //时间更新表

// System Information
SysInfo_t gSysInfo;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_hwinv_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_HWINV, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("HWINV: Error Set FSM State at fsm_hwinv_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_hwinv_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_HWINV, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_HWINV, FSM_STATE_HWINV_INITED) == FAILURE){
		HcuErrorPrint("HWINV: Error Set FSM State!\n");
		return FAILURE;
	}

	//INIT this task global variables
	if (func_hwinv_global_par_init() == FAILURE){
		HcuErrorPrint("HWINV: Error init parameters!\n");
		return FAILURE;
	}

	//输出版本/调试/Copyright版权信息
	func_hwinv_copy_right();


	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_HWINV, FSM_STATE_HWINV_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("HWINV: Enter FSM_STATE_HWINV_ACTIVED status, Keeping refresh here!\n");
	}

	//设置并读取数据库中的工程配置参数
	//这里的原则是：读取数据库中的工程参数，如果读取出错或者配置的参数超过检查范围，则直接使用SYSCONF.H中定义的缺省工程参数，以防止应用崩溃
	//放弃这里的读取，直接从HCU.c初始化时读取，这样更为安全。
/*	if (hcu_hwinv_read_engineering_data_into_mem() == FAILURE){
		zHcuRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Not set all engineering data correctly!\n");
		return FAILURE;
	}*/

	//进入等待反馈状态
	while(1){
		func_hwinv_scan_all();
		hcu_sleep(15);  //休息15S后，继续重复SCAN所有的硬件状态
		//收下消息，防止消息BUFFER爆满而出错
		//如果出现阻塞现象，说明这里不应该收消息，注意观察
		HcuMsgSruct_t rcv;
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv_no_wait(TASK_ID_HWINV, &rcv);
	}

	return SUCCESS;
}

OPSTAT fsm_hwinv_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("HWINV: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_hwinv_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_hwinv_global_par_init(void)
{
	time_t lt;
	struct tm *cu;
	DIR *dtmp;
	char stmp[HCU_DIR_LENGTH_MAX];

	//赋初值
	memset(&zHcuVmCtrTab.hwinv, 0xFF, sizeof(HcuHwinvCtrlTable_t));
	memset(&zHcuVmCtrTab.clock, 0xFF, sizeof(HcuTimeDateTable_t));
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV] = 0;

	//初始本地时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
	zHcuVmCtrTab.clock.year = cu->tm_year;
	zHcuVmCtrTab.clock.mon = cu->tm_mon;
	zHcuVmCtrTab.clock.day = cu->tm_mday;
	zHcuVmCtrTab.clock.hour = cu->tm_hour;
	zHcuVmCtrTab.clock.min = cu->tm_min;
	zHcuVmCtrTab.clock.sec = cu->tm_sec;

	//strftime(zCurDayString, 8, "%Y%m%d", cu);
	//strftime(zCurHourString,10, "%Y%m%d%H", cu);
	//strftime(zCurMinString, 12, "%Y%m%d%H%M", cu);
	sprintf(zHcuVmCtrTab.clock.sYear, "%04d", (UINT16)(1900+cu->tm_year));
	sprintf(zHcuVmCtrTab.clock.sMon,  "%04d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon);
	sprintf(zHcuVmCtrTab.clock.sDay,  "%04d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday);
	sprintf(zHcuVmCtrTab.clock.sHour, "%04d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour);
	sprintf(zHcuVmCtrTab.clock.sMin,  "%04d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min);
	sprintf(zHcuVmCtrTab.clock.sSec,  "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	sprintf(zHcuVmCtrTab.clock.strYear, "./%04d", (UINT16)(1900+cu->tm_year));
	sprintf(zHcuVmCtrTab.clock.strMon,  "./%04d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon);
	sprintf(zHcuVmCtrTab.clock.strDay,  "./%04d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday);
	sprintf(zHcuVmCtrTab.clock.strHour, "./%04d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour);
	sprintf(zHcuVmCtrTab.clock.strMin,  "./%04d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min);
	sprintf(zHcuVmCtrTab.clock.strSec,  "./%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	zHcuVmCtrTab.clock.grad = cu->tm_hour * 60 + cu->tm_min;

	//获取当前工作目录，并存入全局表
	if (getcwd(zHcuVmCtrTab.clock.curHcuDir, sizeof(zHcuVmCtrTab.clock.curHcuDir)-1) == NULL){
		HcuErrorPrint("HWINV: Error get current working directory!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		return FAILURE;
	}
	if (strlen(zHcuVmCtrTab.clock.curHcuDir) > (HCU_DIR_LENGTH_MAX - 30)){
		strcpy(zHcuVmCtrTab.clock.curHcuDir, "/home"); //强行设置工作目录
		HcuErrorPrint("HWINV: Too deep working directory!\n");
	}

	//给curLogDir以及curYmDir赋值
	strcpy(stmp, zHcuVmCtrTab.clock.curHcuDir);
	strcat(stmp, HCU_RECORD_LOG_DIR_NAME_CLEAN);
	strcpy(zHcuVmCtrTab.clock.curLogDir, stmp);
	strcat(stmp, "/");
	strcat(stmp, zHcuVmCtrTab.clock.sMon);
	strcpy(zHcuVmCtrTab.clock.curYmDir, stmp);

	//创建备份记录的基本工作目录，如果存在，就直接使用
	dtmp = opendir(zHcuVmCtrTab.clock.curLogDir);
	if (dtmp == NULL){
		if (mkdir(HCU_RECORD_LOG_DIR_NAME_LOCAL, 0755) == -1){
			HcuErrorPrint("HWINV: Create directory error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
			return FAILURE;
		}
		//成功就不打印提示了
	}else{
		//HcuDebugPrint("HWINV: Dir exist!\n");
		closedir(dtmp);
	}

	//创建YYMM子目录
	dtmp = opendir(zHcuVmCtrTab.clock.curYmDir);
	if (dtmp == NULL){
		chdir(zHcuVmCtrTab.clock.curLogDir);
		if (mkdir(zHcuVmCtrTab.clock.sMon, 0755) == -1){
			HcuErrorPrint("HWINV: Create directory error!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
			return FAILURE;
		}
		chdir(zHcuVmCtrTab.clock.curHcuDir);
		//成功就不打印提示了
	}else{
		//HcuDebugPrint("HWINV: Dir exist!\n");
		closedir(dtmp);
	}

	//创建文件名字
	//Sensor是天级别
	strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);
	strcat(stmp, HCU_RECORD_FILE_NAME_SENSOR);
	strcat(stmp, zHcuVmCtrTab.clock.sDay);
	strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND);
	strcpy(zHcuVmCtrTab.clock.curSensorFd, stmp);

	//Microphone是小时级别
	strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);
	strcat(stmp, HCU_RECORD_FILE_NAME_MICROPHONE);
	strcat(stmp, zHcuVmCtrTab.clock.sHour);
	strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND);
	strcpy(zHcuVmCtrTab.clock.curMicrophoneFd, stmp);

	//Avorion是分钟级别
	strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);
	//strcat(stmp, HCU_RECORD_FILE_NAME_AVORION);
	strcat(stmp, "/");
	strcat(stmp,zHcuSysEngPar.cloud.cloudBhHcuName);
	strcat(stmp,"_");
	strcat(stmp, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
	strcat(stmp, zHcuVmCtrTab.clock.sMin);
	strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND_H264);
	strcpy(zHcuVmCtrTab.clock.curAvorionFdH264, stmp);
	//HcuDebugPrint("HWINV: zCurTimeDate.curAvorionFdH264 %s!\n", zCurTimeDate.curAvorionFdH264);

	strcpy(zHcuVmCtrTab.clock.curAvorionFdAvi, stmp);
	strcat(zHcuVmCtrTab.clock.curAvorionFdAvi, HCU_RECORD_FILE_NAME_EXTEND_AVI);
	strcpy(zHcuVmCtrTab.clock.curAvorionFdMkv, stmp);
	strcat(zHcuVmCtrTab.clock.curAvorionFdMkv, HCU_RECORD_FILE_NAME_EXTEND_MKV);

	//特殊的干净文件名字
	strcpy(zHcuVmCtrTab.clock.curAvorionFnameH264,zHcuSysEngPar.cloud.cloudBhHcuName);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameH264,"_");
	strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, zHcuVmCtrTab.clock.sMin);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, HCU_RECORD_FILE_NAME_EXTEND_H264);
	//HcuDebugPrint("HWINV: zCurTimeDate.curAvorionFnameH264 %s!\n", zCurTimeDate.curAvorionFnameH264);

	strcpy(zHcuVmCtrTab.clock.curAvorionFnameAvi, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameAvi, zHcuVmCtrTab.clock.sMin);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameAvi, HCU_RECORD_FILE_NAME_EXTEND_AVI);

	strcpy(zHcuVmCtrTab.clock.curAvorionFnameMkv, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameMkv, zHcuVmCtrTab.clock.sMin);
	strcat(zHcuVmCtrTab.clock.curAvorionFnameMkv, HCU_RECORD_FILE_NAME_EXTEND_MKV);
	//HcuDebugPrint("%s\n", zCurTimeDate.curSensorFd);
	//HcuDebugPrint("%s\n", zCurTimeDate.curMicrophoneFd);
	//HcuDebugPrint("%s\n", zCurTimeDate.curAvorionFd);


	//初始化ETHERNET后台接口
	zHcuVmCtrTab.hwinv.ethernet.hwBase.taskId = TASK_ID_ETHERNET;
	zHcuVmCtrTab.hwinv.ethernet.hwBase.hwResistence = 0;
	if (HCU_COMM_HW_BOARD_ETHERNET == HCU_COMM_HW_BOARD_OFF)
		zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus = HCU_HWINV_STATUS_NOT_INSTALL;
	if ((HCU_COMM_HW_BOARD_ETHERNET == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_ETHERNET == NULL))
		zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
	if ((HCU_COMM_HW_BOARD_ETHERNET == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_ETHERNET != NULL))
		zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;

	//初始化USBNET后台接口
	zHcuVmCtrTab.hwinv.usbnet.hwBase.taskId = TASK_ID_USBNET;
	zHcuVmCtrTab.hwinv.usbnet.hwBase.hwResistence = 0;
	if (HCU_COMM_HW_BOARD_USBNET == HCU_COMM_HW_BOARD_OFF)
		zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus = HCU_HWINV_STATUS_NOT_INSTALL;
	if ((HCU_COMM_HW_BOARD_USBNET == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_NET == NULL))
		zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
	if ((HCU_COMM_HW_BOARD_USBNET == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_NET != NULL))
		zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;

	//初始化WIFI后台接口
	zHcuVmCtrTab.hwinv.wifi.hwBase.taskId = TASK_ID_WIFI;
	zHcuVmCtrTab.hwinv.wifi.hwBase.hwResistence = 0;
	if (HCU_COMM_HW_BOARD_WIFI == HCU_COMM_HW_BOARD_OFF)
		zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus = HCU_HWINV_STATUS_NOT_INSTALL;
	if ((HCU_COMM_HW_BOARD_WIFI == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_WIFI == NULL))
		zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
	if ((HCU_COMM_HW_BOARD_WIFI == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_WIFI != NULL))
		zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;

	//初始化3G4G后台接口
	zHcuVmCtrTab.hwinv.g3g4.hwBase.taskId = TASK_ID_3G4G;
	zHcuVmCtrTab.hwinv.g3g4.hwBase.hwResistence = 0;
	if (HCU_COMM_HW_BOARD_3G4G == HCU_COMM_HW_BOARD_OFF)
		zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus = HCU_HWINV_STATUS_NOT_INSTALL;
	if ((HCU_COMM_HW_BOARD_3G4G == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_3G4G == NULL))
		zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
	if ((HCU_COMM_HW_BOARD_3G4G == HCU_COMM_HW_BOARD_ON) && (HCU_DEFAULT_DEVICE_USB_3G4G != NULL))
		zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;


	return SUCCESS;
}

void func_hwinv_copy_right(void)
{
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("********************************************************************\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("********************************************************************\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("**                                                                **\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("**              Home and Car Unisystem (HCU)                      **\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("**            Octopus Wireless Communications Co. Ltd.            **\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("**                          Version 1.0                           **\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("**                                                                **\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("********************************************************************\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("********************************************************************\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint(" == SYSINFO ==\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint("SW Build: %s, %s\n", __DATE__, __TIME__); /* prints !!!Hello World!!! */
		HcuDebugPrint("HW Build: P01\n"); /* prints !!!Hello World!!! */
		HcuDebugPrint(" == SYSINFO ==\n"); /* prints !!!Hello World!!! */
	}
}

//将工程参数从数据库中读出来并存入内存，该函数只是在初始化时做一次，并没有随时进行
OPSTAT hcu_hwinv_read_engineering_data_into_mem(void)
{
	int ret = 0;
	//测试成功！！！
	/*sensor_emc_data_element_t emcData;
	memset(&emcData, 0, sizeof(sensor_emc_data_element_t));
	emcData.equipid = 22;
	emcData.timeStamp = 33;
	emcData.emcValue = 44;
	emcData.gps.gpsx = 55;
	emcData.gps.gpsy = 66;
	emcData.gps.gpsz = 77;
	emcData.onOffLineFlag = 1;
	if (dbi_HcuEmcDataInfo_save(&emcData) == FAILURE){
		zHcuRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Can not save data into database!\n");
	}*/

	memset(&zHcuSysEngPar, 0, sizeof(HcuSysEngParTable_t));
	ret = dbi_HcuSysEngPar_inqury(&zHcuSysEngPar, HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE);

	if (ret == SUCCESS){

		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){

			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudBhHcuName = %s\n", zHcuSysEngPar.cloud.cloudBhHcuName);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.emcReqTimer = %d\n", zHcuSysEngPar.timer.emcReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.humidReqTimer = %d\n", zHcuSysEngPar.timer.humidReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.pm25ReqTimer = %d\n", zHcuSysEngPar.timer.pm25ReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.tempreqtimer = %d\n", zHcuSysEngPar.timer.tempReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.winddirreqtimer = %d\n", zHcuSysEngPar.timer.winddirReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.windspdreqtimer = %d\n", zHcuSysEngPar.timer.windspdReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.noisereqtimer = %d\n", zHcuSysEngPar.timer.noiseReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.cmdcontrollongtimer = %d\n", zHcuSysEngPar.timer.cmdcontrolLongTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.heartbeattimer = %d\n", zHcuSysEngPar.timer.cloudvelaHbTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.hsmmpReqTimer = %d\n", zHcuSysEngPar.timer.hsmmpReqTimer);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.hsmmpCapDuration = %d\n", zHcuSysEngPar.timer.hsmmpCapDuration);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.hsmmpCapDurationFB = %d\n", zHcuSysEngPar.timer.hsmmpCapDurationFB);
			HcuDebugPrint("HWINV: zHcuSysEngPar.timer.hsmmpRefRate = %d\n", zHcuSysEngPar.timer.hsmmpRefRate);
			HcuDebugPrint("HWINV: zHcuSysEngPar.videoSev.hcuVideoServerDir = %s\n", zHcuSysEngPar.videoSev.hcuVideoServerDir);
			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudFtpAdd = %s\n", zHcuSysEngPar.cloud.cloudFtpAdd);
			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudFtpUser = %s\n", zHcuSysEngPar.cloud.cloudFtpUser);
			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudFtpPwd = %s\n", zHcuSysEngPar.cloud.cloudFtpPwd);
			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudFtpUserVideo = %s\n", zHcuSysEngPar.cloud.cloudFtpUserVideo);
			HcuDebugPrint("HWINV: zHcuSysEngPar.cloud.cloudFtpPwdVideo = %s\n", zHcuSysEngPar.cloud.cloudFtpPwdVideo);
			HcuDebugPrint("HWINV: zHcuSysEngPar.swDownload.hcuSwDownloadDir = %s\n", zHcuSysEngPar.swDownload.hcuSwDownloadDir);
			HcuDebugPrint("HWINV: SeriesPortForGPS = %d, SeriesPortForModbus = %d, SeriesPortForPm25Sharp = %d\n",zHcuSysEngPar.serialport.SeriesPortForGPS, zHcuSysEngPar.serialport.SeriesPortForModbus, zHcuSysEngPar.serialport.SeriesPortForPm25Sharp);
			HcuDebugPrint("HWINV: Set basic engineering data correctly from DATABASE parameters!\n");
		}
	}

	//ret = FAILURE;
	//如果出错，就使用SYSCONF.H中的DEFAULT参数赋值给这些全局变量
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Read error from system engineering parameter database!\n");
		//通信部分
		zHcuSysEngPar.comm.commBackHawlCon = HCU_SYSCFG_COMM_BACK_HAWL_CON;
		zHcuSysEngPar.comm.commHwBoardEthernet = HCU_COMM_HW_BOARD_ETHERNET;
		zHcuSysEngPar.comm.commHwBoardUsbnet = HCU_COMM_HW_BOARD_USBNET;
		zHcuSysEngPar.comm.commHwBoardWifi = HCU_COMM_HW_BOARD_WIFI;
		zHcuSysEngPar.comm.commHwBoard3g4g = HCU_COMM_HW_BOARD_3G4G;
		zHcuSysEngPar.comm.commHwBoardGps = HCU_COMM_HW_BOARD_GPS;
		zHcuSysEngPar.comm.commHwBoardLcd = HCU_COMM_HW_BOARD_LCD;
		zHcuSysEngPar.comm.commHwBoardLed = HCU_COMM_HW_BOARD_LED;
		zHcuSysEngPar.comm.commHwBoardZeegbe = HCU_COMM_HW_BOARD_ZEEGBE;
		zHcuSysEngPar.comm.commHwBoardFlash = HCU_COMM_HW_BOARD_FLASH;
		zHcuSysEngPar.comm.commFrameModbus = HCU_COMM_FRONT_MODBUS;
		zHcuSysEngPar.comm.commFrameSpsvirgo = HCU_COMM_FRONT_SPSVIRGO;
		zHcuSysEngPar.comm.commFrameAvorion = HCU_COMM_FRONT_AVORION;
		zHcuSysEngPar.comm.commFrameCloudvela = HCU_COMM_FRONT_CLOUDVELA;
		zHcuSysEngPar.comm.commFrameI2cbuslibra = HCU_COMM_FRONT_I2CBUSLIBRA;
		zHcuSysEngPar.comm.commFrameSpibusaries = HCU_COMM_FRONT_SPIBUSARIES;
		zHcuSysEngPar.comm.commFrameNbiotcj188 = HCU_COMM_FRONT_NBIOTCJ188;
		zHcuSysEngPar.comm.commFrameNbiotqg376 = HCU_COMM_FRONT_NBIOTQG376;
		zHcuSysEngPar.comm.commFrontSps485 = HCU_COMM_FRONT_SPS485;
		zHcuSysEngPar.comm.commFrontSps232 = HCU_COMM_FRONT_SPS232;
		zHcuSysEngPar.comm.commFrontMicrophone = HCU_COMM_FRONT_MICROPHONE;
		zHcuSysEngPar.comm.commFrontCamera = HCU_COMM_FRONT_CAMERA;
		zHcuSysEngPar.comm.commFrontBle = HCU_COMM_FRONT_BLE;
		zHcuSysEngPar.comm.commFrontSensorEmc = HCU_COMM_FRONT_SENSOR_EMC;
		zHcuSysEngPar.comm.commFrontSensorPm25 = HCU_COMM_FRONT_SENSOR_PM25;
		zHcuSysEngPar.comm.commFrontSensorTemp = HCU_COMM_FRONT_SENSOR_TEMP;
		zHcuSysEngPar.comm.commFrontSensorHumid = HCU_COMM_FRONT_SENSOR_HUMID;
		zHcuSysEngPar.comm.commFrontSensorWinddir = HCU_COMM_FRONT_SENSOR_WINDDIR;
		zHcuSysEngPar.comm.commFrontSensorWindspd = HCU_COMM_FRONT_SENSOR_WINDSPD;
		zHcuSysEngPar.comm.commFrontSensorNoise = HCU_COMM_FRONT_SENSOR_NOISE;
		zHcuSysEngPar.comm.commFrontSensorHsmmp = HCU_COMM_FRONT_SENSOR_HSMMP;
		zHcuSysEngPar.comm.commFrontSensorPm25Sharp = HCU_COMM_FRONT_SENSOR_PM25SHARP;
		zHcuSysEngPar.comm.commFrontSensorIwm = HCU_COMM_FRONT_SENSOR_IWM;
		zHcuSysEngPar.comm.commFrontSensorIhm = HCU_COMM_FRONT_SENSOR_IHM;
		zHcuSysEngPar.comm.commFrontSensorIgm = HCU_COMM_FRONT_SENSOR_IGM;
		zHcuSysEngPar.comm.commFrontSensorIpm = HCU_COMM_FRONT_SENSOR_IPM;

		//数据库部分
		strcpy(zHcuSysEngPar.dbi.hcuDbHost, HCU_LOCAL_DB_HOST_DEFAULT);
		strcpy(zHcuSysEngPar.dbi.hcuDbUser, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT);
		strcpy(zHcuSysEngPar.dbi.hcuDbPsw, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT);
		strcpy(zHcuSysEngPar.dbi.hcuDbName, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT);
		zHcuSysEngPar.dbi.hcuDbPort = HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT;

		// Sensor timer by Shanchun
		zHcuSysEngPar.timer.emcReqTimer = HCU_EMC_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.emcReqTimerFB = HCU_EMC_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.humidReqTimer = HCU_HUMID_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.humidReqTimerFB = HCU_HUMID_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.noiseReqTimer = HCU_NOISE_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.noiseReqTimerFB = HCU_NOISE_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.pm25ReqTimer = HCU_PM25_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.pm25ReqTimerFB = HCU_PM25_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.tempReqTimer = HCU_TEMP_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.tempReqTimerFB = HCU_TEMP_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.winddirReqTimer = HCU_WINDDIR_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.winddirReqTimerFB = HCU_WINDDIR_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.windspdReqTimer = HCU_WINDSPD_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.windspdReqTimerFB = HCU_WINDSPD_TIMER_DURATION_MODBUS_FB;
		zHcuSysEngPar.timer.cloudvelaHbTimer = HCU_CLOUDVELA_TIMER_DURATION_PERIOD_LINK_HEART_BEAT;
		zHcuSysEngPar.timer.cloudvelaHbBackTimer = HCU_CLOUDVELA_TIMER_DURATION_PERIOD_SEND_DATA_BACK;
		zHcuSysEngPar.timer.nbiotcj188HbTimer = HCU_NBIOTCJ188_TIMER_DURATION_PERIOD_LINK_HEART_BEAT;
		zHcuSysEngPar.timer.nbiotcj188HbBackTimer = HCU_NBIOTCJ188_TIMER_DURATION_PERIOD_SEND_DATA_BACK;
		zHcuSysEngPar.timer.nbiotqg376HbTimer = HCU_NBIOTQG376_TIMER_DURATION_PERIOD_LINK_HEART_BEAT;
		zHcuSysEngPar.timer.nbiotqg376HbBackTimer = HCU_NBIOTQG376_TIMER_DURATION_PERIOD_SEND_DATA_BACK;
		zHcuSysEngPar.timer.cmdcontrolLongTimer = HCU_CLOUDVELA_TIMER_DURATION_PERIOD_CMD_CONTROL_LONG;
		zHcuSysEngPar.timer.cmdcontrolShortTimer = HCU_CLOUDVELA_TIMER_DURATION_PERIOD_CMD_CONTROL_SHORT;
        zHcuSysEngPar.timer.hsmmpReqTimer = HCU_HSMMP_TIMER_DURATION_PERIOD_AVORION_READ;
		zHcuSysEngPar.timer.hsmmpCapDuration = HCU_HSMMP_AVORION_CAPTURE_DURATION_DEFAULT;
		zHcuSysEngPar.timer.hsmmpCapDurationFB = HCU_HSMMP_TIMER_DURATION_AVORION_FB;
		zHcuSysEngPar.timer.hsmmpRefRate = HCU_HSMMP_AVORION_REFRESH_RATE_DEFAULT;
		zHcuSysEngPar.timer.airprsReqTimer = HCU_AIRPRS_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.co1ReqTimer = HCU_CO1_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.lightstrReqTimer = HCU_LIGHTSTR_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.alcoholReqTimer = HCU_ALCOHOL_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.hchoReqTimer = HCU_HCHO_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.toxicgasReqTimer = HCU_TOXICGAS_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.pm25sharpReqTimer = HCU_PM25SHARP_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.iwmReqTimer = HCU_IWM_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.ihmReqTimer = HCU_IHM_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.igmReqTimer = HCU_IGM_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.ipmReqTimer = HCU_IPM_TIMER_DURATION_PERIOD_READ;
		zHcuSysEngPar.timer.syspmWorkingTimer = HCU_SYSPM_TIMER_DURATION_PERIOD_WORKING;

		//Series Port config by Shanchun
		zHcuSysEngPar.serialport.SeriesPortForModbus = HCU_SYSCFG_SERIESPORT_NO_FOR_MODBUS_DEFAULT;
		zHcuSysEngPar.serialport.SeriesPortForGPS = HCU_SYSCFG_SERIESPORT_NUM_FOR_GPS_DEFAULT;
		zHcuSysEngPar.serialport.SeriesPortForPm25Sharp = HCU_SERIESPORT_NO_FOR_PM25SHARP_DEFAULT;
		//zHcuSysEngPar.serialport.BautRate = HCU_SERIESPORT_BAUTRATE_DEFAULT;

		//后台部分
		strcpy(zHcuSysEngPar.cloud.cloudHttpAddLocal, HCU_SYSCFG_CLOUD_HTTP_SVR_ADDR_LOCAL);
		strcpy(zHcuSysEngPar.cloud.cloudHttpAddTest, HCU_CLOUDVELA_HTTP_ADDRESS_TEST);
		strcpy(zHcuSysEngPar.cloud.cloudHttpAddSae, HCU_SYSCFG_CLOUD_HTTP_SVR_ADDR_DEFAULT);
		strcpy(zHcuSysEngPar.cloud.cloudHttpAddJd, HCU_SYSCFG_CLOUD_HTTP_ADDR_JD);
		strcpy(zHcuSysEngPar.cloud.cloudHttpAddWechat, HCU_SYSCFG_CLOUD_HTTP_ADDR_WECHAT);
		strcpy(zHcuSysEngPar.cloud.cloudBhServerName, HCU_CLOUDVELA_HTTP_SERVER_NAME);
		strcpy(zHcuSysEngPar.cloud.cloudBhHcuName, HCU_SYSCFG_CLOUD_HCU_NAME);
		zHcuSysEngPar.cloud.cloudBhItfFrameStd = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ITF_STD_SET;

		//for HCU SW FTP by shanchun
		strcpy(zHcuSysEngPar.cloud.cloudFtpAdd, HCU_SWM_SERVER_FTP_ADDRESS);
		strcpy(zHcuSysEngPar.cloud.cloudFtpUser, HCU_CLOUDVELA_SWM_FTP_USER);
		strcpy(zHcuSysEngPar.cloud.cloudFtpPwd, HCU_CLOUDVELA_SWM_FTP_PWD);

		//for HCU video FTP upload by shanchun
		strcpy(zHcuSysEngPar.cloud.cloudFtpUserVideo, HCU_CLOUDVELA_SWM_FTP_USER);
		strcpy(zHcuSysEngPar.cloud.cloudFtpPwdVideo, HCU_CLOUDVELA_SWM_FTP_PWD);

		//local SW storage address
		strcpy(zHcuSysEngPar.swDownload.hcuSwDownloadDir, HCU_SYSCFG_SWM_LOCAL_DOWNLOAD_DIR_DEFAULT);
		strcpy(zHcuSysEngPar.swDownload.hcuSwActiveDir, HCU_SYSCFG_SWM_LOCAL_ACTIVE_DIR_DEFAULT);
		strcpy(zHcuSysEngPar.swDownload.hcuSwBackupDir, HCU_SYSCFG_SWM_LOCAL_BACKUP_DIR_DEFAULT);

		//视频服务器部分
		strcpy(zHcuSysEngPar.videoSev.hcuVideoServerDir, HCU_SYSCFG_VIDEO_STREAM_SERVER_DIR_DEFAULT);
		strcpy(zHcuSysEngPar.videoSev.hcuVideoServerHttp, HCU_SYSCFG_VIDEO_STREAM_SERVER_HTTP_DEFAULT);
		//DEBUG部分
		zHcuSysEngPar.debugMode = HCU_SYSCFG_TRACE_DEBUG_SET;
		//TRACE部分
		zHcuSysEngPar.traceMode = HCU_SYSCFG_TRACE_MSG_SET;
		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("HWINV: Set basic engineering data correctly from SYSTEM DEFAULT parameters!\n");
		}
	}

	//读取HcuTraceModuleCtr表单到系统内存中
	ret = dbi_HcuTraceModuleCtr_inqury(&zHcuSysEngPar);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Read Trace Module Control DB error!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("HWINV: Set Trace Moduble based engineering data correctly from DATABASE parameters!\n");
	}

	//读取HcuTraceMsgCtr表单到系统内存中
	ret = dbi_HcuTraceMsgCtr_inqury(&zHcuSysEngPar);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Read Trace Message Control DB error!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("HWINV: Set Trace Message based engineering data correctly from DATABASE parameters!\n");
	}

	//启动FIREFOX等本地浏览器
	char cmdStr[HCU_SYSENG_PAR_ELEMENT_UI_MAX_LEN] = "";
	//char outputStr[SYS_ENG_PAR_ELEMENT_UI_MAX_LEN] = "";
	if ((zHcuSysEngPar.localUI.browselProg != NULL) && (zHcuSysEngPar.localUI.browselAutoStartUpFlag == TRUE)){
		//判断firefox是否已经在运行，不是太正常，留待未来完善
		/*sprintf(cmdStr, "ps -all | grep %s > %s", zHcuSysEngPar.localUI.browselProg, outputStr);
		system(cmdStr);
		if (strstr(outputStr, zHcuSysEngPar.localUI.browselProg) == NULL){
			sprintf(cmdStr, "%s %s %s", zHcuSysEngPar.localUI.browselProg, zHcuSysEngPar.localUI.browselStartUpAddress, zHcuSysEngPar.localUI.browselWorkingOption);
			system(cmdStr);
		}*/
		//直接运行的方式，现在直接启动可能阻塞主程序的运行，需要再优化
		sprintf(cmdStr, "%s %s %s > /dev/null &", zHcuSysEngPar.localUI.browselProg, zHcuSysEngPar.localUI.browselStartUpAddress, zHcuSysEngPar.localUI.browselWorkingOption);
		system(cmdStr);
	}

    //create video server directory by Shanchun
	ret = hcu_hwinv_create_multi_dir(zHcuSysEngPar.videoSev.hcuVideoServerDir);

    if (ret == FAILURE){
    	HcuErrorPrint("HWINV: Can't create video server directory: %s\n", zHcuSysEngPar.videoSev.hcuVideoServerDir);
	}
	else
	{
		HcuDebugPrint("HWINV: Create successfully for video server directory: %s\n\n", zHcuSysEngPar.videoSev.hcuVideoServerDir);

	}

    //create HCU SW download/active/backup local directory by Shanchun
	ret = hcu_hwinv_create_multi_dir(zHcuSysEngPar.swDownload.hcuSwDownloadDir);
    if (ret == FAILURE){
    	HcuErrorPrint("HWINV: Can't create HCU SW download local directory: %s\n", zHcuSysEngPar.swDownload.hcuSwDownloadDir);
	}
	else
	{
		HcuDebugPrint("HWINV: Create successfully for HCU SW download local directory: %s\n\n", zHcuSysEngPar.swDownload.hcuSwDownloadDir);

	}

	ret = hcu_hwinv_create_multi_dir(zHcuSysEngPar.swDownload.hcuSwActiveDir);
    if (ret == FAILURE){
    	HcuErrorPrint("HWINV: Can't create HCU SW active local directory: %s\n", zHcuSysEngPar.swDownload.hcuSwActiveDir);
	}
	else
	{
		HcuDebugPrint("HWINV: Create successfully for HCU SW active local directory: %s\n\n", zHcuSysEngPar.swDownload.hcuSwActiveDir);

	}

	ret = hcu_hwinv_create_multi_dir(zHcuSysEngPar.swDownload.hcuSwBackupDir);
    if (ret == FAILURE){
    	HcuErrorPrint("HWINV: Can't create HCU SW backup local directory: %s\n", zHcuSysEngPar.swDownload.hcuSwBackupDir);
	}
	else
	{
		HcuDebugPrint("HWINV: Create successfully for HCU SW backup local directory: %s\n\n", zHcuSysEngPar.swDownload.hcuSwBackupDir);

	}


	return SUCCESS;
}

void func_hwinv_scan_all(void)
{
	func_hwinv_scan_date();
	func_hwinv_scan_sysinfo();
	func_hwinv_scan_flash();
	func_hwinv_scan_rtc();
	func_hwinv_scan_memroy();
	func_hwinv_scan_main_board();
	func_hwinv_scan_hard_disc();
	func_hwinv_scan_gpio();
	func_hwinv_scan_sps232();
	func_hwinv_scan_sps485();
	func_hwinv_scan_ethernet();
	func_hwinv_scan_usbnet();
	func_hwinv_scan_wifi();
	func_hwinv_scan_3g4g();
	func_hwinv_scan_microphone();
	func_hwinv_scan_camera();
	func_hwinv_scan_usb();
	func_hwinv_scan_eng_par();
	func_hwinv_scan_dir();
	func_hwinv_scan_db();
	func_hwinv_scan_emc();
	func_hwinv_scan_pm25();
	func_hwinv_scan_winddir();
	func_hwinv_scan_windspd();
	func_hwinv_scan_temp();
	func_hwinv_scan_humid();
	func_hwinv_scan_airprs();
	func_hwinv_scan_co1();
	func_hwinv_scan_lightstr();
	func_hwinv_scan_alcohol();
	func_hwinv_scan_hcho();
	func_hwinv_scan_toxicgas();
	func_hwinv_scan_iwm();
	func_hwinv_scan_ihm();
	func_hwinv_scan_igm();
	func_hwinv_scan_ipm();
	func_hwinv_scan_local_ui();
	func_hwinv_scan_message_queue();
}

void func_hwinv_scan_date(void)
{
	time_t lt;
	struct tm *cu;
	char stmp[HCU_DIR_LENGTH_MAX];
	BOOL flagMon = FALSE;
	BOOL flagDay = FALSE;
	BOOL flagHour = FALSE;
	BOOL flagMin = FALSE;

	//获取日历时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1;

	//更新Current数据
	if (cu->tm_min != zHcuVmCtrTab.clock.min){
		//技巧：这里循环设置FLASE，遇到一次TRUE，则循环到下一轮后再被设置为FLASE，正好让其它SCAN模块有机会干活
		//另外一个技巧是，回写只有一次，防止不断重复写入，从而提高效率
		//因为该循环的周期可能会比较短，提高数据写入的效率，是有一定意义的
		if (cu->tm_mon != zHcuVmCtrTab.clock.mon){
			flagMon = TRUE;
			zHcuVmCtrTab.clock.monChange = TRUE;
		}else{
			if (zHcuVmCtrTab.clock.monChange != FALSE) zHcuVmCtrTab.clock.monChange = FALSE;
		}
		if (cu->tm_mday != zHcuVmCtrTab.clock.day){
			flagDay = TRUE;
			zHcuVmCtrTab.clock.dayChange = TRUE;
		}else{
			if (zHcuVmCtrTab.clock.dayChange != FALSE) zHcuVmCtrTab.clock.dayChange = FALSE;
		}
		if (cu->tm_hour != zHcuVmCtrTab.clock.hour) flagHour = TRUE;
		if (cu->tm_min != zHcuVmCtrTab.clock.min) flagMin = TRUE;
		zHcuVmCtrTab.clock.year = cu->tm_year;
		zHcuVmCtrTab.clock.mon = cu->tm_mon;
		zHcuVmCtrTab.clock.day = cu->tm_mday;
		zHcuVmCtrTab.clock.hour = cu->tm_hour;
		zHcuVmCtrTab.clock.min = cu->tm_min;
		zHcuVmCtrTab.clock.sec = cu->tm_min;
		//strftime(zCurDayString, 8, "%Y%m%d", cu);
		//strftime(zCurHourString,10, "%Y%m%d%H", cu);
		//strftime(zCurMinString, 12, "%Y%m%d%H%M", cu);
		sprintf(zHcuVmCtrTab.clock.sYear, "%04d", (UINT16)(1900+cu->tm_year));
		sprintf(zHcuVmCtrTab.clock.sMon,  "%04d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon);
		sprintf(zHcuVmCtrTab.clock.sDay,  "%04d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday);
		sprintf(zHcuVmCtrTab.clock.sHour, "%04d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour);
		sprintf(zHcuVmCtrTab.clock.sMin,  "%04d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min);
		sprintf(zHcuVmCtrTab.clock.sSec,  "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
		sprintf(zHcuVmCtrTab.clock.strYear, "./%04d", (UINT16)(1900+cu->tm_year));
		sprintf(zHcuVmCtrTab.clock.strMon,  "./%04d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon);
		sprintf(zHcuVmCtrTab.clock.strDay,  "./%04d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday);
		sprintf(zHcuVmCtrTab.clock.strHour, "./%04d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour);
		sprintf(zHcuVmCtrTab.clock.strMin,  "./%04d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min);
		sprintf(zHcuVmCtrTab.clock.strSec,  "./%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
		zHcuVmCtrTab.clock.grad = cu->tm_hour * 60 + cu->tm_min;
	}

	/*
	 * 		检查目录是否存在，不存在，就建立目录
	 * 		./log/yyyymm/sensorlogyyyymmdd.dat
	 * 		./log/yyyymm/microphonelogyyyymmddhh.dat
	 * 		./log/yyyymm/avorionlogyyyymmmmddhhmm.dat
	 */

	//创建YYMM子目录
	if (flagMon == TRUE){
		strcpy(stmp, zHcuVmCtrTab.clock.curLogDir);
		strcat(stmp, "/");
		strcat(stmp, zHcuVmCtrTab.clock.sMon);
		strcpy(zHcuVmCtrTab.clock.curYmDir, stmp);

		DIR *dtmp;
		dtmp = opendir(zHcuVmCtrTab.clock.curYmDir);
		if (dtmp == NULL){
			chdir(zHcuVmCtrTab.clock.curLogDir);
			if (mkdir(zHcuVmCtrTab.clock.sMon, 0755) == -1){
				HcuErrorPrint("HWINV: Create directory error!\n");
				return;
			}
			chdir(zHcuVmCtrTab.clock.curHcuDir);
			//成功就不打印提示了
		}else{
			//HcuDebugPrint("HWINV: Dir exist!\n");
			closedir(dtmp);
		}
	}//Flag=TRUE，如果FLASE，则进不来

	//创建文件名字
	//Sensor是天级别
	if (flagDay == TRUE){
		strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);
		strcat(stmp, HCU_RECORD_FILE_NAME_SENSOR);
		strcat(stmp, zHcuVmCtrTab.clock.sDay);
		strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND);
		strcpy(zHcuVmCtrTab.clock.curSensorFd, stmp);
	}
	//Microphone是小时级别
	if (flagHour == TRUE){
		strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);
		strcat(stmp, HCU_RECORD_FILE_NAME_MICROPHONE);
		strcat(stmp, zHcuVmCtrTab.clock.sHour);
		strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND);
		strcpy(zHcuVmCtrTab.clock.curMicrophoneFd, stmp);
	}
	//Avorion是分钟级别
	if (flagMin == TRUE){
		strcpy(stmp, zHcuVmCtrTab.clock.curYmDir);

		strcat(stmp, "/");
		strcat(stmp,zHcuSysEngPar.cloud.cloudBhHcuName);
		strcat(stmp,"_");
		strcat(stmp, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
		strcat(stmp, zHcuVmCtrTab.clock.sMin);
		strcat(stmp, HCU_RECORD_FILE_NAME_EXTEND_H264);
		strcpy(zHcuVmCtrTab.clock.curAvorionFdH264, stmp);

		strcpy(zHcuVmCtrTab.clock.curAvorionFdAvi, stmp);
		strcat(zHcuVmCtrTab.clock.curAvorionFdAvi, HCU_RECORD_FILE_NAME_EXTEND_AVI);
		strcpy(zHcuVmCtrTab.clock.curAvorionFdMkv, stmp);
		strcat(zHcuVmCtrTab.clock.curAvorionFdMkv, HCU_RECORD_FILE_NAME_EXTEND_MKV);

		//特殊的干净文件名字
		strcpy(zHcuVmCtrTab.clock.curAvorionFnameH264,zHcuSysEngPar.cloud.cloudBhHcuName);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameH264,"_");
		strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, zHcuVmCtrTab.clock.sMin);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameH264, HCU_RECORD_FILE_NAME_EXTEND_H264);

		strcpy(zHcuVmCtrTab.clock.curAvorionFnameAvi, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameAvi, zHcuVmCtrTab.clock.sMin);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameAvi, HCU_RECORD_FILE_NAME_EXTEND_AVI);

	    strcpy(zHcuVmCtrTab.clock.curAvorionFnameMkv, HCU_RECORD_FILE_NAME_AVORION_CLEAN);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameMkv, zHcuVmCtrTab.clock.sMin);
		strcat(zHcuVmCtrTab.clock.curAvorionFnameMkv, HCU_RECORD_FILE_NAME_EXTEND_MKV);
	}

	//HcuDebugPrint("%s\n", zCurTimeDate.curSensorFd);
	//HcuDebugPrint("%s\n", zCurTimeDate.curMicrophoneFd);
	//HcuDebugPrint("%s\n", zCurTimeDate.curAvorionFd);
	//No return
}

void func_hwinv_scan_sysinfo(void)
{
	if (HcuGetSysInfo(&(zHcuVmCtrTab.hwinv.sysInfo), FALSE) != SUCCESS){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		HcuErrorPrint("HWINV: Read system hardware info error!\n");
	}
}

void func_hwinv_scan_flash(void)
{}

void func_hwinv_scan_rtc(void)
{}

void func_hwinv_scan_memroy(void)
{}

void func_hwinv_scan_main_board(void)
{}

void func_hwinv_scan_hard_disc(void)
{
	//by shanchun
	struct statfs diskInfo;
	statfs("/", &diskInfo);
	unsigned long long totalBlocks = diskInfo.f_bsize;
	unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
	size_t mbTotalsize = totalSize>>20;
	unsigned long long freeDisk = diskInfo.f_bfree * totalBlocks;
	size_t mbFreedisk = freeDisk>>20;

	float r = (float)mbFreedisk/mbTotalsize*100;

	//if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		//HcuDebugPrint("HWINV: Disk total = %dMB, free=%dMB, free/total ratio =%.2f%%\n", mbTotalsize, mbFreedisk,r);
	//}

	if(r <= HCU_HARDDISK_TRESHOLD)
	{
		//hcu_delete_file("/home/pi/workspace/hcu/RasberryPi/log");
		HcuDebugPrint("HWINV: / total = %dMB, free=%dMB, free/total=%.2f%%, removing log file to free the disk\n\n\n",mbTotalsize,mbFreedisk,r);
		hcu_delete_file(zHcuVmCtrTab.clock.curLogDir);
	}

}

void func_hwinv_scan_gpio(void)
{}

void func_hwinv_scan_sps232(void)
{}

void func_hwinv_scan_sps485(void)
{}

//扫描相应的接口是否已经存在或者相应的PID还处于活跃状态
//检查的信息包括SYSCONFIG.H的配置/L2COMDEF.H的定义，以及相应DEVICE是否活跃
//然后，根据结果置相应状态
//注意，这里并不关心连接是否真的正常，而只是相应的硬件和驱动是否正常而已
void func_hwinv_scan_ethernet(void)
{
	int ret = SUCCESS; //先假设以太网是好使的
	msg_struct_hwinv_cloudvela_phy_status_chg_t snd;

	//scan接口的硬件状态，以便更新状态字
	if (ret == SUCCESS){
		if(zHcuVmCtrTab.hwinv.ethernet.hwBase.hwResistence > 0){zHcuVmCtrTab.hwinv.ethernet.hwBase.hwResistence = 0;}
		if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.ethStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;
		}
	}

	else if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		zHcuVmCtrTab.hwinv.ethernet.hwBase.hwResistence++;
		if (zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.ethStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.ethernet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
		}
	}
	//其它结果不管了
}

void func_hwinv_scan_usbnet(void)
{
	int ret = FAILURE;  //假设这个物理接口不好使
	msg_struct_hwinv_cloudvela_phy_status_chg_t snd;

	//scan接口的硬件状态，以便更新状态字
	if (ret == SUCCESS){
		if(zHcuVmCtrTab.hwinv.usbnet.hwBase.hwResistence > 0){zHcuVmCtrTab.hwinv.usbnet.hwBase.hwResistence = 0;}
		if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.usbnetStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;
		}
	}

	else if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		zHcuVmCtrTab.hwinv.usbnet.hwBase.hwResistence++;
		if (zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.usbnetStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.usbnet.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
		}
	}
	//其它结果不管了
}

void func_hwinv_scan_wifi(void)
{
	int ret = FAILURE; //假设这个物理接口不好使
	msg_struct_hwinv_cloudvela_phy_status_chg_t snd;

	//scan接口的硬件状态，以便更新状态字
	if (ret == SUCCESS){
		if(zHcuVmCtrTab.hwinv.wifi.hwBase.hwResistence > 0){zHcuVmCtrTab.hwinv.wifi.hwBase.hwResistence = 0;}
		if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.wifiStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;
		}
	}

	else if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		zHcuVmCtrTab.hwinv.wifi.hwBase.hwResistence++;
		if (zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.wifiStatChg = HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.wifi.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
		}
	}
	//其它结果不管了
}

void func_hwinv_scan_3g4g(void)
{
	int ret = FAILURE; //假设这个物理接口不好使
	msg_struct_hwinv_cloudvela_phy_status_chg_t snd;

	//scan接口的硬件状态，以便更新状态字
	if (ret == SUCCESS){
		if(zHcuVmCtrTab.hwinv.g3g4.hwBase.hwResistence > 0){zHcuVmCtrTab.hwinv.g3g4.hwBase.hwResistence = 0;}
		if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus != HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.g3g4StatChg = HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_ACTIVE;
		}
	}

	else if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		zHcuVmCtrTab.hwinv.g3g4.hwBase.hwResistence++;
		if (zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus == HCU_HWINV_STATUS_INSTALL_ACTIVE){
			//发送状态改变给CLOUD
			memset(&snd, 0, sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t));
			snd.length = sizeof(msg_struct_hwinv_cloudvela_phy_status_chg_t);
			snd.g3g4StatChg = HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE;
			ret = hcu_message_send(MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG, TASK_ID_CLOUDVELA, TASK_ID_HWINV, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
				HcuErrorPrint("HWINV: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_HWINV], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return;
			}
			//放在里面，为了提高效率：状态不改变，不用修改控制表的数据
			zHcuVmCtrTab.hwinv.g3g4.hwBase.hwStatus = HCU_HWINV_STATUS_INSTALL_DEACTIVE;
		}
	}
	//其它结果不管了
}

void func_hwinv_scan_usb(void)
{}

void func_hwinv_scan_microphone(void)
{}

void func_hwinv_scan_camera(void)
{}


//让数据表设置有周期性的机会起作用
//这里其实主要是DEBUG/TRACE会起到作用，对于任务启动等等，暂时不起作用，因为模块已经创建完成
//未来可以采用RESTART机制，让其起作用，所以这里只是搭建好框架和工作机制，这些功能可以再未来根据需要，进一步完善
void func_hwinv_scan_eng_par(void)
{
	//这里先使用持续的动态刷新读取，未来可以再优化，降低进入的概率
	if ((dbi_HcuSysEngPar_inqury(&zHcuSysEngPar, HCU_CURRENT_WORKING_PROJECT_NAME_UNIQUE) == SUCCESS) &&
			(dbi_HcuTraceModuleCtr_inqury(&zHcuSysEngPar) == SUCCESS) && (dbi_HcuTraceMsgCtr_inqury(&zHcuSysEngPar) == SUCCESS)){
		if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
			HcuDebugPrint("HWINV: Retrieve all engineering data correctly from DATABASE parameters!\n");
		}
	}
	//如果读数据库失败，则不做任何动作，只是原封不动的保留原先启动时的设置
}

void func_hwinv_scan_dir(void)
{
	UINT32 monNbr = 0;
	UINT32 days = 0;
	UINT32 target = 0;
	UINT32 targetYear = 0, targetMon = 0;
	if (zHcuVmCtrTab.clock.monChange == TRUE){
		//最小保护
		days = HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS;
		if (days < HCU_SNESOR_DATA_SAVE_DURATION_MIN_IN_DAYS) days = HCU_SNESOR_DATA_SAVE_DURATION_MIN_IN_DAYS;
		//算出保留存储的月数
		monNbr = (UINT32)((days-1) / 30) + 1;
		if (zHcuVmCtrTab.clock.mon > monNbr){
			targetMon = zHcuVmCtrTab.clock.mon - monNbr;
			targetYear = zHcuVmCtrTab.clock.year + 1900;
		}else if (zHcuVmCtrTab.clock.mon <= monNbr){
			targetMon = zHcuVmCtrTab.clock.mon + 12 - monNbr;
			targetYear = zHcuVmCtrTab.clock.year + 1900 - 1;
		}
		target = targetYear * 100 + targetMon;

		//去log目录中
		//chdir(zCurTimeDate.curLogDir);
		DIR *dirp = opendir(zHcuVmCtrTab.clock.curLogDir);
		if(!dirp) {
			HcuErrorPrint("HWINV: Open Log directory error!\n");
			chdir(zHcuVmCtrTab.clock.curHcuDir);
			return;
		}

		//准备遍历LOG目录下的所有子目录
		//struct stat st;
		struct dirent *dir;
		char fullpath[HCU_DIR_LENGTH_MAX];
		int tmp = 0;
		while((dir = readdir(dirp)) != NULL) {
			if(!strcmp(dir->d_name, ".") || // 考虑当前目录和上级目录，否则会死循环
			   !strcmp(dir->d_name, "..")) {
				continue;
			}
			tmp = atol(dir->d_name);
			if (tmp <target){
				//初始化目录变量
				strcpy(fullpath, "");
				sprintf(fullpath, "rm %s/%s", zHcuVmCtrTab.clock.curLogDir, dir->d_name); //获取全局路径
				strcat(fullpath, "/*");
				//使用UNLINK/REMOVE函数，均搞不定，所以这里直接使用了系统命令，暂时搞定了
				if (system(fullpath) == -1){
					HcuErrorPrint("HWINV: Can not remove subdir [%s] all files!\n", fullpath);
				}
				strcpy(fullpath, "");
				sprintf(fullpath, "%s/%s", zHcuVmCtrTab.clock.curLogDir, dir->d_name); //获取全局路径
				if (remove(fullpath) == -1){
					HcuErrorPrint("HWINV: Can not remove subdir= [%s]!\n", fullpath);
				}
				//子目录迭代
				/*if(lstat(fullpath, &st) < 0) {
					HcuErrorPrint("HWINV: lstat!\n");
					continue;
				}
				if(S_ISDIR(st.st_mode)) {
					dir_traverse(fullpath); // 递归遍历子目录
				}*/
			}

		}
		//清理现场
		closedir(dirp);
	}
}

void func_hwinv_scan_db(void)
{
	//删除90天以上的旧数据
	//先用变天的标示位来判断，以提高效率，不然每一次扫描都无效的要访问一次数据库，过于频繁
	if (zHcuVmCtrTab.clock.dayChange == TRUE){
		if (dbi_HcuEmcDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuPm25DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuPm25SharpDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuPm25Bmpd300DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuWinddirDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuWindspdDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHumidDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHumidDht11DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHumidSht20DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHumidRht03DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempDht11DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempSht20DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempRht03DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempBmp180DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuTempMth01DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuNoiseDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHsmmpDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuAirprsDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuAirprsBmp180DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuAirprsAltitudeBmp180DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuCo1DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuLightstrDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuLightstrBh1750DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuAlcoholDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuAlcoholMq3alcoDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHchoDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuHchoZe08ch2oDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuToxicgasDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuToxicgasMq135DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuToxicgasZp01vocDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuIwmCj188DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuIhmCj188DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuIgmCj188DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuIpmCj188DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuIpmQg376DataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		if (dbi_HcuSyspmGlobalDataInfo_delete_3monold(HCU_SYSCFG_SNR_DATA_SAVE_DURATION_IN_DAYS) == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
	}
}

void func_hwinv_scan_emc(void)
{
	//其实，这里的难题是，如何获取MODBUS的操作，取得传感器上的硬件状态，待确定
	//其它传感器雷同
}

void func_hwinv_scan_pm25(void)
{}

void func_hwinv_scan_winddir(void)
{}

void func_hwinv_scan_windspd(void)
{}

void func_hwinv_scan_temp(void)
{}

void func_hwinv_scan_humid(void)
{}

void func_hwinv_scan_airprs(void)
{}

void func_hwinv_scan_co1(void)
{}

void func_hwinv_scan_lightstr(void)
{}

void func_hwinv_scan_alcohol(void)
{}

void func_hwinv_scan_hcho(void)
{}

void func_hwinv_scan_toxicgas(void)
{}

void func_hwinv_scan_iwm(void)
{}

void func_hwinv_scan_ihm(void)
{}

void func_hwinv_scan_igm(void)
{}

void func_hwinv_scan_ipm(void)
{}

void func_hwinv_scan_pm25sharp(void)
{}

void func_hwinv_scan_local_ui(void)
{
	//启动FIREFOX等本地浏览器
	//这里需要增加判断，判定firefox是否已经死亡，如果是的，才启动，不然就应该保持现状
	//判定firefox工作，需要用到系统命令ps -ef | grep forefox
	char cmdStr[HCU_SYSENG_PAR_ELEMENT_UI_MAX_LEN] = "";
	char outputStr[HCU_SYSENG_PAR_ELEMENT_UI_MAX_LEN] = "";
	if ((zHcuSysEngPar.localUI.browselProg != NULL) && (zHcuSysEngPar.localUI.browselAutoStartUpFlag == TRUE)){
		sprintf(cmdStr, "ps -all | grep %s > %s", zHcuSysEngPar.localUI.browselProg, outputStr);
		system(cmdStr);
		if (strstr(outputStr, zHcuSysEngPar.localUI.browselProg) == NULL){
			sprintf(cmdStr, "%s %s %s > NULL", zHcuSysEngPar.localUI.browselProg, zHcuSysEngPar.localUI.browselStartUpAddress, zHcuSysEngPar.localUI.browselWorkingOption);
			system(cmdStr);
		}
	}
}

//这里扫描各个任务模块的队列，看下是否有满的，如果有满的，说明这个人物模块已经被完全阻塞，基本上没有复活的可能性，强制复位该模块
//复位的方式，就是直接创建该任务以及队列消息
//扫描的方式，是通过内存中活着的任务，逐个扫
//麻烦在于需要按照单个名字，程序效率低下，可维护性不好，最好是一个循环，创建的命令全部采用指针方式，未来不需要考虑维护
//也就是有新任务增加消减，这个函数也不需要重新修改
void func_hwinv_scan_message_queue(void)
{
	UINT32 taskid;
	for (taskid = TASK_ID_MIN; taskid < TASK_ID_MAX; taskid++){
		//扫描其messageQue，看看是否满，如果是，意味着故障
		if ((taskid != TASK_ID_HWINV) && (zHcuSysCrlTab.taskRun[taskid].swTaskActive == HCU_TASK_SW_ACTIVE) && (zHcuSysCrlTab.taskRun[taskid].QueFullFlag == HCU_TASK_QUEUE_FULL_TRUE))
		{
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
				HcuDebugPrint("HWINV: Taskid = %d [%s] get full Queue, start to restart!\n", taskid, zHcuSysCrlTab.taskRun.taskName[taskid]);
			}
			//重新启动该任务
			hcu_task_delete(taskid);
			hcu_system_task_init_call(taskid, zHcuSysCrlTab.taskRun[taskid].fsmPtr);
			zHcuSysCrlTab.taskRun[taskid].QueFullFlag = HCU_TASK_QUEUE_FULL_FALSE;

			//再发送init消息给该模块，以便启动改模块
			msg_struct_com_init_t snd;
			memset(&snd, 0, sizeof(msg_struct_com_init_t));
			snd.length = sizeof(msg_struct_com_init_t);
			hcu_message_send(MSG_ID_COM_INIT, taskid, TASK_ID_HWINV, &snd, snd.length);
		}
	}
}


//只是提供了硬件和FLASH访问的一种抽象
//一种最为简单的硬盘存储传感器记录的机制搞定，以后可以逐步丰富：碎片文件/内容按照时间轴/记录数量/定时清理等等，待完成
UINT32 hcu_disk_write(UINT32 fId, void *dataBuffer, UINT32 dataLen)
{
	int fHandler;
	char fname[HCU_DIR_LENGTH_MAX];

	//判断入参
	if ((dataBuffer == NULL) || (dataLen > FILE_LENGTH_RECORD_MAX)){
	  HcuErrorPrint("HWINV: Input parameter error!\n");
	  zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
	  return FAILURE;
	}

	if (fId == FILE_OPERATION_TYPE_SENSOR){
		strcpy(fname, zHcuVmCtrTab.clock.curSensorFd);
	}else if (fId == FILE_OPERATION_TYPE_SPSVIRGO){
		strcpy(fname, zHcuVmCtrTab.clock.curMicrophoneFd);
	}else if (fId == FILE_OPERATION_TYPE_AVORION_AVI){
		strcpy(fname, zHcuVmCtrTab.clock.curAvorionFdAvi);
	}else if (fId == FILE_OPERATION_TYPE_AVORION_H264){
		strcpy(fname, zHcuVmCtrTab.clock.curAvorionFdH264);
	}else if (fId == FILE_OPERATION_TYPE_AVORION_MKV){
		strcpy(fname, zHcuVmCtrTab.clock.curAvorionFdMkv);
	}else{
		HcuErrorPrint("HWINV: Error Operation File type!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
		return FAILURE;
	}

	//打开源文件
	if((fHandler=open(fname, O_RDWR|O_CREAT))==-1){
	  HcuErrorPrint("HWINV: Open %s Error!\n", fname);
	  zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;
	  return FAILURE;
	}

	//测得文件大小
	UINT32 file_len=0;
	file_len= lseek(fHandler, 0L, SEEK_END);  //文件总长度
	lseek(fHandler, 0L, SEEK_SET); //移动文件到尾部开头
	lseek(fHandler, 0L, SEEK_END); //移动文件到尾部尾部

	//写入文件内容
	write(fHandler, dataBuffer, dataLen);
	close(fHandler);

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		HcuDebugPrint("HWINV: Save record into hd-disk successful, Orginal file Len =%d, add len = %d!\n", file_len, dataLen);
	}
	return SUCCESS;
}

//create video server directory by Shanchun
UINT32 hcu_hwinv_create_multi_dir(const char *path)
{
        int i, len;

        len = strlen(path);
        char dir_path[len+1];
        dir_path[len] = '\0';

        strncpy(dir_path, path, len);

        for (i=0; i<len; i++)
        {
                if (dir_path[i] == '/' && i > 0)
                {
                        dir_path[i]='\0';
                        if (access(dir_path, F_OK) < 0)
                        {
                                if (mkdir(dir_path, 0777) < 0)
                                {
                                        //printf("mkdir=%s:msg=%s\n", dir_path, strerror(errno));
										HcuErrorPrint("HCU-MAIN: create directory [%s] un-successfully, error code [%s].\n", dir_path, strerror(errno));
                                        return FAILURE;
                                }
                        }
                        dir_path[i]='/';
                }
        }

        return SUCCESS;
}



//by shanchun: for file & directory removal when hard disk treshold is reached
OPSTAT hcu_is_dir(const char *path)
{
	struct stat statbuf;
	if(lstat(path, &statbuf) == 0)
	{
		return S_ISDIR(statbuf.st_mode) != 0;
	}

	return FALSE;

}
OPSTAT hcu_is_file(const char *path)
{
	struct stat statbuf;
	if(lstat(path, &statbuf) == 0)
	{
		return S_ISREG(statbuf.st_mode) != 0;

	}

	return FALSE;

}
OPSTAT hcu_is_special_dir(const char *path)
{
	return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;

}
void hcu_get_file_path(const char *path, const char *file_name, char *file_path)
{
	strcpy(file_path, path);
	if(file_path[strlen(path) - 1] != '/')
		strcat(file_path, "/");
	strcat(file_path, file_name);

}
void hcu_delete_file(const char *path)
{
	DIR *dir;
	struct dirent *dir_info;

	char file_path[HCU_CLOUDVELA_PATH_MAX];
	if(hcu_is_file(path))
	{
		HcuDebugPrint("HSMMP: is file: %s \n\n", path);
		remove(path);
		return;
	}

	if(hcu_is_dir(path))
	{
		//HcuDebugPrint("is dir: %s \n\n\n\n\n\n\n\n\n\n\n", path);
		if((dir = opendir(path)) == NULL)
				return;
		while((dir_info = readdir(dir)) != NULL)
		{
			//HcuDebugPrint("is dir_into: %s \n\n\n\n\n\n\n\n\n\n\n", dir_info->d_name);

			hcu_get_file_path(path,dir_info->d_name,file_path);
			HcuDebugPrint("HWINV: is dir_into: %s \n\n\n", file_path);
			if(hcu_is_special_dir(dir_info->d_name))
				continue;
			hcu_delete_file(file_path);
			rmdir(file_path);
		}
	}

}




/*
 *
Linux时间操作(time、gettimeofday)
一、time函数

#include <time.h>
time_t time(time_t *calptr);
返回距计算机元年的秒数
一旦取得这种以秒计的很大的时间值后，通常要调用另一个时间函数将其变换为人们可读的时间和日期

#include <time.h>
//calendar time into a broken-down time expressed as UTC
struct tm *gmtime(const time_t *calptr);

//converts the calendar time to the local time, taking into account the local time zone and
//daylight saving time flag
struct tm *localtime(const time_t *calptr);

//converts it into a time_t value
time_t mktime(struct tm *tmptr);

  struct tm {         a broken-down time
     int  tm_sec;      seconds after the minute: [0 - 60]
     int  tm_min;      minutes after the hour: [0 - 59]
     int  tm_hour;     hours after midnight: [0 - 23]
     int  tm_mday;     day of the month: [1 - 31]
     int  tm_mon;      months since January: [0 - 11]
     int  tm_year;     years since 1900
     int  tm_wday;     days since Sunday: [0 - 6]
     int  tm_yday;     days since January 1: [0 - 365]
     int  tm_isdst;    daylight saving time flag: <0, 0, >0
　　// 以下两个字段在有些版本中是存在的，使用时需要查看对应的头文件确认
　　long int tm_gmtoff;  Seconds east of UTC.
　　const char *tm_zone;  Timezone abbreviation.
   };

char *asctime(const struct tm *tmptr);
char *ctime(const time_t *calptr);
asctime()和ctime()函数产生形式的26字节字符串，这与date命令的系统默认输出形式类似：
    Tue Feb 10 18:27:38 2004/n/0

二、gettimeofday函数得到更精确的时间
#include <sys/time.h>
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
第二个形参是基于平台实现的，使用的时候最好用NULL
struct timeval{
    time_t tv_sec;        *** second ***
    susecond_t tv_usec;    *** microsecond 微妙***
}
1秒＝1000毫秒，
1毫秒＝1000微秒，
1微妙＝1000纳秒，
1纳秒＝1000皮秒。
秒用s表现,毫秒用ms,微秒用μs表示，纳秒用ns表示，皮秒用ps表示。

三、内核时间
内核有两个重要的全局变量:
unsigned long jiffies;
timeval xtime ;

jiffies 是记录着从电脑开机到现在总共的"时钟中断"的次数。
文件linux-2.6.24/kernel/timer.c
void do_timer(unsigned long ticks)
{
    jiffies_64 += ticks;
    update_times(ticks);
}


xtime 是从cmos电路或rtc芯片中取得的时间，一般是从某一历史时刻开始到现在的时间。 这个就是所谓的"墙上时钟walltimer"，通过它可计算得出操作系统需要的日期时间，它的精确度是微秒。
xtime第一次赋值是在系统启动时调用timekeeping_init或time_init进行的再调用read_persistent_clock进一步调用get_rtc_time得到的

PS：在/proc/uptime里面的两个数字分别表示:
the uptime of the system(seconds),
and the amount of time spent in idle process(seconds).

 注：
　 今天写一个时间管理模块，updateTime(time)  time为year month day hour minute second minlliseconds任意一个，包括正数 负数
    其中一个约定为  1970年之后的时间管理。 主要通过闰年来管理日期。对某个量进行设置后，对应调整日期，比较麻烦。
     如果利用 系统的  mktime 和 localtime/gmtime  在 tm size_t 之间进行转换，及其方便！
*/
