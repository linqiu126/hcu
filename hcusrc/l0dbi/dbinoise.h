/*
 * dbinoise.h
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#ifndef L0DBI_DBINOISE_H_
#define L0DBI_DBINOISE_H_

#include "../l0comvm/vmlayer.h"

#define NOISE_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短


extern OPSTAT dbi_HcuNoiseDataInfo_save(sensor_noise_data_element_t *noiseData);
extern OPSTAT dbi_HcuNoiseDataInfo_inqury_1st_record(UINT32 deviceid, sensor_noise_data_element_t *noiseData);
extern OPSTAT dbi_HcuNoiseDataInfo_delete_3monold(UINT32 days);

extern OPSTAT dbi_HcuNoiseDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata);
extern OPSTAT dbi_HcuNoiseDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata);
extern OPSTAT dbi_HcuNoiseDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata);


//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);




#endif /* L0DBI_DBINOISE_H_ */
