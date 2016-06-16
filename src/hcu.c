/*
 ============================================================================
 Name        : hcu.c
 Author      : ZHANG
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "hcu.h"

#include "l0service/configxml.h"
#include "l0service/configxml.h"
#include "l0service/mmc.h"
#include "l0service/sysinfo.h"
#include "l0service/timer.h"
#include "l0service/trace.h"

#include "./l1com/l1comdef.h"
#include "./l1com/hwinv.h"

#include "./l1hwopt/apigprs.h"
//#include "./l1hwopt/audio.h"
#include "./l1hwopt/ble.h"
#include "./l1hwopt/con3g4g.h"
#include "./l1hwopt/ethernet.h"
#include "./l1hwopt/gps.h"
#include "./l1hwopt/lcd.h"
#include "./l1hwopt/led.h"
#include "./l1hwopt/sps232.h"
#include "./l1hwopt/sps485.h"
#include "./l1hwopt/usbnet.h"
#include "./l1hwopt/wifi.h"
#include "./l1hwopt/camera.h"
#include "./l1hwopt/microphone.h"
#include "./l1hwopt/gpio.h"
#include "./l1hwopt/i2c.h"
#include "./l1hwopt/spi.h"
#include "./l1hwopt/pwm.h"
#include "./l1hwopt/adc.h"
#include "./l1hwopt/switch.h"
#include "./l1hwopt/relay.h"
#include "./l1hwopt/motor.h"

#include "./l2frame/spsvirgo.h"
#include "./l2frame/modbus.h"
#include "./l2frame/avorion.h"
#include "./l2frame/cloudvela.h"
#include "./l2frame/i2cbuslibra.h"
#include "./l2frame/spibusaries.h"

#include "./l3app/svrcon.h"
#include "./l3app/sensoremc.h"
#include "./l3app/sensorpm25.h"
#include "./l3app/sensorwinddir.h"
#include "./l3app/sensorwindspd.h"
#include "./l3app/sensortemp.h"
#include "./l3app/sensorhumid.h"
#include "./l3app/sensornoise.h"
#include "./l3app/hsmmp.h"
#include "./l3app/sensorairprs.h"
#include "./l3app/sensorco1.h"
#include "./l3app/sensorlightstr.h"
#include "./l3app/sensoralcohol.h"
#include "./l3app/sensorhcho.h"
#include "./l3app/sensortoxicgas.h"
#include "./l3app/syspm.h"
#include "./l3app/sensorpm25sharp.h"


//Global variables to be shared with SVRCON


//Main Entry, Father process
int main(void) {

	//硬件初始化
	hcu_vm_system_init();

	//从数据库或者系统缺省配置中，读取系统配置的工程参数
	hcu_hwinv_read_engineering_data_into_mem();

	//任务模块启动初始化
	hcu_app_system_init();
	HcuDebugPrint("HCU-MAIN: System level initialization starting...\n");

	//单进程方式
	if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_SINGLE){
		hcu_working_mode_single();
	}

	//双进程方式
	else if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_DOUBLE){
		hcu_working_mode_double();
	}

	//多进程方式
	else if (HCU_PROCESS_WORK_MODE_CURRENT == HCU_PROCESS_WORK_MODE_TRIPPLE){
		hcu_working_mode_multipy();
	}

	//差错启动模式
	else{
		HcuErrorPrint("HCU: Can not work due to starting mode error!\n");
	}

	return EXIT_SUCCESS;
}

//单进程模式
void hcu_working_mode_single(void)
{
	zHcuCurrentProcessInfo.curProcId = getpid();
	strcpy(zHcuCurrentProcessInfo.curProcName, "PS_MAINAPP");
	hcu_process_1_mainapp_single();
}

//双进程模式
void hcu_working_mode_double(void)
{
	int pid = 0;

	pid = fork();
	if (pid < 0){
		HcuErrorPrint("HCU: Create process error!\n");
		return;
	} else if(pid == 0){
		zHcuCurrentProcessInfo.curProcId = getpid();
		strcpy(zHcuCurrentProcessInfo.curProcName, "PS_MAINAPP");
		hcu_process_2_mainapp_double();
	} else{
		zHcuCurrentProcessInfo.curProcId = getpid();
		strcpy(zHcuCurrentProcessInfo.curProcName, "PS_AVORION");
		hcu_process_23_sensor_avorion();
	}
}

//多进程模式
void hcu_working_mode_multipy(void)
{
	int pid = 0;

	pid = fork();
	if (pid < 0){
		HcuErrorPrint("HCU: Create process error!\n");
		return;
	} else if(pid == 0){
		zHcuCurrentProcessInfo.curProcId = getpid();
		strcpy(zHcuCurrentProcessInfo.curProcName, "PS_SUPVOR");
		hcu_process_3_entry_supervisor();
	} else{
	    pid = fork();
		if (pid < 0){
			HcuErrorPrint("HCU: Create process error!\n");
			return;
		} else if(pid == 0){
			zHcuCurrentProcessInfo.curProcId = getpid();
			strcpy(zHcuCurrentProcessInfo.curProcName, "PS_MAINAPP");
			hcu_process_3_mainapp_multipy();
		} else{
			zHcuCurrentProcessInfo.curProcId = getpid();
			strcpy(zHcuCurrentProcessInfo.curProcName, "PS_AVORION");
			hcu_process_23_sensor_avorion();
		}
	}
}

//单进程模式入口程序组
void hcu_process_1_mainapp_single(void)
{
	hcu_task_create_hcumain_env();
    hcu_task_create_all();
	hcu_task_send_init_to_timer();
	hcu_task_send_init_to_svrcon();

	//wait for ever
	while (1){
		hcu_sleep(60); //可以设置为5秒的定时，甚至更长
		//hcu_usleep(100);
		hcu_task_send_hb_to_svrcon();
	}

	hcu_task_delete_all_and_queue();

	return;
}

//双进程模式入口程序组
void hcu_process_2_mainapp_double(void)
{
	hcu_task_create_hcumain_env();
    hcu_task_create_all_but_avorion();

	//hcu_sleep(2);
    //hcu_msgque_resync();
	hcu_sleep(2);

	hcu_task_send_init_to_timer();
	hcu_task_send_init_to_svrcon();

	//wait for ever
	while (1){
		hcu_sleep(60);
		//hcu_usleep(100);
		hcu_task_send_hb_to_svrcon();
	}

	hcu_task_delete_all_and_queue();

	return;
}

void hcu_process_23_sensor_avorion(void)
{
	//Create task Avorion environments/24
	hcu_system_task_init_call(TASK_ID_AVORION, FsmAvorion);

    //hcu_sleep(2);
    //hcu_msgque_resync();

	HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
			zHcuCurrentProcessInfo.curProcId, zHcuCurrentProcessInfo.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);

	//wait for ever
	while (1){
		hcu_sleep(1); //1S减少信号中断的概率
		//hcu_usleep(100);
		//HcuDebugPrint("HCUMAIN: PS_23_AVORION test!\n");
	}

	hcu_task_delete_all_and_queue();

	return;
}

//多进程模式入口程序组
//由于时钟函数依然跟其它任务之间采用共享内存变量的方式，所以这种模式并不能正常工作，留待未来继续改进TIMER任务后才能正常调试和运行
void hcu_process_3_entry_supervisor(void)
{
	int ret = 0;

	hcu_task_create_hcumain_env();

    //hcu_sleep(2);
    //hcu_msgque_resync();
    hcu_sleep(2);

	hcu_task_send_init_to_timer();
	hcu_task_send_init_to_svrcon();

	//wait for ever
	while (1){
		hcu_sleep(60);
		//hcu_usleep(100);
		hcu_task_send_hb_to_svrcon();
		//HcuDebugPrint("HCUMAIN: PS_3_SUPERVISOR test!\n");

		//Wait for SVRCON feedback
		HcuMsgSruct_t rcv;
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv_no_wait(TASK_ID_HCUMAIN, &rcv);
		if (ret > 0){
			if ((rcv.msgType == MSG_ID_COM_PROCESS_REBOOT) && (rcv.src_id == TASK_ID_SVRCON)){
				HcuDebugPrint("HCU-MAIN: Reboot process start, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
						zHcuCurrentProcessInfo.curProcId, zHcuCurrentProcessInfo.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);
				int pid = 0;
				pid = fork();
				if (pid > 0){
					zHcuCurrentProcessInfo.curProcId = getpid();
					strcpy(zHcuCurrentProcessInfo.curProcName, "PS_AVORION");
					hcu_process_23_sensor_avorion();
				}else if (pid == 0){
					hcu_sleep(2);
					//hcu_msgque_resync(); //重新同步AVORION任务的QUEID
					//hcu_sleep(2);
					hcu_task_send_init_to_avorion(); //初始化该任务，不然它不启动开始工作
				}else{
					HcuErrorPrint("HCUMAIN: Error create AVORION process!\n");
				}
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
			}
		}

	}

	hcu_task_delete_all_and_queue();

	return;
}

void hcu_process_3_mainapp_multipy(void)
{
    hcu_task_create_all_but_avorion();

	//hcu_sleep(2);
    //hcu_msgque_resync();

	HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
			zHcuCurrentProcessInfo.curProcId, zHcuCurrentProcessInfo.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);

	//wait for ever
	while (1){
		hcu_sleep(1); //1S减少信号中断的概率
		//hcu_usleep(100);
		//HcuDebugPrint("HCUMAIN: PS_3_MAINAPP test!\n");
	}

	hcu_task_delete_all_and_queue();

	return;
}

//HCU local init for whole application
void hcu_app_system_init()
{
	//Checking sysconfig.h to see any task shall bet set active, or shall not start!
	int i=0;
	for (i=0; i<MAX_TASK_NUM_IN_ONE_HCU; i++){
		zHcuTaskInfo[i].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[i].hwActive = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[i].swTaskActive = HCU_TASK_PNP_INVALID;
	}
	//肯定需要启动的任务模块
	zHcuTaskInfo[TASK_ID_HCUMAIN].swTaskActive = HCU_TASK_PNP_OFF;
	zHcuTaskInfo[TASK_ID_TIMER].swTaskActive = HCU_TASK_PNP_ON;
	zHcuTaskInfo[TASK_ID_SVRCON].swTaskActive = HCU_TASK_PNP_ON;
	zHcuTaskInfo[TASK_ID_MMC].swTaskActive = HCU_TASK_PNP_ON;
	zHcuTaskInfo[TASK_ID_HWINV].swTaskActive = HCU_TASK_PNP_ON;
	zHcuTaskInfo[TASK_ID_SYSPM].swTaskActive = HCU_TASK_PNP_ON;

	//Switch-case
	//printf("zHcuSysEngPar.comm.commFrameCloudvela = %d\n", zHcuSysEngPar.comm.commFrameCloudvela);
	if (zHcuSysEngPar.comm.commFrameCloudvela == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_CLOUDVELA].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CLOUDVELA].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CLOUDVELA].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_CLOUDVELA].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_CLOUDVELA].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CLOUDVELA].hwActive = HCU_TASK_PNP_ON;
	}

	if (zHcuSysEngPar.comm.commFrameModbus == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_MODBUS].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_MODBUS].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_MODBUS].hwActive = HCU_TASK_PNP_INVALID;
	}else{
		zHcuTaskInfo[TASK_ID_MODBUS].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_MODBUS].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_MODBUS].hwActive = HCU_TASK_PNP_INVALID;
	}

	if (zHcuSysEngPar.comm.commFrameAvorion == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_AVORION].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_AVORION].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_AVORION].hwActive = HCU_TASK_PNP_INVALID;
	}else{
		zHcuTaskInfo[TASK_ID_AVORION].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_AVORION].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_AVORION].hwActive = HCU_TASK_PNP_INVALID;
	}
	if (zHcuSysEngPar.comm.commFrameSpsvirgo == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SPSVIRGO].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPSVIRGO].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_SPSVIRGO].hwActive = HCU_TASK_PNP_INVALID;
	}else{
		zHcuTaskInfo[TASK_ID_SPSVIRGO].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPSVIRGO].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_SPSVIRGO].hwActive = HCU_TASK_PNP_INVALID;
	}
	if (zHcuSysEngPar.comm.commFrameI2cbuslibra == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].hwActive = HCU_TASK_PNP_INVALID;
	}else{
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].hwActive = HCU_TASK_PNP_INVALID;
	}
	if (zHcuSysEngPar.comm.commFrameSpibusaries == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].hwActive = HCU_TASK_PNP_INVALID;
	}else{
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].hwPlugin = HCU_TASK_PNP_INVALID;
		zHcuTaskInfo[TASK_ID_SPIBUSARIES].hwActive = HCU_TASK_PNP_INVALID;
	}

	if (zHcuSysEngPar.comm.commHwBoardGps == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_GPS].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_GPS].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_GPS].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_GPS].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_GPS].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_GPS].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoardLcd == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_LCD].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LCD].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LCD].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_LCD].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LCD].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LCD].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoardLed == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_LED].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LED].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LED].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_LED].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LED].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LED].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoardEthernet == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_ETHERNET].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ETHERNET].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ETHERNET].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_ETHERNET].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ETHERNET].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ETHERNET].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoardWifi == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_WIFI].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WIFI].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WIFI].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_WIFI].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WIFI].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WIFI].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoardUsbnet == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_USBNET].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_USBNET].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_USBNET].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_USBNET].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_USBNET].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_USBNET].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commHwBoard3g4g == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_3G4G].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_3G4G].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_3G4G].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_3G4G].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_3G4G].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_3G4G].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSps232 == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SPS232].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPS232].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPS232].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_SPS232].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPS232].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPS232].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSps485 == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SPS485].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPS485].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPS485].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_SPS485].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPS485].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPS485].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontMicrophone == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_MICROPHONE].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_MICROPHONE].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_MICROPHONE].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_MICROPHONE].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_MICROPHONE].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_MICROPHONE].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontCamera == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_CAMERA].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CAMERA].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CAMERA].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_CAMERA].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_CAMERA].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_CAMERA].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontBle == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_BLE].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_BLE].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_BLE].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_BLE].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_BLE].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_BLE].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontGpio == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_GPIO].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_GPIO].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_GPIO].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_GPIO].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_GPIO].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_GPIO].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontI2c == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_I2C].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_I2C].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_I2C].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_I2C].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_I2C].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_I2C].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSpi == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SPI].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPI].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SPI].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_SPI].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPI].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SPI].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontPwm == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_PWM].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PWM].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PWM].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_PWM].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PWM].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PWM].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontAdc == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_ADC].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ADC].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ADC].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_ADC].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ADC].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ADC].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSwitch == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_SWITCH].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SWITCH].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_SWITCH].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_SWITCH].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SWITCH].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_SWITCH].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontRelay == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_RELAY].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_RELAY].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_RELAY].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_RELAY].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_RELAY].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_RELAY].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontMotor == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_MOTOR].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_MOTOR].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_MOTOR].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_MOTOR].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_MOTOR].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_MOTOR].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorEmc == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_EMC].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_EMC].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_EMC].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_EMC].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_EMC].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_EMC].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorPm25 == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_PM25].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PM25].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PM25].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_PM25].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PM25].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PM25].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorTemp == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_TEMP].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_TEMP].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_TEMP].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_TEMP].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_TEMP].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_TEMP].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorHumid == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_HUMID].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HUMID].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HUMID].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_HUMID].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HUMID].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HUMID].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorWinddir == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_WINDDIR].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WINDDIR].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WINDDIR].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_WINDDIR].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WINDDIR].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WINDDIR].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorWindspd == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_WINDSPD].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WINDSPD].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_WINDSPD].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_WINDSPD].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WINDSPD].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_WINDSPD].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorHsmmp == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_HSMMP].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HSMMP].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HSMMP].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_HSMMP].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HSMMP].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HSMMP].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorNoise == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_NOISE].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_NOISE].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_NOISE].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_NOISE].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_NOISE].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_NOISE].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorAirprs == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_AIRPRS].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_AIRPRS].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_AIRPRS].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_AIRPRS].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_AIRPRS].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_AIRPRS].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorCo1 == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_CO1].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CO1].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_CO1].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_CO1].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_CO1].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_CO1].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorLightstr == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_LIGHTSTR].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LIGHTSTR].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_LIGHTSTR].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_LIGHTSTR].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LIGHTSTR].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_LIGHTSTR].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorAlcohol == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_ALCOHOL].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ALCOHOL].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_ALCOHOL].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_ALCOHOL].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ALCOHOL].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_ALCOHOL].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorHcho == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_HCHO].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HCHO].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_HCHO].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_HCHO].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HCHO].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_HCHO].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorToxicgas == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_TOXICGAS].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_TOXICGAS].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_TOXICGAS].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_TOXICGAS].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_TOXICGAS].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_TOXICGAS].hwActive = HCU_TASK_PNP_OFF;
	}
	if (zHcuSysEngPar.comm.commFrontSensorPm25Sharp == COMM_HW_BOARD_ON){
		zHcuTaskInfo[TASK_ID_PM25SHARP].swTaskActive = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PM25SHARP].hwPlugin = HCU_TASK_PNP_ON;
		zHcuTaskInfo[TASK_ID_PM25SHARP].hwActive = HCU_TASK_PNP_ON;
	}else{
		zHcuTaskInfo[TASK_ID_PM25SHARP].swTaskActive = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PM25SHARP].hwPlugin = HCU_TASK_PNP_OFF;
		zHcuTaskInfo[TASK_ID_PM25SHARP].hwActive = HCU_TASK_PNP_OFF;
	}//by shanchun
	//Nothing
}

//目前系统启动暂时就是使用了这种方式，并没有采用其他多进程的方式
void hcu_task_create_all(void)
{
	//Create task Timer environments /2
	hcu_system_task_init_call(TASK_ID_TIMER, FsmTimer);

	//Create task MMC environments /3
	hcu_system_task_init_call(TASK_ID_MMC, FsmMmc);

	//Create task HWINV environments /4
	hcu_system_task_init_call(TASK_ID_HWINV, FsmHwinv);

	//Create task SvnCont environments /5
	hcu_system_task_init_call(TASK_ID_SVRCON, FsmSvrCon);

	//Create task CloudCont environments /6
	hcu_system_task_init_call(TASK_ID_CLOUDVELA, FsmCloudvela);

	//Create task Modbus environments /7
	hcu_system_task_init_call(TASK_ID_MODBUS, FsmModbus);

	//Create task EMC environments/8
	hcu_system_task_init_call(TASK_ID_EMC, FsmEmc);

	//Create task PM25 environments/9
	hcu_system_task_init_call(TASK_ID_PM25, FsmPm25);

	//Create task WindDir environments/10
	hcu_system_task_init_call(TASK_ID_WINDDIR, FsmWinddir);

	//Create task WindSpeed environments/11
	hcu_system_task_init_call(TASK_ID_WINDSPD, FsmWindspd);

	//Create task Temperature environments/12
	hcu_system_task_init_call(TASK_ID_TEMP, FsmTemp);

	//Create task Humidity environments/13
	hcu_system_task_init_call(TASK_ID_HUMID, FsmHumid);

	//Create task Hsmmp environments/14
	hcu_system_task_init_call(TASK_ID_HSMMP, FsmHsmmp);

	//Create task Noise environments/15
	hcu_system_task_init_call(TASK_ID_NOISE, FsmNoise);

	//Create task Ethernet environments/16
	hcu_system_task_init_call(TASK_ID_ETHERNET, FsmEthernet);

	//Create task WIFI environments/17
	hcu_system_task_init_call(TASK_ID_WIFI, FsmWifi);

	//Create task USBNET environments/18
	hcu_system_task_init_call(TASK_ID_USBNET, FsmUsbnet);

	//Create task 3G4G environments/19
	hcu_system_task_init_call(TASK_ID_3G4G, Fsm3g4g);

	//Create task SPS232 environments/20
	hcu_system_task_init_call(TASK_ID_SPS232, FsmSps232);

	//Create task SPS485 environments/21
	hcu_system_task_init_call(TASK_ID_SPS485, FsmSps485);

	//Create task BLE environments/22
	hcu_system_task_init_call(TASK_ID_BLE, FsmBle);

	//Create task Audio environments/23
	hcu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);

	//Create task Avorion environments/24
	hcu_system_task_init_call(TASK_ID_AVORION, FsmAvorion);

	//Create task GPS environments/25
	hcu_system_task_init_call(TASK_ID_GPS, FsmGps);

	//Create task LCD environments/26
	hcu_system_task_init_call(TASK_ID_LCD, FsmLcd);

	//Create task LCD environments/26.1
	hcu_system_task_init_call(TASK_ID_LED, FsmLed);

	//Create task Camera environments/27
	hcu_system_task_init_call(TASK_ID_CAMERA, FsmCamera);

	//Create task Microphone environments/28
	hcu_system_task_init_call(TASK_ID_MICROPHONE, FsmMicrophone);

	//Create task GPIO environments/29
	hcu_system_task_init_call(TASK_ID_GPIO, FsmGpio);

	//Create task I2C environments/30
	hcu_system_task_init_call(TASK_ID_I2C, FsmI2c);

	//Create task SPI environments/31
	hcu_system_task_init_call(TASK_ID_SPI, FsmSpi);

	//Create task PWM environments/32
	hcu_system_task_init_call(TASK_ID_PWM, FsmPwm);

	//Create task ADC environments/33
	hcu_system_task_init_call(TASK_ID_ADC, FsmAdc);

	//Create task SWITCH environments/34
	hcu_system_task_init_call(TASK_ID_SWITCH, FsmSwitch);

	//Create task RELAY environments/35
	hcu_system_task_init_call(TASK_ID_RELAY, FsmRelay);

	//Create task MOTOR environments/36
	hcu_system_task_init_call(TASK_ID_MOTOR, FsmMotor);

	//Create task SYSPM environments/37
	hcu_system_task_init_call(TASK_ID_SYSPM, FsmSyspm);

	//Create task AirPress environments/38
	hcu_system_task_init_call(TASK_ID_AIRPRS, FsmAirprs);

	//Create task CO1 environments/39
	hcu_system_task_init_call(TASK_ID_CO1, FsmCo1);

	//Create task LightStrenght environments/40
	hcu_system_task_init_call(TASK_ID_LIGHTSTR, FsmLightstr);

	//Create task Alcohol environments/41
	hcu_system_task_init_call(TASK_ID_ALCOHOL, FsmAlcohol);

	//Create task HCHO environments/42
	hcu_system_task_init_call(TASK_ID_HCHO, FsmHcho);

	//Create task ToxicGas environments/43
	hcu_system_task_init_call(TASK_ID_TOXICGAS, FsmToxicgas);

	//Create task Pm25Sharp environments/44
	hcu_system_task_init_call(TASK_ID_PM25SHARP, FsmPm25Sharp);

	//Create task Pm25Sharp environments/45
	hcu_system_task_init_call(TASK_ID_I2CBUSLIBRA, FsmI2cbuslibra);

	//Create task Pm25Sharp environments/46
	hcu_system_task_init_call(TASK_ID_SPIBUSARIES, FsmSpibusaries);
}

void hcu_task_create_all_but_avorion(void)
{
	//Create task Timer environments /2
	hcu_system_task_init_call(TASK_ID_TIMER, FsmTimer);

	//Create task MMC environments /3
	hcu_system_task_init_call(TASK_ID_MMC, FsmMmc);

	//Create task HWINV environments /4
	hcu_system_task_init_call(TASK_ID_HWINV, FsmHwinv);

	//Create task SvnCont environments /5
	hcu_system_task_init_call(TASK_ID_SVRCON, FsmSvrCon);

	//Create task CloudCont environments /6
	hcu_system_task_init_call(TASK_ID_CLOUDVELA, FsmCloudvela);

	//Create task Modbus environments /7
	hcu_system_task_init_call(TASK_ID_MODBUS, FsmModbus);

	//Create task EMC environments/8
	hcu_system_task_init_call(TASK_ID_EMC, FsmEmc);

	//Create task PM25 environments/9
	hcu_system_task_init_call(TASK_ID_PM25, FsmPm25);

	//Create task WindDir environments/10
	hcu_system_task_init_call(TASK_ID_WINDDIR, FsmWinddir);

	//Create task WindSpeed environments/11
	hcu_system_task_init_call(TASK_ID_WINDSPD, FsmWindspd);

	//Create task Temperature environments/12
	hcu_system_task_init_call(TASK_ID_TEMP, FsmTemp);

	//Create task Humidity environments/13
	hcu_system_task_init_call(TASK_ID_HUMID, FsmHumid);

	//Create task Hsmmp environments/14
	hcu_system_task_init_call(TASK_ID_HSMMP, FsmHsmmp);

	//Create task Noise environments/15
	hcu_system_task_init_call(TASK_ID_NOISE, FsmNoise);

	//Create task Ethernet environments/16
	hcu_system_task_init_call(TASK_ID_ETHERNET, FsmEthernet);

	//Create task WIFI environments/17
	hcu_system_task_init_call(TASK_ID_WIFI, FsmWifi);

	//Create task USBNET environments/18
	hcu_system_task_init_call(TASK_ID_USBNET, FsmUsbnet);

	//Create task 3G4G environments/19
	hcu_system_task_init_call(TASK_ID_3G4G, Fsm3g4g);

	//Create task SPS232 environments/20
	hcu_system_task_init_call(TASK_ID_SPS232, FsmSps232);

	//Create task SPS485 environments/21
	hcu_system_task_init_call(TASK_ID_SPS485, FsmSps485);

	//Create task BLE environments/22
	hcu_system_task_init_call(TASK_ID_BLE, FsmBle);

	//Create task Audio environments/23
	hcu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);

	//Create task GPS environments/25
	hcu_system_task_init_call(TASK_ID_GPS, FsmGps);

	//Create task LCD environments/26
	hcu_system_task_init_call(TASK_ID_LCD, FsmLcd);

	//Create task LCD environments/26.1
	hcu_system_task_init_call(TASK_ID_LED, FsmLed);

	//Create task Camera environments/27
	hcu_system_task_init_call(TASK_ID_CAMERA, FsmCamera);

	//Create task Microphone environments/28
	hcu_system_task_init_call(TASK_ID_MICROPHONE, FsmMicrophone);

	/*
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Create task failure in the middle!\n");
	}else if (ret == SUCCESS){
		HcuDebugPrint("HCU-MAIN: Create all task successful, congratulation!\n");
	}*/
}

