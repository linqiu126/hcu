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

extern OPSTAT dbi_HcuBfsc_Cfgpar_read_into_syseng(UINT32 sid, HcuSysEngBfscCfgpar_t *bfscCfgpar);
extern OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo);
extern OPSTAT dbi_HcuBfsc_ui_ctrl_exg_read(UINT32 *output);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFSC_H_ */
