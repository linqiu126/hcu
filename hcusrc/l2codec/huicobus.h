/*
 * huicobus.h
 *
 *  Created on: 2017年11月25日
 *      Author: ZJL
 */

//pack函数必须严格放在结构的里面，且不得包含任何其它包含文件，不然会发生意想不到的问题！！！


#ifndef _HUICOBUS_H_
#define _HUICOBUS_H_
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

#pragma pack (1) //强制1字节对齐
/*
 *
 *  顺从并更新到技术规范《慧HUICOSBUS接口规范v0.3, LAST UPDATE@2017/11/25》
 *
 *
 */

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUICOBUS公共定义常量
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#define HUICOBUS_CMDID_CUI_BUF_HLC_MAX_LEN		400

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUICOBUS字典定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//标准命令字统一定义
typedef enum HuicobusCmdidCuiDefination
{
	HUICOBUS_CMDID_cui_null                  	  		= 0x0000,

	//UIR2HCU频道
	HUICOBUS_CMDID_cui_uir2hcu_min                		= 0x0100,
	HUICOBUS_CMDID_cui_uir2hcu_init_req           		= 0x0100,
	HUICOBUS_CMDID_cui_uir2hcu_start_resume_req   		= 0x0101,
	HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req   		= 0x0102,
	HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req      		= 0x0103,
	HUICOBUS_CMDID_cui_uir2hcu_cali_full_req      		= 0x0104,
	HUICOBUS_CMDID_cui_uir2hcu_study_start_req    		= 0x0105,
	HUICOBUS_CMDID_cui_uir2hcu_study_stop_req     		= 0x0106,
	HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req       		= 0x0107,
	HUICOBUS_CMDID_cui_uir2hcu_max,

	//HCU2UIR频道
	HUICOBUS_CMDID_cui_hcu2uir_min                		= 0x0180,
	HUICOBUS_CMDID_cui_hcu2uir_init_resp          		= 0x0180,
	HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp  		= 0x0181,
	HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp  		= 0x0182,
	HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp     		= 0x0183,
	HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp     		= 0x0184,
	HUICOBUS_CMDID_cui_hcu2uir_study_start_resp   		= 0x0185,
	HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp    		= 0x0186,
	HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp      		= 0x0187,
	HUICOBUS_CMDID_cui_hcu2uir_version_report     		= 0x0188,
	HUICOBUS_CMDID_cui_hcu2uir_status_report       		= 0x0189,
	HUICOBUS_CMDID_cui_hcu2uir_alarm_report       		= 0x018A,
	HUICOBUS_CMDID_cui_hcu2uir_error_report       		= 0x018B,
	HUICOBUS_CMDID_cui_hcu2uir_restart_report      		= 0x018C,
	HUICOBUS_CMDID_cui_hcu2uir_logout_report      		= 0x018D,
	HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report      	= 0x0190,
	HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report      	= 0x0191,
	HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report      	= 0x0192,
	HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report     = 0x0193,
	HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report     = 0x0194,
	HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report     = 0x0195,
	HUICOBUS_CMDID_cui_hcu2uir_max,

	//UIR2UIP频道
	HUICOBUS_CMDID_cui_uir2uip_min                		= 0x0280,
	HUICOBUS_CMDID_cui_uir2uip_init_inq_resp            = 0x0280,
	HUICOBUS_CMDID_cui_uir2uip_start_resume_resp  		= 0x0281,
	HUICOBUS_CMDID_cui_uir2uip_stop_suspend_resp  		= 0x0282,
	HUICOBUS_CMDID_cui_uir2uip_cali_zero_resp     		= 0x0283,
	HUICOBUS_CMDID_cui_uir2uip_cali_full_resp     		= 0x0284,
	HUICOBUS_CMDID_cui_uir2uip_study_start_resp   		= 0x0285,
	HUICOBUS_CMDID_cui_uir2uip_study_stop_resp    		= 0x0286,
	HUICOBUS_CMDID_cui_uir2uip_test_cmd_resp      		= 0x0287,
	HUICOBUS_CMDID_cui_uir2uip_version_report     		= 0x0288,
	HUICOBUS_CMDID_cui_uir2uip_status_report       		= 0x0289,
	HUICOBUS_CMDID_cui_uir2uip_alarm_report       		= 0x028A,
	HUICOBUS_CMDID_cui_uir2uip_error_report       		= 0x028B,
	HUICOBUS_CMDID_cui_uir2uip_restart_report      		= 0x028C,
	HUICOBUS_CMDID_cui_uir2uip_logout_report      		= 0x028D,
	HUICOBUS_CMDID_cui_uir2uip_inswgt_bfsc_report      	= 0x0290,
	HUICOBUS_CMDID_cui_uir2uip_inswgt_bfdf_report      	= 0x0291,
	HUICOBUS_CMDID_cui_uir2uip_inswgt_bfhs_report      	= 0x0292,
	HUICOBUS_CMDID_cui_uir2uip_callcell_bfsc_report     = 0x0293,
	HUICOBUS_CMDID_cui_uir2uip_callcell_bfdf_report     = 0x0294,
	HUICOBUS_CMDID_cui_uir2uip_callcell_bfhs_report     = 0x0295,
	HUICOBUS_CMDID_cui_uir2uip_max,

	//QRPRINT频道
	HUICOBUS_CMDID_cui_printflow_min                	= 0x0380,
	HUICOBUS_CMDID_cui_printflow_new_record            	= 0x0380,
	HUICOBUS_CMDID_cui_printflow_max,

	//DBACCESS频道
	HUICOBUS_CMDID_cui_dbaccess_min                		= 0x0400,
	HUICOBUS_CMDID_cui_dbaccess_read_req            	= 0x0400,
	HUICOBUS_CMDID_cui_dbaccess_max,

	//DBFEEDBACK频道
	HUICOBUS_CMDID_cui_dbfeedback_min               	= 0x0480,
	HUICOBUS_CMDID_cui_dbfeedback_read_resp         	= 0x0480,
	HUICOBUS_CMDID_cui_dbfeedback_max,

	//BHPROTO频道
	HUICOBUS_CMDID_cui_bhproto_min               		= 0x0500,
	HUICOBUS_CMDID_cui_bhproto_max,

	//LOGERRFLOW频道
	HUICOBUS_CMDID_cui_logerrflow_min               	= 0x0680,
	HUICOBUS_CMDID_cui_logerrflow_new_record      		= 0x0680,
	HUICOBUS_CMDID_cui_logerrflow_max,

	//LOGTRACEFLOW频道
	HUICOBUS_CMDID_cui_logtraceflow_min               	= 0x0780,
	HUICOBUS_CMDID_cui_logtraceflow_new_record      	= 0x0780,
	HUICOBUS_CMDID_cui_logtraceflow_max,

	//OPN2HCU频道
	HUICOBUS_CMDID_cui_opn2hcu_min               		= 0x0800,
	HUICOBUS_CMDID_cui_opn2hcu_data_req      			= 0x0800,
	HUICOBUS_CMDID_cui_opn2hcu_ctrl_req					= 0x0801,
	HUICOBUS_CMDID_cui_opn2hcu_data_confirm				= 0x0802,
	HUICOBUS_CMDID_cui_opn2hcu_max,

	//HCU2OPN频道
	HUICOBUS_CMDID_cui_hcu2opn_min               		= 0x0880,
	HUICOBUS_CMDID_cui_hcu2opn_data_resp				= 0x0880,
	HUICOBUS_CMDID_cui_hcu2opn_ctrl_resp				= 0x0881,
	HUICOBUS_CMDID_cui_hcu2opn_data_report				= 0x0882,
	HUICOBUS_CMDID_cui_hcu2opn_max,

	HUICOBUS_CMDID_cui_invalid               = 0xFFFF, //无效
}HuicobusCmdidCuiDefination_t;


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUICOBUS标准消息头定义
//
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
{
	“srcNode”:1,
	“destNode”:2,
	“srcId”:33,
	“destId”:44,
	“topicId”:55,
	“cmdId”:1234,
	“cmdValue”:5678,
	“hlContent”:
	{
		“aa”: 12,
		“bb”: “34”
	}
}
*/
#define HUICOBUS_CMDID_STRING_SRCNODE		"srcNode"
#define HUICOBUS_CMDID_STRING_DESTNODE		"destNode"
#define HUICOBUS_CMDID_STRING_SRCID			"srcId"
#define HUICOBUS_CMDID_STRING_DESTID		"destId"
#define HUICOBUS_CMDID_STRING_TOPICID		"topicId"
#define HUICOBUS_CMDID_STRING_CMDID			"cmdId"
#define HUICOBUS_CMDID_STRING_CMDVALUE		"cmdValue"
#define HUICOBUS_CMDID_STRING_HLCONTENT		"hlContent"

