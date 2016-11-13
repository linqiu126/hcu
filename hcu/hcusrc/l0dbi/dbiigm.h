/*
 * dbiigm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIIGM_H_
#define L0DBI_DBIIGM_H_

#include "../l0comvm/vmlayer.h"

#define IGM_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuIgmCj188DataInfo_save(sensor_igm_cj188_data_element_t *igmData);
extern OPSTAT dbi_HcuIgmCj188DataInfo_inqury_1st_record(char *addr, sensor_igm_cj188_data_element_t *igmData);
extern OPSTAT dbi_HcuIgmCj188DataInfo_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_DBIIGM_H_ */
