/*
 * ethernet.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#include "../l1hwopt/ethernet.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the ETHERNET
*/
HcuFsmStateItem_t HcuFsmEthernet[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_ethernet_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_ethernet_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,		FSM_STATE_COMMON,            			fsm_ethernet_restart},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_ETHERNET_RECEIVED,            fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_ETHERNET_RECEIVED,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_ETHERNET_RECEIVED,       		fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global Variables

//Taks Global Variables
extern HcuCloudvelaTaskContext_t gCloudvelaTaskContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_ethernet_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_ETHERNET, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("ETHERNET: Error Set FSM State at fsm_ethernet_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_ethernet_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_ETHERNET, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("ETHERNET: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ETHERNET].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_ETHERNET, FSM_STATE_ETHERNET_INITED) == FAILURE){
		HcuErrorPrint("ETHERNET: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET] = 0;

	//初始化参数
	if (func_ethernet_int_init() == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
		HcuErrorPrint("ETHERNET: Error initialize interface!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ETHERNET, FSM_STATE_ETHERNET_RECEIVED) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("ETHERNET: Enter FSM_STATE_ETHERNET_ACTIVED status, Keeping refresh here!\n");

	//初始化MSGSEND参数
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;
	memset(&receiveBuffer, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));
	if (hcu_ethernet_socket_link_setup() == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: socket line setup failure!\n");

	struct sockaddr_in serveraddr;
	bzero((char *)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(zHcuSysEngPar.cloud.svrAddrSocketipDefault);
	serveraddr.sin_port = htons(HCU_CLOUDSRV_SOCKET_PORT);
	UINT32 echolen;

	//Heart beat checking in LLC
	int keepAlive = HCU_CLOUDSRV_SOCKET_KEEPALIVE; // set KeepAlive
	int keepIdle = HCU_CLOUDSRV_SOCKET_KEEPIDLE; //tcp idle time before first KeepAlive checking
	int keepInterval = HCU_CLOUDSRV_SOCKET_KEEPINTERVAL; //interval between two KeepAlive checking
	int keepCount = HCU_CLOUDSRV_SOCKET_KEEPCOUNT; //count before disconnect Keepalive

	int idata;
	//进入阻塞式接收数据状态，收到数据后发送给CLOUDCONT进行处理
	while(1){
		idata = 0;
		//send(zHcuEthConClientFd,receiveBuffer.buf,receiveBuffer.length,0);
		memset(&receiveBuffer, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));

		idata = recv(gCloudvelaTaskContext.defaultSvrethConClientFd, &receiveBuffer.buf,HCU_SYSMSG_COM_MSG_BODY_LEN_MAX,0);
		receiveBuffer.length = idata;

		if(idata <= 0){
			HcuErrorPrint("ETHERNET: Socket receive error: %d !\n", idata);
			zHcuSysStaPm.statisCnt.SocketDiscCnt++;
			gCloudvelaTaskContext.defaultSvrSocketCon = FALSE;

			close(gCloudvelaTaskContext.defaultSvrethConClientFd);
			gCloudvelaTaskContext.defaultSvrethConClientFd = socket(AF_INET, SOCK_STREAM,0);

			if(gCloudvelaTaskContext.defaultSvrethConClientFd < 0){
				HcuErrorPrint("ETHERNET: Can not create socket!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
			}

			if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_SOCKET, SO_KEEPALIVE,(void*)&keepAlive, sizeof(keepAlive)) == -1)
				HcuErrorPrint("ETHERNET: setsockopt SO_KEEPALIVE error!\n");
			if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPIDLE,(void*)&keepIdle, sizeof(keepIdle)) == -1)
				HcuErrorPrint("ETHERNET: setsockopt SO_KEEPIDLE error!\n");
			if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPINTVL,(void*)&keepInterval, sizeof(keepInterval)) == -1)
				HcuErrorPrint("ETHERNET: setsockopt TCP_KEEPINTVL error!\n");
			if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPCNT,(void*)&keepCount, sizeof(keepCount)) == -1)
				HcuErrorPrint("ETHERNET: setsockopt TCP_KEEPCNT error!\n");
			if( connect(gCloudvelaTaskContext.defaultSvrethConClientFd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
			{
				HcuErrorPrint("ETHERNET: Socket can not connect!\n");
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
				gCloudvelaTaskContext.defaultSvrSocketCon = FALSE;
			}
			else
			{
				HCU_DEBUG_PRINT_INF("ETHERNET: Socket reconnected\n");
				echolen = strlen(zHcuSysEngPar.cloud.hcuName);
				if (send(gCloudvelaTaskContext.defaultSvrethConClientFd, zHcuSysEngPar.cloud.hcuName, echolen, 0) != echolen){
					HcuErrorPrint("ETHERNET: Mismatch in number of send bytes");
					zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
				}
				else{
					gCloudvelaTaskContext.defaultSvrSocketCon = TRUE;
					HCU_DEBUG_PRINT_INF("ETHERNET: Socket reconnected & send data to Server succeed: %s!\n", zHcuSysEngPar.cloud.hcuName);
				}
			}
		}

		else{
			//receiveBuffer.length = idata;
			HCU_DEBUG_PRINT_INF("ETHERNET: Socket receive data from cloud, Data Len=%d, Buffer=%s\n\n", receiveBuffer.length,  receiveBuffer.buf);

			//ret = hcu_message_send(MSG_ID_ETHERNET_CLOUDVELA_DATA_RX, TASK_ID_CLOUDVELA, TASK_ID_ETHERNET, receiveBuffer.buf, receiveBuffer.length);
			ret = hcu_message_send(MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX, TASK_ID_CLOUDVELA, TASK_ID_ETHERNET, receiveBuffer.buf, receiveBuffer.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
				HcuErrorPrint("ETHERNET: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ETHERNET].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
			}

		}
		//为了防止负荷太高，休眠一会儿
		hcu_usleep(HCU_ETHERNET_SOCKET_DURATION_PERIOD_RECV*5);
	}//while(1) end

	//出来后，断链
	close(gCloudvelaTaskContext.defaultSvrethConClientFd);

	//返回
	return SUCCESS;
}