//公共消息
typedef struct StrMsg_HUICOBUS_CMDID_cui_general_msg_head
{
	UINT32 srcNode;
	UINT32 destNode;
	UINT32 srcId;
	UINT32 destId;
	UINT32 topicId;
	UINT32 cmdId;
	UINT32 cmdValue;
}StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t;
typedef struct StrMsg_HUICOBUS_CMDID_cui_general_message
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	UINT8  data[HUICOBUS_CMDID_CUI_BUF_HLC_MAX_LEN];
}StrMsg_HUICOBUS_CMDID_cui_general_message_t;

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUICOBUS消息中HLC部分：High Layer Content
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//HUICOBUS_CMDID_cui_null                  	  		= 0x0000,

//UIR2HCU频道
//HUICOBUS_CMDID_cui_uir2hcu_min                		= 0x0100,
//HUICOBUS_CMDID_cui_uir2hcu_init_req           		= 0x0100,
typedef struct StrHlcIe_cui_uir2hcu_init_req
{
}StrHlcIe_cui_uir2hcu_init_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_start_resume_req   		= 0x0101,
typedef struct StrHlcIe_cui_uir2hcu_start_resume_req
{
}StrHlcIe_cui_uir2hcu_start_resume_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req   		= 0x0102,
typedef struct StrHlcIe_cui_uir2hcu_stop_suspend_req
{
}StrHlcIe_cui_uir2hcu_stop_suspend_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req      		= 0x0103,
typedef struct StrHlcIe_cui_uir2hcu_cali_zero_req
{
}StrHlcIe_cui_uir2hcu_cali_zero_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_cali_full_req      		= 0x0104,
typedef struct StrHlcIe_cui_uir2hcu_cali_full_req
{
}StrHlcIe_cui_uir2hcu_cali_full_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_study_start_req    		= 0x0105,
typedef struct StrHlcIe_cui_uir2hcu_study_start_req
{
}StrHlcIe_cui_uir2hcu_study_start_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_study_stop_req     		= 0x0106,
typedef struct StrHlcIe_uir2hcu_study_stop_req
{
}StrHlcIe_cui_uir2hcu_study_stop_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req       		= 0x0107,
typedef struct StrHlcIe_uir2hcu_test_cmd_req
{
}StrHlcIe_cui_uir2hcu_test_cmd_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_max,

