/*
 * huirest.h
 *
 *  Created on: 2017年11月25日
 *      Author: ZJL
 */

#ifndef _HUIREST_H_
#define _HUIREST_H_
typedef unsigned char             UINT8;
typedef signed char               INT8;
typedef char                      CHAR;
typedef unsigned short            UINT16;
typedef signed short              INT16;
typedef unsigned int              UINT32;
typedef	signed int                INT32;
typedef unsigned long int         UINT64;
typedef unsigned long long int    LP64;
typedef signed long int           INT64;
typedef unsigned long             ULONG;
typedef signed long               SLONG;
typedef UINT8                     BOOLEAN;
typedef void                      VOID;


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUIREST操作字的统一定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//以下字段定义，实际上是可以重复的，因为不同的ACTIONID是在不同的空间中．这里定义成统一数据空间，只是为了统一规划，空间大小不是个问题．
//标准命令字统一定义
typedef enum HuirestActionidDefination
{
	HUIREST_ACTIONID_null                  	  			= 0x000,

	//PRINTER
	HUIREST_ACTIONID_PRINTER_min                		= 0x0100,
	HUIREST_ACTIONID_PRINTER_callcell_bfsc           	= 0x0100,
	HUIREST_ACTIONID_PRINTER_callcell_bfdf           	= 0x0101,
	HUIREST_ACTIONID_PRINTER_callcell_bfhs           	= 0x0102,
	HUIREST_ACTIONID_PRINTER_fam_sdqx_md1           	= 0x0110,
	HUIREST_ACTIONID_PRINTER_fam_sdqx_md2           	= 0x0111,
	HUIREST_ACTIONID_PRINTER_max,

	//DBA
	HUIREST_ACTIONID_DBA_min                			= 0x1000,
	HUIREST_ACTIONID_DBA_yczx_temp_update           	= 0x1000,
	HUIREST_ACTIONID_DBA_max,

	//VISION
	HUIREST_ACTIONID_VISION_min                			= 0x2000,
	HUIREST_ACTIONID_VISION_test1           			= 0x2000,
	HUIREST_ACTIONID_VISION_max,

	HUIREST_ACTIONID_invalid               				= 0xFFFF, //无效
}HuirestActionidDefination_t;


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUIREST采用json结构，公共结构定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//HUIREST结构
#define HUIREST_ACCESS_CONST_SERVICE_TAG  		"restTag"
#define HUIREST_ACCESS_CONST_ACTION_ID  		"actionId"
#define HUIREST_ACCESS_CONST_PAR_FLAG  			"parFlag"
#define HUIREST_ACCESS_CONST_PAR_CONTENT  		"parConent"
#define HUIREST_ACCESS_CONST_SVRTAGID_PRINTER  	1
#define HUIREST_ACCESS_CONST_SVRTAGID_DBA  		2
#define HUIREST_ACCESS_CONST_SVRTAGID_VISION  	3
#define HUIREST_ACCESS_CONST_SVRTAG_PRINTER  	"printer"
#define HUIREST_ACCESS_CONST_SVRTAG_DBA  		"dba"
#define HUIREST_ACCESS_CONST_SVRTAG_VISION  	"vision"

