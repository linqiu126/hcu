/*
 * mqtt.c
 *
 *  Created on: 2017年08月29日
 *      Author: ZHANG Jianlin
 */

#include "../l0service/mqtt.h"
#include "../l0service/trace.h"

/*
** FSM of the MQTT
*/
HcuFsmStateItem_t HcuFsmMqtt[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_mqtt_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_mqtt_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_mqtt_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_MQTT_ACTIVED,            		fsm_mqtt_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_MQTT_ACTIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_MQTT_ACTIVED,   				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_MQTT_ACTIVED,   				fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTab_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_mqtt_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_MQTT, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("MQTT: Error Set FSM State at fsm_timer_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_mqtt_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to HCU-MAIN
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);
		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_MQTT, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_MQTT, FSM_STATE_MQTT_INITED) == FAILURE){
		HcuErrorPrint("MQTT: Error Set FSM State!\n");
		return FAILURE;
	}

	//INIT this task global variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_MQTT] = 0;

	//进入等待反馈状态
	if (FsmSetState(TASK_ID_MQTT, FSM_STATE_MQTT_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_MQTT]++;
		HcuErrorPrint("MQTT: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("MQTT: Enter FSM_STATE_MQTT_ACTIVED status, Keeping loop of signal sending here!\n");
	}

	//正式进入无限循环等待工作
	if (hcu_mqtt_msg_rcv() != 0){
		HcuDebugPrint("MQTT: Exit MQTT receiving cycle. So far continue to work!\n");
		//exit(EXIT_FAILURE);
	}
	return SUCCESS;
}

OPSTAT fsm_mqtt_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("MQTT: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_mqtt_init(0, 0, NULL, 0);
	return SUCCESS;
}

//MQTT同步模式发送消息
int hcu_mqtt_msg_send_syn_mode(msg_struct_com_mqtt_send_t *in)
{
	//HATE测试环境
#ifdef HATE_TRIGGER_ENABLE
	msg_struct_l3hate_mqtt_frame_rcv_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3hate_mqtt_frame_rcv_t));
	memcpy(snd.dataBuf, in->jsonCont, strlen((char*)in->jsonCont));
	snd.bufValidLen = strlen((char*)in->jsonCont);
	snd.length = sizeof(msg_struct_l3hate_mqtt_frame_rcv_t);
	if (hcu_message_send(MSG_ID_MQTT_L3HATE_FRAME_RCV, TASK_ID_L3HATE, TASK_ID_MQTT, &snd, snd.length) == FAILURE){
		HcuErrorPrint("ETHERNET: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName);
		return FAILURE;
	}
	return SUCCESS;
#else

	MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    char input[1000];
    char topic[100];

    MQTTClient_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENTID_HCU, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        HcuErrorPrint("MQTT: Failed to connect, return code %d. So far set to continue work!\n", rc);
        //exit(EXIT_FAILURE);
    }

    //生成json字符串
    memset(input, 0, sizeof(input));
    struct json_object *jsonobj = NULL;
    jsonobj = json_object_new_object();
    //不能用简单的==NULL来判定
    //if (is_error(jsonobj))
    if (jsonobj == NULL)
    	HcuErrorPrint("MQTT: Failed to create json object!\n");
    //json_object_object_add(para_object, "MacAddr", json_object_new_string("AA:BB:CC:DD:EE:FF"));
    json_object_object_add(jsonobj, "CommandId", json_object_new_int(in->cmdId));
    json_object_object_add(jsonobj, "CommandValue", json_object_new_int(in->cmdValue));
    json_object_object_add(jsonobj, "HighLayerContent", json_object_new_string(in->jsonCont));
    sprintf(input, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pubmsg.payload = input;
    pubmsg.payloadlen = strlen(input);
    pubmsg.qos = MQTT_QOS_CONST;
    pubmsg.retained = 0;
    memset(topic, 0, sizeof(topic));
    func_mqtt_clientid_translate_to_text(in->topicId, topic);
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    HCU_DEBUG_PRINT_NOR("MQTT: Waiting for up to %d seconds for publication of %s\n, on topic %s for client with ClientID: %s\n", (int)(MQTT_TIMEOUT_CONST/1000), input, topic, MQTT_CLIENTID_HCU);
    rc = MQTTClient_waitForCompletion(client, token, MQTT_TIMEOUT_CONST);
    HCU_DEBUG_PRINT_NOR("MQTT: Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
#endif
}

/** @endcode
*
* @page pubasync Asynchronous publication example
@code{.c}*/
volatile MQTTClient_deliveryToken deliveredtoken_send;

void func_mqtt_msg_send_asy_delivered(void *context, MQTTClient_deliveryToken dt)
{
	HCU_DEBUG_PRINT_NOR("MQTT: Message with token value %d delivery confirmed\n", dt);
	deliveredtoken_send = dt;
}

int func_mqtt_msg_send_asy_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payloadptr;

  HCU_DEBUG_PRINT_NOR("MQTT: Message arrived\n");
  HCU_DEBUG_PRINT_NOR("MQTT: topic: %s\n", topicName);
  HCU_DEBUG_PRINT_NOR("MQTT: message: ");

  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++)
  {
      putchar(*payloadptr++);
  }
  putchar('\n');
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void func_mqtt_msg_send_asy_connlost(void *context, char *cause)
{
	HCU_DEBUG_PRINT_NOR("MQTT: \nConnection lost\n");
	HCU_DEBUG_PRINT_NOR("MQTT:      cause: %s\n", cause);
}

