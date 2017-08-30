/*
 * mqtt.h
 *
 *  Created on: 2017年08月29日
 *      Author: ZHANG Jianlin
 */

#ifndef L0SERVICE_MQTT_H_
#define L0SERVICE_MQTT_H_

#include "../l0comvm/vmlayer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_MQTT
{
	FSM_STATE_MQTT_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_MQTT_ACTIVED,
	FSM_STATE_MQTT_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmMqtt[];

//GLOBAL CONST
#define MQTT_BROKER_ADDRESS     	"tcp://localhost:1883"

#define MQTT_CLIENTID_HCU    		"MQTT_CLIENTID_HCU"
#define MQTT_CLIENTID_CTRL_UI   	"MQTT_CLIENTID_CTRL_UI"
#define MQTT_CLIENTID_QR_PRINTER   	"MQTT_CLIENTID_QR_PRINTER"
#define MQTT_CLIENTID_DATABASE   	"MQTT_CLIENTID_DATABASE"
#define MQTT_CLIENTID_BH_PROTO   	"MQTT_CLIENTID_BH_PROTO"

#define MQTT_TOPIC_UI_TO_HCU   		"MQTT_TOPIC_UI_TO_HCU"
#define MQTT_TOPIC_UI_FROM_HCU   	"MQTT_TOPIC_UI_FROM_HCU"
#define MQTT_TOPIC_QR_PRINT   		"MQTT_TOPIC_QR_PRINT"
#define MQTT_TOPIC_DATABASE     	"MQTT_TOPIC_DATABASE"
#define MQTT_TOPIC_BH_TRANS     	"TOPIC_BH_TRANS"

#define MQTT_QOS_CONST         		1
#define MQTT_TIMEOUT_CONST     		10000L

//API
extern OPSTAT fsm_mqtt_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//GLOBAL API
extern int hcu_mqtt_msg_send_syn_mode(char *topic, char *input);
extern int hcu_mqtt_msg_send_asy_mode(char *topic, char *input);
extern int hcu_mqtt_msg_rcv(void);

//Local APIs
void func_mqtt_msg_send_asy_delivered(void *context, MQTTClient_deliveryToken dt);
int  func_mqtt_msg_send_asy_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void func_mqtt_msg_send_asy_connlost(void *context, char *cause);
void func_mqtt_msg_rcv_delivered(void *context, MQTTClient_deliveryToken dt);
int  func_mqtt_msg_rcv_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void func_mqtt_msg_rcv_connlost(void *context, char *cause);


#endif /* L0SERVICE_MQTT_H_ */


