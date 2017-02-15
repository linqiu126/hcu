/*
 * syspm.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SYSPM_H_
#define L3APP_SYSPM_H_

#include "../l0comvm/vmlayer.h"
#include <time.h>

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_SYSPM
{
	FSM_STATE_SYSPM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_SYSPM_ACTIVED,
	FSM_STATE_SYSPM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

typedef struct PmCpuOccupyInfo         //定义一个cpu occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	UINT32 user; //定义一个无符号的int类型的user
	UINT32 nice; //定义一个无符号的int类型的nice
	UINT32 system;//定义一个无符号的int类型的system
	UINT32 idle; //定义一个无符号的int类型的idle
}PmCpuOccupyInfo_t;

typedef struct PmMemOccupyInfo         //定义一个mem occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	UINT32 total;
	char name2[20];
	UINT32 used;
}PmMemOccupyInfo_t;
/*
const unsigned N = 10000;
typedef struct PmCpuTempInfo         //定义一个cpu temp的结构体
{

	struct timespec t;
	unsigned temp;

}PmCpuTempInfo_t;
*/

//Global variables
extern HcuFsmStateItem_t HcuFsmSyspm[];

typedef struct gTaskSyspmContext
{
	PmCpuOccupyInfo_t cpu;
	PmMemOccupyInfo_t mem;
}gTaskSyspmContext_t;


//API
extern OPSTAT fsm_syspm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_cloudvela_alarm_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_cloudvela_alarm_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_cloudvela_perfm_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_cloudvela_perfm_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_com_alarm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_syspm_com_pm_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_syspm_int_init(void);
void func_syspm_cal_cpu_mem_disk_occupy(void);
void func_syspm_get_memoccupy (PmMemOccupyInfo_t *mem);
void func_syspm_get_cpuoccupy (PmCpuOccupyInfo_t *cpust);
UINT32 func_syspm_cal_cpuoccupy (PmCpuOccupyInfo_t *o, PmCpuOccupyInfo_t *n);
void func_syspm_get_diskoccupy(void);
void func_syspm_get_cpu_temp(void);

#endif /* L3APP_SYSPM_H_ */
