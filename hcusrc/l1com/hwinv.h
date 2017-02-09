/*
 * hwinv.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L1COM_HWINV_H_
#define L1COM_HWINV_H_

#include "../l0main/hcu.h"
#include "../l0comvm/vmlayer.h"
#include "../l0service/sysinfo.h"
#include "../l0dbi/l0dbi_inc.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_HWINV
{
	FSM_STATE_HWINV_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_HWINV_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_HWINV_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmHwinv[];

//API
extern OPSTAT fsm_hwinv_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hwinv_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hwinv_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT func_hwinv_global_par_init(void);
extern OPSTAT hcu_hwinv_engpar_read_pop_data_into_mem(void);
extern OPSTAT hcu_hwinv_engpar_create_storage_dir_env(void);
extern OPSTAT hcu_hwinv_engpar_read_mac_address(void);
extern void func_hwinv_copy_right(void);
extern void func_hwinv_scan_all(void);
extern void func_hwinv_scan_date();
extern void func_hwinv_scan_sysinfo(void);
extern void func_hwinv_scan_flash(void);
extern void func_hwinv_scan_rtc(void);
extern void func_hwinv_scan_memroy(void);
extern void func_hwinv_scan_main_board(void);
extern void func_hwinv_scan_hard_disc(void);//updated by shanchun
extern void func_hwinv_scan_gpio(void);
extern void func_hwinv_scan_sps232(void);
extern void func_hwinv_scan_sps485(void);
extern void func_hwinv_scan_ethernet(void);
extern void func_hwinv_scan_usbnet(void);
extern void func_hwinv_scan_3g4g(void);
extern void func_hwinv_scan_wifi(void);
extern void func_hwinv_scan_usb(void);
extern void func_hwinv_scan_microphone(void);
extern void func_hwinv_scan_camera(void);
extern void func_hwinv_scan_eng_par(void);
extern void func_hwinv_scan_dir(void);
extern void func_hwinv_scan_db(void);
extern void func_hwinv_scan_emc(void);
extern void func_hwinv_scan_pm25(void);
extern void func_hwinv_scan_winddir(void);
extern void func_hwinv_scan_windspd(void);
extern void func_hwinv_scan_temp(void);
extern void func_hwinv_scan_humid(void);
extern void func_hwinv_scan_airprs(void);
extern void func_hwinv_scan_co1(void);
extern void func_hwinv_scan_lightstr(void);
extern void func_hwinv_scan_alcohol(void);
extern void func_hwinv_scan_hcho(void);
extern void func_hwinv_scan_toxicgas(void);
extern void func_hwinv_scan_iwm(void);
extern void func_hwinv_scan_ihm(void);
extern void func_hwinv_scan_igm(void);
extern void func_hwinv_scan_ipm(void);
extern void func_hwinv_scan_pm25sharp(void);
extern void func_hwinv_scan_local_ui();
extern void func_hwinv_scan_message_queue(void);
extern UINT32 hcu_hwinv_create_multi_dir(const char *path); //create video server directory by Shanchun

//extern OPSTAT func_hwinv_scan_disksize(void);//for disk size checking by shanchun
extern OPSTAT hcu_is_dir(const char *path);//check if it is directory
extern OPSTAT hcu_is_file(const char *path);//check if it is file
extern OPSTAT hcu_is_special_dir(const char *path);//check if it is special file
extern void hcu_get_file_path(const char *path, const char *file_name, char *file_path);//check if it is special file
extern void hcu_delete_file(const char *path);//file delete

//External APIs


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_HWINV(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_HWINV]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L1COM_HWINV_H_ */
