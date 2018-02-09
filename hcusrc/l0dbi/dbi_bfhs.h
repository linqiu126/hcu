/*
 * dbi_bfhs.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_BFHS_H_
#define L0DBI_BFHS_H_

#include "../l0comvm/vmlayer.h"

#define DBI_BFHS_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


//APIs
extern OPSTAT dbi_HcuBfhs_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfhsContextStaElement_t *StaDatainfo);
extern UINT32 dbi_HcuBfhs_CallCellMaxIdGet(void);
extern OPSTAT dbi_HcuBfhs_callcell_save(HcuSysMsgIeL3bfhsCallcellElement_t *input);
extern OPSTAT dbi_HcuBfhs_callcell_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFHS_H_ */
