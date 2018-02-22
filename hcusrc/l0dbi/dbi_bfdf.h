/*
 * dbi_bfdf.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L0DBI_BFDF_H_
#define L0DBI_BFDF_H_

#include "../l0comvm/vmlayer.h"

#define DBI_BFDF_DATA_SAVE_DAYS_MIN 90  //最短90天，不能再短
#define HCU_SYSCFG_BFDF_DB_COLUMN_NUM_MAX 15

#define HCU_L3BFDF_CONTEXT_OPERATOR_NAME_LEN_MAX    20
#define HCU_L3BFDF_CONTEXT_CONFIG_NAME_LEN_MAX    	20

typedef struct DbiL3BfdfProductPara
{
	UINT16	configId;  				//用来标识系统工作在哪一套配置参数中
	UINT8   groupPerLine;
	char    configName[HCU_L3BFDF_CONTEXT_CONFIG_NAME_LEN_MAX];
	char    operatorName[HCU_L3BFDF_CONTEXT_OPERATOR_NAME_LEN_MAX];
} DbiL3BfdfProductPara_t;

typedef struct DbiL3BfdfGroupPara
{
	UINT16  groupId;
	UINT8   lineId;
	UINT16	configId;
	UINT16	hopperNum;
	UINT16	hopperBitmap;
	UINT32	targetWeight;	//目标重量
	UINT32	targetUpLimit;	//上限比例部分，小数，不是最大值，所以这里定义的并不是目标最大值，而是最大值与目标之间的差值
	UINT32	bufWgtTarget;	//篮筐之外的缓冲区
	UINT32  rangeLow;
	UINT32  rangeHigh;
}DbiL3BfdfGroupPara_t;

//APIs
extern OPSTAT dbi_HcuBfdf_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfdfContextStaElement_t *StaDatainfo);
extern UINT32 dbi_HcuBfdf_CallCellMaxIdGet(void);
extern OPSTAT dbi_HcuBfdf_callcell_save(HcuSysMsgIeL3bfdfCallcellElement_t *input);
extern OPSTAT dbi_HcuBfdf_callcell_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuBfdf_sysConfigData_read(UINT32  sysConfigData[HCU_SYSCFG_BFDF_DB_COLUMN_NUM_MAX]);
extern OPSTAT dbi_HcuBfdf_productConfigData_read(UINT16 configId, DbiL3BfdfProductPara_t *productConfigData);
extern OPSTAT dbi_HcuBfdf_groupConfigData_read(UINT16 configId, UINT8 groupTotal, DbiL3BfdfGroupPara_t groupConfigData[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX*2]);

//引用外部的API
extern void dbi_display_header(MYSQL_RES *resPtr);
extern void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow);

#endif /* L0DBI_BFDF_H_ */