//MQTT异步模式发送消息
int hcu_mqtt_msg_send_asy_mode(msg_struct_com_mqtt_send_t *in)
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  int rc;
  char input[1000];
  char topic[100];

  MQTTClient_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENTID_HCU, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, func_mqtt_msg_send_asy_connlost, func_mqtt_msg_send_asy_msgarrvd, func_mqtt_msg_send_asy_delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
	  HcuErrorPrint("MQTT: Failed to connect, return code %d. So far continue to work!\n", rc);
      //exit(EXIT_FAILURE);
  }

  //生成json字符串
  memset(input, 0, sizeof(input));
  struct json_object *jsonobj = NULL;
  jsonobj = json_object_new_object();
  //不能用简单的==NULL来判定
  //if (is_error(jsonobj))
  if (jsonobj == NULL)
	  HcuErrorPrint("MQTT: Failed to create json object!\n");
  //json_object_object_add(para_object, "MacAddr", json_object_new_string("AA:BB:CC:DD:EE:FF"));
  json_object_object_add(jsonobj, "CommandId", json_object_new_int(in->cmdId));
  json_object_object_add(jsonobj, "CommandValue", json_object_new_int(in->cmdValue));
  json_object_object_add(jsonobj, "HighLayerContent", json_object_new_string(in->jsonCont));
  sprintf(input, "%s", json_object_to_json_string(jsonobj));
  json_object_put(jsonobj);//free
  pubmsg.payload = input;
  pubmsg.payloadlen = strlen(input);
  pubmsg.qos = MQTT_QOS_CONST;
  pubmsg.retained = 0;
  deliveredtoken_send = 0;

  memset(topic, 0, sizeof(topic));
  func_mqtt_clientid_translate_to_text(in->topicId, topic);
  MQTTClient_publishMessage(client, topic, &pubmsg, &token);
  HCU_DEBUG_PRINT_NOR("MQTT: Waiting for publication of %s\n, on topic %s for client with ClientID: %s\n", input, topic, MQTT_CLIENTID_HCU);
  while(deliveredtoken_send != token);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}

/*** @endcode
* @page subasync Asynchronous subscription example
@code*/
volatile MQTTClient_deliveryToken deliveredtoken_rcv;

void func_mqtt_msg_rcv_delivered(void *context, MQTTClient_deliveryToken dt)
{
  HCU_DEBUG_PRINT_NOR("MQTT: Message with token value %d delivery confirmed\n", dt);
  deliveredtoken_rcv = dt;
}

int func_mqtt_msg_rcv_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  //int i;
  //char* payloadptr;
  struct json_object *jsonobj = NULL;
  //struct json_object *result_jsonobj = NULL;
  struct json_object *cmdid_jsonobj = NULL;
  struct json_object *cmdval_jsonobj = NULL;
  struct json_object *cont_jsonobj = NULL;

  //打印输出
