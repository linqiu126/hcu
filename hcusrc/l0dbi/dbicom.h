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
extern OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTab_t *engPar, char *prjname);
extern OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTab_t *engPar);
extern OPSTAT dbi_HcuTraceModuleCtr_engpar_intelligence_init(void);
extern OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTab_t *engPar);
extern OPSTAT dbi_HcuTraceMsgCtr_engpar_intelligence_init(void);
extern OPSTAT dbi_HcuDbVersion_inqury(SysEngParElementSwInvInfo_t *hcuInv);

//引用外部API


#endif /* L0DBI_DBICOM_H_ */
