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

/*
JSON ==>
	srcNode: 字符串
	destNode: 字符串
	srcId: 字符串
	destId: 字符串
	topicId: 字符串
	cmdId: 数字
	cmdValie: 数字
	hlContent: 字符串
	未来可考虑将srcNode/destNode改造为字符串
*/

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
	memcpy(snd.dataBuf, in->hlContent, strlen((char*)in->hlContent));
	snd.bufValidLen = strlen((char*)in->hlContent);
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
    char stmp[100];

    MQTTClient_create(&client, HUICOBUS_MQTT_BROKER_ADDRESS, HUICOBUS_MQTT_CLIENTID_HCUENTRY, MQTTCLIENT_PERSISTENCE_NONE, NULL);
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
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Failed to create json object!\n");

    //json_object_object_add(para_object, "MacAddr", json_object_new_string("AA:BB:CC:DD:EE:FF"));
    memset(stmp, 0, sizeof(stmp));
    func_mqtt_nodeid_translate_to_text(in->srcNode, stmp);
    json_object_object_add(jsonobj, "srcNode", json_object_new_string(stmp));

    memset(stmp, 0, sizeof(stmp));
    func_mqtt_nodeid_translate_to_text(in->destNode, stmp);
    json_object_object_add(jsonobj, "destNode", json_object_new_string(stmp));

    memset(stmp, 0, sizeof(stmp));
    func_mqtt_clientid_translate_to_text(in->srcId, stmp);
    json_object_object_add(jsonobj, "srcId", json_object_new_string(stmp));

    memset(stmp, 0, sizeof(stmp));
    func_mqtt_clientid_translate_to_text(in->destId, stmp);
    json_object_object_add(jsonobj, "destId", json_object_new_string(stmp));

    memset(stmp, 0, sizeof(stmp));
    func_mqtt_topicid_translate_to_text(in->topicId, stmp);
    json_object_object_add(jsonobj, "topicId", json_object_new_string(stmp));

    json_object_object_add(jsonobj, "cmdId", json_object_new_int(in->cmdId));
    json_object_object_add(jsonobj, "cmdValue", json_object_new_int(in->cmdValue));

    if (in->hlcLen > 0) json_object_object_add(jsonobj, "hlContent", json_object_new_string(in->hlContent));

    sprintf(input, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pubmsg.payload = input;
    pubmsg.payloadlen = strlen(input);
    pubmsg.qos = HUICOBUS_MQTT_QOS_CONST;
    pubmsg.retained = 0;
    memset(topic, 0, sizeof(topic));
    func_mqtt_topicid_translate_to_text(in->topicId, topic);
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    HCU_DEBUG_PRINT_NOR("MQTT: Waiting for up to %d seconds for publication of %s\n, on topic %s for client with ClientID: %s\n", (int)(HUICOBUS_MQTT_TIMEOUT_CONST/1000), input, topic, HUICOBUS_MQTT_CLIENTID_HCUENTRY);
    rc = MQTTClient_waitForCompletion(client, token, HUICOBUS_MQTT_TIMEOUT_CONST);
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
	char stmp[100];

	MQTTClient_create(&client, HUICOBUS_MQTT_BROKER_ADDRESS, HUICOBUS_MQTT_CLIENTID_HCUENTRY, MQTTCLIENT_PERSISTENCE_NONE, NULL);
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
	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Failed to create json object!\n");

	//json_object_object_add(para_object, "MacAddr", json_object_new_string("AA:BB:CC:DD:EE:FF"));
	memset(stmp, 0, sizeof(stmp));
	func_mqtt_nodeid_translate_to_text(in->srcNode, stmp);
	json_object_object_add(jsonobj, "srcNode", json_object_new_string(stmp));

	memset(stmp, 0, sizeof(stmp));
	func_mqtt_nodeid_translate_to_text(in->destNode, stmp);
	json_object_object_add(jsonobj, "destNode", json_object_new_string(stmp));

	memset(stmp, 0, sizeof(stmp));
	func_mqtt_clientid_translate_to_text(in->srcId, stmp);
	json_object_object_add(jsonobj, "srcId", json_object_new_string(stmp));

	memset(stmp, 0, sizeof(stmp));
	func_mqtt_clientid_translate_to_text(in->destId, stmp);
	json_object_object_add(jsonobj, "destId", json_object_new_string(stmp));

	memset(stmp, 0, sizeof(stmp));
	func_mqtt_topicid_translate_to_text(in->topicId, stmp);
	json_object_object_add(jsonobj, "topicId", json_object_new_string(stmp));

	json_object_object_add(jsonobj, "cmdId", json_object_new_int(in->cmdId));
	json_object_object_add(jsonobj, "cmdValue", json_object_new_int(in->cmdValue));

	if (in->hlcLen > 0) json_object_object_add(jsonobj, "hlContent", json_object_new_string(in->hlContent));

	sprintf(input, "%s", json_object_to_json_string(jsonobj));
	json_object_put(jsonobj);//free
	pubmsg.payload = input;
	pubmsg.payloadlen = strlen(input);
	pubmsg.qos = HUICOBUS_MQTT_QOS_CONST;
	pubmsg.retained = 0;
	deliveredtoken_send = 0;

	memset(topic, 0, sizeof(topic));
	func_mqtt_clientid_translate_to_text(in->topicId, topic);
	MQTTClient_publishMessage(client, topic, &pubmsg, &token);
	HCU_DEBUG_PRINT_NOR("MQTT: Waiting for publication of %s\n, on topic %s for client with ClientID: %s\n", input, topic, HUICOBUS_MQTT_CLIENTID_HCUENTRY);
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
	UINT32 t;
	struct json_object *jsonobj = NULL;
	struct json_object *srcNode_jsonobj = NULL;
	struct json_object *destNode_jsonobj = NULL;
	struct json_object *srcId_jsonobj = NULL;
	struct json_object *destId_jsonobj = NULL;
	struct json_object *topicId_jsonobj = NULL;
	struct json_object *cmdId_jsonobj = NULL;
	struct json_object *cmdVal_jsonobj = NULL;
	struct json_object *cont_jsonobj = NULL;
	char stmp[100];

	//打印输出
	//  payloadptr = message->payload;
	//  for(i=0; i<message->payloadlen; i++)
	//  {
	//      putchar(*payloadptr++);
	//  }
	//  putchar('\n');
	HCU_DEBUG_PRINT_FAT("MQTT: Message arrived, topic: %s, message: %s\n", topicName, message->payload);

	//上面这一段，未来将送到内部程序，并进行处理
	//也可以采取CallBack函数的形式，让内部任务自行处理具体的过程：传递消息内容以及长度
	msg_struct_com_mqtt_rcv_t snd;
	memset(&snd, 0, sizeof(msg_struct_com_mqtt_rcv_t));
	//发送的srcId，不能用context做解码，否则崩溃
	//snd.srcId = func_mqtt_clientid_translate_to_id(context);
	snd.topicId = func_mqtt_topicid_translate_to_id(topicName);
	jsonobj = json_tokener_parse(message->payload);
	//不能用简单的==NULL来判定
	//if (is_error(jsonobj))
	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Failed to create json object!\n");

	srcNode_jsonobj = json_object_object_get(jsonobj, "srcNode");
	destNode_jsonobj = json_object_object_get(jsonobj, "destNode");
	srcId_jsonobj = json_object_object_get(jsonobj, "srcId");
	destId_jsonobj = json_object_object_get(jsonobj, "destId");
	topicId_jsonobj = json_object_object_get(jsonobj, "topicId");
	cmdId_jsonobj = json_object_object_get(jsonobj, "cmdId");
	cmdVal_jsonobj = json_object_object_get(jsonobj, "cmdValue");
	cont_jsonobj = json_object_object_get(jsonobj, "hlContent");

	//解码具体数值
	//SRCNODE
	if (srcNode_jsonobj != NULL){
		t = strlen(json_object_get_string(srcNode_jsonobj));
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, json_object_get_string(srcNode_jsonobj), t<100?t:100);
		json_object_put(srcNode_jsonobj);
		snd.srcNode = (UINT32)func_mqtt_nodeid_translate_to_id(stmp);
	}

	//DESTNODE
	if (destNode_jsonobj != NULL){
		t = strlen(json_object_get_string(destNode_jsonobj));
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, json_object_get_string(destNode_jsonobj), t<100?t:100);
		json_object_put(destNode_jsonobj);
		snd.destNode = (UINT32)func_mqtt_nodeid_translate_to_id(stmp);
	}

	//SRCID复用了系统功能
	if (srcId_jsonobj != NULL){
		snd.srcId = (UINT32)(json_object_get_int(srcId_jsonobj));
		json_object_put(srcId_jsonobj);
	}
	if (srcId_jsonobj != NULL) json_object_put(srcId_jsonobj);

	//DESTID
	if (destId_jsonobj != NULL){
		t = strlen(json_object_get_string(destId_jsonobj));
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, json_object_get_string(destId_jsonobj), t<100?t:100);
		json_object_put(destId_jsonobj);
		snd.destId = (UINT32)func_mqtt_clientid_translate_to_id(stmp);
	}

	//TOPIC复用了系统功能
	//  if (topicId_jsonobj != NULL){
	//	  snd.destId = (UINT32)(json_object_get_int(topicId_jsonobj));
	//	  json_object_put(topicId_jsonobj);
	//  }
	json_object_put(topicId_jsonobj);

	//CMDID
	if (cmdId_jsonobj != NULL){
		snd.cmdId = (UINT32)(json_object_get_int(cmdId_jsonobj));
		json_object_put(cmdId_jsonobj);
	}

	//CMDVAL
	if (cmdVal_jsonobj != NULL){
		snd.cmdValue = (INT32)(json_object_get_int(cmdVal_jsonobj));
		json_object_put(cmdVal_jsonobj);
	}

	//HLC
	if (cont_jsonobj != NULL){
		t = strlen(json_object_get_string(cont_jsonobj));
		strncpy(snd.hlContent, json_object_get_string(cont_jsonobj), t<HCU_SYSMSG_MQTT_DESC_MAX_LEN?t:HCU_SYSMSG_MQTT_DESC_MAX_LEN);
		json_object_put(cont_jsonobj);
	}
	json_object_put(jsonobj);
	snd.hlcLen = strlen(snd.hlContent);

	//发送消息
	snd.length = sizeof(msg_struct_com_mqtt_rcv_t);
	//发送命令给BFHSUICOMM
	if (hcu_message_send(MSG_ID_COM_MQTT_RCV, TASK_ID_HUICOBUSCODEC, TASK_ID_MQTT, &snd, snd.length) == FAILURE)
	HCU_ERROR_PRINT_TASK(TASK_ID_MQTT, "MQTT: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MQTT].taskName, zHcuVmCtrTab.task[TASK_ID_HUICOBUSCODEC].taskName);
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

	MQTTClient_create(&client, HUICOBUS_MQTT_BROKER_ADDRESS, HUICOBUS_MQTT_CLIENTID_HCUENTRY, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client, NULL, func_mqtt_msg_rcv_connlost, func_mqtt_msg_rcv_msgarrvd, func_mqtt_msg_rcv_delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		HcuErrorPrint("MQTT: Failed to connect, return code %d. So far set to continue work!\n", rc);
		//exit(EXIT_FAILURE);
	}
	HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", HUICOBUS_MQTT_TOPIC_HCU2UIR, HUICOBUS_MQTT_CLIENTID_HCUENTRY, HUICOBUS_MQTT_QOS_CONST);
	HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", HUICOBUS_MQTT_TOPIC_BHTRANS, HUICOBUS_MQTT_CLIENTID_HCUENTRY, HUICOBUS_MQTT_QOS_CONST);
	MQTTClient_subscribe(client, HUICOBUS_MQTT_TOPIC_UIR2HCU, HUICOBUS_MQTT_QOS_CONST);
	MQTTClient_subscribe(client, HUICOBUS_MQTT_TOPIC_OPN2HCU, HUICOBUS_MQTT_QOS_CONST);
	MQTTClient_subscribe(client, HUICOBUS_MQTT_TOPIC_BHTRANS, HUICOBUS_MQTT_QOS_CONST);

	//退出条件，未来待完善
	do
	{
		hcu_usleep(100);
		ch = getchar();
	}while(1);
	//}while(ch!='Q' && ch != 'q');

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	return rc;
}