/*
//http://www.aaa.com/printer/request.php?POST={json}
{
	“restTag”:”printer”,
	“actionId”:1,
	“parFlag”:1,  //FALSE-0, TRUE-1
	“parConent”:
	{
		“weight”: 12.33,
		“unit”: "kg",
		“operator”: "王芳",
		“sn”: 55,
		“barCode”: "1234567",
		“qrCode”: "http://12121.com/fasdfa"
	}
}

{
	“restTag”:”printer”,
	“actionId”:1,
	“parFlag”:1,  //FALSE-0, TRUE-1
	“parConent”:
	{
		“sn”: 55,
		"sucFlag":1,
		"errCode":0
	}
}


*/


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUIREST各操作字的详细结构定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//HUIREST_ACTIONID_null                  	  			= 0x000,
///PRINTER
//HUIREST_ACTIONID_PRINTER_min                		= 0x0100,
//HUIREST_ACTIONID_PRINTER_callcell_bfsc           	= 0x0100,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfsc
{
	UINT16 	cfgId;
	UINT8 	matNbr;
	float 	wgtTarget;
	float 	wgtCurrent;
	char  	wgtUnit[3];
	char  	oprName[13];
	UINT32 	sn;
	UINT32 	timeStamp;
	char  	barCode[13];
	char  	qrCode[100];
	char  	barCodeFile[30];
	char  	qrCodeFile[30];
}StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfsc_t;
//图像文件名字，统一约定在web服务器的根目录下，./printer/pic/aaa.jpg

//通用返回消息，不特别定义的，都采用通用结构
typedef struct StrRestMsgOut_HUIREST_ACTIONID_general
{
	UINT8   sucFlag;  //0-FALSE, 1-TRUE
	UINT32 	errCode;
}StrRestMsgOut_HUIREST_ACTIONID_general_t;

//HUIREST_ACTIONID_PRINTER_callcell_bfdf           	= 0x0101,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfdf
{
	UINT16 	cfgId;
	float 	wgtTarget;
	float 	wgtCurrent;
	char  	wgtUnit[3];
	char  	oprName[13];
	UINT32 	sn;
	UINT32 	timeStamp;
	char  	barCode[13];
	char  	qrCode[100];
	char  	barCodeFile[30];
	char  	qrCodeFile[30];
}StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfdf_t;
//图像文件名字，统一约定在web服务器的根目录下，./printer/pic/aaa.jpg

//HUIREST_ACTIONID_PRINTER_callcell_bfhs           	= 0x0102,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfhs
{
	UINT16 	cfgId;
	float 	wgtTarget;
	float 	wgtCurrent;
	char  	wgtUnit[3];
	char  	oprName[13];
	UINT32 	sn;
	UINT32 	timeStamp;
	char  	barCode[13];
	char  	qrCode[100];
	char  	barCodeFile[30];
	char  	qrCodeFile[30];
}StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfhs_t;
//图像文件名字，统一约定在web服务器的根目录下，./printer/pic/aaa.jpg

//HUIREST_ACTIONID_PRINTER_fam_sdqx_md1           	= 0x0110,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1
{
	char  	oprName[13];
	UINT32 	sn;
	char  	devId[20];
	UINT32 	timeStamp;
	char  	barCode[13];
	char  	qrCode[100];
}StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1_t;

//HUIREST_ACTIONID_PRINTER_fam_sdqx_md2           	= 0x0111,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2
{
	char  	oprName[13];
	UINT32 	sn;
	char  	devId[20];
	UINT32 	timeStamp;
	char  	barCode[13];
	char  	qrCode[100];
}StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2_t;

//HUIREST_ACTIONID_PRINTER_max,

///DBA
//HUIREST_ACTIONID_DBA_min                			= 0x1000,
//HUIREST_ACTIONID_DBA_yczx_temp_update           	= 0x1000,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_DBA_yczx_temp_update
{
	float 	tempCur;
	float 	tempMin;
	float 	tempMax;
	float 	tempAvg;
	char	uint[3];  //in Degree or in K
	UINT32	timeStamp;
}StrRestMsgIn_HUIREST_ACTIONID_DBA_yczx_temp_update_t;

//HUIREST_ACTIONID_DBA_max,

///VISION
//HUIREST_ACTIONID_VISION_min                		= 0x2000,
//HUIREST_ACTIONID_VISION_test1           			= 0x2000,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_VISION_test1
{
	float 	testData;
}StrRestMsgIn_HUIREST_ACTIONID_VISION_test1_t;

//HUIREST_ACTIONID_VISION_max,

//HUIREST_ACTIONID_invalid               			= 0xFFFF, //无效





#endif /* _HUIREST_H_ */


