/*
 * l2usbcan.c
 *
 *  Created on: May 15, 2017
 *      Author: MA Yuchu
 */
#include "l2usbcan.h"


//全局变量
unsigned gDevType = 0;
unsigned gDevIdx = 0;
unsigned gChMask = 0;
unsigned gBaud = 0;
unsigned gTxType = 0;
unsigned gTxSleep = 0;
unsigned gTxFrames = 0;


//本地用到的API
unsigned func_zlg_usbcan_s2n(const char *s)
{
    unsigned l = strlen(s);
    unsigned v = 0;
    unsigned h = (l > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
    unsigned char c;
    unsigned char t;
    if (!h) return atoi(s);
    if (l > 10) return 0;
    for (s += 2; c = (char)*s; s++)
    {
        if (c >= 'A' && c <= 'F') c += 32;
        if (c >= '0' && c <= '9') t = c - '0';
        else if (c >= 'a' && c <= 'f') t = c - 'a' + 10;
        else return 0;
        v = (v << 4) | t;
    }
    return v;
}

void func_zlg_usbcan_generate_frame(VCI_CAN_OBJ *can)
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

int func_zlg_usbcan_verify_frame(VCI_CAN_OBJ *can)
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
unsigned __stdcall func_zlg_usbcan_rx_thread(void *data)
#else
void * func_zlg_usbcan_rx_thread(void *data)
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
            if (func_zlg_usbcan_verify_frame(&can[i]))
            {
           	HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d: received [%02X %02X %02X %02X %02X %02X %02X %02X] \n", ctx->channel,
           			can[i].Data[0], can[i].Data[1], can[i].Data[2], can[i].Data[3],
					can[i].Data[4], can[i].Data[5], can[i].Data[6], can[i].Data[7]);
            	continue;
            }
            HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d: verify_frame() failed\n", ctx->channel);
            ctx->error = 1;
            break;
        }
        if (ctx->error) break;

        ctx->total += cnt;
        if (ctx->total / CHECK_POINT >= check_point) {
            HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d: %d frames received & verified\n", ctx->channel, ctx->total);
            check_point++;
        }
    }

    HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d RX thread terminated, %d frames received & verified: %s\n",
        ctx->channel, ctx->total, ctx->error ? "error(s) detected" : "no error");

#ifdef WIN32
    _endthreadex(0);
    return 0;
#else
    pthread_exit(0);
#endif
}

int func_zlg_usbcan_test_main1()
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
            HCU_DEBUG_PRINT_INF("VCI_InitCAN(%d) failed\n", i);
            return 0;
        }
        HCU_DEBUG_PRINT_INF("VCI_InitCAN(%d) succeeded\n", i);

        if (!VCI_StartCAN(gDevType, gDevIdx, i))
        {
            HCU_DEBUG_PRINT_INF("VCI_StartCAN(%d) failed\n", i);
            return 0;
        }
        HCU_DEBUG_PRINT_INF("VCI_StartCAN(%d) succeeded\n", i);
    }

    // ----- RX-timeout test ----------------------------------------------

    VCI_CAN_OBJ can;
    time_t tm1, tm2;
    for (i = 0; i < 3; i++)
    {
        time(&tm1);
        VCI_Receive(gDevType, gDevIdx, 0, &can, 1, (i + 1) * 1000/*ms*/);
        time(&tm2);
        HCU_DEBUG_PRINT_INF("VCI_Receive returned: time ~= %ld seconds\n", tm2 - tm1);
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
        rx_threads[i] = (HANDLE)_beginthreadex(NULL, 0, func_zlg_usbcan_rx_thread, &rx_ctx[i], 0, NULL);
#else
        pthread_create(&rx_threads[i], NULL, func_zlg_usbcan_rx_thread, &rx_ctx[i]);
#endif
    }

    // ----- wait --------------------------------------------------------

    HCU_DEBUG_PRINT_INF("<ENTER> to start TX: %d frames/channel, baud: t0=0x%02x, t1=0x%02x...\n",
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

            func_zlg_usbcan_generate_frame(&can);
            if (1 != VCI_Transmit(gDevType, gDevIdx, i, &can, 1))
            {
                HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d TX failed: ID=%08x\n", i, can.ID);
                err = 1;
                break;
            }
        }
        if (gTxSleep) msleep(gTxSleep);
    }
    time(&tm2);

    // ----- stop TX & RX -------------------------------------------------

    msleep(1000);
    HCU_DEBUG_PRINT_INF("TX stopped, <ENTER> to terminate RX-threads...\n");
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
        HCU_DEBUG_PRINT_INF("error(s) detected, test failed\n");
        return 0;
    }

    HCU_DEBUG_PRINT_INF("\n ***** %d frames/channel transferred, %ld seconds elapsed *****\n",
        gTxFrames, tm2 - tm1);
    if (tm2 - tm1)
        HCU_DEBUG_PRINT_INF("        performance: %ld frames/channel/second\n", gTxFrames / (tm2 - tm1));

    return 1;
}