//NODEID to CONTEXT transfer
void func_mqtt_nodeid_translate_to_text(UINT32 nodeId, char *output)
{
	if (nodeId <= HUICOBUS_MQTT_NBID_MIN) strncpy(output, HUICOBUS_MQTT_NODEID_MIN, strlen(HUICOBUS_MQTT_NODEID_MIN));
	else if (nodeId >= HUICOBUS_MQTT_NBID_MAX) strncpy(output, HUICOBUS_MQTT_NODEID_MAX, strlen(HUICOBUS_MQTT_NODEID_MAX));

	switch (nodeId)
	{
	case HUICOBUS_MQTT_NBID_LOCALHOST:
		strncpy(output, HUICOBUS_MQTT_NODEID_LOCALHOST, strlen(HUICOBUS_MQTT_NODEID_LOCALHOST));
		break;

	case HUICOBUS_MQTT_NBID_FACSERVER:
		strncpy(output, HUICOBUS_MQTT_NODEID_FACSERVER, strlen(HUICOBUS_MQTT_NODEID_FACSERVER));
		break;

	case HUICOBUS_MQTT_NBID_REMSERVER:
		strncpy(output, HUICOBUS_MQTT_NODEID_FACSERVER, strlen(HUICOBUS_MQTT_NODEID_FACSERVER));
		break;

	case HUICOBUS_MQTT_NBID_MATINC:
		strncpy(output, HUICOBUS_MQTT_NODEID_MATINC, strlen(HUICOBUS_MQTT_NODEID_MATINC));
		break;

	default:
		break;
	}

	return;
}

