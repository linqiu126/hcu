/*
 * bhcodeczhb.h
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#ifndef L2FRAME_BHCODECZHB_H_
#define L2FRAME_BHCODECZHB_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//ZHB message pack/unpack
extern OPSTAT func_cloudvela_huanbao_heart_beat_msg_pack(CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_cmd_control_msg_pack(CloudDataSendBuf_t *buf);//for cmd control by Shanchun
extern OPSTAT func_cloudvela_huanbao_emc_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 emcValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp,CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_pm25_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 pm1d0Value, UINT32 pm2d5Value, UINT32 pm10Value, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_pm25_cmd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 powerOnOff, UINT32 interSample, UINT32 meausTimes, UINT32 newEquId, UINT32 workCycle,CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_winddir_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 winddirValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_windspd_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 windspdValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_temp_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 tempValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_humid_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 humidValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_noise_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 noiseValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_hsmmp_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew,  UINT32 timeStamp, char *linkName, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_sw_download_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 swDownload, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_av_upload_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 avUpload, char *avFileName, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_hcu_inventory_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, SysEngParElementSwInvInfo_t *hcuInventoryInfo, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_bfsc_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 optpar, UINT32 equipId, UINT8 dataFormat, UINT32 modbusValue, UINT8 snesorNbr, UINT32 *sensorValue, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_standard_zhb_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_zhb_pack(CloudBhItfDevReportStdZhb_t *zhbFormat, CloudDataSendBuf_t *buf);

//for alarm & pm report
extern OPSTAT func_cloudvela_huanbao_alarm_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 useroptid, UINT8 cmdIdBackType, UINT8 alarmType, UINT32 alarmContent, UINT8 equID, UINT8 alarmSeverity, UINT8 alarmClearFlag, char *avFileName, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_huanbao_pm_msg_pack(UINT8 msgType, UINT8 cmdId, UINT8 useroptid, UINT8 cmdIdBackType, UINT32 cloudVelaConnCnt, UINT32 cloudVelaConnFailCnt, UINT32 cloudVelaDiscCnt, UINT32 SocketDiscCnt, UINT32 TaskRestartCnt,  UINT32 cpu_occupy, UINT32 mem_occupy, UINT32 disk_occupy, UINT32 timeStamp, CloudDataSendBuf_t *buf);


//Extern API
//Added by Shanchun for SW download, SW inventory, AV upload
extern OPSTAT func_cloudvela_heart_beat_received_handle(void);
extern OPSTAT func_cloudvela_sw_download(char *filename);
extern OPSTAT func_cloudvela_av_upload(char *filename);

#endif /* L2FRAME_BHCODECZHB_H_ */