//HCU2UIR频道
//HUICOBUS_CMDID_cui_hcu2uir_min                		= 0x0180,
//HUICOBUS_CMDID_cui_hcu2uir_init_resp          		= 0x0180,
typedef struct StrHlcIe_cui_hcu2uir_init_resp
{
}StrHlcIe_cui_hcu2uir_init_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp  		= 0x0181,
typedef struct StrHlcIe_cui_hcu2uir_start_resume_resp
{
}StrHlcIe_cui_hcu2uir_start_resume_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp  		= 0x0182,
typedef struct StrHlcIe_cui_hcu2uir_stop_suspend_resp
{
}StrHlcIe_cui_hcu2uir_stop_suspend_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp     		= 0x0183,
typedef struct StrHlcIe_cui_hcu2uir_cali_zero_resp
{
}StrHlcIe_cui_hcu2uir_cali_zero_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp     		= 0x0184,
typedef struct StrHlcIe_cui_hcu2uir_cali_full_resp
{
}StrHlcIe_cui_hcu2uir_cali_full_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_study_start_resp   		= 0x0185,
typedef struct StrHlcIe_cui_hcu2uir_study_start_resp
{
}StrHlcIe_cui_hcu2uir_study_start_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp    		= 0x0186,
typedef struct StrHlcIe_cui_hcu2uir_study_stop_resp
{
}StrHlcIe_cui_hcu2uir_study_stop_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp      		= 0x0187,
typedef struct StrHlcIe_cui_hcu2uir_test_cmd_resp
{
}StrHlcIe_cui_hcu2uir_test_cmd_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_version_report     		= 0x0188,
typedef struct StrHlcIe_cui_hcu2uir_version_report
{
}StrHlcIe_cui_hcu2uir_version_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_status_report       		= 0x0189,
typedef struct StrHlcIe_cui_hcu2uir_status_report
{
}StrHlcIe_cui_hcu2uir_status_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_alarm_report       		= 0x018A,
typedef struct StrHlcIe_cui_hcu2uir_alarm_report
{
}StrHlcIe_cui_hcu2uir_alarm_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_error_report       		= 0x018B,
typedef struct StrHlcIe_cui_hcu2uir_error_report
{
}StrHlcIe_cui_hcu2uir_error_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_restart_report      		= 0x018C,
typedef struct StrHlcIe_cui_hcu2uir_restart_report
{
}StrHlcIe_cui_hcu2uir_restart_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_logout_report      		= 0x018D,
typedef struct StrHlcIe_cui_hcu2uir_logout_report
{
}StrHlcIe_cui_hcu2uir_logout_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report      	= 0x0190,
typedef struct StrHlcIe_cui_hcu2uir_inswgt_bfsc_report
{
}StrHlcIe_cui_hcu2uir_inswgt_bfsc_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report      	= 0x0191,
typedef struct StrHlcIe_cui_hcu2uir_inswgt_bfdf_report
{
}StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report      	= 0x0192,
typedef struct StrHlcIe_cui_hcu2uir_inswgt_bfhs_report
{
}StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report     = 0x0193,
typedef struct StrHlcIe_cui_hcu2uir_callcell_bfsc_report
{
}StrHlcIe_cui_hcu2uir_callcell_bfsc_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report     = 0x0194,
typedef struct StrHlcIe_cui_hcu2uir_callcell_bfdf_report
{
}StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report     = 0x0195,
typedef struct StrHlcIe_cui_hcu2uir_callcell_bfhs_report
{
}StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_max,

