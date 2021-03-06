/*
 * dbihcho.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIHCHO_H_
#define L0DBI_DBIHCHO_H_

#include "../l0comvm/vmlayer.h"

#define HCHO_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define HCHO_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuHchoDataInfo_save(sensor_hcho_data_element_t *hchoData);
extern OPSTAT dbi_HcuHchoDataInfo_inqury_1st_record(UINT32 deviceid, sensor_hcho_data_element_t *hchoData);
extern OPSTAT dbi_HcuHchoDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHcHoZe08ch2oDataInfo_save(sensor_hcho_ze08ch2o_data_element_t *hchoData);
extern OPSTAT dbi_HcuHcHoZe08ch2oDataInfo_delete_3monold(UINT32 days);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);



#endif /* L0DBI_DBIHCHO_H_ */