//CONTEXT to NODEID transfer
UINT32 func_mqtt_nodeid_translate_to_id(char *input)
{
	if (strcmp(input, HUICOBUS_MQTT_NODEID_MIN) == 0) return HUICOBUS_MQTT_NBID_MIN;
	else if (strcmp(input, HUICOBUS_MQTT_NODEID_LOCALHOST) == 0) return HUICOBUS_MQTT_NBID_LOCALHOST;
	else if (strcmp(input, HUICOBUS_MQTT_NODEID_FACSERVER) == 0) return HUICOBUS_MQTT_NBID_FACSERVER;
	else if (strcmp(input, HUICOBUS_MQTT_NODEID_FACSERVER) == 0) return HUICOBUS_MQTT_NBID_REMSERVER;
	else if (strcmp(input, HUICOBUS_MQTT_NODEID_MATINC) == 0) return HUICOBUS_MQTT_NBID_MATINC;
	else return HUICOBUS_MQTT_NBID_MAX;
}

//CLIENTID to CONTEXT transfer
void func_mqtt_clientid_translate_to_text(UINT32 clId, char *output)
{
	if (clId <= HUICOBUS_MQTT_CLID_MIN) strncpy(output, HUICOBUS_MQTT_CLIENTID_MIN, strlen(HUICOBUS_MQTT_CLIENTID_MIN));
	else if (clId >= HUICOBUS_MQTT_CLID_MAX) strncpy(output, HUICOBUS_MQTT_CLIENTID_MAX, strlen(HUICOBUS_MQTT_CLIENTID_MAX));

	switch (clId)
	{
	case HUICOBUS_MQTT_CLID_HCUENTRY:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_HCUENTRY, strlen(HUICOBUS_MQTT_CLIENTID_HCUENTRY));
		break;
	case HUICOBUS_MQTT_CLID_UIROUTER:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_UIROUTER, strlen(HUICOBUS_MQTT_CLIENTID_UIROUTER));
		break;
	case HUICOBUS_MQTT_CLID_UIPRESENT:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_UIPRESENT, strlen(HUICOBUS_MQTT_CLIENTID_UIPRESENT));
		break;
	case HUICOBUS_MQTT_CLID_QRPRINTER:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_QRPRINTER, strlen(HUICOBUS_MQTT_CLIENTID_QRPRINTER));
		break;
	case HUICOBUS_MQTT_CLID_DBRESTFUL:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_DBRESTFUL, strlen(HUICOBUS_MQTT_CLIENTID_DBRESTFUL));
		break;
	case HUICOBUS_MQTT_CLID_BHPROTO:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_BHPROTO, strlen(HUICOBUS_MQTT_CLIENTID_BHPROTO));
		break;
	case HUICOBUS_MQTT_CLID_LOGERR:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_LOGERR, strlen(HUICOBUS_MQTT_CLIENTID_LOGERR));
		break;
	case HUICOBUS_MQTT_CLID_LOGTRACE:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_LOGTRACE, strlen(HUICOBUS_MQTT_CLIENTID_LOGTRACE));
		break;
	case HUICOBUS_MQTT_CLID_OPRNODE:
		strncpy(output, HUICOBUS_MQTT_CLIENTID_OPRNODE, strlen(HUICOBUS_MQTT_CLIENTID_OPRNODE));
		break;
	default:
		break;
	}

	return;
}