//UIR2UIP频道
//HUICOBUS_CMDID_cui_uir2uip_min                		= 0x0280,
//HUICOBUS_CMDID_cui_uir2uip_init_inq_resp            = 0x0280,
typedef struct StrHlcIe_cui_uir2uip_init_inq_resp
{
}StrHlcIe_cui_uir2uip_init_inq_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_start_resume_resp  		= 0x0281,
typedef struct StrHlcIe_cui_uir2uip_start_resume_resp
{
}StrHlcIe_cui_uir2uip_start_resume_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_stop_suspend_resp  		= 0x0282,
typedef struct StrHlcIe_cui_uir2uip_stop_suspend_resp
{
}StrHlcIe_cui_uir2uip_stop_suspend_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_cali_zero_resp     		= 0x0283,
typedef struct StrHlcIe_cui_uir2uip_cali_zero_resp
{
}StrHlcIe_cui_uir2uip_cali_zero_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_cali_full_resp     		= 0x0284,
typedef struct StrHlcIe_cui_uir2uip_cali_full_resp
{
}StrHlcIe_cui_uir2uip_cali_full_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_study_start_resp   		= 0x0285,
typedef struct StrHlcIe_cui_uir2uip_study_start_resp
{
}StrHlcIe_cui_uir2uip_study_start_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_study_stop_resp    		= 0x0286,
typedef struct StrHlcIe_cui_uir2uip_study_stop_resp
{
}StrHlcIe_cui_uir2uip_study_stop_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_test_cmd_resp      		= 0x0287,
typedef struct StrHlcIe_cui_uir2uip_test_cmd_resp
{
}StrHlcIe_cui_uir2uip_test_cmd_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_version_report     		= 0x0288,
typedef struct StrHlcIe_cui_uir2uip_version_report
{
}StrHlcIe_cui_uir2uip_version_report_t;

