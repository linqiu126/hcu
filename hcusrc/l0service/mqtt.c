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
int hcu_mqtt_msg_send_syn_mode(char *topic, char *input)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENTID_HCU, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        HcuErrorPrint("MQTT: Failed to connect, return code %d. So far set to continue work!\n", rc);
        //exit(EXIT_FAILURE);
    }
    pubmsg.payload = input;
    pubmsg.payloadlen = strlen(input);
    pubmsg.qos = MQTT_QOS_CONST;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    HCU_DEBUG_PRINT_NOR("MQTT: Waiting for up to %d seconds for publication of %s\n, on topic %s for client with ClientID: %s\n", (int)(MQTT_TIMEOUT_CONST/1000), input, topic, MQTT_CLIENTID_HCU);
    rc = MQTTClient_waitForCompletion(client, token, MQTT_TIMEOUT_CONST);
    HCU_DEBUG_PRINT_NOR("MQTT: Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
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
  HCU_DEBUG_PRINT_NOR("MQTT:      topic: %s\n", topicName);
  HCU_DEBUG_PRINT_NOR("MQTT:    message: ");

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
int hcu_mqtt_msg_send_asy_mode(char *topic, char *input)
{
  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  int rc;

  MQTTClient_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENTID_HCU, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  MQTTClient_setCallbacks(client, NULL, func_mqtt_msg_send_asy_connlost, func_mqtt_msg_send_asy_msgarrvd, func_mqtt_msg_send_asy_delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
	  HcuErrorPrint("MQTT: Failed to connect, return code %d. So far continue to work!\n", rc);
      //exit(EXIT_FAILURE);
  }
  pubmsg.payload = input;
  pubmsg.payloadlen = strlen(input);
  pubmsg.qos = MQTT_QOS_CONST;
  pubmsg.retained = 0;
  deliveredtoken_send = 0;
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
  int i;
  char* payloadptr;

  HCU_DEBUG_PRINT_FAT("MQTT: Message arrived\n");
  HCU_DEBUG_PRINT_FAT("MQTT: topic: %s\n", topicName);
  HCU_DEBUG_PRINT_FAT("MQTT: message: ");

  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++)
  {
      putchar(*payloadptr++);
  }
  putchar('\n');

  //上面这一段，未来将送到内部程序，并进行处理
  //也可以采取CallBack函数的形式，让内部任务自行处理具体的过程：传递消息内容以及长度

  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void func_mqtt_msg_rcv_connlost(void *context, char *cause)
{
	HCU_DEBUG_PRINT_NOR("MQTT: \nConnection lost\n");
	HCU_DEBUG_PRINT_NOR("MQTT:      cause: %s\n", cause);
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
  HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", MQTT_TOPIC_UI_TO_HCU, MQTT_CLIENTID_HCU, MQTT_QOS_CONST);
  HCU_DEBUG_PRINT_NOR("MQTT: Subscribing to topic %s\n for client %s using QoS%d\n\n, Press Q<Enter> to quit\n\n", MQTT_TOPIC_BH_TRANS, MQTT_CLIENTID_HCU, MQTT_QOS_CONST);
  MQTTClient_subscribe(client, MQTT_TOPIC_UI_TO_HCU, MQTT_QOS_CONST);
  MQTTClient_subscribe(client, MQTT_TOPIC_BH_TRANS, MQTT_QOS_CONST);

  do
  {
      ch = getchar();
  } while(ch!='Q' && ch != 'q');

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}



