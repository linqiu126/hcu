/*
 * dbialcohol.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIALCOHOL_H_
#define L0DBI_DBIALCOHOL_H_

#include "../l0comvm/vmlayer.h"

#define ALCOHOL_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define ALCOHOL_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuAlcoholDataInfo_save(sensor_alcohol_data_element_t *alcoholData);
extern OPSTAT dbi_HcuAlcoholDataInfo_inqury_1st_record(UINT32 deviceid, sensor_alcohol_data_element_t *alcoholData);
extern OPSTAT dbi_HcuAlcoholDataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);



#endif /* L0DBI_DBIALCOHOL_H_ */