//CONTEXT to CLIENTID transfer
UINT32 func_mqtt_clientid_translate_to_id(char *input)
{
	if (strcmp(input, HUICOBUS_MQTT_CLIENTID_MIN) == 0) return HUICOBUS_MQTT_CLID_MIN;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_HCUENTRY) == 0) return HUICOBUS_MQTT_CLID_HCUENTRY;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_UIROUTER) == 0) return HUICOBUS_MQTT_CLID_UIROUTER;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_UIPRESENT) == 0) return HUICOBUS_MQTT_CLID_UIPRESENT;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_QRPRINTER) == 0) return HUICOBUS_MQTT_CLID_QRPRINTER;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_DBRESTFUL) == 0) return HUICOBUS_MQTT_CLID_DBRESTFUL;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_BHPROTO) == 0) return HUICOBUS_MQTT_CLID_BHPROTO;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_LOGERR) == 0) return HUICOBUS_MQTT_CLID_LOGERR;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_LOGTRACE) == 0) return HUICOBUS_MQTT_CLID_LOGTRACE;
	else if (strcmp(input, HUICOBUS_MQTT_CLIENTID_OPRNODE) == 0) return HUICOBUS_MQTT_CLID_OPRNODE;
	else return HUICOBUS_MQTT_CLID_MAX;
}