int func_zlg_usbcan_test_main2(int argc, char* argv[])
{
    if (argc < 7) {
        HCU_DEBUG_PRINT_INF("test [DevType] [DevIdx] [ChMask] [Baud] [TxType] [TxSleep] [TxFrames]\n"
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

    gDevType = func_zlg_usbcan_s2n(argv[1]);
    gDevIdx = func_zlg_usbcan_s2n(argv[2]);
    gChMask = func_zlg_usbcan_s2n(argv[3]);
    gBaud = func_zlg_usbcan_s2n(argv[4]);
    gTxType = func_zlg_usbcan_s2n(argv[5]);
    gTxSleep = func_zlg_usbcan_s2n(argv[6]);
    gTxFrames = func_zlg_usbcan_s2n(argv[7]);
    HCU_DEBUG_PRINT_INF("DevType=%d, DevIdx=%d, ChMask=0x%x, Baud=0x%04x, TxType=%d, TxSleep=%d, TxFrames=0x%08x(%d)\n",
        gDevType, gDevIdx, gChMask, gBaud, gTxType, gTxSleep, gTxFrames, gTxFrames);

    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        HCU_DEBUG_PRINT_INF("VCI_OpenDevice failed\n");
        return 0;
    }
    HCU_DEBUG_PRINT_INF("VCI_OpenDevice succeeded\n");

    func_zlg_usbcan_test_main1();

    VCI_CloseDevice(gDevType, gDevIdx);
    HCU_DEBUG_PRINT_INF("VCI_CloseDevice\n");

    func_bfsc_usbcan_test();

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


HCU_HUITP_L2FRAME_Desc_t g_can_packet_desc[2][WMC_NODE_NUMBER];
uint8_t g_can_rx_buffer[HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE][WMC_NODE_NUMBER];
HcuCanL2framItfDef_t can_l2frame_itf_rx_buffer[WMC_NODE_NUMBER];   //newly add by MYC
HcuCanL2framItfDef_t can_l2frame_itf_tx_buffer; 					//newly add by MYC

UINT32 func_usbcan_WmcCanIdMapToWmcId(UINT32 wmc_can_id)
{
	return ((wmc_can_id & 0xFF));
}

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
UINT32 func_usbcan_bandrate_to_timing_mapping(UINT32 band_rate_kbps, UINT8 *timing0, UINT8 *timing1)
{
	UINT32 ret = SUCCESS;

	if( (NULL == timing0) || (NULL == timing1) )
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  (NULL == timing0) || (NULL == timing1), return FAILURE\n");
		return FAILURE;
	}

	switch(band_rate_kbps)
	{
		case CAN_BANDRATE_5KBPS:
			*timing0 = 0xBF;
			*timing1 = 0xFF;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_10KBPS:
			*timing0 = 0xFF;
			*timing1 = 0xFF;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_20KBPS:
			*timing0 = 0x53;
			*timing1 = 0x2F;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_40KBPS:
			*timing0 = 0x87;
			*timing1 = 0xFF;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_50KBPS:
			*timing0 = 0x47;
			*timing1 = 0x2F;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_80KBPS:
			*timing0 = 0x83;
			*timing1 = 0xFF;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_100KBPS:
			*timing0 = 0x43;
			*timing1 = 0x2F;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_125KBPS:
			*timing0 = 0x03;
			*timing1 = 0x1C;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_200KBPS:
			*timing0 = 0x81;
			*timing1 = 0xFA;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_250KBPS:
			*timing0 = 0x01;
			*timing1 = 0x1C;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_400KBPS:
			*timing0 = 0x80;
			*timing1 = 0xFA;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_500KBPS:
			*timing0 = 0x00;
			*timing1 = 0x1C;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_666KBPS:
			*timing0 = 0x80;
			*timing1 = 0xB6;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_800KBPS:
			*timing0 = 0x00;
			*timing1 = 0x16;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		case CAN_BANDRATE_1000KBPS:
			*timing0 = 0x00;
			*timing1 = 0x14;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, timing0 = 0x%02X, timing1 = 0x%02X\n", band_rate_kbps, *timing0, *timing1);
			break;

		default:
			ret = FAILURE;
			HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping  band_rate_kbps = %d, not supported, return FAILURE\n", band_rate_kbps);
	}

	return ret;
}

/*
 * CAN receiving thread
 */
void *func_usbcan_rx_thread(void *data)
{

	HcuUsbCanHandleTypeDef_t *husbcan = NULL;

	//VCI_CAN_OBJ can[RX_BUFF_SIZE]; // buffer
	husbcan = (HcuUsbCanHandleTypeDef_t *)data;

	UINT32 cnt = 0;
	UINT32 i = 0;
	UINT32 total_cnt = 0;
	UINT32 wmc_id;

	HCU_DEBUG_PRINT_INF("can_rx_thread: husbcan->can_dev_type = 0x%X, husbcan->can_dev_idx = 0x%X, husbcan->can_channel_id = 0x%X\n", husbcan->can_dev_type, husbcan->can_dev_idx, husbcan->can_channel_id);

	while (1)
    {
        cnt = VCI_Receive(husbcan->can_dev_type, husbcan->can_dev_idx, 0, husbcan->can_rx_data, RX_BUFF_SIZE, RX_WAIT_TIME);
        /* husbcan->can_channel_id => 0 */

        if (0 == cnt) continue;



        total_cnt = total_cnt + cnt;

        /* for debug */
        for (i = 0; i < cnt; i++)
        {
        	/* 2018/01/05 added by MA Yuchu */
        	/* After we have BFHS, WMS and MWC only accept extended CAN frame, */
        	/* as standard frame reserved for CANopen between MWC and MOTOR/SENSOR */
        	if(TRUE == husbcan->can_rx_data[i].ExternFlag)
        	{
            	wmc_id = func_usbcan_WmcCanIdMapToWmcId(husbcan->can_rx_data[i].ID);
            	func_usbcan_RxCpltCallback(husbcan, &husbcan->can_rx_data[i], wmc_id);
        	}

//        	HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN%d: received [%02X %02X %02X %02X %02X %02X %02X %02X], total_frame = %d\n", husbcan->can_channel_id,
//           			can[i].Data[0], can[i].Data[1], can[i].Data[2], can[i].Data[3],
//					can[i].Data[4], can[i].Data[5], can[i].Data[6], can[i].Data[7], total_cnt);
//        	printf("USBCAN_DH: CAN%d: received [%02X %02X %02X %02X %02X %02X %02X %02X], len=%d, canid=0x%X, wmc_id=%d, total_frame=%d\n", husbcan->can_channel_id,
//        			husbcan->can_rx_data[i].Data[0], husbcan->can_rx_data[i].Data[1], husbcan->can_rx_data[i].Data[2], husbcan->can_rx_data[i].Data[3],
//					husbcan->can_rx_data[i].Data[4], husbcan->can_rx_data[i].Data[5], husbcan->can_rx_data[i].Data[6], husbcan->can_rx_data[i].Data[7],
//					husbcan->can_rx_data[i].DataLen, husbcan->can_rx_data[i].ID, wmc_id, total_cnt);
        }

        /* to add framing */

    }

    pthread_exit(0);
}

/*
 * Init the CAN device
 */
UINT32 hcu_bsp_usbcan_init(HcuUsbCanHandleTypeDef_t *husbcan, UINT32 can_dev_type, UINT32 can_dev_idx, UINT32 can_channel_id, UINT32 band_rate_kbps, pthread_t can_forwarding_thread_id, UINT32 can_l2_forwarding_mode)
{

	UINT8 Timing0, Timing1;
	UINT8 i = 0;
	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: NULL == husbcan, return FAILURE\n");
		return FAILURE;
	}

	/* Check handler is valid or not */
	if(CAN_STATUS_INITIALIZED == husbcan->can_status)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN_STATUS_INITIALIZED == husbcan->can_status, make sure DeInit it firstly, return FAILURE\n");
		return FAILURE;
	}

	/* Check handler content */
	if(can_dev_type > CAN_DEVICE_TYPE_16)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_dev_type > CAN_DEVICE_TYPE_16, return FAILURE\n");
		return FAILURE;
	}

	if(can_dev_idx > CAN_DEVIDE_IDX_CARD1)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_dev_id > CAN_DEVIDE_IDX_CARD1, return FAILURE\n");
		return FAILURE;
	}

	if(can_l2_forwarding_mode > CAN_L2_FRAME_FORWARD_YES)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_l2_forwarding_mode > CAN_L2_FRAME_FORWARD_YES, return FAILURE\n");
		return FAILURE;
	}

	if( (can_channel_id < CAN_DEVIDE_CHANNEL_CAN0) || (can_channel_id > CAN_DEVIDE_CHANNEL_CAN1) )
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_channel_id < CAN_DEVIDE_CHANNEL_CAN0, or > CAN_DEVIDE_CHANNEL_CAN1, return FAILURE\n");
		return FAILURE;
	}

	if( FAILURE == func_usbcan_bandrate_to_timing_mapping(band_rate_kbps, &Timing0, &Timing1) )
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: can_bandrate_to_timing_mapping find invalid bandrate(%d), return FAILURE\n", band_rate_kbps);
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

	HCU_DEBUG_PRINT_FAT("husbcan->can_dev_type = 0x%X, husbcan->can_dev_idx = 0x%X\n", husbcan->can_dev_type, husbcan->can_dev_idx);

    if (!VCI_OpenDevice(husbcan->can_dev_type, husbcan->can_dev_idx, 0))
    {
        HCU_DEBUG_PRINT_INF("USBCAN_DH: VCI_OpenDevice failed\n");
        husbcan->can_status = CAN_STATUS_NULL;
        return FAILURE;
    }
    HCU_DEBUG_PRINT_INF("USBCAN_DH: VCI_OpenDevice succeeded\n");

	if (!VCI_InitCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &husbcan->can_config))
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: VCI_InitCAN(%d) failed\n", 0);
		husbcan->can_status = CAN_STATUS_NULL;
		return FAILURE;
	}
	HCU_DEBUG_PRINT_INF("USBCAN_DH: VCI_InitCAN(%d) succeeded\n", 0);

	if (!VCI_StartCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0))
	{
		husbcan->can_status = CAN_STATUS_NULL;
		HCU_DEBUG_PRINT_INF("USBCAN_DH: VCI_StartCAN(%d) failed\n", 0);
		return FAILURE;
	}

	/* Create receiving thread */
	for(i = 0; i < WMC_NODE_NUMBER; i++)
	{
		can_l2frame_itf_rx_buffer[i].can_id_rx_wmc_id = i; ///!!!!!!!
		func_bsp_usbcan_start_rx(husbcan, func_usbcan_loopback_callback, can_l2frame_itf_rx_buffer[i].can_l2frame, i, HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE, (void *)husbcan);
	}
	pthread_create(&husbcan->can_receiving_thread_id, NULL, func_usbcan_rx_thread, (void *)husbcan);

	husbcan->can_status = CAN_STATUS_INITIALIZED;
	HCU_DEBUG_PRINT_FAT("USBCAN_DH: VCI_StartCAN(%d) succeeded\n", 0);
	return SUCCESS;
}

