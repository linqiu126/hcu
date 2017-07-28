/*
 * dbi_bfsc.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_BFSC_H_
#define L0DBI_BFSC_H_

#include "../l0comvm/vmlayer.h"
#include "../l2codec/huitp.h"

#define DBI_BFSC_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短
#define DBI_MAX_FBINFO_STRING_LENGTH 500 //String length for field fbinfo in table hcubfscfb2ui

//extern OPSTAT dbi_HcuBfsc_Cfgpar_read_into_syseng(UINT32 sid, HcuSysEngBfscCfgpar_t *bfscCfgpar);
extern OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo);
//extern OPSTAT dbi_HcuBfsc_ui_ctrl_exg_read(UINT32 *output);
extern OPSTAT dbi_HcuBfsc_WmcStatusUpdate(uint32_t aws_id, uint32_t wmc_id, uint32_t wmc_status, uint32_t wmc_weight_value);
extern OPSTAT dbi_HcuBfsc_WmcCurComWgtUpdate(uint32_t wgt);
extern OPSTAT dbi_HcuBfsc_WmcStatusForceInvalid(uint32_t aws_id);
extern OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);
extern OPSTAT dbi_HcuBfsc_CalibrationDataUpdate(UINT8 cmdid, UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid);
extern OPSTAT dbi_HcuBfsc_CalibrationDataGet( UINT32 data[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3] );
extern OPSTAT dbi_HcuBfsc_DynamicConfigDataGet(UINT32 config_index, UINT32  dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_StaticConfigDataGet(UINT32  staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfsc_TestCmdRespUpdate(UINT8 cmdid, UINT8 validFlag, char strInput[DBI_MAX_FBINFO_STRING_LENGTH]);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFSC_H_ */
