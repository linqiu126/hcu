/*
 * dbinbiot.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_DBINBIOT_H_
#define L0DBI_DBINBIOT_H_

#include "../l0comvm/vmlayer.h"

#define DBI_MAX_SQL_INQUERY_STRING_LENGTH 1000

extern OPSTAT dbi_NbiotCJ188CntSerContext_save(char *add, INT8 cntser, INT8 flag);
extern OPSTAT dbi_NbiotCJ188CntSerContext_ser_inqury(char *add, INT8 cntser);
extern OPSTAT dbi_NbiotCJ188CntSerContext_ser_increase(char *add);
extern OPSTAT dbi_NbiotQG376CntPcfContext_save(char *add, INT8 cntpcf, INT8 flag);
extern OPSTAT dbi_NbiotQG376CntPcfContext_ser_inqury(char *add, INT8 cntpcf);
extern OPSTAT dbi_NbiotQG376CntPcfContext_ser_increase(char *add);

//引用外部API
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

#endif /* L0DBI_DBINBIOT_H_ */