UINT32 hcu_system_task_init_call(UINT32 task_id, FsmStateItem_t *p)
{
	int ret = 0;
	if (zHcuTaskInfo[task_id].swTaskActive != HCU_TASK_PNP_ON){
		HcuErrorPrint("HCU-MAIN: no need create this task [%s]!\n", zHcuTaskNameList[task_id]);
		return FAILURE;
	}
	HcuDebugPrint("HCU-MAIN: Staring to create task [%s] related environments...\n", zHcuTaskNameList[task_id]);
	ret = hcu_task_create_and_run(task_id, p);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: create task env [%s] un-successfully, program exit.\n", zHcuTaskNameList[task_id]);
		return FAILURE;
	}

	//将该任务执行时对应的状态转移机指针存入，以供以后扫描时使用
	zHcuTaskInfo[task_id].fsmPtr = p;
	return SUCCESS;
}

//创建队列环境
void hcu_task_create_hcumain_env(void)
{
	int ret = 0, taskId = 0;

	//Create all child task and relevant massage queue
	//一个不成功，就应该返回，如果不提前返回，纯粹是为了测试
	//Create HCU-Main Queid /1  该队列创建，纯粹是为了测试，以后需要删掉
	taskId = TASK_ID_HCUMAIN;
	HcuDebugPrint("HCU-MAIN: Staring to create task [%s] related environments...\n", zHcuTaskNameList[taskId]);
   	ret = hcu_msgque_create(taskId);
    if (ret == FAILURE){
    	HcuErrorPrint("HCU-MAIN: create queue [%s] un-successfully, program exit.\n", zHcuTaskNameList[taskId]);
    	return;
    }else{
    	HcuDebugPrint("HCU-MAIN: create queue zHcuTaskNameList[%s] successfully.\n", zHcuTaskNameList[taskId]);
    }
    return;
}

