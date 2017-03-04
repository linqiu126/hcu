/*
 * comtype.h
 *
 *  Created on: 2016年1月6日
 *      Author: test
 */

#ifndef L0COMVM_COMTYPE_H_
#define L0COMVM_COMTYPE_H_

#include <netdb.h>

/*1. 公共结构名定义
** Date types used in this project.
*/
typedef unsigned char   		UINT8;
typedef signed char     		INT8;
typedef char            		CHAR;           //only for character array
typedef unsigned short  		UINT16;
typedef signed short    		INT16;
typedef unsigned int    		UINT32;
typedef	signed int      		INT32;  		//redefined at system lib jpeglib.h
typedef unsigned long int 		UINT64;
typedef unsigned long long int  LLP64;   //defined by shanchun for HJT
typedef signed long int 		INT64;
typedef unsigned long   		ULONG;
typedef signed long     		SLONG;
typedef UINT8           		BOOLEAN;          //defined for RP
typedef void            		VOID;
typedef BOOLEAN					bool;
typedef BOOLEAN					BOOL;
typedef uint16_t 				ke_task_id_t;
typedef uint16_t 				ke_msg_id_t;
typedef uint16_t 				ke_state_t;


/*
** Function return value used in the project.
** Only two possible values for OPSTAT return type:
** 1. SUCCESS
** 0. FAILURE
*/
typedef UINT32         OPSTAT;
#define OPRSUCC        1
#define SUCCESS        0
#define FAILURE        -1
#define true   			1
#define false			0

#define 	TRUE 		1
#define 	FALSE 		0

/* For thread creation */
typedef void *(*CALLBACK)(void *);

/*
** Definition of invalid values used in the project.
*/
#define  	HCU_INVALID_U32					0xffffffff
#define  	HCU_INVALID_U16					0xffff
#define  	HCU_INVALID_U8					0xff
#define 	HCU_LOGIC_TRUE					0x1
#define 	HCU_LOGIC_FALSE					0x0
#define 	HCU_INVALID_PARAMETER			0xffffffff
#define 	HCU_MEASURE_RECEIVED			0x0
#define 	HCU_COMMAND_RECEIVED			0x1
#define 	HCU_MEASURE_COMMAND_RECEIVED	0x2


#endif /* L0COMVM_COMTYPE_H_ */