//  payloadptr = message->payload;
//  for(i=0; i<message->payloadlen; i++)
//  {
//      putchar(*payloadptr++);
//  }
//  putchar('\n');
  HCU_DEBUG_PRINT_NOR("MQTT: Message arrived, topic: %s, message: %s\n", topicName, message->payload);

  //上面这一段，未来将送到内部程序，并进行处理
  //也可以采取CallBack函数的形式，让内部任务自行处理具体的过程：传递消息内容以及长度
  msg_struct_com_mqtt_rcv_t snd;
  memset(&snd, 0, sizeof(msg_struct_com_mqtt_rcv_t));
  snd.srcId = func_mqtt_clientid_translate_to_id(context);
  snd.topicId = func_mqtt_topicid_translate_to_id(topicName);
  jsonobj = json_tokener_parse(message->payload);
  //不能用简单的==NULL来判定
  //if (is_error(jsonobj))
  if (jsonobj == NULL)
	  HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Failed to create json object!\n");
  cmdid_jsonobj = json_object_object_get(jsonobj, "CommandId");
  cmdval_jsonobj = json_object_object_get(jsonobj, "CommandValue");
  cont_jsonobj = json_object_object_get(jsonobj, "HighLayerContent");

  //解码具体数值
  if (cmdid_jsonobj != NULL){
	  snd.cmdId = (UINT32)(json_object_get_int(cmdid_jsonobj));
	  json_object_put(cmdid_jsonobj);
  }
  if (cmdval_jsonobj != NULL){
	  snd.cmdValue = (UINT32)(json_object_get_int(cmdval_jsonobj));
	  json_object_put(cmdval_jsonobj);
  }
  if (cont_jsonobj != NULL){
	  UINT32 t = strlen(json_object_get_string(cont_jsonobj));
	  strncpy(snd.jsonCont, json_object_get_string(cont_jsonobj), t<HCU_SYSMSG_MQTT_DESC_MAX_LEN?t:HCU_SYSMSG_MQTT_DESC_MAX_LEN);
	  json_object_put(cont_jsonobj);
  }
  json_object_put(jsonobj);

  //发送消息
  snd.length = sizeof(msg_struct_com_mqtt_rcv_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
  //发送命令给BFSCUICOMM
  if (hcu_message_send(MSG_ID_COM_MQTT_RCV, TASK_ID_BFSCUICOMM, TASK_ID_MQTT, &snd, snd.length) == FAILURE)
	HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
  //发送命令给L3AQYCG20
  if (hcu_message_send(MSG_ID_COM_MQTT_RCV, TASK_ID_L3AQYCG20, TASK_ID_MQTT, &snd, snd.length) == FAILURE)
	HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
//  //发送命令给BFSCUICOMM
//  if (hcu_message_send(MSG_ID_COM_MQTT_RCV, TASK_ID_BFSCUICOMM, TASK_ID_MQTT, &snd, snd.length) == FAILURE)
//	HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
//  //发送命令给BFSCUICOMM
//  if (hcu_message_send(MSG_ID_COM_MQTT_RCV, TASK_ID_BFSCUICOMM, TASK_ID_MQTT, &snd, snd.length) == FAILURE)
//	HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
#else
	#error Set project name id error!
#endif
  //Free Message
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void func_mqtt_msg_rcv_connlost(void *context, char *cause)
{
	HCU_DEBUG_PRINT_NOR("MQTT: Connection lost, cause: %s\n", cause);
}

//MQTT接收消息
int hcu_mqtt_msg_rcv(void)
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;

  MQTTClient_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENTID_HCU, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, func_mqtt_msg_rcv_connlost, func_mqtt_msg_rcv_msgarrvd, func_mqtt_msg_rcv_delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
      HcuErrorPrint("MQTT: Failed to connect, return code %d. So far set to continue work!\n", rc);
      //exit(EXIT_FAILURE);
  }
  HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", MQTT_TOPIC_UI2HCU, MQTT_CLIENTID_HCU, MQTT_QOS_CONST);
  HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", MQTT_TOPIC_BH_TRANS, MQTT_CLIENTID_HCU, MQTT_QOS_CONST);
  MQTTClient_subscribe(client, MQTT_TOPIC_UI2HCU, MQTT_QOS_CONST);
  MQTTClient_subscribe(client, MQTT_TOPIC_BH_TRANS, MQTT_QOS_CONST);

  //退出条件，未来待完善
  do
  {
	  hcu_usleep(100);
      ch = getchar();
  } while(ch!='Q' && ch != 'q');

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}

//CLIENTID to CONTEXT transfer
void func_mqtt_clientid_translate_to_text(UINT32 clId, char *output)
{
	if (clId <= MQTT_CLID_MIN) strncpy(output, MQTT_CLIENTID_MIN, strlen(MQTT_CLIENTID_MIN));
	else if (clId == MQTT_CLID_HCU) strncpy(output, MQTT_CLIENTID_HCU, strlen(MQTT_CLIENTID_HCU));
	else if (clId == MQTT_CLID_CTRL_UI) strncpy(output, MQTT_CLIENTID_CTRL_UI, strlen(MQTT_CLIENTID_CTRL_UI));
	else if (clId == MQTT_CLID_FINGER_UI) strncpy(output, MQTT_CLIENTID_FINGER_UI, strlen(MQTT_CLIENTID_FINGER_UI));
	else if (clId == MQTT_CLID_QR_PRINTER) strncpy(output, MQTT_CLIENTID_QR_PRINTER, strlen(MQTT_CLIENTID_QR_PRINTER));
	else if (clId == MQTT_CLID_DATABASE) strncpy(output, MQTT_CLIENTID_DATABASE, strlen(MQTT_CLIENTID_DATABASE));
	else if (clId == MQTT_CLID_BH_PROTO) strncpy(output, MQTT_CLIENTID_BH_PROTO, strlen(MQTT_CLIENTID_BH_PROTO));
	else strncpy(output, MQTT_CLIENTID_MAX, strlen(MQTT_CLIENTID_MAX));
}

