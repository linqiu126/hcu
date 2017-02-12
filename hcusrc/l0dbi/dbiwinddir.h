/*
 * dbiwinddir.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBIWINDDIR_H_
#define L0DBI_DBIWINDDIR_H_

#include "../l0comvm/vmlayer.h"

#define WINDDIR_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define WINDDIR_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuWinddirDataInfo_save(sensor_winddir_data_element_t *winddirData);
extern OPSTAT dbi_HcuWinddirDataInfo_inqury_1st_record(UINT32 deviceid, sensor_winddir_data_element_t *winddirData);
extern OPSTAT dbi_HcuWinddirDataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);


#endif /* L0DBI_DBIWINDDIR_H_ */
