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
    HUIREST_ACTIONID_VISION_test2   					= 0x2001,
    HUIREST_ACTIONID_VISION_worm_clasify_single   		= 0x2002,
    HUIREST_ACTIONID_VISION_worm_clasify_batch    		= 0x2003,
	HUIREST_ACTIONID_VISION_max,

	//AIWGT
	HUIREST_ACTIONID_AIWGT_min                			= 0x3000,
	HUIREST_ACTIONID_AIWGT_test1           				= 0x3000,
	HUIREST_ACTIONID_AIWGT_max,

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
#define HUIREST_ACCESS_CONST_PAR_CONTENT  		"parContent"
#define HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER  		1
#define HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_NUMBER  			2
#define HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_NUMBER  		3
#define HUIREST_ACCESS_CONST_SVRTAG_AIWGT_IN_NUMBER  		4
#define HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING		"printer"
#define HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_STRING			"dba"
#define HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_STRING		"vision"
#define HUIREST_ACCESS_CONST_SVRTAG_AIWGT_IN_STRING			"aiwgt"

/*
//http://www.aaa.com/printer/request.php?POST={json}
{
	“restTag”:”printer”,
	“actionId”:1,
	“parFlag”:1,  //FALSE-0, TRUE-1
	“parContent”:
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
    "restTag": "printer",
    "actionId": 1,
    "parFlag": 1,
    "parContent": {
        "sn": 55,
        "sucFlag": 1,
        "errCode": 0
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
	float 	testData1;
	float 	testData2;
	float 	testData3;
	float 	testData4;
}StrRestMsgIn_HUIREST_ACTIONID_VISION_test1_t;

//HUIREST_ACTIONID_VISION_test2   = 0x2001
typedef struct StrRestMsgIn_HUIREST_ACTIONID_VISION_test2
{
	float 	testData;
	float 	testData1;
	float 	testData2;
	float 	testData3;
	float 	testData4;
}StrRestMsgIn_HUIREST_ACTIONID_VISION_test2_t;

//HUIREST_ACTIONID_VISION_worm_clasify_single   = 0x2002
typedef struct StrRestMsgIn_HUIREST_ACTIONID_VISION_worm_clasify_single
{
	char 		fileName[20];
	int 		cfBase;
	int 		cfSmall2MidIndex;
	int 		cfMid2BigIndex;
	int 		cfBig2TopIndex;
}StrRestMsgIn_HUIREST_ACTIONID_VISION_worm_clasify_single_t;

typedef struct StrRestMsgIn_HUIREST_ACTIONID_VISION_worm_clasify_single_fb
{
	int 		totalNbr;
	int 		bigAlive;
	int 		bigDead;
	int 		middleAlive;
	int 		middleDead;
	int 		smallAlive;
	int 		smallDead;
	int 		totalAlive;
	int 		totalDead;
}StrRestMsgIn_HUIREST_ACTIONID_VISION_worm_clasify_single_fb_t;

//IN PYTHON or JSON
//Sun Dec 17 22:00:46 2017 HUIREST: Receiving Post Data Buf =  {'restTag': 'vision', 'actionId': 8194, 'parFlag': 1, 'parContent': {'fileName': '1.jpg', 'cfBase': 500, 'cfSmall2MidIndex': 1500, 'cfMid2BigIndex': 3000, 'cfBig2TopIndex': 5000}}
//Sun Dec 17 22:01:01 2017 HUIREST: Sending Post Data Buf =  {"restTag": "vision", "actionId": 8194, "parFlag": 1, "parContent": {"totalNbr": 121, "bigAlive": 2, "bigDead": 1, "middleAlive": 10, "middleDead": 27, "smallAlive": 26, "smallDead": 55, "totalAlive": 38, "totalDead": 83}}


//HUIREST_ACTIONID_VISION_worm_clasify_batch    = 0x2003
//TBD


//HUIREST_ACTIONID_VISION_max,


//AIWGT
//HUIREST_ACTIONID_AIWGT_min                			= 0x3000,
//HUIREST_ACTIONID_AIWGT_test1           				= 0x3000,
typedef struct StrRestMsgIn_HUIREST_ACTIONID_AIWGT_test1
{
	int 		test1;
}StrRestMsgIn_HUIREST_ACTIONID_AIWGT_test1_t;

//HUIREST_ACTIONID_AIWGT_max,



//HUIREST_ACTIONID_invalid               			= 0xFFFF, //无效





#endif /* _HUIREST_H_ */


