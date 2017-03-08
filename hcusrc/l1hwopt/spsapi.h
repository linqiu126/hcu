/*
 * spsapi.h
 *
 *  Created on: 2016年6月15日
 *      Author: hitpony
 */

#ifndef L1HWOPT_SPSAPI_H_
#define L1HWOPT_SPSAPI_H_

#include "../l0comvm/vmlayer.h"

#define SPSAPI_COM_PORT_0 	0x00
#define SPSAPI_COM_PORT_1 	0x01
#define SPSAPI_COM_PORT_2 	0x02
#define SPSAPI_COM_PORT_3 	0x03
#define SPSAPI_COM_PORT_4 	0x04
#define SPSAPI_COM_PORT_5 	0x05
#define SPSAPI_COM_PORT_6 	0x06
#define SPSAPI_COM_PORT_7 	0x07

#define SPSAPI_COM_PORT_INVALIAD	0xFF
#define SPSAPI_TIMER_FOR_SERIAL_PORT_READ 100000

extern UINT32 spsapi_SerialPortOpen(UINT8 com_port_to_open, UINT16 *fd);
extern UINT32 spsapi_SerialPortSet(SerialPortCom_t *sp);
extern UINT32 spsapi_SerialPortClose(UINT16 fd);

extern UINT32 hcu_spsapi_SerialPortSetVtimeVmin(SerialPortCom_t *sp, UINT8 vTime, UINT8 vMin);
extern UINT32 hcu_spsapi_serial_init(SerialPortCom_t *sp);
extern UINT32 hcu_spsapi_serial_port_get(SerialPortCom_t *sp, UINT8 *send_buf, UINT32 Len);
extern UINT32 hcu_spsapi_serial_port_send(SerialPortCom_t *sp, UINT8 *rcv_buf, UINT32 Len);

#endif /* L1HWOPT_SPSAPI_H_ */
