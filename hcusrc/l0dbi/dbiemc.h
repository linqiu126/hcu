/*
 * dbiemc.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIEMC_H_
#define L0DBI_DBIEMC_H_

#include "../l0comvm/vmlayer.h"

#define EMC_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuEmcDataInfo_save(sensor_emc_data_element_t *emcData);
extern OPSTAT dbi_HcuEmcDataInfo_save_example(sensor_emc_data_element_t *emcData);
extern OPSTAT dbi_HcuEmcDataInfo_inqury_1st_record(UINT32 deviceid, sensor_emc_data_element_t *emcData);
extern OPSTAT dbi_HcuEmcDataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_DBIEMC_H_ */
