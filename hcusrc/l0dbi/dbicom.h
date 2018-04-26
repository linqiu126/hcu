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

//高级定义，简化程序的可读性

//通知刷新界面的复合宏定义
#define HCU_L0DBICOM_INIT_DB_CONN() \
	do{\
	    sqlHandler = mysql_init(NULL);\
	    if(!sqlHandler)\
	    {\
	    	HcuErrorPrint("DBICOM: MySQL init failed!, Init ErrCode = %d\n", mysql_error(sqlHandler));\
	        return FAILURE;\
	    }\
	    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);\
	    if (!sqlHandler){\
	    	mysql_close(sqlHandler);\
	    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));\
	        return FAILURE;\
	    }\
	}while(0)

//CLOSE DB CONNECTION
#define HCU_L0DBICOM_CLOSE_DB_CONN() \
	do{\
		mysql_free_result(resPtr);\
	    mysql_close(sqlHandler);\
	    return SUCCESS;\
	}while(0)







#endif /* L0DBI_DBICOM_H_ */
