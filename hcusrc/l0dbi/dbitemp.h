/*
 * dbitemp.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBITEMP_H_
#define L0DBI_DBITEMP_H_

#include "../l0comvm/vmlayer.h"

#define TEMP_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define TEMP_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuTempDataInfo_save(sensor_temp_data_element_t *tempData);
extern OPSTAT dbi_HcuTempDataInfo_inqury_1st_record(UINT32 deviceid, sensor_temp_data_element_t *tempData);
extern OPSTAT dbi_HcuTempDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuTempDht11DataInfo_save(sensor_temp_dht11_data_element_t *tempData);
extern OPSTAT dbi_HcuTempDht11DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempSht20DataInfo_save(sensor_temp_sht20_data_element_t *tempData);
extern OPSTAT dbi_HcuTempSht20DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempRht03DataInfo_save(sensor_temp_rht03_data_element_t *tempData);
extern OPSTAT dbi_HcuTempRht03DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempBmp180DataInfo_save(sensor_temp_bmp180_data_element_t *tempData);
extern OPSTAT dbi_HcuTempBmp180DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempMth01DataInfo_save(sensor_temp_mth01_data_element_t *tempData);
extern OPSTAT dbi_HcuTempMth01DataInfo_delete_3monold(UINT32 days);


extern OPSTAT dbi_HcuTempDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *tempData);
extern OPSTAT dbi_HcuTempDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *tempData);
extern OPSTAT dbi_HcuTempDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *tempData);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBITEMP_H_ */
