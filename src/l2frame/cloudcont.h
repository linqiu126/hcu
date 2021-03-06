/*
 * cloudconnect.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L2FRAME_CLOUDCONT_H_
#define L2FRAME_CLOUDCONT_H_

#include "../comvm/vmlayer.h"
#include "../l2frame/l2comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_CLOUDCONT
{
	FSM_STATE_CLOUDCONT_INITED = 0x02,
	FSM_STATE_CLOUDCONT_OFFLINE,  //离线只干一件事，就是想办法连上，通过定时心跳检测的方式来进行
	FSM_STATE_CLOUDCONT_ONLINE,
	FSM_STATE_CLOUDCONT_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF


//Global variables
extern FsmStateItem_t FsmCloudCont[];
extern HcuDiscDataSampleStorage_t zHcuMemStorageBuf;
extern CloudControlTable_t zHcuCloudConTable;
extern CURL *zHcuCloudCurlPtr;

#define CLOUD_TIMER_DURATION_PERIOD_HEART_BEAT 10  //in second
#define CLOUD_TIMER_DURATION_PERIOD_SEND_DATA_BACK 1 //in second


//FSM API
extern OPSTAT fsm_cloudcont_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_emc_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_pm25_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_winddir_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_windspd_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_temp_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_humid_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_usbnet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_wifi_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_3g4g_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_cloudcont_hwinv_phy_status_chg(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_coundcont_http_conn_setup(void);
OPSTAT func_coundcont_heart_beat_check(void);
OPSTAT func_cloudcont_time_out_period(void);
OPSTAT func_cloudcont_time_out_sendback_offline_data(void);
OPSTAT func_cloudcont_send_data_to_cloud(CloudDataSendBuf_t *buf);

//message pack/unpack
extern OPSTAT func_cloudcont_huanbao_emc_msg_pack();
extern OPSTAT func_cloudcont_huanbaos_emc_msg_unpack();
extern OPSTAT func_cloudcont_huanbao_pm25_msg_pack();
extern OPSTAT func_cloudcont_huanbao_pm25_msg_unpack();
extern OPSTAT func_cloudcont_huanbao_winddir_msg_pack();
extern OPSTAT func_cloudcont_huanbao_winddir_msg_unpack();
extern OPSTAT func_cloudcont_huanbao_windspd_msg_pack();
extern OPSTAT func_cloudcont_huanbao_windspd_msg_unpack();
extern OPSTAT func_cloudcont_huanbao_temp_msg_pack();
extern OPSTAT func_cloudcont_huanbao_temp_msg_unpack();
extern OPSTAT func_cloudcont_huanbao_humid_msg_pack();
extern OPSTAT func_cloudcont_huanbao_humid_msg_unpack();

//Global API
extern OPSTAT hcu_save_to_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_read_from_storage_disc(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT hcu_save_to_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT hcu_read_from_storage_mem(HcuDiscDataSampleStorageArray_t *record);
extern OPSTAT hcu_cloud_http_link_init(void);
extern size_t hcu_cloud_write_callback(void *buffer, size_t size, size_t nmemb, void *userp);

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

#endif /* L2FRAME_CLOUDCONT_H_ */