void hcu_task_send_init_to_timer(void)
{
	int ret = 0;

    //先初始化TIMER任务，不然后面的其它任务没法正常工作
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_TIMER, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_HCUMAIN], zHcuTaskNameList[TASK_ID_TIMER]);
	}

	//Wait for TIMER task feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_TIMER)){
				HcuDebugPrint("HCU-MAIN: TIMER task init completed. \n");
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}

}

void hcu_task_send_init_to_avorion(void)
{
	int ret = 0;

    //先初始化TIMER任务，不然后面的其它任务没法正常工作
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_AVORION, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_HCUMAIN], zHcuTaskNameList[TASK_ID_AVORION]);
	}

	//Wait for AVORION task feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_AVORION)){
				HcuDebugPrint("HCU-MAIN: AVORION task init completed. \n");
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}

}

void hcu_task_send_init_to_svrcon(void)
{
	int ret = 0;

	//发送初始化消息给主控任务SVRCON，以便初始化所有任务模块
	msg_struct_com_init_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_init_t));
	snd.length = sizeof(msg_struct_com_init_t);
	ret = hcu_message_send(MSG_ID_COM_INIT, TASK_ID_SVRCON, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_HCUMAIN], zHcuTaskNameList[TASK_ID_SVRCON]);
	}

	//Wait for SVRCON feedback
	HcuMsgSruct_t rcv;
	while (1){
		memset(&rcv, 0, sizeof(HcuMsgSruct_t));
		ret = hcu_message_rcv(TASK_ID_HCUMAIN, &rcv);
		if (ret == EINTR){
			continue;
		}
		else if (ret == FAILURE){
			HcuErrorPrint("HCU-MAIN: Receive message error, hold for Main-Control action!\n");
			hcu_sleep(1);
			continue;
			//hcu_sleep(1);  //wait for HCUMAIN clean all message queue and task, otherwise rubbish can not be clean.
			//exit(EXIT_FAILURE);  //do nothing, wait for main control clean myself.
		}else{
			if ((rcv.msgType == MSG_ID_COM_INIT_FEEDBACK) && (rcv.src_id == TASK_ID_SVRCON)){
				HcuDebugPrint("HCU-MAIN: Init completed, current process Id=%X[%s], Work Mode=%d, enter into SLEEP mode forever!\n",
						zHcuCurrentProcessInfo.curProcId, zHcuCurrentProcessInfo.curProcName, HCU_PROCESS_WORK_MODE_CURRENT);
				break;
			}else{
				HcuErrorPrint("HCU-MAIN: Error! Not right message received!\n");
				continue;
			}
		}
	}
}

