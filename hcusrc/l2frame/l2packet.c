#include <string.h>
#include <stdint.h>

#include "l2packet.h"

/* generate check sum */
uint8_t l2packet_gen_chksum(IHU_HUITP_L2FRAME_STD_frame_header_t *pMsgHeader)
{
	uint8_t *pData = (uint8_t *)pMsgHeader;
	
	return pData[0]^pData[2]^pData[3];
}

/* return: the size of consumed data */
uint16_t l2packet_rx_bytes(IHU_HUITP_L2FRAME_Desc_t *pdesc, uint8_t *data, uint16_t data_size)
{
	uint32_t i = 0;
	IHU_HUITP_L2FRAME_STD_frame_header_t *pMsgHeader;

	if(pdesc == NULL || pdesc->pRxBuffPtr == NULL)
		return 0;
	
	pMsgHeader = (IHU_HUITP_L2FRAME_STD_frame_header_t *)pdesc->pRxBuffPtr;
	
	for(i=0; i<data_size; i++)
	{
		/* save one byte one time */
		pdesc->pRxBuffPtr[pdesc->RxXferCount] = data[i];
		pdesc->RxXferCount ++;

		switch(pdesc->RxState)
		{
			case IHU_L2PACKET_RX_STATE_START:
			if(pMsgHeader->start != IHU_L2PACKET_START_CHAR)
			{
				// not synchonrized, reset the RxXferCount
				pdesc->RxXferCount = 0;
			}
			else
				pdesc->RxState = IHU_L2PACKET_RX_STATE_HEADER;
			break;

			case IHU_L2PACKET_RX_STATE_HEADER:
			if(pdesc->RxXferCount >= sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
			{
				if(l2packet_gen_chksum(pMsgHeader) != pMsgHeader->chksum)
				{
					pdesc->RxXferCount = 0;
					pdesc->RxState = IHU_L2PACKET_RX_STATE_START;
				}
				else
				{
					pdesc->RxState = IHU_L2PACKET_RX_STATE_BODY;
				}
			}
			break;

			case IHU_L2PACKET_RX_STATE_BODY:
			if(pdesc->RxXferCount >= pMsgHeader->len)
			{
				// call user's callback after the receive is complete
				if(pdesc->app_rx_callback)
					pdesc->app_rx_callback(pdesc);
				
				pdesc->RxState = IHU_L2PACKET_RX_STATE_START;
				pdesc->RxXferCount = 0;
			}

			if(pdesc->RxXferCount >= pdesc->RxBuffSize)
			{
				pdesc->RxXferCount = 0;
				pdesc->RxState = IHU_L2PACKET_RX_STATE_START;
			}
			break;
			
			default:
			pdesc->RxXferCount = 0;
			pdesc->RxState = IHU_L2PACKET_RX_STATE_START;
			break;
		}
	}
	
	return i;
}