/*
 * DeInit the CAN device
 */
UINT32 hcu_bsp_usbcan_deinit(HcuUsbCanHandleTypeDef_t *husbcan)
{

	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usbcan_deinit NULL == husbcan, return FAILURE\n");
		return FAILURE;
	}

	/* Check handler is valid or not */
	if(CAN_STATUS_INITIALIZED != husbcan->can_status)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usbcan_deinit CAN_STATUS_INITIALIZED != husbcan->can_status, make sure it has been initialized firstly, do nothing, return SUCESSS\n");
		return SUCCESS;
	}

    VCI_CloseDevice(husbcan->can_dev_type, husbcan->can_dev_idx);

    /* Kill the thread of receiving */
    /* TO DO */

    husbcan->can_status = CAN_STATUS_NULL;

    HCU_DEBUG_PRINT_INF("USBCAN_DH: usbcan_deinit VCI_CloseDevice\n");
    return SUCCESS;

}


/*
 * Transmit one CAN frame
 */
UINT32 func_usbcan_transmit(HcuUsbCanHandleTypeDef_t *husbcan, UINT8 *ptr_data, UINT32 data_len, UINT32 can_id, UINT32 extern_flag)
{

	/* Check handler is valid or not */
	if(NULL == husbcan)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: NULL == husbcan, return FAILURE\n");
		return FAILURE;
	}

	/* Check ptr_data is valid or not */
	if(NULL == ptr_data)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: NULL == ptr_data, return FAILURE\n");
		return FAILURE;
	}

	/* Check ptr_data is valid or not */
	if(data_len > 8)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: data_len > 8, return FAILURE\n");
		return FAILURE;
	}

	/* Check extern_flag is valid only when it equals to 0 (normal) or 1(extended) */
	if(extern_flag > 1)
	{
		HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: extern_flag > 1, return FAILURE\n");
		return FAILURE;
	}
	husbcan->can_tx_data.ID = can_id;
	husbcan->can_tx_data.DataLen = data_len;
	husbcan->can_tx_data.SendType = 1;  // always for 1 for mini PCIe from ZLG
	memcpy(&husbcan->can_tx_data.Data[0], ptr_data, data_len);
	husbcan->can_tx_data.ExternFlag = extern_flag;

	printf("func_usbcan_transmit: usb_can_transmit: ID=%08x, husbcan->can_dev_type=%d, husbcan->can_dev_idx=%d\n", husbcan->can_tx_data.ID, husbcan->can_dev_type, husbcan->can_dev_idx);
	if (1 != VCI_Transmit(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &(husbcan->can_tx_data), 1))
    {
		printf("USBCAN_DH: usb_can_transmit: CAN%d TX failed: ID=%08x, husbcan->can_dev_type=%d, husbcan->can_dev_idx=%d\n", 0, husbcan->can_tx_data.ID, husbcan->can_dev_type, husbcan->can_dev_idx);
        return FAILURE;
    }

