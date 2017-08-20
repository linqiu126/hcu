/*
 * dbi_bfsc.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_BFSC_H_
#define L0DBI_BFSC_H_

#include "../l0comvm/vmlayer.h"

#define DBI_BFSC_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短
#define DBI_MAX_FBINFO_STRING_LENGTH 500 //String length for field fbinfo in table hcubfscfb2ui

//为了表达传感器状态，需要定义一系列状态
#define DBI_BFSC_SNESOR_STATUS_INVALID  		0
#define DBI_BFSC_SNESOR_STATUS_DATA_VALID		1
#define DBI_BFSC_SNESOR_STATUS_STARTUP  		2
#define DBI_BFSC_SNESOR_STATUS_CONFIG_RCV  		3
#define DBI_BFSC_SNESOR_STATUS_CONFIG_ERR  		4
#define DBI_BFSC_SNESOR_STATUS_START_RCV  		5
#define DBI_BFSC_SNESOR_STATUS_STOP_RCV  		6
#define DBI_BFSC_SNESOR_STATUS_TEST_CMD_RCV  	7
#define DBI_BFSC_SNESOR_STATUS_FAULT_RCV  		8
#define DBI_BFSC_SNESOR_STATUS_ERR_RESP_RCV  	9
#define DBI_BFSC_SNESOR_STATUS_INV_RPT_RCV  	10
#define DBI_BFSC_SNESOR_STATUS_SW_PKG_RCV  		11
#define DBI_BFSC_SNESOR_STATUS_NULL  			0xFF


extern OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo);
extern OPSTAT dbi_HcuBfsc_WmcStatusUpdate(uint32_t aws_id, uint32_t wmc_id, uint32_t wmc_status, uint32_t wmc_weight_value);
extern OPSTAT dbi_HcuBfsc_WmcCurComWgtUpdate(uint32_t wgt);
extern OPSTAT dbi_HcuBfsc_WmcStatusForceInvalid(uint32_t aws_id);
extern OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);
//extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate(UINT8 cmdid, UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid);
extern OPSTAT dbi_HcuBfsc_CalibrationDataGet( UINT32 data[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3] );
extern OPSTAT dbi_HcuBfsc_DynamicConfigDataGet(UINT16 config_index, UINT32  dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_StaticConfigDataGet(UINT32  staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_TestCmdRespUpdate(UINT8 cmdid, UINT8 validFlag, char strInput[DBI_MAX_FBINFO_STRING_LENGTH]);
extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adczero(UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid);
extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adcfull(UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFSC_H_ */
