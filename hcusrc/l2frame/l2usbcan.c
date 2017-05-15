/*
 * l2usbcan.c
 *
 *  Created on: May 15, 2017
 *      Author: MA Yuchu
 */

#ifdef WIN32 // for windows
#   include <windows.h>
#   include <process.h>
#   include <stdio.h>
#   include <time.h>
#   include "controlcan.h"
#   pragma comment(lib, "controlcan.lib")
#   define msleep(ms)  Sleep(ms)
typedef HANDLE pthread_t;
#else // for linux
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <pthread.h>
//#   include "controlcan.h"
#   include "l2usbcan.h"
#   include "../l0comvm/comtype.h"
#   include "../l1com/l1comdef.h"
#	include "l2packet.h"
#   define msleep(ms)  usleep((ms)*1000)
#   define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define MAX_CHANNELS  4
#define CHECK_POINT  200
#define RX_WAIT_TIME  100
#define RX_BUFF_SIZE  1000

unsigned gDevType = 0;
unsigned gDevIdx = 0;
unsigned gChMask = 0;
unsigned gBaud = 0;
unsigned gTxType = 0;
unsigned gTxSleep = 0;
unsigned gTxFrames = 0;

void bfsc_can_test();
void USB_CAN_RxCpltCallback(USB_CAN_HandleTypeDef* CanHandle, VCI_CAN_OBJ *Can);

int bsp_can_start_rx(USB_CAN_HandleTypeDef* CanHandle, void (*app_rx_callback)(), uint8_t *pRxBuffPtr, uint16_t rxBufferSize, void *user_data);
void app_can_loopback_callback(IHU_HUITP_L2FRAME_Desc_t *pdesc);