//    HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: CAN%d TX [0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X]: ID=%08x\n", 0, husbcan->can_data.ID,
//    		husbcan->can_data.Data[0], husbcan->can_data.Data[1], husbcan->can_data.Data[2], husbcan->can_data.Data[3],
//			husbcan->can_data.Data[4], husbcan->can_data.Data[5], husbcan->can_data.Data[6], husbcan->can_data.Data[7]);
//    	HcuErrorPrint("USBCAN_DH: usb_can_transmit: CAN%d TX failed: ID=%08x\n", 0, husbcan->can_tx_data.ID);
//        return FAILURE;
//    }

//    HCU_DEBUG_PRINT_INF("USBCAN_DH: usb_can_transmit: CAN%d TX [%02X %02X %02X %02X %02X %02X %02X %02X]: ID=%08x\n", 0,
//    		husbcan->can_tx_data.Data[0], husbcan->can_tx_data.Data[1], husbcan->can_tx_data.Data[2], husbcan->can_tx_data.Data[3],
//			husbcan->can_tx_data.Data[4], husbcan->can_tx_data.Data[5], husbcan->can_tx_data.Data[6], husbcan->can_tx_data.Data[7],
//			husbcan->can_tx_data.ID);

    return SUCCESS;
}



//#define IhuDebugPrint(fmt, ...)