//当做定时时钟使用，不依赖于任何时钟任务，比较安全可靠，废物利用
void hcu_task_send_hb_to_svrcon(void)
{
	int ret = 0;

	msg_struct_com_heart_beat_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_heart_beat_t));
	snd.length = sizeof(msg_struct_com_heart_beat_t);
	ret = hcu_message_send(MSG_ID_COM_HEART_BEAT, TASK_ID_SVRCON, TASK_ID_HCUMAIN, &snd, snd.length);
	if (ret == FAILURE){
		HcuErrorPrint("HCU-MAIN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_HCUMAIN], zHcuTaskNameList[TASK_ID_SVRCON]);
	}
	return;
}

void hcu_task_delete_all_and_queue(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	HcuDebugPrint("HCU-MAIN: Clean all running task and message queue for next time debug!\n");
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if (task_id != TASK_ID_HCUMAIN){hcu_task_delete(task_id);}
		hcu_msgque_delete(task_id);
	}
	return;
}

void hcu_task_delete_except_avorion_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_AVORION)){hcu_task_delete(task_id);}
	}
	return;
}

void hcu_task_delete_except_svrcon_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_SVRCON)){hcu_task_delete(task_id);}
	}
	return;
}

void hcu_task_delete_except_timer_and_hcumain(void)
{
	int task_id=0;

	//这是的清除，只是为了进程在系统终止后，清除相应的上下文
	//Test purpose, remove all message queue, to be removed at formal situation
	//task_id => starting from TASK_NAME_ID_MIN to TASK_NAME_ID_MAX
	for (task_id=TASK_ID_MIN; task_id<TASK_ID_MAX; task_id++){
		if ((task_id != TASK_ID_HCUMAIN) && (task_id != TASK_ID_TIMER)){hcu_task_delete(task_id);}
	}
	return;
}

