/*
 * hcu.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef HCU_H_
#define HCU_H_

#include "../l0comvm/vmlayer.h"

void hcu_working_mode_single(void);
void hcu_working_mode_double(void);
void hcu_working_mode_multipy(void);

void hcu_process_1_mainapp_single(void);
void hcu_process_2_mainapp_double(void);
void hcu_process_23_sensor_avorion(void);
void hcu_process_3_entry_supervisor(void);
void hcu_process_3_mainapp_multipy(void);

void  hcu_app_system_init();
UINT32 hcu_system_task_init(void);
UINT32 hcu_system_task_init_call(UINT32 task_id, FsmStateItem_t *p);
void hcu_task_send_init_to_timer(void);
void hcu_task_send_init_to_avorion(void);
void hcu_task_send_init_to_svrcon(void);
void hcu_task_send_hb_to_svrcon(void);

void hcu_task_create_all(void);
void hcu_task_create_all_but_avorion(void);
void hcu_task_create_hcumain_env(void);

void hcu_task_delete_all_and_queue(void);
void hcu_task_delete_except_avorion_and_hcumain(void);
void hcu_task_delete_except_timer_and_hcumain(void);
void hcu_task_delete_except_svrcon_and_hcumain(void);

//Global variables
extern HcuCurrentTaskTag_t zHcuCurrentProcessInfo;

#endif /* HCU_H_ */
