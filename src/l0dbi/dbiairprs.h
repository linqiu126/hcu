/*
 * dbiairprs.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIAIRPRS_H_
#define L0DBI_DBIAIRPRS_H_

#include "../l0comvm/vmlayer.h"

#define AIRPRS_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

#define AIRPRS_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuAirprsDataInfo_save(sensor_airprs_data_element_t *airprsData);
extern OPSTAT dbi_HcuAirprsDataInfo_inqury_1st_record(UINT32 deviceid, sensor_airprs_data_element_t *airprsData);
extern OPSTAT dbi_HcuAirprsDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuAirprsBmp180DataInfo_save(sensor_airprs_bmp180_data_element_t *airprsData);
extern OPSTAT dbi_HcuAirprsBmp180DataInfo_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBIAIRPRS_H_ */
