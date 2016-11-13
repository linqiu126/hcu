/*
 * dbiihm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIIHM_H_
#define L0DBI_DBIIHM_H_

#include "../l0comvm/vmlayer.h"

#define IHM_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuIhmCj188DataInfo_save(sensor_ihm_cj188_data_element_t *ihmData);
extern OPSTAT dbi_HcuIhmCj188DataInfo_inqury_1st_record(char *addr, sensor_ihm_cj188_data_element_t *ihmData);
extern OPSTAT dbi_HcuIhmCj188DataInfo_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_DBIIHM_H_ */
