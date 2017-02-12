/*
 * dbisysperfm.h
 *
 *  Created on: 2016年1月19日
 *      Author: hitpony
 */

#ifndef L0DBI_DBISYSPERFM_H_
#define L0DBI_DBISYSPERFM_H_

#include "../l0comvm/vmlayer.h"

#define SP_GLOBAL_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuSyspmGlobalDataInfo_save(void);
extern OPSTAT dbi_HcuSyspmGlobalDataInfo_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBISYSPERFM_H_ */
