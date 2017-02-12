/*
 * dbilightstr.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L0DBI_DBILIGHTSTR_H_
#define L0DBI_DBILIGHTSTR_H_

#include "../l0comvm/vmlayer.h"

#define LIGHTSTR_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define LIGHTSTR_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuLightstrDataInfo_save(sensor_lightstr_data_element_t *lightstrData);
extern OPSTAT dbi_HcuLightstrDataInfo_inqury_1st_record(UINT32 deviceid, sensor_lightstr_data_element_t *lightstrData);
extern OPSTAT dbi_HcuLightstrDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuLightstrBh1750DataInfo_save(sensor_lightstr_bh1750_data_element_t *lightstrData);
extern OPSTAT dbi_HcuLightstrBh1750DataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);



#endif /* L0DBI_DBILIGHTSTR_H_ */