//TOPICID to CONTEXT transfer
void func_mqtt_topicid_translate_to_text(UINT32 tpId, char *output)
{
	if (tpId <= HUICOBUS_MQTT_TPID_MIN) strncpy(output, HUICOBUS_MQTT_TOPIC_MIN, strlen(HUICOBUS_MQTT_TOPIC_MIN));
	else if (tpId >= HUICOBUS_MQTT_TPID_MAX) strncpy(output, HUICOBUS_MQTT_TOPIC_MAX, strlen(HUICOBUS_MQTT_TOPIC_MAX));

	switch (tpId)
	{
	case HUICOBUS_MQTT_TPID_HCU2UIR:
		strncpy(output, HUICOBUS_MQTT_TOPIC_HCU2UIR, strlen(HUICOBUS_MQTT_TOPIC_HCU2UIR));
		break;
	case HUICOBUS_MQTT_TPID_UIR2HCU:
		strncpy(output, HUICOBUS_MQTT_TOPIC_UIR2HCU, strlen(HUICOBUS_MQTT_TOPIC_UIR2HCU));
		break;
	case HUICOBUS_MQTT_TPID_UIR2UIP:
		strncpy(output, HUICOBUS_MQTT_TOPIC_UIR2UIP, strlen(HUICOBUS_MQTT_TOPIC_UIR2UIP));
		break;
	case HUICOBUS_MQTT_TPID_PRINTFLOW:
		strncpy(output, HUICOBUS_MQTT_TOPIC_PRINTFLOW, strlen(HUICOBUS_MQTT_TOPIC_PRINTFLOW));
		break;
	case HUICOBUS_MQTT_TPID_DBACCESS:
		strncpy(output, HUICOBUS_MQTT_TOPIC_DBACCESS, strlen(HUICOBUS_MQTT_TOPIC_DBACCESS));
		break;
	case HUICOBUS_MQTT_TPID_BHTRANS:
		strncpy(output, HUICOBUS_MQTT_TOPIC_BHTRANS, strlen(HUICOBUS_MQTT_TOPIC_BHTRANS));
		break;
	case HUICOBUS_MQTT_TPID_LOGERRFLOW:
		strncpy(output, HUICOBUS_MQTT_TOPIC_LOGERRFLOW, strlen(HUICOBUS_MQTT_TOPIC_LOGERRFLOW));
		break;
	case HUICOBUS_MQTT_TPID_LOGTRACEFLOW:
		strncpy(output, HUICOBUS_MQTT_TOPIC_LOGTRACEFLOW, strlen(HUICOBUS_MQTT_TOPIC_LOGTRACEFLOW));
		break;
	case HUICOBUS_MQTT_TPID_HCU2OPN:
		strncpy(output, HUICOBUS_MQTT_TOPIC_HCU2OPN, strlen(HUICOBUS_MQTT_TOPIC_HCU2OPN));
		break;
	case HUICOBUS_MQTT_TPID_OPN2HCU:
		strncpy(output, HUICOBUS_MQTT_TOPIC_OPN2HCU, strlen(HUICOBUS_MQTT_TOPIC_OPN2HCU));
		break;
	default:
		break;
	}

	return;
}

