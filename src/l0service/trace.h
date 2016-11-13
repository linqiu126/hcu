/*
 * trace.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L0SERVICE_TRACE_H_
#define L0SERVICE_TRACE_H_

#include "../l0comvm/vmlayer.h"

/*
 * Trace and Debug Interface
 */
extern void TraceIdPrintf(char * fileName, UINT32 lineNum);
extern void TracePrintf(char * format, ...);
extern void ErrorTracePrintf(char * format, ...);
extern void TraceInit();

char *GetFileName( char *s);
void GetCurTimeWithMs( char* );

#define HcuDebugPrint \
TraceIdPrintf(__FILE__, __LINE__), \
TracePrintf

#define HcuErrorPrint \
TraceIdPrintf(__FILE__, __LINE__), \
ErrorTracePrintf


#endif /* L0SERVICE_TRACE_H_ */