//HUICOBUS_CMDID_cui_uir2uip_status_report       		= 0x0289,
typedef struct StrHlcIe_cui_uir2uip_status_report
{
}StrHlcIe_cui_uir2uip_status_report_t;

//HUICOBUS_CMDID_cui_uir2uip_alarm_report       		= 0x028A,
typedef struct StrHlcIe_cui_uir2uip_alarm_report
{
}StrHlcIe_cui_uir2uip_alarm_report_t;

//HUICOBUS_CMDID_cui_uir2uip_error_report       		= 0x028B,
typedef struct StrHlcIe_cui_uir2uip_error_report
{
}StrHlcIe_cui_uir2uip_error_report_t;

//HUICOBUS_CMDID_cui_uir2uip_restart_report      		= 0x028C,
typedef struct StrHlcIe_cui_uir2uip_restart_report
{
}StrHlcIe_cui_uir2uip_restart_report_t;

//HUICOBUS_CMDID_cui_uir2uip_logout_report      		= 0x028D,
typedef struct StrHlcIe_cui_uir2uip_logout_report
{
}StrHlcIe_cui_uir2uip_logout_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfsc_report      	= 0x0290,
typedef struct StrHlcIe_cui_uir2uip_inswgt_bfsc_report
{
}StrHlcIe_cui_uir2uip_inswgt_bfsc_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfdf_report      	= 0x0291,
typedef struct StrHlcIe_cui_uir2uip_inswgt_bfdf_report
{
}StrHlcIe_cui_uir2uip_inswgt_bfdf_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfhs_report      	= 0x0292,
typedef struct StrHlcIe_cui_uir2uip_inswgt_bfhs_report
{
}StrHlcIe_cui_uir2uip_inswgt_bfhs_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfsc_report     = 0x0293,
typedef struct StrHlcIe_cui_uir2uip_callcell_bfsc_report
{
}StrHlcIe_cui_uir2uip_callcell_bfsc_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfdf_report     = 0x0294,
typedef struct StrHlcIe_cui_uir2uip_callcell_bfdf_report
{
}StrHlcIe_cui_uir2uip_callcell_bfdf_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfhs_report     = 0x0295,
typedef struct StrHlcIe_cui_uir2uip_callcell_bfhs_report
{
}StrHlcIe_cui_uir2uip_callcell_bfhs_report_t;

//HUICOBUS_CMDID_cui_uir2uip_max,

//QRPRINT频道
//HUICOBUS_CMDID_cui_printflow_min                	= 0x0380,
//HUICOBUS_CMDID_cui_printflow_new_record            	= 0x0380,
typedef struct StrHlcIe_cui_printflow_new_record
{
}StrHlcIe_cui_printflow_new_record_t;

//HUICOBUS_CMDID_cui_printflow_max,

//DBACCESS频道
//HUICOBUS_CMDID_cui_dbaccess_min                		= 0x0400,
//HUICOBUS_CMDID_cui_dbaccess_read_req            	= 0x0400,
typedef struct StrHlcIe_cui_dbaccess_read_req
{
}StrHlcIe_cui_dbaccess_read_req_t;

//HUICOBUS_CMDID_cui_dbaccess_max,

//DBFEEDBACK频道
//HUICOBUS_CMDID_cui_dbfeedback_min               	= 0x0480,
//HUICOBUS_CMDID_cui_dbfeedback_read_resp         	= 0x0480,
typedef struct StrHlcIe_cui_dbfeedback_read_resp
{
}StrHlcIe_cui_dbfeedback_read_resp_t;

//HUICOBUS_CMDID_cui_dbfeedback_max,

//BHPROTO频道
//HUICOBUS_CMDID_cui_bhproto_min               		= 0x0500,
//HUICOBUS_CMDID_cui_bhproto_max,

