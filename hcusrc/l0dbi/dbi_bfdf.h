/*
 * dbi_bfdf.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_BFDF_H_
#define L0DBI_BFDF_H_

#include "../l0comvm/vmlayer.h"

#define DBI_BFDF_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

//APIs
extern OPSTAT dbi_HcuBfdf_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfdfContextStaElement_t *StaDatainfo);
extern UINT32 dbi_HcuBfdf_CallCellMaxIdGet(void);
extern OPSTAT dbi_HcuBfdf_callcell_save(HcuSysMsgIeL3bfdfCallcellElement_t *input);
extern OPSTAT dbi_HcuBfdf_callcell_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFDF_H_ */
