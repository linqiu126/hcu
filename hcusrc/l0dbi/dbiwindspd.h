/*
 * dbiwindspd.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIWINDSPD_H_
#define L0DBI_DBIWINDSPD_H_

#include "../l0comvm/vmlayer.h"

#define WINDSPD_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define WINDSPD_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuWindspdDataInfo_save(sensor_windspd_data_element_t *windspdData);
extern OPSTAT dbi_HcuWindspdDataInfo_inqury_1st_record(UINT32 deviceid, sensor_windspd_data_element_t *windspdData);
extern OPSTAT dbi_HcuWindspdDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuWindspdDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *windspdData);
extern OPSTAT dbi_HcuWindspdDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *windspdData);
extern OPSTAT dbi_HcuWindspdDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *windspdData);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBIWINDSPD_H_ */
