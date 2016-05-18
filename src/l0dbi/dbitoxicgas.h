/*
 * dbitoxicgas.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L0DBI_DBITOXICGAS_H_
#define L0DBI_DBITOXICGAS_H_

#include "../l0comvm/vmlayer.h"

#define TOXICGAS_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


#define TOXICGAS_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短

extern OPSTAT dbi_HcuToxicgasDataInfo_save(sensor_toxicgas_data_element_t *toxicgasData);
extern OPSTAT dbi_HcuToxicgasDataInfo_inqury_1st_record(UINT32 deviceid, sensor_toxicgas_data_element_t *toxicgasData);
extern OPSTAT dbi_HcuToxicgasDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuToxicgasMq135DataInfo_save(sensor_toxicgas_mq135_data_element_t *toxicgasData);
extern OPSTAT dbi_HcuToxicgasMq135DataInfo_delete_3monold(UINT32 days);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);



#endif /* L0DBI_DBITOXICGAS_H_ */