unsigned s2n(const char *s)
{
    unsigned l = strlen(s);
    unsigned v = 0;
    unsigned h = (l > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
    unsigned char c;
    unsigned char t;
    if (!h) return atoi(s);
    if (l > 10) return 0;
    for (s += 2; c = *s; s++)
    {
        if (c >= 'A' && c <= 'F') c += 32;
        if (c >= '0' && c <= '9') t = c - '0';
        else if (c >= 'a' && c <= 'f') t = c - 'a' + 10;
        else return 0;
        v = (v << 4) | t;
    }
    return v;
}

void generate_frame(VCI_CAN_OBJ *can)
{
    memset(can, 0, sizeof(VCI_CAN_OBJ));
    can->SendType = gTxType;
    //can->DataLen = 1 + (rand() % 8); // random data length: 1~8
    can->DataLen = 8; // random data length: 1~8

    unsigned i;
    for (i = 0; i < can->DataLen; i++)
    {
        //can->Data[i] = rand() & 0xff; // random data
        can->Data[i] = i & 0xff; // random data
        //can->ID ^= can->Data[i]; // id: bit0~7, checksum of data0~N
        //can->ID = 0x603; // id: bit0~7, checksum of data0~N
    }
    can->ID |= ((unsigned)can->DataLen - 1) << 8; // id: bit8~bit10 = data_length-1
    can->ExternFlag = 0;//rand() % 2; // random frame format
    if (!can->ExternFlag)
        return;
    //can->ID |= can->ID << 11; // id: bit11~bit21 == bit0~bit10
    //can->ID |= can->ID << 11; // id: bit22~bit28 == bit0~bit7
}

int verify_frame(VCI_CAN_OBJ *can)
{
    if (can->DataLen > 8) return 0; // error: data length
    return 1;
    unsigned bcc = 0;
    unsigned i;
    for (i = 0; i < can->DataLen; i++)
        bcc ^= can->Data[i];
    if ((can->ID & 0xff) != bcc) return 0; // error: data checksum
    if (((can->ID >> 8) & 7) != (can->DataLen - 1)) return 0; // error: data length
    if (!can->ExternFlag) return 1; // std-frame ok
    if (((can->ID >> 11) & 0x7ff) != (can->ID & 0x7ff)) return 0; // error: frame id
    if (((can->ID >> 22) & 0x7f) != (can->ID & 0x7f)) return 0; // error: frame id
    return 1; // ext-frame ok
}

typedef struct {
    unsigned channel; // channel index, 0~3
    unsigned stop; // stop RX-thread
    unsigned total; // total received
    unsigned error; // error(s) detected
} RX_CTX;

#ifdef WIN32
unsigned __stdcall rx_thread(void *data)
#else
void * rx_thread(void *data)
#endif
{
    RX_CTX *ctx = (RX_CTX *)data;
    ctx->total = 0; // reset counter

    VCI_CAN_OBJ can[RX_BUFF_SIZE]; // buffer
    int cnt; // current received
    int i;

    unsigned check_point = 0;
    while (!ctx->stop && !ctx->error)
    {
        cnt = VCI_Receive(gDevType, gDevIdx, ctx->channel, can, RX_BUFF_SIZE, RX_WAIT_TIME);
        if (!cnt)
            continue;

        for (i = 0; i < cnt; i++) {
            if (verify_frame(&can[i]))
            {
           	printf("CAN%d: received [%02X %02X %02X %02X %02X %02X %02X %02X] \n", ctx->channel,
           			can[i].Data[0], can[i].Data[1], can[i].Data[2], can[i].Data[3],
					can[i].Data[4], can[i].Data[5], can[i].Data[6], can[i].Data[7]);
            	continue;
            }
            printf("CAN%d: verify_frame() failed\n", ctx->channel);
            ctx->error = 1;
            break;
        }
        if (ctx->error) break;

        ctx->total += cnt;
        if (ctx->total / CHECK_POINT >= check_point) {
            printf("CAN%d: %d frames received & verified\n", ctx->channel, ctx->total);
            check_point++;
        }
    }

    printf("CAN%d RX thread terminated, %d frames received & verified: %s\n",
        ctx->channel, ctx->total, ctx->error ? "error(s) detected" : "no error");

#ifdef WIN32
    _endthreadex(0);
    return 0;
#else
    pthread_exit(0);
#endif
}

int usbcan_test()
{
    // ----- init & start -------------------------------------------------

    VCI_INIT_CONFIG config;
    config.AccCode = 0;
    config.AccMask = 0xffffffff;
    config.Filter = 1;
    config.Mode = 0;
    config.Timing0 = gBaud & 0xff;
    config.Timing1 = gBaud >> 8;

    int i;
    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if ((gChMask & (1 << i)) == 0) continue;

        if (!VCI_InitCAN(gDevType, gDevIdx, i, &config))
        {
            printf("VCI_InitCAN(%d) failed\n", i);
            return 0;
        }
        printf("VCI_InitCAN(%d) succeeded\n", i);

        if (!VCI_StartCAN(gDevType, gDevIdx, i))
        {
            printf("VCI_StartCAN(%d) failed\n", i);
            return 0;
        }
        printf("VCI_StartCAN(%d) succeeded\n", i);
    }

    // ----- RX-timeout test ----------------------------------------------

    VCI_CAN_OBJ can;
    time_t tm1, tm2;
    for (i = 0; i < 3; i++)
    {
        time(&tm1);
        VCI_Receive(gDevType, gDevIdx, 0, &can, 1, (i + 1) * 1000/*ms*/);
        time(&tm2);
        printf("VCI_Receive returned: time ~= %ld seconds\n", tm2 - tm1);
    }

    // ----- create RX-threads --------------------------------------------

    RX_CTX rx_ctx[MAX_CHANNELS];
    pthread_t rx_threads[MAX_CHANNELS];
    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if ((gChMask & (1 << i)) == 0) continue;

        rx_ctx[i].channel = i;
        rx_ctx[i].stop = 0;
        rx_ctx[i].total = 0;
        rx_ctx[i].error = 0;
#ifdef WIN32
        rx_threads[i] = (HANDLE)_beginthreadex(NULL, 0, rx_thread, &rx_ctx[i], 0, NULL);
#else
        pthread_create(&rx_threads[i], NULL, rx_thread, &rx_ctx[i]);
#endif
    }

    // ----- wait --------------------------------------------------------

    printf("<ENTER> to start TX: %d frames/channel, baud: t0=0x%02x, t1=0x%02x...\n",
        gTxFrames, config.Timing0, config.Timing1);
    getchar();

    // ----- start transmit -----------------------------------------------

    time(&tm1);
    int err = 0;
    unsigned tx;
    for (tx = 0; !err && tx < gTxFrames; tx++)
    {
        for (i = 0; i < MAX_CHANNELS; i++)
        {
            if ((gChMask & (1 << i)) == 0) continue;

            generate_frame(&can);
            if (1 != VCI_Transmit(gDevType, gDevIdx, i, &can, 1))
            {
                printf("CAN%d TX failed: ID=%08x\n", i, can.ID);
                err = 1;
                break;
            }
        }
        if (gTxSleep) msleep(gTxSleep);
    }
    time(&tm2);

    // ----- stop TX & RX -------------------------------------------------

    msleep(1000);
    printf("TX stopped, <ENTER> to terminate RX-threads...\n");
    getchar();

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if ((gChMask & (1 << i)) == 0) continue;

        rx_ctx[i].stop = 1;
