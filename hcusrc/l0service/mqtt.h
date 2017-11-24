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
	MQTT_CLID_HCUENTRY,		//工控机主题控制器
	MQTT_CLID_UIROUTER,		//PHP为基础的控制界面业务逻辑
	MQTT_CLID_UIPRESENT,	//触屏UI界面
	MQTT_CLID_QRPRINTER,	//打印机
	MQTT_CLID_DBRESTFUL,		//数据库
	MQTT_CLID_BHPROTO,		//回传协议
	MQTT_CLID_LOGERR,		//差错log
	MQTT_CLID_LOGTRACE,		//调测打印log
	MQTT_CLID_OPRNODE,		//操控控制节点，即为控制服务器
	MQTT_CLID_MAX,
	MQTT_CLID_INVALID = 0xFFFFFFFF,
}; //end of MQTT_CLIENTID_LIST
#define MQTT_CLIENTID_MIN    		"MQTT_CLIENTID_MIN"
#define MQTT_CLIENTID_HCUENTRY    	"MQTT_CLIENTID_HCUENTRY"
#define MQTT_CLIENTID_UIROUTER   	"MQTT_CLIENTID_UIROUTER"
#define MQTT_CLIENTID_UIPRESENT   	"MQTT_CLIENTID_UIPRESENT"
#define MQTT_CLIENTID_QRPRINTER   	"MQTT_CLIENTID_QRPRINTER"
#define MQTT_CLIENTID_DBRESTFUL   	"MQTT_CLIENTID_DBRESTFUL"
#define MQTT_CLIENTID_BHPROTO   	"MQTT_CLIENTID_BHPROTO"
#define MQTT_CLIENTID_LOGERR   	    "MQTT_CLIENTID_LOGERR"
#define MQTT_CLIENTID_LOGTRACE   	"MQTT_CLIENTID_LOGTRACE"
#define MQTT_CLIENTID_OPRNODE   	"MQTT_CLIENTID_OPRNODE"
#define MQTT_CLIENTID_MAX    		"MQTT_CLIENTID_MAX"
enum MQTT_TOPICID_LIST
{
	MQTT_TPID_MIN = 0,
	MQTT_TPID_HCU2UIR,
	MQTT_TPID_UIR2HCU,
	MQTT_TPID_UIR2UIP,
	MQTT_TPID_PRINTFLOW,
	MQTT_TPID_DBACCESS,
	MQTT_TPID_DBFEEDBACK,
	MQTT_TPID_BHTRANS,
	MQTT_TPID_LOGERRFLOW,
	MQTT_TPID_LOGTRACEFLOW,
	MQTT_TPID_HCU2OPN,
	MQTT_TPID_OPN2HCU,
	MQTT_TPID_MAX,
	MQTT_TPID_INVALID = 0xFFFFFFFF,
}; //end of MQTT_TOPICID_LIST
#define MQTT_TOPIC_MIN       		"MQTT_TOPIC_MIN"
#define MQTT_TOPIC_HCU2UIR   		"MQTT_TOPIC_HCU2UIR"
#define MQTT_TOPIC_UIR2HCU   		"MQTT_TOPIC_UIR2HCU"
#define MQTT_TOPIC_UIR2UIP    		"MQTT_TOPIC_UIR2UIP"
#define MQTT_TOPIC_PRINTFLOW   		"MQTT_TOPIC_PRINTFLOW"
#define MQTT_TOPIC_DBACCESS     	"MQTT_TOPIC_DBACCESS"
#define MQTT_TOPIC_DBFEEDBACK     	"MQTT_TOPIC_DBFEEDBACK"
#define MQTT_TOPIC_BHTRANS          "MQTT_TOPIC_BHTRANS"
#define MQTT_TOPIC_LOGERRFLOW     	"MQTT_TOPIC_LOGERRFLOW"
#define MQTT_TOPIC_LOGTRACEFLOW     "MQTT_TOPIC_LOGTRACEFLOW"
#define MQTT_TOPIC_HCU2OPN     		"MQTT_TOPIC_HCU2OPN"
#define MQTT_TOPIC_OPN2HCU     		"MQTT_TOPIC_OPN2HCU"
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