//LOGERRFLOW频道
//HUICOBUS_CMDID_cui_logerrflow_min               	= 0x0680,
//HUICOBUS_CMDID_cui_logerrflow_new_record      		= 0x0680,
typedef struct StrHlcIe_cui_logerrflow_new_record
{
}StrHlcIe_cui_logerrflow_new_record_t;

//HUICOBUS_CMDID_cui_logerrflow_max,

//LOGTRACEFLOW频道
//HUICOBUS_CMDID_cui_logtraceflow_min               	= 0x0780,
//HUICOBUS_CMDID_cui_logtraceflow_new_record      	= 0x0780,
typedef struct StrHlcIe_cui_logtraceflow_new_record
{
}StrHlcIe_cui_logtraceflow_new_record_t;

//HUICOBUS_CMDID_cui_logtraceflow_max,

//OPN2HCU频道
//HUICOBUS_CMDID_cui_opn2hcu_min               		= 0x0800,
//HUICOBUS_CMDID_cui_opn2hcu_data_req      			= 0x0800,
typedef struct StrHlcIe_cui_opn2hcu_data_req
{
}StrHlcIe_cui_opn2hcu_data_req_t;

//HUICOBUS_CMDID_cui_opn2hcu_ctrl_req					= 0x0801,
typedef struct StrHlcIe_cui_opn2hcu_ctrl_req
{
}StrHlcIe_cui_opn2hcu_ctrl_req_t;

//HUICOBUS_CMDID_cui_opn2hcu_data_confirm				= 0x0802,
typedef struct StrHlcIe_cui_opn2hcu_data_confirm
{
}StrHlcIe_cui_opn2hcu_data_confirm_t;

//HUICOBUS_CMDID_cui_opn2hcu_max,

//HCU2OPN频道
//HUICOBUS_CMDID_cui_hcu2opn_min               		= 0x0880,
//HUICOBUS_CMDID_cui_hcu2opn_data_resp				= 0x0880,
typedef struct StrHlcIe_cui_hcu2opn_data_resp
{
}StrHlcIe_cui_hcu2opn_data_resp_t;

//HUICOBUS_CMDID_cui_hcu2opn_ctrl_resp				= 0x0881,
typedef struct StrHlcIe_cui_hcu2opn_ctrl_resp
{
}StrHlcIe_cui_hcu2opn_ctrl_resp_t;

//HUICOBUS_CMDID_cui_hcu2opn_data_report				= 0x0882,
typedef struct StrHlcIe_cui_hcu2opn_data_report
{
}StrHlcIe_cui_hcu2opn_data_report_t;

//HUICOBUS_CMDID_cui_hcu2opn_max,

//HUICOBUS_CMDID_cui_invalid               = 0xFFFF, //无效


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  HUICOBUS消息
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//HUICOBUS_CMDID_cui_null                  	  		= 0x0000,

//UIR2HCU频道
//HUICOBUS_CMDID_cui_uir2hcu_min                		= 0x0100,
//HUICOBUS_CMDID_cui_uir2hcu_init_req           		= 0x0100,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_init_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_init_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_init_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_start_resume_req   		= 0x0101,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_start_resume_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_start_resume_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_start_resume_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req   		= 0x0102,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_stop_suspend_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req      		= 0x0103,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_cali_zero_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_cali_full_req      		= 0x0104,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_cali_full_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_cali_full_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_cali_full_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_study_start_req    		= 0x0105,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_study_start_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_study_start_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_study_start_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_study_stop_req     		= 0x0106,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_study_stop_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_study_stop_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_study_stop_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req       		= 0x0107,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2hcu_test_cmd_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req_t;

//HUICOBUS_CMDID_cui_uir2hcu_max,

//HCU2UIR频道
//HUICOBUS_CMDID_cui_hcu2uir_min                		= 0x0180,
//HUICOBUS_CMDID_cui_hcu2uir_init_resp          		= 0x0180,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_init_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_init_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_init_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp  		= 0x0181,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_start_resume_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp  		= 0x0182,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_stop_suspend_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp     		= 0x0183,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_cali_zero_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp     		= 0x0184,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_cali_full_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_study_start_resp   		= 0x0185,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_study_start_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_study_start_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_study_start_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp    		= 0x0186,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_study_stop_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp      		= 0x0187,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_test_cmd_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp_t;