UINT32 func_mqtt_clientid_translate_to_id(char *input)
{
	if (strcmp(input, MQTT_CLIENTID_MIN) == 0) return MQTT_CLID_MIN;
	else if (strcmp(input, MQTT_CLIENTID_HCU) == 0) return MQTT_CLID_HCU;
	else if (strcmp(input, MQTT_CLIENTID_CTRL_UI) == 0) return MQTT_CLID_CTRL_UI;
	else if (strcmp(input, MQTT_CLIENTID_FINGER_UI) == 0) return MQTT_CLID_FINGER_UI;
	else if (strcmp(input, MQTT_CLIENTID_QR_PRINTER) == 0) return MQTT_CLID_QR_PRINTER;
	else if (strcmp(input, MQTT_CLIENTID_DATABASE) == 0) return MQTT_CLID_DATABASE;
	else if (strcmp(input, MQTT_CLIENTID_BH_PROTO) == 0) return MQTT_CLID_BH_PROTO;
	else return MQTT_CLID_MAX;
}

//TOPICID to CONTEXT transfer
void func_mqtt_topicid_translate_to_text(UINT32 tpId, char *output)
{
	if (tpId <= MQTT_TPID_MIN) strncpy(output, MQTT_TOPIC_MIN, strlen(MQTT_TOPIC_MIN));
	else if (tpId == MQTT_TPID_UI2HCU) strncpy(output, MQTT_TOPIC_UI2HCU, strlen(MQTT_TOPIC_UI2HCU));
	else if (tpId == MQTT_TPID_HCU2UI) strncpy(output, MQTT_TOPIC_HCU2UI, strlen(MQTT_TOPIC_HCU2UI));
	else if (tpId == MQTT_TPID_CTRL2FINGER_UI) strncpy(output, MQTT_TOPIC_CTRL2FINGER_UI, strlen(MQTT_TOPIC_CTRL2FINGER_UI));
	else if (tpId == MQTT_TPID_QR_PRINT) strncpy(output, MQTT_TOPIC_QR_PRINT, strlen(MQTT_TOPIC_QR_PRINT));
	else if (tpId == MQTT_TPID_DATABASE) strncpy(output, MQTT_TOPIC_DATABASE, strlen(MQTT_TOPIC_DATABASE));
	else if (tpId == MQTT_TPID_BH_TRANS) strncpy(output, MQTT_TOPIC_BH_TRANS, strlen(MQTT_TOPIC_BH_TRANS));
	else strncpy(output, MQTT_TOPIC_MAX, strlen(MQTT_TOPIC_MAX));
}

UINT32 func_mqtt_topicid_translate_to_id(char *input)
{
	if (strcmp(input, MQTT_TOPIC_MIN) == 0) return MQTT_TPID_MIN;
	else if (strcmp(input, MQTT_TOPIC_UI2HCU) == 0) return MQTT_TPID_UI2HCU;
	else if (strcmp(input, MQTT_TOPIC_HCU2UI) == 0) return MQTT_TPID_HCU2UI;
	else if (strcmp(input, MQTT_TOPIC_CTRL2FINGER_UI) == 0) return MQTT_TPID_CTRL2FINGER_UI;
	else if (strcmp(input, MQTT_TOPIC_QR_PRINT) == 0) return MQTT_TPID_QR_PRINT;
	else if (strcmp(input, MQTT_TOPIC_DATABASE) == 0) return MQTT_TPID_DATABASE;
	else if (strcmp(input, MQTT_TOPIC_BH_TRANS) == 0) return MQTT_TPID_BH_TRANS;
	else return MQTT_TPID_MAX;
}

//HATE测试环境
OPSTAT hcu_mqtt_hate_data_send(void *context, char *topicName, int payloadLen, char *payload)
{
	MQTTClient_message message;
	message.payloadlen = payloadLen;
	memcpy(message.payload, payload, message.payloadlen);
	return func_mqtt_msg_rcv_msgarrvd(context, topicName, payloadLen, &message);
}




