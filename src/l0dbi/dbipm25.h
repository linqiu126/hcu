/*
 * dbipm25.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIPM25_H_
#define L0DBI_DBIPM25_H_

#include "../l0comvm/vmlayer.h"

#define PM25_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


extern OPSTAT dbi_HcuPm25DataInfo_save(sensor_pm25_data_element_t *pm25Data);
extern OPSTAT dbi_HcuPm25DataInfo_inqury_1st_record(UINT32 deviceid, sensor_pm25_data_element_t *pm25Data);
extern OPSTAT dbi_HcuPm25DataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuPm25ConfigData_save(sensor_modbus_opertion_general_t *PM25ConfigData);
extern OPSTAT dbi_HcuPm25ConfigData_update(UINT8 optId, sensor_modbus_opertion_general_t *PM25ConfigData);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);



#endif /* L0DBI_DBIPM25_H_ */