//All child task and Queue creation
UINT32 hcu_system_task_init(void)
{
	int ret=0;
	int taskId = 0;
	//一个不成功，就应该返回，如果不提前返回，纯粹是为了测试
	//Create HCU-Main Queid /1  该队列创建，纯粹是为了测试，以后需要删掉
	taskId = TASK_ID_HCUMAIN;
	HcuDebugPrint("HCU-MAIN: Staring to create task [%s] related environments...\n", zHcuTaskNameList[taskId]);
   	ret = hcu_msgque_create(taskId);
    if (ret == FAILURE){
    	HcuErrorPrint("HCU-MAIN: create queue [%s] un-successfully, program exit.\n", zHcuTaskNameList[taskId]);
    	return FAILURE;
    }else{HcuDebugPrint("HCU-MAIN: create queue zHcuTaskNameList[%s] successfully.\n", zHcuTaskNameList[taskId]);}

	   //Create task Timer environments /2
	    hcu_system_task_init_call(TASK_ID_TIMER, FsmTimer);

	    //Create task Timer environments /3
	    hcu_system_task_init_call(TASK_ID_MMC, FsmMmc);

	    //Create task Timer environments /4
	    hcu_system_task_init_call(TASK_ID_HWINV, FsmHwinv);

	    //Create task SvnCont environments /5
	    hcu_system_task_init_call(TASK_ID_SVRCON, FsmSvrCon);

	    //Create task CloudCont environments /6
	    hcu_system_task_init_call(TASK_ID_CLOUDVELA, FsmCloudvela);

	    //Create task Modbus environments /7
	    hcu_system_task_init_call(TASK_ID_MODBUS, FsmModbus);

	    //Create task EMC environments/8
	    hcu_system_task_init_call(TASK_ID_EMC, FsmEmc);

	    //Create task PM25 environments/9
	    hcu_system_task_init_call(TASK_ID_PM25, FsmPm25);

	    //Create task WindDir environments/10
	    hcu_system_task_init_call(TASK_ID_WINDDIR, FsmWinddir);

	    //Create task WindSpeed environments/11
	    hcu_system_task_init_call(TASK_ID_WINDSPD, FsmWindspd);

	    //Create task Temperature environments/12
	    hcu_system_task_init_call(TASK_ID_TEMP, FsmTemp);

	    //Create task Humidity environments/13
	    hcu_system_task_init_call(TASK_ID_HUMID, FsmHumid);

	    //Create task Hsmmp environments/14
	    hcu_system_task_init_call(TASK_ID_HSMMP, FsmHsmmp);

	    //Create task Noise environments/15
	    hcu_system_task_init_call(TASK_ID_NOISE, FsmNoise);

	    //Create task Ethernet environments/16
	    hcu_system_task_init_call(TASK_ID_ETHERNET, FsmEthernet);

	    //Create task WIFI environments/17
	    hcu_system_task_init_call(TASK_ID_WIFI, FsmWifi);

	    //Create task USBNET environments/18
	    hcu_system_task_init_call(TASK_ID_USBNET, FsmUsbnet);

	    //Create task 3G4G environments/19
	    hcu_system_task_init_call(TASK_ID_3G4G, Fsm3g4g);

	    //Create task SPS232 environments/20
	    hcu_system_task_init_call(TASK_ID_SPS232, FsmSps232);

	    //Create task SPS485 environments/21
	    hcu_system_task_init_call(TASK_ID_SPS485, FsmSps485);

	    //Create task BLE environments/22
	    hcu_system_task_init_call(TASK_ID_BLE, FsmBle);

	    //Create task Audio environments/23
	    hcu_system_task_init_call(TASK_ID_SPSVIRGO, FsmSpsvirgo);

	    //Create task Avorion environments/24
	    hcu_system_task_init_call(TASK_ID_AVORION, FsmAvorion);

	    //Create task GPS environments/25
	    hcu_system_task_init_call(TASK_ID_GPS, FsmGps);

	    //Create task LCD environments/26
	    hcu_system_task_init_call(TASK_ID_LCD, FsmLcd);

	    //Create task LCD environments/26.1
	    hcu_system_task_init_call(TASK_ID_LED, FsmLed);

	    //Create task CAMERA environments/27
	    hcu_system_task_init_call(TASK_ID_CAMERA, FsmCamera);

		//Create task Microphone environments/28
		hcu_system_task_init_call(TASK_ID_MICROPHONE, FsmMicrophone);

		//Create task Pm25Sharp environments/28
		hcu_system_task_init_call(TASK_ID_PM25SHARP, FsmPm25Sharp);

    return SUCCESS;
}


/*
//test function area, to be removed afterwards
int i=0;
msg_struct_com_debug_t debug1;
for (i=0;i<5;i++){
	memset(&debug1, 0, sizeof(msg_struct_com_debug_t));
	debug1.testid = i;
	strcpy(&(debug1.testinfo[0]),"My test!");
	//debug1.testinfo[] = "My test!";
	debug1.length = sizeof(msg_struct_com_debug_t);
	ret = hcu_message_send(MSG_ID_COM_DEBUG_TEST, TASK_ID_SVRCON, TASK_ID_HCUMAIN, &debug1, debug1.length);
	if (ret == FAILURE){
		HcuDebugPrint("HCU-MAIN: Send message error!\n");
	}else{
		HcuDebugPrint("HCU-MAIN: Send message successful, testid = %d\n", debug1.testid);
	}
	hcu_sleep(1);
}
*/