#ifdef WIN32
        WaitForSingleObject(rx_threads[i], INFINITE);
        CloseHandle(rx_threads[i]);
#else
        pthread_join(rx_threads[i], NULL);
#endif
        if (rx_ctx[i].error)
            err = 1;
    }

    // ----- report -------------------------------------------------------

    if (err) {
        printf("error(s) detected, test failed\n");
        return 0;
    }

    printf("\n ***** %d frames/channel transferred, %ld seconds elapsed *****\n",
        gTxFrames, tm2 - tm1);
    if (tm2 - tm1)
        printf("        performance: %ld frames/channel/second\n", gTxFrames / (tm2 - tm1));

    return 1;
}

int usbcan_test_main(int argc, char* argv[])
{
    if (argc < 7) {
        printf("test [DevType] [DevIdx] [ChMask] [Baud] [TxType] [TxSleep] [TxFrames]\n"
            "    example: test 16 0 3 0x1400 2 3 1000\n"
            "                  |  | | |      | | |\n"
            "                  |  | | |      | | |1000 frames / channel\n"
            "                  |  | | |      | |\n"
            "                  |  | | |      | |tx > sleep(3ms) > tx > sleep(3ms) ....\n"
            "                  |  | | |      |\n"
            "                  |  | | |      |0-normal, 1-single, 2-self_test, 3-single_self_test, 4-single_no_wait....\n"
            "                  |  | | |\n"
            "                  |  | | |0x1400-1M, 0x1c03-125K, ....\n"
            "                  |  | |\n"
            "                  |  | |bit0-CAN1, bit1-CAN2, bit2-CAN3, bit3-CAN4, 3=CAN1+CAN2, 7=CAN1+CAN2+CAN3\n"
            "                  |  |\n"
            "                  |  |Card0\n"
            "                  |\n"
            "                  |5-pci9820, 14-pci9840, 16-pci9820i, ....\n"
            );
        return 0;
    }

    gDevType = s2n(argv[1]);
    gDevIdx = s2n(argv[2]);
    gChMask = s2n(argv[3]);
    gBaud = s2n(argv[4]);
    gTxType = s2n(argv[5]);
    gTxSleep = s2n(argv[6]);
    gTxFrames = s2n(argv[7]);
    printf("DevType=%d, DevIdx=%d, ChMask=0x%x, Baud=0x%04x, TxType=%d, TxSleep=%d, TxFrames=0x%08x(%d)\n",
        gDevType, gDevIdx, gChMask, gBaud, gTxType, gTxSleep, gTxFrames, gTxFrames);

    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        return 0;
    }
    printf("VCI_OpenDevice succeeded\n");

    usbcan_test();

    VCI_CloseDevice(gDevType, gDevIdx);
    printf("VCI_CloseDevice\n");

    bfsc_can_test();

    return 0;
}


