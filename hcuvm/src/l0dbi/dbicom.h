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


#define	DB_SENSOR_DATA_FOMAT_INT_ONLY  1
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1  0.1
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2  0.01
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3  0.001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF4  0.0001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF5  0.00001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF6  0.000001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF7  0.0000001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF8  0.00000001
#define	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF9  0.000000001

/*
const int	DB_SENSOR_DATA_FOMAT_INT_ONLY = 1
const float	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1 = 0.1
const float	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2 = 0.01
const float	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3 = 0.001
const float	DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF4 = 0.0001
*/
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);
extern OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTab_t *engPar, char *prjname);
extern OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTab_t *engPar);
extern OPSTAT dbi_HcuTraceModuleCtr_engpar_intelligence_init(void);
extern OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTab_t *engPar);
extern OPSTAT dbi_HcuTraceMsgCtr_engpar_intelligence_init(void);
//extern OPSTAT dbi_HcuDbVersion_inqury(SysEngParElementHwBurnPhyIdAddr_t *hcuInv);
extern OPSTAT dbi_HcuSysEngTimer_inqury(HcuSysEngParTab_t *engPar);
extern OPSTAT dbi_HcuSysEngTimer_engpar_intelligence_init(void);

//引用外部API


#endif /* L0DBI_DBICOM_H_ */
