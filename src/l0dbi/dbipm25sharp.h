/*
 * dbipm25sharp.h
 *
 *  Created on: 12 May 2016
 *      Author: pi
 */

#ifndef L0DBI_DBIPM25SHARP_H_
#define L0DBI_DBIPM25SHARP_H_

#include "../l0comvm/vmlayer.h"

#define PM25SHARP_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

//extern OPSTAT dbi_HcuHumidDataInfo_save(sensor_humid_data_element_t *humidData);
extern OPSTAT dbi_HcuPM25SharpDataInfo_save(int PM25data);
//extern OPSTAT dbi_HcuHumidDataInfo_inqury_1st_record(UINT32 deviceid, sensor_humid_data_element_t *humidData);
//extern OPSTAT dbi_HcuHumidDataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_DBIPM25SHARP_H_ */
