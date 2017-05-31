#ifndef  __L2_PACKET_H__
#define  __L2_PACKET_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//GENERAL FRAME帧结构定义
typedef struct HCU_HUITP_L2FRAME_STD_frame_header
{
  uint8_t start;
  uint8_t chksum;
  uint16_t len;     // the length including the size of header
}HCU_HUITP_L2FRAME_STD_frame_header_t;

typedef struct HCU_HUITP_L2FRAME_Desc
{
	uint32_t RxState;
	uint8_t *pRxBuffPtr;
	uint8_t  wmc_id;
	uint16_t RxBuffSize;
	uint16_t RxXferCount;
	void (*app_rx_callback)();
	void *UserData;
}HCU_HUITP_L2FRAME_Desc_t;


#define IHU_L2PACKET_RX_STATE_START 0
#define IHU_L2PACKET_RX_STATE_HEADER 1
#define IHU_L2PACKET_RX_STATE_BODY 2
#define IHU_L2PACKET_START_CHAR 0xFE

uint8_t l2packet_gen_chksum(HCU_HUITP_L2FRAME_STD_frame_header_t *pMsgHeader);
uint16_t l2packet_rx_bytes(HCU_HUITP_L2FRAME_Desc_t *pdesc, uint8_t *data, uint16_t data_size);

#ifdef __cplusplus
}
#endif
#endif //__L2_PACKET_H__
