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
#define HUICOBUS_MQTT_BROKER_ADDRESS     	"tcp://localhost:1883"
//MQTT的LEIENTID和TOPICID都不得超过MQTT_NAME_MAX_LEN
#define HUICOBUS_MQTT_NAME_MAX_LEN			30

//NODEID
enum STR_HUICOBUS_MQTT_NODEID_LIST
{
	HUICOBUS_MQTT_NBID_MIN = 0,
	HUICOBUS_MQTT_NBID_LOCALHOST,		//本机
	HUICOBUS_MQTT_NBID_FACSERVER,		//本地服务器
	HUICOBUS_MQTT_NBID_REMSERVER,		//远程服务器
	HUICOBUS_MQTT_NBID_MATINC,		//进料控制机
	HUICOBUS_MQTT_NBID_MAX,
	HUICOBUS_MQTT_NBID_INVALID = 0xFFFFFFFF,
}STR_HUICOBUS_MQTT_NODEID_LIST_T;
#define HUICOBUS_MQTT_NODEID_MIN    		"HUICOBUS_MQTT_NODEID_MIN"
#define HUICOBUS_MQTT_NODEID_LOCALHOST    	"HUICOBUS_MQTT_NODEID_LOCALHOST"
#define HUICOBUS_MQTT_NODEID_FACSERVER   	"HUICOBUS_MQTT_NODEID_FACSERVER"
#define HUICOBUS_MQTT_NODEID_MATINC    		"HUICOBUS_MQTT_NODEID_MATINC"
#define HUICOBUS_MQTT_NODEID_MAX    		"HUICOBUS_MQTT_NODEID_MAX"

//CLIENT ID
enum STR_HUICOBUS_MQTT_CLIENTID_LIST
{
	HUICOBUS_MQTT_CLID_MIN = 0,
	HUICOBUS_MQTT_CLID_HCUENTRY,		//工控机主题控制器
	HUICOBUS_MQTT_CLID_UIROUTER,		//PHP为基础的控制界面业务逻辑
	HUICOBUS_MQTT_CLID_UIPRESENT,		//触屏UI界面
	HUICOBUS_MQTT_CLID_QRPRINTER,		//打印机
	HUICOBUS_MQTT_CLID_DBRESTFUL,		//数据库
	HUICOBUS_MQTT_CLID_BHPROTO,			//回传协议
	HUICOBUS_MQTT_CLID_LOGERR,			//差错log
	HUICOBUS_MQTT_CLID_LOGTRACE,		//调测打印log
	HUICOBUS_MQTT_CLID_OPRNODE,			//操控控制节点，即为控制服务器
	HUICOBUS_MQTT_CLID_MAX,
	HUICOBUS_MQTT_CLID_INVALID = 0xFFFFFFFF,
}STR_HUICOBUS_MQTT_CLIENTID_LIST_T; //end of HUICOBUS_MQTT_CLIENTID_LIST
#define HUICOBUS_MQTT_CLIENTID_MIN    		"HUICOBUS_MQTT_CLIENTID_MIN"
#define HUICOBUS_MQTT_CLIENTID_HCUENTRY    	"HUICOBUS_MQTT_CLIENTID_HCUENTRY"
#define HUICOBUS_MQTT_CLIENTID_UIROUTER   	"HUICOBUS_MQTT_CLIENTID_UIROUTER"
#define HUICOBUS_MQTT_CLIENTID_UIPRESENT   	"HUICOBUS_MQTT_CLIENTID_UIPRESENT"
#define HUICOBUS_MQTT_CLIENTID_QRPRINTER   	"HUICOBUS_MQTT_CLIENTID_QRPRINTER"
#define HUICOBUS_MQTT_CLIENTID_DBRESTFUL   	"HUICOBUS_MQTT_CLIENTID_DBRESTFUL"
#define HUICOBUS_MQTT_CLIENTID_BHPROTO   	"HUICOBUS_MQTT_CLIENTID_BHPROTO"
#define HUICOBUS_MQTT_CLIENTID_LOGERR   	"HUICOBUS_MQTT_CLIENTID_LOGERR"
#define HUICOBUS_MQTT_CLIENTID_LOGTRACE   	"HUICOBUS_MQTT_CLIENTID_LOGTRACE"
#define HUICOBUS_MQTT_CLIENTID_OPRNODE   	"HUICOBUS_MQTT_CLIENTID_OPRNODE"
#define HUICOBUS_MQTT_CLIENTID_MAX    		"HUICOBUS_MQTT_CLIENTID_MAX"

