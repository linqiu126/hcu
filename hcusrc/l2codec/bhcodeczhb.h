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
#include "../l2frame/cloudvela.h"

//ZHB message pack/unpack
extern OPSTAT func_cloudvela_stdzhb_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdzhb_msg_pack(CloudBhItfDevReportStdZhb_t *zhbFormat, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_heart_beat_pack(CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_emc_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 emcValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp,CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_pm25_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 pm1d0Value, UINT32 pm2d5Value, UINT32 pm10Value, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_pm25_cmd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 powerOnOff, UINT32 interSample, UINT32 meausTimes, UINT32 newEquId, UINT32 workCycle,CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_winddir_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 winddirValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_windspd_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 windspdValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_temp_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 tempValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_humid_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 humidValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_noise_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT8 dataFormat, UINT32 noiseValue, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_hsmmp_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT32 equipId, UINT32 gpsx, UINT32 gpsy, UINT32 gpsz, CHAR ns, CHAR ew,  UINT32 timeStamp, char *linkName, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_sw_download_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 swDownload, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_av_upload_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, UINT8 avUpload, char *avFileName, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_hcu_inventory_pack(UINT8 msgType, UINT8 cmdId, UINT8 optId, UINT8 backType, SysEngParElementHwBurnPhyIdAddr_t *hcuInventoryInfo, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_alarm_pack(UINT8 msgType, UINT8 cmdId, UINT8 useroptid, UINT8 cmdIdBackType, UINT8 alarmType, UINT32 alarmContent, UINT8 equID, UINT8 alarmSeverity, UINT8 alarmClearFlag, char *avFileName, UINT32 timeStamp, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdzhb_msg_pm_pack(UINT8 msgType, UINT8 cmdId, UINT8 useroptid, UINT8 cmdIdBackType, UINT32 cloudVelaConnCnt, UINT32 cloudVelaConnFailCnt, UINT32 cloudVelaDiscCnt, UINT32 SocketDiscCnt, UINT32 TaskRestartCnt,  UINT32 cpu_occupy, UINT32 mem_occupy, UINT32 disk_occupy, UINT32 timeStamp, CloudDataSendBuf_t *buf);

//Extern API


#endif /* L2FRAME_BHCODECZHB_H_ */
