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
extern OPSTAT func_cloudvela_standard_xml_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_heart_beat_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_emc_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_pm25_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_winddir_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_windspd_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_temp_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_humid_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_noise_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_hsmmp_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_hcuinventory_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_swpackage_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_standard_xml_hsmmp_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);

//Extern API
//Added by Shanchun for SW download, SW inventory, AV upload
extern OPSTAT func_cloudvela_heart_beat_received_handle(void);
extern OPSTAT func_cloudvela_sw_download(char *filename);
extern OPSTAT func_cloudvela_av_upload(char *filename);

#endif /* L2FRAME_BHCODECXML_H_ */
