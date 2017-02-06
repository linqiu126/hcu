/*
 * dbicom.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBICOM_H_
#define L0DBI_DBICOM_H_

#include "../l0comvm/vmlayer.h"

#define DBI_MAX_SQL_INQUERY_STRING_LENGTH 1000

extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);
extern OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTable_t *engPar, char *prjname);
extern OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTable_t *engPar);
extern OPSTAT dbi_HcuTraceModuleCtr_init_table_by_vmlayer(void);
extern OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTable_t *engPar);
extern OPSTAT dbi_HcuDbVersion_inqury(HcuInventoryInfo_t *hcuInv);

//引用外部API
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表
extern HcuInventoryInfo_t zHcuInventoryInfo;

#endif /* L0DBI_DBICOM_H_ */