//CONTEXT to TOPICID transfer
UINT32 func_mqtt_topicid_translate_to_id(char *input)
{
	if (strcmp(input, HUICOBUS_MQTT_TOPIC_MIN) == 0) return HUICOBUS_MQTT_TPID_MIN;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_HCU2UIR) == 0) return HUICOBUS_MQTT_TPID_HCU2UIR;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_UIR2HCU) == 0) return HUICOBUS_MQTT_TPID_UIR2HCU;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_UIR2UIP) == 0) return HUICOBUS_MQTT_TPID_UIR2UIP;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_PRINTFLOW) == 0) return HUICOBUS_MQTT_TPID_PRINTFLOW;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_DBACCESS) == 0) return HUICOBUS_MQTT_TPID_DBACCESS;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_BHTRANS) == 0) return HUICOBUS_MQTT_TPID_BHTRANS;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_LOGERRFLOW) == 0) return HUICOBUS_MQTT_TPID_LOGERRFLOW;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_LOGTRACEFLOW) == 0) return HUICOBUS_MQTT_TPID_LOGTRACEFLOW;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_HCU2OPN) == 0) return HUICOBUS_MQTT_TPID_HCU2OPN;
	else if (strcmp(input, HUICOBUS_MQTT_TOPIC_OPN2HCU) == 0) return HUICOBUS_MQTT_TPID_OPN2HCU;
	else return HUICOBUS_MQTT_TPID_MAX;
}

//HATE测试环境
OPSTAT hcu_mqtt_hate_data_send(void *context, char *topicName, int payloadLen, char *payload)
{
	MQTTClient_message message;
	message.payloadlen = payloadLen;
	memcpy(message.payload, payload, message.payloadlen);
	return func_mqtt_msg_rcv_msgarrvd(context, topicName, payloadLen, &message);
}