/*
 ** ====================================================================================================
 **
 **
*/
/////////////////////////////////////////////////////////////////////////////////////////////////
//  CAN interface by puhuix
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "l2packet.h"

#define BFSC_CAN_MAX_RX_BUF_SIZE 256
IHU_HUITP_L2FRAME_Desc_t g_can_packet_desc[2];
uint8_t g_can_rx_buffer[BFSC_CAN_MAX_RX_BUF_SIZE];

/*
 * @brief  CAN handle Structure definition
 */
//typedef struct
//{
//	UINT32 can_dev_type;
//	UINT32 can_dev_id;
//	UINT32 can_channel_id;
//	UINT32 band_rate_kbps;
//	VCI_INIT_CONFIG can_config;
//	VCI_CAN_OBJ can_data;
//}USB_CAN_HandleTypeDef;

/*
 * Mapping the bandrate to Timing register value according to the ZLG manual
 */
UINT32 can_bandrate_to_timing_mapping(UINT32 band_rate_kbps, UINT8 *timing0, UINT8 *timing1)
{
	UINT32 ret = SUCCESS;

	if( (NULL == timing0) || (NULL == timing1) )
	{
		printf("can_bandrate_to_timing_mapping: (NULL == timing0) || (NULL == timing1), return FAILURE\r\n");
		return FAILURE;
	}

	switch(band_rate_kbps)
	{
		case CAN_BANDRATE_5KBPS:
			*timing0 = 0xBF;
			*timing1 = 0xFF;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_10KBPS:
			*timing0 = 0xFF;
			*timing1 = 0xFF;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_20KBPS:
			*timing0 = 0x53;
			*timing1 = 0x2F;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_40KBPS:
			*timing0 = 0x87;
			*timing1 = 0xFF;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_50KBPS:
			*timing0 = 0x47;
			*timing1 = 0x2F;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_80KBPS:
			*timing0 = 0x83;
			*timing1 = 0xFF;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_100KBPS:
			*timing0 = 0x43;
			*timing1 = 0x2F;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_125KBPS:
			*timing0 = 0x03;
			*timing1 = 0x1C;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_200KBPS:
			*timing0 = 0x81;
			*timing1 = 0xFA;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_250KBPS:
			*timing0 = 0x01;
			*timing1 = 0x1C;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_400KBPS:
			*timing0 = 0x80;
			*timing1 = 0xFA;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_500KBPS:
			*timing0 = 0x00;
			*timing1 = 0x1C;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_666KBPS:
			*timing0 = 0x80;
			*timing1 = 0xB6;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_800KBPS:
			*timing0 = 0x00;
			*timing1 = 0x16;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_1000KBPS:
			*timing0 = 0x00;
			*timing1 = 0x14;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\r\n", band_rate_kbps, *timing0, *timing1);
			break;

		default:
			ret = FAILURE;
			printf("can_bandrate_to_timing_mapping: band_rate_kbps = %d, not supported, return FAILURE\r\n", band_rate_kbps);
	}

	return ret;
}

/*
 * CAN receiving thread
 */
void *can_rx_thread(void *data)
{

	USB_CAN_HandleTypeDef *husbcan = NULL;

	VCI_CAN_OBJ can[RX_BUFF_SIZE]; // buffer
	husbcan = (USB_CAN_HandleTypeDef *)data;

	UINT32 cnt = 0;
	UINT32 i = 0;
	UINT32 total_cnt;

	printf("can_rx_thread: husbcan->can_dev_type = 0x%X, husbcan->can_dev_idx = 0x%X, husbcan->can_channel_id = 0x%X\r\n", husbcan->can_dev_type, husbcan->can_dev_idx, husbcan->can_channel_id);

	while (1)
    {
        cnt = VCI_Receive(husbcan->can_dev_type, husbcan->can_dev_idx, 0, can, RX_BUFF_SIZE, RX_WAIT_TIME);
        /* husbcan->can_channel_id => 0 */

        if (0 == cnt) continue;

        total_cnt = total_cnt + cnt;

        /* for debug */
        for (i = 0; i < cnt; i++)
        {
        	USB_CAN_RxCpltCallback(husbcan, &can[i]);

        	printf("CAN%d: received [%02X %02X %02X %02X %02X %02X %02X %02X], total_frame = %d\r\n", husbcan->can_channel_id,
           			can[i].Data[0], can[i].Data[1], can[i].Data[2], can[i].Data[3],
					can[i].Data[4], can[i].Data[5], can[i].Data[6], can[i].Data[7], total_cnt);
        }

        /* to add framing */

    }

    pthread_exit(0);
}

