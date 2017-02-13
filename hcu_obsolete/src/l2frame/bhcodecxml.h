/*
 * bhcodecxml.h
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#ifndef L2FRAME_BHCODECXML_H_
#define L2FRAME_BHCODECXML_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//XML message pack/unpack
extern OPSTAT func_cloudvela_standard_xml_pack(CloudBhItfDevReportStdXml_t *xmlFormat, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdxml_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_heart_beat_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_emc_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_pm25_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_winddir_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_windspd_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_temp_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_humid_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_noise_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_hsmmp_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_hcuinventory_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_swpackage_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_hsmmp_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);

//Extern API
//Added by Shanchun for SW download, SW inventory, AV upload
extern OPSTAT func_cloudvela_hb_link_rcv_signal_check(void);
extern OPSTAT func_cloudvela_sw_download(char *filename);
extern OPSTAT func_cloudvela_av_upload(char *filename);

#endif /* L2FRAME_BHCODECXML_H_ */