//HUICOBUS_CMDID_cui_hcu2uir_version_report     		= 0x0188,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_version_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_version_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_version_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_status_report       		= 0x0189,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_status_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_status_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_status_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_alarm_report       		= 0x018A,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_alarm_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_alarm_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_alarm_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_error_report       		= 0x018B,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_error_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_error_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_error_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_restart_report      		= 0x018C,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_restart_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_restart_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_restart_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_logout_report      		= 0x018D,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_logout_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_logout_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_logout_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report      	= 0x0190,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_inswgt_bfsc_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report      	= 0x0191,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report      	= 0x0192,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report     = 0x0193,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_callcell_bfsc_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report     = 0x0194,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report     = 0x0195,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report_t;

//HUICOBUS_CMDID_cui_hcu2uir_max,

//UIR2UIP频道
//HUICOBUS_CMDID_cui_uir2uip_min                		= 0x0280,
//HUICOBUS_CMDID_cui_uir2uip_init_inq_resp            = 0x0280,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_init_inq_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_init_inq_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_init_inq_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_start_resume_resp  		= 0x0281,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_start_resume_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_start_resume_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_start_resume_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_stop_suspend_resp  		= 0x0282,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_stop_suspend_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_stop_suspend_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_stop_suspend_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_cali_zero_resp     		= 0x0283,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_cali_zero_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_cali_zero_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_cali_zero_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_cali_full_resp     		= 0x0284,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_cali_full_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_cali_full_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_cali_full_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_study_start_resp   		= 0x0285,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_study_start_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_study_start_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_study_start_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_study_stop_resp    		= 0x0286,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_study_stop_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_study_stop_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_study_stop_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_test_cmd_resp      		= 0x0287,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_test_cmd_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_test_cmd_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_test_cmd_resp_t;

//HUICOBUS_CMDID_cui_uir2uip_version_report     		= 0x0288,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_version_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_version_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_version_report_t;

//HUICOBUS_CMDID_cui_uir2uip_status_report       		= 0x0289,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_status_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_status_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_status_report_t;

//HUICOBUS_CMDID_cui_uir2uip_alarm_report       		= 0x028A,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_alarm_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_alarm_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_alarm_report_t;

//HUICOBUS_CMDID_cui_uir2uip_error_report       		= 0x028B,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_error_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_error_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_error_report_t;

//HUICOBUS_CMDID_cui_uir2uip_restart_report      		= 0x028C,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_restart_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_restart_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_restart_report_t;

//HUICOBUS_CMDID_cui_uir2uip_logout_report      		= 0x028D,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_logout_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_logout_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_logout_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfsc_report      	= 0x0290,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfsc_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_inswgt_bfsc_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfsc_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfdf_report      	= 0x0291,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfdf_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_inswgt_bfdf_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfdf_report_t;

//HUICOBUS_CMDID_cui_uir2uip_inswgt_bfhs_report      	= 0x0292,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfhs_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_inswgt_bfhs_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_inswgt_bfhs_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfsc_report     = 0x0293,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfsc_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_callcell_bfsc_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfsc_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfdf_report     = 0x0294,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfdf_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_callcell_bfdf_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfdf_report_t;

//HUICOBUS_CMDID_cui_uir2uip_callcell_bfhs_report     = 0x0295,
typedef struct StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfhs_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_uir2uip_callcell_bfhs_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_uir2uip_callcell_bfhs_report_t;

//HUICOBUS_CMDID_cui_uir2uip_max,

//QRPRINT频道
//HUICOBUS_CMDID_cui_printflow_min                	= 0x0380,
//HUICOBUS_CMDID_cui_printflow_new_record            	= 0x0380,
typedef struct StrMsg_HUICOBUS_CMDID_cui_printflow_new_record
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_printflow_new_record_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_printflow_new_record_t;

//HUICOBUS_CMDID_cui_printflow_max,

