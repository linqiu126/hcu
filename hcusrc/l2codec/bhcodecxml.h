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
#include "../l2frame/cloudvela.h"

//XML message pack/unpack
extern OPSTAT func_cloudvela_stdxml_msg_pack(CloudBhItfDevReportStdXml_t *xmlFormat, CloudDataSendBuf_t *buf);
extern OPSTAT func_cloudvela_stdxml_msg_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_stdxml_msg_emc_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
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
extern OPSTAT func_cloudvela_stdxml_heart_beat_unpack(msg_struct_com_cloudvela_data_rx_t *rcv);
extern OPSTAT func_cloudvela_av_upload(char *filename);


//Extern API

#endif /* L2FRAME_BHCODECXML_H_ */