OPSTAT fsm_ethernet_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("ETHERNET: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_ethernet_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_ethernet_int_init(void)
{
	return SUCCESS;
}

OPSTAT hcu_ethernet_phy_link_setup(void)
{
	return SUCCESS;
}


OPSTAT hcu_ethernet_phy_link_disconnect(void)
{
	return SUCCESS;
}

//为SOCKET建立链路
OPSTAT hcu_ethernet_socket_link_setup(void)
{
	int ret=0;
	//创建Client端套接字描述符
	gCloudvelaTaskContext.defaultSvrethConClientFd = socket(AF_INET, SOCK_STREAM,0);

	if(gCloudvelaTaskContext.defaultSvrethConClientFd < 0){
		gCloudvelaTaskContext.defaultSvrSocketCon = FALSE;
		HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: Can not create socket!\n");
	}

	//创建用于服务的Client端套接字，注意与 Server端创建的套接字的区别  IP段里，Server端是可以为任何IP提供服务的，客户端里的IP是请求的端点
	struct sockaddr_in serveraddr;
	//struct hostent *hp;//
	bzero((char *)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//inet_pton(AF_INET,CLOUDSRV_ADDRESS,&serveraddr.sin_addr);
	//serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//temporarily use the element, to update DB later
	serveraddr.sin_addr.s_addr = inet_addr(zHcuSysEngPar.cloud.svrAddrSocketipDefault);
	serveraddr.sin_port = htons(HCU_CLOUDSRV_SOCKET_PORT);
	UINT32 echolen;

	//Heart beat checking in LLC
	int keepAlive = HCU_CLOUDSRV_SOCKET_KEEPALIVE; // set KeepAlive
	int keepIdle = HCU_CLOUDSRV_SOCKET_KEEPIDLE; //tcp idle time before first KeepAlive checking
	int keepInterval = HCU_CLOUDSRV_SOCKET_KEEPINTERVAL; //interval between two KeepAlive checking
	int keepCount = HCU_CLOUDSRV_SOCKET_KEEPCOUNT; //count before disconnect Keepalive


	if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_SOCKET, SO_KEEPALIVE,(void*)&keepAlive, sizeof(keepAlive)) == -1)
		HcuErrorPrint("ETHERNET: setsockopt SO_KEEPALIVE error!\n");
	if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPIDLE,(void*)&keepIdle, sizeof(keepIdle)) == -1)
		HcuErrorPrint("ETHERNET: setsockopt SO_KEEPIDLE error!\n");
	if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPINTVL,(void*)&keepInterval, sizeof(keepInterval)) == -1)
		HcuErrorPrint("ETHERNET: setsockopt TCP_KEEPINTVL error!\n");
	if(setsockopt(gCloudvelaTaskContext.defaultSvrethConClientFd, SOL_TCP, TCP_KEEPCNT,(void*)&keepCount, sizeof(keepCount)) == -1)
		HcuErrorPrint("ETHERNET: setsockopt TCP_KEEPCNT error!\n");
	if( connect(gCloudvelaTaskContext.defaultSvrethConClientFd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		HcuErrorPrint("ETHERNET: Socket can not connect!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
		gCloudvelaTaskContext.defaultSvrSocketCon = FALSE;
		//to restart this task
		hcu_usleep(HCU_ETHERNET_SOCKET_DURATION_PERIOD_RECV * 10);
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_ETHERNET, TASK_ID_ETHERNET, &snd0, snd0.length);
		if (ret == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ETHERNET].taskName, zHcuVmCtrTab.task[TASK_ID_ETHERNET].taskName);
		return FAILURE;
	}
	else
	{
		HCU_DEBUG_PRINT_INF("ETHERNET: Socket connected\n");
		echolen = strlen(zHcuSysEngPar.cloud.hcuName);
		if (send(gCloudvelaTaskContext.defaultSvrethConClientFd, zHcuSysEngPar.cloud.hcuName, echolen, 0) != echolen){
			HcuErrorPrint("ETHERNET: Mismatch in number of send bytes");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
		}
		else{
			gCloudvelaTaskContext.defaultSvrSocketCon = TRUE;
		}
	}

	return SUCCESS;
}