/**
  * @brief  Transmission  complete callback in non blocking mode
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void func_usbcan_RxCpltCallback(HcuUsbCanHandleTypeDef_t* CanHandle, VCI_CAN_OBJ *Can, UINT32 wmc_id)
{
	HCU_HUITP_L2FRAME_Desc_t *frame_desc;
	if(CanHandle->can_channel_id == CAN_DEVIDE_CHANNEL_CAN1)
		frame_desc = &g_can_packet_desc[0][wmc_id];
	else
		frame_desc = &g_can_packet_desc[1][wmc_id];

//	printf("USBCAN: func_usbcan_RxCpltCallback (wmc_id=%d). DataLen=%d, %02X %02X %02X %02X %02X %02X %02X %02X\r\n", wmc_id, Can->DataLen, \
//			Can->Data[0], Can->Data[1], Can->Data[2], Can->Data[3], Can->Data[4], Can->Data[5], Can->Data[6], Can->Data[7]);
//	HCU_DEBUG_PRINT_INF("stdId 0x%x length %d, data: 0x%08x 0x%08x\n",
//		Can->ID,
//		Can->DataLen,
//		*(uint32_t *)(&Can->Data[0]),
//		*(uint32_t *)(&Can->Data[4])
//		);

	l2packet_rx_bytes(frame_desc, &Can->Data[0], Can->DataLen);

	/* Receive again */
	//if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	//{
		/* Reception Error */
	//	IhuErrorPrint("USBCAN_DH: HAL_CAN_Receive_IT() error.\n");
	//}
}