/*
 * Init the CAN device
 */
UINT32 usb_can_init(USB_CAN_HandleTypeDef *husbcan, UINT32 can_dev_type, UINT32 can_dev_idx, UINT32 can_channel_id, UINT32 band_rate_kbps, pthread_t can_forwarding_thread_id, UINT32 can_l2_forwarding_mode)
{

	UINT8 Timing0, Timing1;

	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		printf("usb_can_init: NULL == husbcan, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check handler is valid or not */
	if(CAN_STATUS_INITIALIZED == husbcan->can_status)
	{
		printf("usb_can_init: CAN_STATUS_INITIALIZED == husbcan->can_status, make sure DeInit it firstly, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check handler content */
	if(can_dev_type > CAN_DEVICE_TYPE_16)
	{
		printf("usb_can_init: can_dev_type > CAN_DEVICE_TYPE_16, return FAILURE\r\n");
		return FAILURE;
	}

	if(can_dev_idx > CAN_DEVIDE_IDX_CARD1)
	{
		printf("usb_can_init: can_dev_id > CAN_DEVIDE_IDX_CARD1, return FAILURE\r\n");
		return FAILURE;
	}

	if(can_l2_forwarding_mode > CAN_L2_FRAME_FORWARD_YES)
	{
		printf("usb_can_init: can_l2_forwarding_mode > CAN_L2_FRAME_FORWARD_YES, return FAILURE\r\n");
		return FAILURE;
	}

	if( (can_channel_id < CAN_DEVIDE_CHANNEL_CAN0) || (can_channel_id > CAN_DEVIDE_CHANNEL_CAN1) )
	{
		printf("usb_can_init: can_channel_id < CAN_DEVIDE_CHANNEL_CAN0, or > CAN_DEVIDE_CHANNEL_CAN1, return FAILURE\r\n");
		return FAILURE;
	}

	if( FAILURE == can_bandrate_to_timing_mapping(band_rate_kbps, &Timing0, &Timing1) )
	{
		printf("usb_can_init: can_bandrate_to_timing_mapping find invalid bandrate(%d), return FAILURE\r\n", band_rate_kbps);
		return FAILURE;
	}

	/* save parameters */
	husbcan->band_rate_kbps = band_rate_kbps;
	husbcan->can_channel_id = can_channel_id;
	husbcan->can_dev_idx = can_dev_idx;
	husbcan->can_dev_type = can_dev_type;
	husbcan->can_receiving_thread_id  = can_forwarding_thread_id;
	husbcan->can_l2_forwarding_mode = can_l2_forwarding_mode;

	/* Initialize can_config */
	husbcan->can_config.AccCode = 0;
	husbcan->can_config.AccMask = 0xffffffff;
	husbcan->can_config.Filter = 1;
	husbcan->can_config.Mode = 0;
	husbcan->can_config.Timing0 = Timing0;
	husbcan->can_config.Timing1 = Timing1;

	printf("husbcan->can_dev_type = 0x%X, husbcan->can_dev_idx = 0x%X\r\n", husbcan->can_dev_type, husbcan->can_dev_idx);

    if (!VCI_OpenDevice(husbcan->can_dev_type, husbcan->can_dev_idx, 0))
    {
        printf("usb_can_init: VCI_OpenDevice failed\r\n");
        husbcan->can_status = CAN_STATUS_NULL;
        return FAILURE;
    }
    printf("usb_can_init: VCI_OpenDevice succeeded\r\n");

	if (!VCI_InitCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &husbcan->can_config))
	{
		printf("usb_can_init: VCI_InitCAN(%d) failed\r\n", 0);
		husbcan->can_status = CAN_STATUS_NULL;
		return FAILURE;
	}
	printf("usb_can_init: VCI_InitCAN(%d) succeeded\r\n", 0);

	if (!VCI_StartCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0))
	{
		husbcan->can_status = CAN_STATUS_NULL;
		printf("usb_can_init: VCI_StartCAN(%d) failed\r\n", 0);
		return FAILURE;
	}

	/* Create receiving thread */
	bsp_can_start_rx(husbcan, app_can_loopback_callback, g_can_rx_buffer, BFSC_CAN_MAX_RX_BUF_SIZE, (void *)husbcan);
	pthread_create(&husbcan->can_receiving_thread_id, NULL, can_rx_thread, (void *)husbcan);



	husbcan->can_status = CAN_STATUS_INITIALIZED;
	printf("usb_can_init: VCI_StartCAN(%d) succeeded\r\n", 0);
	return SUCCESS;
}

/*
 * DeInit the CAN device
 */
UINT32 usb_can_deinit(USB_CAN_HandleTypeDef *husbcan)
{

	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		printf("usb_can_deinit: NULL == husbcan, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check handler is valid or not */
	if(CAN_STATUS_INITIALIZED != husbcan->can_status)
	{
		printf("usb_can_deinit: CAN_STATUS_INITIALIZED != husbcan->can_status, make sure it has been initialized firstly, do nothing, return SUCESSS\r\n");
		return SUCCESS;
	}

    VCI_CloseDevice(husbcan->can_dev_type, husbcan->can_dev_idx);

    /* Kill the thread of receiving */
    /* TO DO */

    husbcan->can_status = CAN_STATUS_NULL;

    printf("usb_can_deinit: VCI_CloseDevice\r\n");
    return SUCCESS;

}


/*
 * Transmit one CAN frame
 */
UINT32 usb_can_transmit(USB_CAN_HandleTypeDef *husbcan, UINT8 *ptr_data, UINT32 data_len, UINT32 can_id, UINT32 extern_flag)
{

	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		printf("usb_can_transmit: NULL == husbcan, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check ptr_data is valid or not */
	if(NULL == ptr_data)
	{
		printf("usb_can_transmit: NULL == ptr_data, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check ptr_data is valid or not */
	if(data_len > 8)
	{
		printf("usb_can_transmit: data_len > 8, return FAILURE\r\n");
		return FAILURE;
	}

	/* Check extern_flag is valid only when it equals to 0 (normal) or 1(extended) */
	if(extern_flag > 1)
	{
		printf("usb_can_transmit: extern_flag > 1, return FAILURE\r\n");
		return FAILURE;
	}
	husbcan->can_data.ID = can_id;
	husbcan->can_data.DataLen = data_len;
	husbcan->can_data.SendType = 1;  // always for 1 for mini PCIe from ZLG
	memcpy(&husbcan->can_data.Data[0], ptr_data, data_len);
	husbcan->can_data.ExternFlag = extern_flag;

    if (1 != VCI_Transmit(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &(husbcan->can_data), 1))
    {
        printf("usb_can_transmit: CAN%d TX failed: ID=%08x\r\n", 0, husbcan->can_data.ID);
        return FAILURE;
    }

    printf("usb_can_transmit: CAN%d TX [0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]: ID=%08x\r\n", 0, husbcan->can_data.ID,
    		husbcan->can_data.Data[0], husbcan->can_data.Data[1], husbcan->can_data.Data[2], husbcan->can_data.Data[3],
			husbcan->can_data.Data[4], husbcan->can_data.Data[5], husbcan->can_data.Data[6], husbcan->can_data.Data[7]);

    return SUCCESS;
}



//#define IhuDebugPrint(fmt, ...)

/**
  * @brief  Transmission  complete callback in non blocking mode
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void USB_CAN_RxCpltCallback(USB_CAN_HandleTypeDef* CanHandle, VCI_CAN_OBJ *Can)
{
	IHU_HUITP_L2FRAME_Desc_t *frame_desc;
	if(CanHandle->can_channel_id == CAN_DEVIDE_CHANNEL_CAN1)
		frame_desc = &g_can_packet_desc[0];
	else
		frame_desc = &g_can_packet_desc[1];

	printf("stdId 0x%x length %d, data: 0x%08x 0x%08x\r\n",
		Can->ID,
		Can->DataLen,
		*(uint32_t *)(&Can->Data[0]),
		*(uint32_t *)(&Can->Data[4])
		);

	l2packet_rx_bytes(frame_desc, &Can->Data[0], Can->DataLen);

	/* Receive again */
	//if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	//{
		/* Reception Error */
	//	IhuErrorPrint("HAL_CAN_Receive_IT() error.\n");
	//}
}

void app_can_loopback_callback(IHU_HUITP_L2FRAME_Desc_t *pdesc)
{
	USB_CAN_HandleTypeDef* CanHandle;
	int ret = 0;

	//assert(pdesc);
	//assert(CanHandle);

	CanHandle = (USB_CAN_HandleTypeDef* )pdesc->UserData;

	printf("CAN ISR: L2Packet %d bytes, first: 0x%02x %02x last: 0x%02x %02x\r\n",
		pdesc->RxXferCount,
		CanHandle->can_data.Data[0], CanHandle->can_data.Data[1],
		CanHandle->can_data.Data[6], CanHandle->can_data.Data[7]);

	//Forward to TASK_ID_CANVELA
	//ret = ihu_message_send_isr(MSG_ID_CAN_L2FRAME_RCV, TASK_ID_CANVELA, TASK_ID_CANVELA, g_can_rx_buffer, pdesc->RxXferCount);
	//printf("");
	if (ret == FAILURE){
		//zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
		//printf("CAN ISR: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName);
		return;
	}
	//printf("CAN ISR: Send message OK, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);

	//bsp_can_transmit(CanHandle, pdesc->pRxBuffPtr, pdesc->RxXferCount, 10);
}

int bsp_can_start_rx(USB_CAN_HandleTypeDef* CanHandle, void (*app_rx_callback)(), uint8_t *pRxBuffPtr, uint16_t rxBufferSize, void *user_data)
{
	IHU_HUITP_L2FRAME_Desc_t *frame_desc;

	if(CanHandle->can_channel_id == CAN_DEVIDE_CHANNEL_CAN1)
		frame_desc = &g_can_packet_desc[0];
	else
		frame_desc = &g_can_packet_desc[1];

	memset(frame_desc, 0, sizeof(IHU_HUITP_L2FRAME_Desc_t));
	memset(pRxBuffPtr, 0, rxBufferSize);

	frame_desc->RxState = IHU_L2PACKET_RX_STATE_START;
	frame_desc->pRxBuffPtr = pRxBuffPtr;
	frame_desc->RxBuffSize = rxBufferSize;
	frame_desc->RxXferCount = 0;
	frame_desc->app_rx_callback = app_rx_callback;
	frame_desc->UserData = user_data;

	//if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	//{
		/* Reception Error */
		//return (1);
	//}

	return 0;
}

/* return the size of transmitted */
uint32_t bsp_can_l2_frame_transmit(USB_CAN_HandleTypeDef* CanHandle, uint8_t *buffer, uint32_t length, uint32_t timeout)
{
	uint8_t translen;
	//HAL_StatusTypeDef status;
	UINT32 ret;

	while(length > 0)
	{
		translen = (length > 8)?8:length;
		CanHandle->can_data.DataLen = translen;
		memcpy(CanHandle->can_data.Data, buffer, translen);
		//ret = HAL_CAN_Transmit(CanHandle, timeout);
		ret = usb_can_transmit(CanHandle, buffer, translen, CanHandle->can_data.ID, 0);
		if(ret == SUCCESS)
		{
			length -= translen;
			buffer += translen;
		}
		else
		{
			printf("bsp_can_l2_frame_transmit() ret = %d\r\n", ret);
			break;
		}
	}

	return length;
}

//void bsp_can_init(CAN_HandleTypeDef* CanHandle, uint32_t std_id)
//{
//	CAN_FilterConfTypeDef  sFilterConfig;
//	static CanTxMsgTypeDef        TxMessage;
//  static CanRxMsgTypeDef        RxMessage;
//
//	CanHandle->pTxMsg = &TxMessage;
//	CanHandle->pRxMsg = &RxMessage;
//
//	/*##-2- Configure the CAN Filter ###########################################*/
//  sFilterConfig.FilterNumber = 0;
//  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
//  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//  sFilterConfig.FilterIdHigh = 0x0000;
//  sFilterConfig.FilterIdLow = 0x0000;
//  sFilterConfig.FilterMaskIdHigh = 0x0000;
//  sFilterConfig.FilterMaskIdLow = 0x0000;
//  sFilterConfig.FilterFIFOAssignment = 0;
//  sFilterConfig.FilterActivation = ENABLE;
//  sFilterConfig.BankNumber = 14;
//
//	if(HAL_CAN_ConfigFilter(CanHandle, &sFilterConfig) != HAL_OK)
//  {
//    /* Filter configuration Error */
//    return;
//  }
//
//	/*##-3- Configure Transmission process #####################################*/
//	CanHandle->pTxMsg->StdId = AWS_CAN_ID;
//  CanHandle->pTxMsg->ExtId = 0x01;
//  CanHandle->pTxMsg->RTR = CAN_RTR_DATA;
//  CanHandle->pTxMsg->IDE = CAN_ID_STD;
//  CanHandle->pTxMsg->DLC = 0;
//
//	bsp_can_start_rx(CanHandle, app_can_loopback_callback, g_can_rx_buffer, BFSC_CAN_MAX_RX_BUF_SIZE, CanHandle);
//}

////MYC Forward the complete/valid CAN Frame from CAN Interrupt to CAN Task
////MSG_ID_CAN_L2FRAME_RCV
//OPSTAT complete_wmc_control_frame_forward_canvela(void *p_msg, UINT16 msg_len)
//{
//	int ret = 0;
//	msg_struct_canvela_l2frame_rcv_t rcv;

//	ret = ihu_message_send(MSG_ID_CAN_L3BFSC_CMD_CTRL, TASK_ID_BFSC, TASK_ID_CANVELA, p_msg, msg_len);
//	if (ret == IHU_FAILURE){
//		zIhuSysStaPm.taskRunErrCnt[TASK_ID_CANVELA]++;
//		IhuErrorPrint("CANVELA: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//		return IHU_FAILURE;
//	}
//	//返回
//	return IHU_SUCCESS;
//}

void bfsc_can_test()
{
	USB_CAN_HandleTypeDef can1;
	UINT32 ret;
	UINT8 data[8];
	UINT32 cnt = 0;
	UINT32 total_cnt = 0;
	UINT32 i = 0;

	ret = usb_can_init(&can1, CAN_DEVICE_TYPE_PCI9820I, CAN_DEVIDE_IDX_CARD1,
						CAN_DEVIDE_CHANNEL_CAN0, CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);

	if(FAILURE == ret)
	{
		printf("usb_can_int failure, exit\r\n");
		exit(0);
	}

	while(1)
	{
		for(i = 0; i<8; i++)
		{
			data[i] = data[i] + i;
		}

		//ret = usb_can_transmit(&can1, data, 8, 0x602, 0);
		total_cnt = total_cnt + 1;

		//printf("CAN Transmit total_cnt = %d\r\n", total_cnt);

		if (FAILURE == ret)
		{
			exit(0);
		}

		if (gTxSleep) msleep(gTxSleep);
	}
}

