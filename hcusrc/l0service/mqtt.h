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
#include <json-c/json.h>
#include <json-c/json_object.h>

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

//本消息区域，需要定义成自洽的，以便拷贝到其它项目中，省得各个项目之间相互包含
//GLOBAL CONST
#define MQTT_BROKER_ADDRESS     	"tcp://localhost:1883"
//MQTT的LEIENTID和TOPICID都不得超过MQTT_NAME_MAX_LEN
#define MQTT_NAME_MAX_LEN			30
enum MQTT_CLIENTID_LIST
{
	MQTT_CLID_MIN = 0,
	MQTT_CLID_HCU,
	MQTT_CLID_CTRL_UI,
	MQTT_CLID_QR_PRINTER,
	MQTT_CLID_DATABASE,
	MQTT_CLID_BH_PROTO,
	MQTT_CLID_MAX,
	MQTT_CLID_INVALID = 0xFFFFFFFF,
}; //end of MQTT_CLIENTID_LIST
#define MQTT_CLIENTID_MIN    		"MQTT_CLIENTID_MIN"
#define MQTT_CLIENTID_HCU    		"MQTT_CLIENTID_HCU"
#define MQTT_CLIENTID_CTRL_UI   	"MQTT_CLIENTID_CTRL_UI"
#define MQTT_CLIENTID_QR_PRINTER   	"MQTT_CLIENTID_QR_PRINTER"
#define MQTT_CLIENTID_DATABASE   	"MQTT_CLIENTID_DATABASE"
#define MQTT_CLIENTID_BH_PROTO   	"MQTT_CLIENTID_BH_PROTO"
#define MQTT_CLIENTID_MAX    		"MQTT_CLIENTID_MAX"
enum MQTT_TOPICID_LIST
{
	MQTT_TPID_MIN = 0,
	MQTT_TPID_UI_TO_HCU,
	MQTT_TPID_UI_FROM_HCU,
	MQTT_TPID_UI_QR_PRINT,
	MQTT_TPID_UI_DATABASE,
	MQTT_TPID_UI_BH_TRANS,
	MQTT_TPID_MAX,
	MQTT_TPID_INVALID = 0xFFFFFFFF,
}; //end of MQTT_TOPICID_LIST
#define MQTT_TOPIC_MIN       		"MQTT_TOPIC_MIN"
#define MQTT_TOPIC_UI_TO_HCU   		"MQTT_TOPIC_UI_TO_HCU"
#define MQTT_TOPIC_UI_FROM_HCU   	"MQTT_TOPIC_UI_FROM_HCU"
#define MQTT_TOPIC_QR_PRINT   		"MQTT_TOPIC_QR_PRINT"
#define MQTT_TOPIC_DATABASE     	"MQTT_TOPIC_DATABASE"
#define MQTT_TOPIC_BH_TRANS     	"TOPIC_BH_TRANS"
#define MQTT_TOPIC_MAX       		"MQTT_TOPIC_MAX"

#define MQTT_QOS_CONST         		1
#define MQTT_TIMEOUT_CONST     		10000L

//API
extern OPSTAT fsm_mqtt_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//GLOBAL API
extern int hcu_mqtt_msg_send_syn_mode(msg_struct_com_mqtt_send_t *in);
extern int hcu_mqtt_msg_send_asy_mode(msg_struct_com_mqtt_send_t *in);
extern int hcu_mqtt_msg_rcv(void);
extern OPSTAT hcu_mqtt_hate_data_send(void *context, char *topicName, int payloadLen, char *payload);

//Local APIs
void func_mqtt_msg_send_asy_delivered(void *context, MQTTClient_deliveryToken dt);
int  func_mqtt_msg_send_asy_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void func_mqtt_msg_send_asy_connlost(void *context, char *cause);
void func_mqtt_msg_rcv_delivered(void *context, MQTTClient_deliveryToken dt);
int  func_mqtt_msg_rcv_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void func_mqtt_msg_rcv_connlost(void *context, char *cause);
void func_mqtt_clientid_translate_to_text(UINT32 clId, char *output);
void func_mqtt_topicid_translate_to_text(UINT32 tpId, char *output);
UINT32 func_mqtt_clientid_translate_to_id(char *input);
UINT32 func_mqtt_topicid_translate_to_id(char *input);

#endif /* L0SERVICE_MQTT_H_ */


