/*
 * cloudvela.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L2FRAME_CLOUDVELA_H_
#define L2FRAME_CLOUDVELA_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/bhcodecxml.h"
#include "../l2frame/bhcodeczhb.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CLOUDVELA
{
	FSM_STATE_CLOUDVELA_INITED = 0x02,
	FSM_STATE_CLOUDVELA_OFFLINE,  //离线只干一件事，就是想办法连上，通过定时心跳检测的方式来进行
	FSM_STATE_CLOUDVELA_ONLINE,
	FSM_STATE_CLOUDVELA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmCloudvela[];
extern HcuDiscDataSampleStorage_t zHcuMemStorageBuf;
extern CloudvelaTable_t zHcuCloudvelaTable;
extern CURL *zHcuCloudCurlPtr;
extern GpsPosInfo_t zHcuGpsPosInfo;//for test by shanchun

//Added by Shanchun for CMD command
extern UINT8 CMDShortTimerFlag;
extern UINT8 CMDLongTimerFlag;
extern UINT32 CMDPollingNoCommandNum;

//#define CLOUDVELA_TIMER_DURATION_PERIOD_HEART_BEAT 600  //in second
//#define CLOUDVELA_TIMER_DURATION_PERIOD_SEND_DATA_BACK 80 //in second
//#define CLOUDVELA_TIMER_DURATION_PERIOD_CMD_CONTROL_LONG 600  //for cmd control long timer by Shanchun
//#define CLOUDVELA_TIMER_DURATION_PERIOD_CMD_CONTROL_SHORT 5  //for cmd control short timer by Shanchun

//FSM API
extern OPSTAT fsm_cloudvela_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_noise_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_control_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hsmmp_data_link_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_emc_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm25_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_winddir_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_windspd_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_temp_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_humid_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_noise_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//for alarm & pm report added by ZSC
extern OPSTAT fsm_cloudvela_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudvela_pm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_cloudvela_http_conn_setup(void);
OPSTAT func_cloudvela_heart_beat_check(void);
OPSTAT func_cloudvela_time_out_period(void);
OPSTAT func_cloudvela_cmd_control_check(void);//for cmd control by Shanchun
OPSTAT func_cloudvela_time_out_period_for_cmd_control(void);//for cmd control by Shanchun
OPSTAT func_cloudvela_time_out_sendback_offline_data(void);
OPSTAT func_cloudvela_send_data_to_cloud(CloudDataSendBuf_t *buf);

//Added by Shanchun for SW download, SW inventory, AV upload
extern OPSTAT func_cloudvela_heart_beat_received_handle(void);
extern OPSTAT func_cloudvela_sw_download(char *filename);
extern OPSTAT func_cloudvela_av_upload(char *filename);

//Global API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT hcu_read_from_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT hcu_cloudvela_http_link_init(void);
extern size_t hcu_cloudvela_write_callback(void *buffer, size_t size, size_t nmemb, void *userp);

//char* func_cloudvela_get_file_path(char *file, char *buf, int count);

//引用外部函数
extern UINT32 hcu_disk_write(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_ethernet_date_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_usbnet_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_wifi_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_3g4g_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_ethernet_phy_link_setup(void);
extern OPSTAT hcu_usbnet_phy_link_setup(void);
extern OPSTAT hcu_wifi_phy_link_setup(void);
extern OPSTAT hcu_3g4g_phy_link_setup(void);
extern OPSTAT hcu_ethernet_phy_link_disconnect(void);
extern OPSTAT hcu_usbnet_phy_link_disconnect(void);
extern OPSTAT hcu_wifi_phy_link_disconnect(void);
extern OPSTAT hcu_3g4g_phy_link_disconnect(void);
extern void CheckCRCModBus(UINT8* pDataIn, UINT32 iLenIn, UINT16* pCRCOut);
extern void func_hwinv_scan_date(void);

#endif /* L2FRAME_CLOUDVELA_H_ */