//DBACCESS频道
//HUICOBUS_CMDID_cui_dbaccess_min                		= 0x0400,
//HUICOBUS_CMDID_cui_dbaccess_read_req            	= 0x0400,
typedef struct StrMsg_HUICOBUS_CMDID_cui_dbaccess_read_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_dbaccess_read_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_dbaccess_read_req_t;

//HUICOBUS_CMDID_cui_dbaccess_max,

//DBFEEDBACK频道
//HUICOBUS_CMDID_cui_dbfeedback_min               	= 0x0480,
//HUICOBUS_CMDID_cui_dbfeedback_read_resp         	= 0x0480,
typedef struct StrMsg_HUICOBUS_CMDID_cui_dbfeedback_read_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_dbfeedback_read_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_dbfeedback_read_resp_t;

//HUICOBUS_CMDID_cui_dbfeedback_max,

//BHPROTO频道
//HUICOBUS_CMDID_cui_bhproto_min               		= 0x0500,
//HUICOBUS_CMDID_cui_bhproto_max,

//LOGERRFLOW频道
//HUICOBUS_CMDID_cui_logerrflow_min               	= 0x0680,
//HUICOBUS_CMDID_cui_logerrflow_new_record      		= 0x0680,
typedef struct StrMsg_HUICOBUS_CMDID_cui_logerrflow_new_record
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_logerrflow_new_record_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_logerrflow_new_record_t;

//HUICOBUS_CMDID_cui_logerrflow_max,

//LOGTRACEFLOW频道
//HUICOBUS_CMDID_cui_logtraceflow_min               	= 0x0780,
//HUICOBUS_CMDID_cui_logtraceflow_new_record      	= 0x0780,
typedef struct StrMsg_HUICOBUS_CMDID_cui_logtraceflow_new_record
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_logtraceflow_new_record_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_logtraceflow_new_record_t;

//HUICOBUS_CMDID_cui_logtraceflow_max,

//OPN2HCU频道
//HUICOBUS_CMDID_cui_opn2hcu_min               		= 0x0800,
//HUICOBUS_CMDID_cui_opn2hcu_data_req      			= 0x0800,
typedef struct StrMsg_HUICOBUS_CMDID_cui_opn2hcu_data_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_opn2hcu_data_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_opn2hcu_data_req_t;

//HUICOBUS_CMDID_cui_opn2hcu_ctrl_req					= 0x0801,
typedef struct StrMsg_HUICOBUS_CMDID_cui_opn2hcu_ctrl_req
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_opn2hcu_ctrl_req_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_opn2hcu_ctrl_req_t;

//HUICOBUS_CMDID_cui_opn2hcu_data_confirm				= 0x0802,
typedef struct StrMsg_HUICOBUS_CMDID_cui_opn2hcu_data_confirm
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_opn2hcu_data_confirm_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_opn2hcu_data_confirm_t;

//HUICOBUS_CMDID_cui_opn2hcu_max,

//HCU2OPN频道
//HUICOBUS_CMDID_cui_hcu2opn_min               		= 0x0880,
//HUICOBUS_CMDID_cui_hcu2opn_data_resp				= 0x0880,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2opn_data_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2opn_data_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2opn_data_resp_t;

//HUICOBUS_CMDID_cui_hcu2opn_ctrl_resp				= 0x0881,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2opn_ctrl_resp
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2opn_ctrl_resp_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2opn_ctrl_resp_t;

//HUICOBUS_CMDID_cui_hcu2opn_data_report				= 0x0882,
typedef struct StrMsg_HUICOBUS_CMDID_cui_hcu2opn_data_report
{
	StrHuiIe_HUICOBUS_CMDID_cui_general_msg_head_t head;
	StrHlcIe_cui_hcu2opn_data_report_t hlc;
}StrMsg_HUICOBUS_CMDID_cui_hcu2opn_data_report_t;

//HUICOBUS_CMDID_cui_hcu2opn_max,

//HUICOBUS_CMDID_cui_invalid               = 0xFFFF, //无效

#pragma pack () //取消字节对齐
#endif /* _HUICOBUS_H_ */