//为SOCKET断掉链路
OPSTAT hcu_ethernet_socket_link_disconnect(void)
{
	return SUCCESS;
}

//在SOCKET上发送数据
OPSTAT hcu_ethernet_socket_data_send(CloudDataSendBuf_t *buf)
{

	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_INF_ON) != FALSE){
		if(gCloudvelaTaskContext.defaultSvrethConClientFd < 0) HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "CLOUDVELA: socket id is not valid!\n");
	}

	if (send(gCloudvelaTaskContext.defaultSvrethConClientFd, buf->curBuf, buf->curLen, 0) != buf->curLen){
		gCloudvelaTaskContext.defaultSvrSocketCon = FALSE;
		HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: Socket disconnected & Mismatch in number of send bytes!\n\n");
	}else{
		HCU_DEBUG_PRINT_INF("ETHERNET: Socket connected, send message to socket server success: %s!\n\n", buf->curBuf);
	}

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　CURL/ETHERNET函数暂时没有启用，因为HTTP/CURL方式不再激活使用
 *
 ***************************************************************************************************************************/
OPSTAT hcu_ethernet_curl_data_send(CloudDataSendBuf_t *buf)
{
	CURLcode curlRes;
	int ret = 0;
	CURL *curl;

	//初始化MSGSEND参数
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;
	memset(&receiveBuffer, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));

	curl = curl_easy_init();

	if (curl == NULL){
		HcuErrorPrint("ETHERNET: Init CURL error!\n");
	}else{
		//打印测试
		//HcuDebugPrint("ETHERNET: Enter data send status, to send data for back-cloud!\n");

		//如果你想CURL报告每一件意外的事情，设置这个选项为一个非零值
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		//设置头，以便返回完整的信息
		//curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//也就是说，默认情况下libcurl完成一个任务以后，出于重用连接的考虑不会马上关闭如果没有新的TCP请求来重用这个连接，那么只能等到CLOSE_WAIT超时
		//这个时间默认在7200秒甚至更高，太多的CLOSE_WAIT连接会导致性能问题。这里设置为0就是为了重用
		//curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0);

		//设置目标地址
		curl_easy_setopt(curl, CURLOPT_URL, zHcuSysEngPar.cloud.svrAddrHttpDefault);
		//curl_easy_setopt(curl, CURLOPT_URL, zHcuSysEngPar.cloud.cloudHttpAddLocal);

		//设置超时时长，做为发送API，这个设置绝对必要，不然会阻塞在这儿
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, ETHERNET_INSTANCE_DATA_SEND_TIME_OUT_IN_MS);

		//就是当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//设置发送数据的信息
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf->curBuf);

		//设置回调函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, hcu_cloudvela_write_callback);

		//设置接收buffer
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&receiveBuffer);

		//实施调用
		curlRes = curl_easy_perform(curl);

		//Clean curl pointer
		curl_easy_cleanup(curl);

		if(curlRes != CURLE_OK){
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuErrorPrint("ETHERNET: curl_easy_perform() failed: %s\n", curl_easy_strerror(curlRes));
			}
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_ETHERNET]++;
			return FAILURE;
		}else{
			HCU_DEBUG_PRINT_INF("ETHERNET: Snd/Rcv pair operation curl_easy_perform data Len=%d, Buffer=%s\n", receiveBuffer.length,  receiveBuffer.buf);
		}

		//将数据发送给CLOUD，有关这个长度应该>0或者>1的问题，最后还是1，因为心跳握手帧只有二个字节的长度
		if (receiveBuffer.length > 1){
			//发送数据给CLOUDCONT
			ret = hcu_message_send(MSG_ID_ETHERNET_CLOUDVELA_DATA_RX, TASK_ID_CLOUDVELA, TASK_ID_ETHERNET, receiveBuffer.buf, receiveBuffer.length);
			if (ret == FAILURE)
				HCU_ERROR_PRINT_TASK(TASK_ID_ETHERNET, "ETHERNET: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ETHERNET].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
		}//end of send data
		//这里放zHcuGlobalCounter.CloudDataTimeOutCnt的计数器，Not sure是否是最好的地方，再探究
		else{
			zHcuSysStaPm.statisCnt.CloudDataTimeOutCnt++;
		}

	}//End of working condition

	//返回
	return SUCCESS;
}
/***************************************************************************************************************************
 *
 * 　CURL/ETHERNET未启用函数END
 *
 ***************************************************************************************************************************/
/*
static int base64_encode(char *str, int str_len, char *encode, int encode_len){
    BIO *bmem,*b64;
    BUF_MEM *bptr;
    b64=BIO_new(BIO_f_base64());
    bmem=BIO_new(BIO_s_mem());
    b64=BIO_push(b64,bmem);
    BIO_write(b64,str,str_len); //encode
    BIO_flush(b64);
    BIO_get_mem_ptr(b64,&bptr);
    if(bptr->length>encode_len){
        DPRINTF("encode_len too small\n");
        return -1;
    }
    encode_len=bptr->length;
    memcpy(encode,bptr->data,bptr->length);
//  write(1,encode,bptr->length);
    BIO_free_all(b64);
    return encode_len;
}

static int base64_decode(char *str,int str_len,char *decode,int decode_buffer_len){
    int len=0;
    BIO *b64,*bmem;
    b64=BIO_new(BIO_f_base64());
    bmem=BIO_new_mem_buf(str,str_len);
    bmem=BIO_push(b64,bmem);
    len=BIO_read(bmem,decode,str_len);
    decode[len]=0;
    BIO_free_all(bmem);
    return 0;
}
*/
