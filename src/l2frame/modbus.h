/*
 * modbus.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L2FRAME_MODBUS_H_
#define L2FRAME_MODBUS_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/sps485.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_MODBUS
{
	FSM_STATE_MODBUS_INITED = 0x02,
	FSM_STATE_MODBUS_ACTIVED,  //基本上采用无状态设计，全部采用同步读取的方式进行，简化操控和程序处理
	FSM_STATE_MODBUS_ACTIVED_WFFB,
	FSM_STATE_MODBUS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmModbus[];
extern GpsPosInfo_t zHcuGpsPosInfo;
extern sensor_modbus_opertion_general_t zPM25ConfigData; //by Shanchun for sensor config data

typedef struct SerialModbusMsgBuf
{
	UINT32 curLen;
	UINT8  curBuf[MAX_HCU_MSG_BODY_LENGTH];
}SerialModbusMsgBuf_t;

//FSM API
extern OPSTAT fsm_modbus_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_emc_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_pm25_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_winddir_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_windspd_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_temp_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_humid_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_noise_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_sps232_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_sps485_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_ble_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_emc_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_pm25_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_winddir_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_windspd_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_temp_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_humid_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_modbus_noise_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local func
extern OPSTAT func_modbus_emc_msg_pack(msg_struct_emc_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_emc_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_emc_modbus_data_read_t *rcv, msg_struct_modbus_emc_data_report_t *snd);
extern OPSTAT func_modbus_pm25_msg_pack(msg_struct_pm25_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_pm25_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_data_read_t *rcv, msg_struct_modbus_pm25_data_report_t *snd);
extern OPSTAT func_modbus_winddir_msg_pack(msg_struct_winddir_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_winddir_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_winddir_modbus_data_read_t *rcv, msg_struct_modbus_winddir_data_report_t *snd);
extern OPSTAT func_modbus_windspd_msg_pack(msg_struct_windspd_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_windspd_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_windspd_modbus_data_read_t *rcv, msg_struct_modbus_windspd_data_report_t *snd);
extern OPSTAT func_modbus_temp_msg_pack(msg_struct_temp_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_temp_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_temp_modbus_data_read_t *rcv, msg_struct_modbus_temp_data_report_t *snd);
extern OPSTAT func_modbus_humid_msg_pack(msg_struct_humid_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_humid_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_humid_modbus_data_read_t *rcv, msg_struct_modbus_humid_data_report_t *snd);
extern OPSTAT func_modbus_noise_msg_pack(msg_struct_noise_modbus_data_read_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_noise_msg_unpack(SerialModbusMsgBuf_t *buf, msg_struct_noise_modbus_data_read_t *rcv, msg_struct_modbus_noise_data_report_t *snd);

//Added by Shanchun for Control command
extern OPSTAT func_modbus_pm25_cmd_pack(msg_struct_pm25_modbus_control_cmd_t *inMsg, SerialModbusMsgBuf_t *outMsg);
extern OPSTAT func_modbus_pm25_cmd_unpack(SerialModbusMsgBuf_t *buf, msg_struct_pm25_modbus_control_cmd_t *rcv, msg_struct_modbus_pm25_control_fb_t *snd);

//CRC16
extern UINT16 CalcCRCModBus(UINT8 cDataIn, UINT16 wCRCIn);
extern void CheckCRCModBus(UINT8* pDataIn, UINT32 iLenIn, UINT16* pCRCOut);
extern UINT16 crc_ccitt(unsigned char *q, int len);

//外部函数引用API by Shanchun
extern UINT32 hcu_sps485_serial_init(SerialPort_t *sp);
extern UINT32 hcu_sps485_serial_port_get(SerialPort_t *sp, UINT8 *send_buf, UINT32 Len);
extern UINT32 hcu_sps485_serial_port_send(SerialPort_t *sp, UINT8 *rcv_buf, UINT32 Len);


#endif /* L2FRAME_MODBUS_H_ */