void func_usbcan_loopback_callback(HCU_HUITP_L2FRAME_Desc_t *pdesc)
{
	//int ret = 0;
	//HcuUsbCanHandleTypeDef_t* CanHandle;
	//入参检查
	if ((pdesc->wmc_id >= HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE) || (can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame_len > HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("USBCAN: Receive parameter error!\n");
		printf("USBCAN: L2Packet received error, wmc_id=%d, can_l2frame_len=%d!\n", pdesc->wmc_id, can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame_len);
		return;
	}

	//CanHandle = (HcuUsbCanHandleTypeDef_t* )pdesc->UserData;

//	HCU_DEBUG_PRINT_INF("USBCAN: L2Packet %d bytes, first: 0x%02x %02x last: 0x%02x %02x\n",
//		pdesc->RxXferCount,
//		CanHandle->can_data.Data[0], CanHandle->can_data.Data[1],
//		CanHandle->can_data.Data[6], CanHandle->can_data.Data[7]);

	/* SAVE THE FRAME LEN !!!!! */
	can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame_len = pdesc->RxXferCount;

	//发送给CANITFLEO模块
	msg_struct_usbcan_l2frame_rcv_t snd;
	memset(&snd, 0, sizeof(msg_struct_usbcan_l2frame_rcv_t));
	snd.nodeId = pdesc->wmc_id;

	/* 2017/06/03, MA Yuchu: -4 and +4 is to remove the 4 byted L2 Frame Header FE CC L1 L2 */
	//snd.validDataLen = can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame_len;
	snd.validDataLen = can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame_len - 4;
	//memcpy(snd.databuf, (void *)&(can_l2frame_itf_rx_buffer[pdesc->wmc_id]), snd.validDataLen); /* 2017/06/03, MA Yuchu: THIS IS A BUG, CORRECT IN BELOW LINE !!!*/
	memcpy(snd.databuf, (void *)&(can_l2frame_itf_rx_buffer[pdesc->wmc_id].can_l2frame) + 4, snd.validDataLen);

//	HcuErrorPrint("USBCAN: snd.nodeId=%d, snd.validDataLen=%d\n", snd.nodeId, snd.validDataLen);
//	HcuErrorPrint("USBCAN: [%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X]\n",
//			snd.databuf[0], snd.databuf[1], snd.databuf[2], snd.databuf[3],
//			snd.databuf[4], snd.databuf[5], snd.databuf[6], snd.databuf[7]);

	snd.length = sizeof(msg_struct_usbcan_l2frame_rcv_t);

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	if (hcu_message_send(MSG_ID_USBCAN_L2FRAME_RCV, TASK_ID_CANITFLEO, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("USBCAN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		return;
	}
#else
	if (hcu_message_send(MSG_ID_USBCAN_L2FRAME_RCV, TASK_ID_CANALPHA, TASK_ID_CANALPHA, &snd, snd.length) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANALPHA]++;
		HcuErrorPrint("USBCAN: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANALPHA].taskName, zHcuVmCtrTab.task[TASK_ID_CANALPHA].taskName);
		return;
	}
#endif
	//ret = hcu_message_send(MSG_ID_USBCAN_L2FRAME_RCV, TASK_ID_CANITFLEO, TASK_ID_CANITFLEO, (void *)&(can_l2frame_itf_rx_buffer[pdesc->wmc_id]), CAN_L2_FRAME_ITF_LEN);

	return;
}

int func_bsp_usbcan_start_rx(HcuUsbCanHandleTypeDef_t* CanHandle, void (*app_rx_callback)(), uint8_t *pRxBuffPtr, uint32_t wmc_id, uint16_t rxBufferSize, void *user_data)
{
	HCU_HUITP_L2FRAME_Desc_t *frame_desc;

	if(CanHandle->can_channel_id == CAN_DEVIDE_CHANNEL_CAN1)
		frame_desc = &g_can_packet_desc[0][wmc_id];
	else
		frame_desc = &g_can_packet_desc[1][wmc_id];

	memset(frame_desc, 0, sizeof(HCU_HUITP_L2FRAME_Desc_t));
	memset(pRxBuffPtr, 0, rxBufferSize);

	frame_desc->RxState = IHU_L2PACKET_RX_STATE_START;
	frame_desc->pRxBuffPtr = pRxBuffPtr;
	frame_desc->RxBuffSize = rxBufferSize;
	frame_desc->RxXferCount = 0;
	frame_desc->app_rx_callback = app_rx_callback;
	frame_desc->UserData = user_data;
	frame_desc->wmc_id = wmc_id;   // SAVE WMC NODE ID !!!!!!

	//if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
	//{
		/* Reception Error */
		//return (1);
	//}

	return 0;
}

/* return the size of transmitted */
uint32_t hcu_bsp_usbcan_l2frame_transmit(HcuUsbCanHandleTypeDef_t* CanHandle, uint8_t *buffer, uint32_t length, UINT32 wmc_id_bitmap)
{
	uint8_t translen;
	UINT32 ret;
	uint8_t l2frame[HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE];
	uint32_t l2framelen = length + sizeof(HCU_HUITP_L2FRAME_STD_frame_header_t);
	HCU_HUITP_L2FRAME_STD_frame_header_t *p = (HCU_HUITP_L2FRAME_STD_frame_header_t *)l2frame;
	uint8_t *pL2Frame = (uint8_t *)&l2frame[0];

	//入参检查
	if ((CanHandle == NULL) || (buffer == NULL) || (length == 0) || (length > HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE - 4)){
		HcuErrorPrint("USBCAN_DH: Error input parameters reveived!\n");
		return 0;
	}

	/* Need to add L2 frame Header */
	p->start = 0xFE;
	p->len = l2framelen;
	p->chksum = (0xFE) ^ (p->len & 0xFF) ^ ((p->len>>8) & 0xFF);

	memcpy(l2frame + 4, buffer, length);

	while(l2framelen > 0)
	{
		translen = (l2framelen > 8)?8:l2framelen;
		CanHandle->can_tx_data.DataLen = translen;

		/* THIS IS FOR BROADCAST */
		CanHandle->can_tx_data.ID = (wmc_id_bitmap | AWS_TO_WMC_CAN_ID_PREFIX);

		memcpy(CanHandle->can_tx_data.Data, pL2Frame, translen);
		printf("hcu_bsp_usbcan_l2frame_transmit: CanHandle->can_tx_data.ID=0x%0X\r\n", CanHandle->can_tx_data.ID);
		ret = func_usbcan_transmit(CanHandle, pL2Frame, translen, CanHandle->can_tx_data.ID, 1); ///!!! EXT FRAMEs
		//usleep(500);
		if(ret == SUCCESS)
		{
			l2framelen = l2framelen - translen;
			pL2Frame = pL2Frame + translen;
		}
		else
		{
			HcuErrorPrint("USBCAN_DH: bsp_can_l2_frame_transmit() ret = %d\n", ret);
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
//		IhuErrorPrint("USBCAN_DH: Send message error, TASK [%s] to TASK[%s]!\n", zIhuVmCtrTab.task[TASK_ID_CANVELA].taskName, zIhuVmCtrTab.task[TASK_ID_BFSC].taskName);
//		return IHU_FAILURE;
//	}
//	//返回
//	return IHU_SUCCESS;
//}

void func_bfsc_usbcan_test(void)
{
	HcuUsbCanHandleTypeDef_t can1;
	UINT32 ret;
	UINT8 data[8];
	UINT32 total_cnt = 0;
	UINT32 i = 0;

	ret = hcu_bsp_usbcan_init(&can1, CAN_DEVICE_TYPE_PCI9820I, CAN_DEVIDE_IDX_CARD1,
						CAN_DEVIDE_CHANNEL_CAN0, CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);

	if(FAILURE == ret)
	{
		HcuErrorPrint("USBCAN_DH: usb_can_int failure, exit\n");
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

		//HCU_DEBUG_PRINT_INF("USBCAN_DH: CAN Transmit total_cnt = %d\n", total_cnt);

		if (FAILURE == ret)
		{
			exit(0);
		}

		if (gTxSleep) msleep(gTxSleep);
	}
}

/*
 **  CAN API Stub for Ubuntu PC Linux for debug
 **
 **  cnt = VCI_Receive(husbcan->can_dev_type, husbcan->can_dev_idx, 0, husbcan->can_rx_data, RX_BUFF_SIZE, RX_WAIT_TIME);
 **  if (!VCI_OpenDevice(husbcan->can_dev_type, husbcan->can_dev_idx, 0));
 **  if (!VCI_InitCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &husbcan->can_config));
 **  if (!VCI_StartCAN(husbcan->can_dev_type, husbcan->can_dev_idx, 0));
 **  VCI_CloseDevice(husbcan->can_dev_type, husbcan->can_dev_idx);
 **  if (1 != VCI_Transmit(husbcan->can_dev_type, husbcan->can_dev_idx, 0, &(husbcan->can_tx_data), 1))
 */
#ifndef TARGET_LINUX_X86_ADVANTECH

DWORD VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved)
{
	return 1;
}

DWORD VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd)
{
	return 1;
}

DWORD VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig)
{
	return 1;
}

DWORD VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo)
{
	return 0;
}

DWORD VCI_ReadErrInfo(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo)
{
	return 0;
}

DWORD VCI_ReadCANStatus(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_STATUS pCANStatus)
{
	return 0;
}

DWORD VCI_GetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData)
{
	return 0;
}

DWORD VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData)
{
	return 0;
}

ULONG VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	return 0;
}

DWORD VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	return 0;
}

DWORD VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	return 0;
}

DWORD VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	return 1;
}

ULONG VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,unsigned int Len)
{
	return 1;
}

ULONG VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,unsigned int Len,INT WaitTime)
{
	return 0;
}

#endif /* #ifndef TARGET_LINUX_X86_ADVANTECH */

