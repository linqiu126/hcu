/*
 * dbihsmmp.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIHSMMP_H_
#define L0DBI_DBIHSMMP_H_

#include "../l0comvm/vmlayer.h"

#define HSMMP_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuHsmmpDataInfo_save(sensor_hsmmp_data_element_t *hsmmpData);
extern OPSTAT dbi_HcuHsmmpDataInfo_inqury_1st_record(UINT32 deviceid, sensor_hsmmp_data_element_t *hsmmpData);
extern OPSTAT dbi_HcuHsmmpDataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBIHSMMP_H_ */
