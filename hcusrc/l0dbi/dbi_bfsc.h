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
#define DBI_SW_UPGRADE_INFO_MAX_LEN 99

//为了兼容HUITP中下位机定义的传感器errCode和HCU定义的传感器状态，将状态值定义从(100)开始，(100)以下保留给errCode定义
#define DBI_BFSC_SNESOR_ERRCODE_NO_ERROR				(0) //Starting point //兼容老旧系统
#define DBI_BFSC_SNESOR_ERRCODE_CALLING_ERROR			(1)
#define DBI_BFSC_SNESOR_ERRCODE_PARA_KO					(2)
#define DBI_BFSC_SNESOR_ERRCODE_WRONG_WMC				(3)
#define DBI_BFSC_SNESOR_ERRCODE_NOT_CALIBRATED			(4)
#define DBI_BFSC_SNESOR_ERRCODE_WRONG_CALI_SEQUENCE		(5)
#define DBI_BFSC_SNESOR_ERRCODE_ADC_OVERLOADED			(6)
#define DBI_BFSC_SNESOR_ERRCODE_FLASH_SAVING_KO			(7)
#define DBI_BFSC_SNESOR_ERRCODE_MOTOR_FAILURE			(8)
#define DBI_BFSC_SNESOR_ERRCODE_UNKNOWN					(9)

#define DBI_BFSC_SNESOR_STATUS_INVALID  				(1000)
#define DBI_BFSC_SNESOR_STATUS_DATA_VALID				(1001)
#define DBI_BFSC_SNESOR_STATUS_STARTUP  				(1002)
#define DBI_BFSC_SNESOR_STATUS_CONFIG_RCV  				(1003)
#define DBI_BFSC_SNESOR_STATUS_CONFIG_ERR  				(1004)
#define DBI_BFSC_SNESOR_STATUS_START_RCV  				(1005)
#define DBI_BFSC_SNESOR_STATUS_STOP_RCV  				(1006)
#define DBI_BFSC_SNESOR_STATUS_TEST_CMD_RCV  			(1007)
#define DBI_BFSC_SNESOR_STATUS_FAULT_RCV  				(1008)
#define DBI_BFSC_SNESOR_STATUS_ERR_RESP_RCV  			(1009)
#define DBI_BFSC_SNESOR_STATUS_INV_RPT_RCV  			(1010)
#define DBI_BFSC_SNESOR_STATUS_SW_PKG_RCV  				(1011)
#define DBI_BFSC_SNESOR_STATUS_SUSPEND  				(1012)
#define DBI_BFSC_SNESOR_STATUS_ISOLATED  				(1013)
#define DBI_BFSC_SNESOR_STATUS_NULL  					(0xFFFF)





extern OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo);
extern OPSTAT dbi_HcuBfsc_WmcStatusUpdate(uint32_t aws_id, uint32_t wmc_id, uint32_t wmc_status, uint32_t wmc_weight_value);
extern OPSTAT dbi_HcuBfsc_WmcCurComWgtUpdate(uint32_t wgt);
extern OPSTAT dbi_HcuBfsc_hcusw_ver_Update(char *input, int len);
extern OPSTAT dbi_HcuBfsc_ihusw_ver_Update(char *input, int len);
extern OPSTAT dbi_HcuBfsc_WmcStatusForceInvalid(void);
extern OPSTAT dbi_HcuBfsc_WmcStatusForceSuspend(void);
extern OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);
extern OPSTAT dbi_HcuBfsc_CalibrationDataGet( UINT32 data[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3] );
extern OPSTAT dbi_HcuBfsc_DynamicConfigDataGet(UINT16 config_index, UINT32  dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_StaticConfigDataGet(UINT32  staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_TestCmdRespUpdate(UINT8 cmdid, UINT8 validFlag, char strInput[DBI_MAX_FBINFO_STRING_LENGTH]);
extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adczero(UINT32  adcvalue, UINT8  sensorid);
extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adcfull(UINT32  adcvalue, UINT8  sensorid);
extern OPSTAT dbi_HcuBfsc_FlowSheetUpdate(UINT16 configId, UINT32 targetWgt, UINT32 realWgt, UINT32 pkgNum);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFSC_H_ */
