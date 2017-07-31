/*
 * dbi_sysswm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_SYSSWM_H_
#define L0DBI_SYSSWM_H_

#include "../l0comvm/vmlayer.h"

#define DBI_SYSSWM_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

#define DBI_SYSSWM_EQU_ENTRY_NULL  			0
#define DBI_SYSSWM_EQU_ENTRY_HCU_CLIENT 	1
#define DBI_SYSSWM_EQU_ENTRY_IHU_CLIENT 	2
#define DBI_SYSSWM_EQU_ENTRY_IHU_SERVER 	3
#define DBI_SYSSWM_EQU_ENTRY_INVALID  		0xFF


//Local APIs
extern OPSTAT dbi_HcuSysSwm_SwPkg_save(HcuSysMsgIeL3SysSwmSwPkgElement_t *prtSwPkg);
extern OPSTAT dbi_HcuSysSwm_SwPkg_inquery_to_judge_existance(UINT8 equEntry, UINT16 hwType, UINT16 hwPem, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, INT32 *sid);
extern OPSTAT dbi_HcuSysSwm_SwPkg_inquery_whole_record(UINT8 equEntry, UINT16 hwType, UINT16 hwPem, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, HcuSysMsgIeL3SysSwmSwPkgElement_t *prtSwPkg);
extern OPSTAT dbi_HcuSysSwm_SwPkg_orphane_file_delete(void);
extern OPSTAT dbi_HcuSysSwm_SwDownLoad_save(HcuSysMsgIeL3SysSwmSwDlElement_t *prtSwDl);



//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_SYSSWM_H_ */
