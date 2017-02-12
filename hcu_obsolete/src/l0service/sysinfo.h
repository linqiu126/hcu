/*
 * sysinfo.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L0SERVICE_SYSINFO_H_
#define L0SERVICE_SYSINFO_H_

#include "../l0comvm/vmlayer.h"

extern SysInfo_t gSysInfo;

UINT32 IsGprsPppdOk();
UINT32 IsNetItfExist(char *itfname);
void KillPppdProcess();
extern OPSTAT HcuGetSysInfo(SysInfo_t *si, bool mode);

#endif /* L0SERVICE_SYSINFO_H_ */