//TOPIC ID
enum HUICOBUS_MQTT_TOPICID_LIST
{
	HUICOBUS_MQTT_TPID_MIN = 0,
	HUICOBUS_MQTT_TPID_HCU2UIR,
	HUICOBUS_MQTT_TPID_UIR2HCU,
	HUICOBUS_MQTT_TPID_UIR2UIP,
	HUICOBUS_MQTT_TPID_PRINTFLOW,
	HUICOBUS_MQTT_TPID_DBACCESS,
	HUICOBUS_MQTT_TPID_DBFEEDBACK,
	HUICOBUS_MQTT_TPID_BHTRANS,
	HUICOBUS_MQTT_TPID_LOGERRFLOW,
	HUICOBUS_MQTT_TPID_LOGTRACEFLOW,
	HUICOBUS_MQTT_TPID_HCU2OPN,
	HUICOBUS_MQTT_TPID_OPN2HCU,
	HUICOBUS_MQTT_TPID_MAX,
	HUICOBUS_MQTT_TPID_INVALID = 0xFFFFFFFF,
}STR_HUICOBUS_MQTT_TOPICID_LIST_T; //end of MQTT_TOPICID_LIST
#define HUICOBUS_MQTT_TOPIC_MIN       		"HUICOBUS_MQTT_TOPIC_MIN"
#define HUICOBUS_MQTT_TOPIC_HCU2UIR   		"HUICOBUS_MQTT_TOPIC_HCU2UIR"
#define HUICOBUS_MQTT_TOPIC_UIR2HCU   		"HUICOBUS_MQTT_TOPIC_UIR2HCU"
#define HUICOBUS_MQTT_TOPIC_UIR2UIP    		"HUICOBUS_MQTT_TOPIC_UIR2UIP"
#define HUICOBUS_MQTT_TOPIC_PRINTFLOW   	"HUICOBUS_MQTT_TOPIC_PRINTFLOW"
#define HUICOBUS_MQTT_TOPIC_DBACCESS     	"HUICOBUS_MQTT_TOPIC_DBACCESS"
#define HUICOBUS_MQTT_TOPIC_DBFEEDBACK     	"HUICOBUS_MQTT_TOPIC_DBFEEDBACK"
#define HUICOBUS_MQTT_TOPIC_BHTRANS         "HUICOBUS_MQTT_TOPIC_BHTRANS"
#define HUICOBUS_MQTT_TOPIC_LOGERRFLOW     	"HUICOBUS_MQTT_TOPIC_LOGERRFLOW"
#define HUICOBUS_MQTT_TOPIC_LOGTRACEFLOW    "HUICOBUS_MQTT_TOPIC_LOGTRACEFLOW"
#define HUICOBUS_MQTT_TOPIC_HCU2OPN     	"HUICOBUS_MQTT_TOPIC_HCU2OPN"
#define HUICOBUS_MQTT_TOPIC_OPN2HCU     	"HUICOBUS_MQTT_TOPIC_OPN2HCU"
#define HUICOBUS_MQTT_TOPIC_MAX       		"HUICOBUS_MQTT_TOPIC_MAX"

#define HUICOBUS_MQTT_QOS_CONST         		1
#define HUICOBUS_MQTT_TIMEOUT_CONST     		10000L

//MQTT全局变量
typedef struct gTaskMqttContext
{
	MQTTClient 					gclient;
    MQTTClient_deliveryToken 	gtoken;
    //MQTTClient_connectOptions 	gconn_opts;
    //MQTTClient_message 			gpubmsg;
}gTaskMqttContext_t;

//API
extern OPSTAT fsm_mqtt_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_mqtt_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//GLOBAL API
extern int hcu_mqtt_msg_send_syn_mode(msg_struct_com_mqtt_send_t *in);  			//持久连接模式
extern int hcu_mqtt_msg_send_syn_by_single_mode(msg_struct_com_mqtt_send_t *in); 	//单次连接并释放模式
extern int hcu_mqtt_msg_send_asy_mode(msg_struct_com_mqtt_send_t *in);  			//持久连接模式
extern int hcu_mqtt_msg_send_asy_by_single_mode(msg_struct_com_mqtt_send_t *in);  	//单次连接并释放模式
extern int hcu_mqtt_msg_send_asy_mode(msg_struct_com_mqtt_send_t *in);
extern int hcu_mqtt_msg_rcv(void);
extern OPSTAT hcu_mqtt_hate_data_send(void *context, char *topicName, int payloadLen, char *payload);

//Local APIs
int  func_mqtt_msg_link_setup_by_send_syn_mode(void);
int  func_mqtt_msg_link_setup_by_send_asy_mode(void);
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
void func_mqtt_nodeid_translate_to_text(UINT32 nodeId, char *output);
UINT32 func_mqtt_nodeid_translate_to_id(char *input);

#endif /* L0SERVICE_MQTT_H_ */


