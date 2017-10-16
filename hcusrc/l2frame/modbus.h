/*
 * modbus.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L2FRAME_MODBUS_H_
#define L2FRAME_MODBUS_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"
#include "../l1com/l1comdef.h"

#include "../l3appctrl/l3aqycg20.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_MODBUS
{
	FSM_STATE_MODBUS_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_MODBUS_ACTIVED,  //基本上采用无状态设计，全部采用同步读取的方式进行，简化操控和程序处理
	FSM_STATE_MODBUS_ACTIVED_WFFB,
	FSM_STATE_MODBUS_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmModbus[];

typedef struct gTaskModbusContext
{
	UINT32 NoiseHW_AlarmFlag;
	UINT32 TspHW_AlarmFlag;
	UINT32 TempHW_AlarmFlag;
	UINT32 HumidHW_AlarmFlag;
	UINT32 WindDirHW_AlarmFlag;
	UINT32 WindSpdHW_AlarmFlag;
}gTaskModbusContext_t;

typedef struct SerialModbusMsgBuf
{
	UINT32 curLen;
	UINT8  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
}SerialModbusMsgBuf_t;

#define MODBUS_TIMER_FOR_SERIAL_PORT_READ 10

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

//外部函数引用API by Shanchun
extern UINT32 hcu_sps485_serial_init(SerialPortCom_t *sp);
extern UINT32 hcu_sps485_serial_port_get(SerialPortCom_t *sp, UINT8 *send_buf, UINT32 Len);
extern UINT32 hcu_sps485_serial_port_send(SerialPortCom_t *sp, UINT8 *rcv_buf, UINT32 Len);

long long int hcu_getS(int e,int m);
float hcu_hex2float(unsigned char *p);

#endif /* L2FRAME_MODBUS_H_ */
