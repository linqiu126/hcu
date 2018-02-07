/*
 ============================================================================
 Name        : hcu.c
 Author      : ZHANG
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "hcu.h"

#include "../l0service/configxml.h"
#include "../l0service/configxml.h"
#include "../l0service/mmc.h"
#include "../l0service/sysinfo.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l0service/mqtt.h"


#include "../l1com/l1comdef.h"
#include "../l1com/hwinv.h"
#include "../l1hwopt/apigprs.h"
#include "../l1hwopt/ble.h"
#include "../l1hwopt/con3g4g.h"
#include "../l1hwopt/ethernet.h"
#include "../l1hwopt/gps.h"
#include "../l1hwopt/lcd.h"
#include "../l1hwopt/led.h"
#include "../l1hwopt/sps232.h"
#include "../l1hwopt/sps485.h"
#include "../l1hwopt/usbnet.h"
#include "../l1hwopt/wifi.h"
#include "../l1hwopt/camera.h"
#include "../l1hwopt/microphone.h"
#include "../l1hwopt/gpio.h"
#include "../l1hwopt/i2c.h"
#include "../l1hwopt/spi.h"
#include "../l1hwopt/pwm.h"
#include "../l1hwopt/adc.h"
#include "../l1hwopt/switch.h"
#include "../l1hwopt/relay.h"
#include "../l1hwopt/motor.h"
#include "../l1hwopt/zeegbe.h"

#include "../l2frame/spsvirgo.h"
#include "../l2frame/modbus.h"
#include "../l2frame/avorion.h"
#include "../l2frame/cloudvela.h"
#include "../l2frame/i2cbuslibra.h"
#include "../l2frame/spibusaries.h"
#include "../l2frame/nbiotcj188.h"
#include "../l2frame/nbiotqg376.h"
#include "../l2frame/canitfleo.h"
#include "../l2frame/llczhb.h"


#include "../l3app/svrcon.h"
#include "../l3app/sensoremc.h"
#include "../l3app/sensorpm25.h"
#include "../l3app/sensorwinddir.h"
#include "../l3app/sensorwindspd.h"
#include "../l3app/sensortemp.h"
#include "../l3app/sensorhumid.h"
#include "../l3app/sensornoise.h"
#include "../l3app/hsmmp.h"
#include "../l3app/sensorairprs.h"
#include "../l3app/sensorco1.h"
#include "../l3app/sensorlightstr.h"
#include "../l3app/sensoralcohol.h"
#include "../l3app/sensorhcho.h"
#include "../l3app/sensortoxicgas.h"
#include "../l3app/sensoriwm.h"
#include "../l3app/sensorihm.h"
#include "../l3app/sensorigm.h"
#include "../l3app/sensoripm.h"
#include "../l3app/sensorpm25sharp.h"
#include "../l3app/syspm.h"
#include "../l3app/sysswm.h"
#include "../l3hate/l3hate.h"

//不同任务模块的包含关系
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	#include "../l3appctrl/l3aqycg10.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	#include "../l3appctrl/l3aqycg20.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_DAYCG21_RASBERRY_ID)
	#include "../l3appctrl/l3daycg21.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
	#include "../l3appctrl/l3tbswrg30.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
	#include "../l3appctrl/l3gqybg40.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
	#include "../l3appctrl/l3cxilc.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
	#include "../l3appctrl/l3cxglacm.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
	#include "../l3appctrl/l3nbhpm.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
	#include "../l3appctrl/l3nblpm.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	#include "../l3uiconnect/bfscuicomm.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
	#include "../l3appctrl/l3opwlotdr.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	#include "../l3appctrl/l3bfdf.h"
	#include "../l3uiconnect/bfdfuicomm.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	#include "../l3appctrl/l3bfhs.h"
	#include "../l3uiconnect/bfhsuicomm.h"
//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif

//Global variables to be shared with VM

//Main Entry, Father process
int main(int argc, char* argv[]) {

	if(argv[1] != 0)
	{
		if (!strcmp(argv[1],"version")){
			printf("%d\n", HCU_CURRENT_SW_DELIVERY);
			return EXIT_SUCCESS;
		}
	}else{
		hcu_vm_main_entry();
	}
	return EXIT_SUCCESS;
}





