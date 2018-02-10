/*
 * l3daycg21.c
 *
 *  Created on: 2017年11月22日
 *      Author: test
 */


#include "l3daycg21.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"
#include "../l3app/sensornoise.h"
#include "../l3app/sensorpm25.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wchar.h>

/*
** FSM of the L3DAYCG21
*/
HcuFsmStateItem_t HcuFsmL3daycg21[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_l3daycg21_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_l3daycg21_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3DAYCG21_INITED,            		fsm_l3daycg21_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3DAYCG21_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            				fsm_l3daycg21_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          					fsm_l3daycg21_time_out},

    //Normal working status
	//{MSG_ID_CLOUDVELA_YCJK_DATA_REQ,    		FSM_STATE_L3DAYCG21_ACTIVED,      			fsm_l3daycg21_cloudvela_data_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Global variables
LedMsgBuf_t currentLedBuf;

//Task Global variables


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3daycg21_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error Set FSM State at fsm_l3daycg21_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3daycg21_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//LCD_AlarmFlag = 0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3DAYCG21, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3DAYCG21: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_L3DAYCG21_INITED) == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3daycg21_int_init() == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error initialize interface!\n");
		return FAILURE;
	}

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3DAYCG21, TIMER_ID_1S_L3DAYCG21_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3DAYCG21_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
		HcuErrorPrint("L3DAYCG21: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_L3DAYCG21_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3DAYCG21("L3DAYCG21: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3DAYCG21: Enter FSM_STATE_L3DAYCG21_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3daycg21_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3DAYCG21: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3daycg21_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3daycg21_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3daycg21_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3DAYCG21: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3DAYCG21]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3DAYCG21, TASK_ID_L3DAYCG21, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
			HcuErrorPrint("L3DAYCG21: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName, zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3DAYCG21_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S))
	{
		//READ processing func!

		int ret=0;

		//对信息进行ZH系列字符控制卡协议的编码
		memset(&currentLedBuf, 0, sizeof(LedMsgBuf_t));

		//对发送数据进行编码
//////////////////////////////////////////////////////////
		wchar_t *chinese_str = L"扬尘监控系统";
		wchar_t *noise_str = L"噪声：";
		unsigned int *p_noise = (wchar_t*)noise_str;

		wchar_t *noise_unit_str = L" dB";
		unsigned int *p_noise_unit = (wchar_t*)noise_unit_str;


		wchar_t *tsp_str = L"  PM2.5：";
		unsigned int *p_tsp = (wchar_t*)tsp_str;

		wchar_t *tsp_unit_str = L"ug/m3                                                                                                                     ";
		unsigned int *p_tsp_unit = (wchar_t*)tsp_unit_str;

/*
		wchar_t *temp_str = L"温度：";
		unsigned int *p_temp = (wchar_t*)temp_str;

		wchar_t *temp_unit_str = L" ℃   ";
		unsigned int *p_temp_unit = (wchar_t*)temp_unit_str;


		wchar_t *humid_str = L"湿度：";
		unsigned int *p_humid = (wchar_t*)humid_str;

		wchar_t *humid_unit_str = L" RH%                                                                                                                     ";
		unsigned int *p_humid_unit = (wchar_t*)humid_unit_str;


		wchar_t *windspd_str = L"风速：";
		unsigned int *p_windspd = (wchar_t*)windspd_str;

		wchar_t *windspd_unit_str = L" 米/秒 ";
		unsigned int *p_windspd_unit = (wchar_t*)windspd_unit_str;



		wchar_t *winddir_str = L"风向：";
		unsigned int *p_winddir = (wchar_t*)winddir_str;
*/
		///////////////////////////////////////////////////
		if(gTaskNoiseContext.noiseValue == 0 || gTaskNoiseContext.noiseValue >= 80)
			gTaskNoiseContext.noiseValue = 35.9;

		float noise = gTaskNoiseContext.noiseValue;
		char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
		memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		//itoa(number,string,16);
		sprintf(CStr,"%.1f",noise);
		printf("integer = %f CStr = %s\n", noise,CStr);

	   //把char*转换为wchar_t*
		size_t len = strlen(CStr) + 1;
		wchar_t *noise_value_str;
		noise_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
		mbstowcs(noise_value_str,CStr,len);

		unsigned int *p_noise_value = (wchar_t*)noise_value_str;
		///////////////////////////////////////////////////


		///////////////////////////////////////////////////
		if(gTaskPm25Context.PM25Value == 0 || gTaskPm25Context.PM25Value >= 80)
			gTaskPm25Context.PM25Value = 98.3;

		float tsp = gTaskPm25Context.PM25Value;

		//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
		memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		//itoa(number,string,16);
		sprintf(CStr,"%.1f",tsp);
		printf("integer = %f CStr = %s\n", tsp,CStr);

	   //把char*转换为wchar_t*
		len = strlen(CStr) + 1;
		wchar_t *tsp_value_str;
		tsp_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
		mbstowcs(tsp_value_str,CStr,len);

		unsigned int *p_tsp_value = (wchar_t*)tsp_value_str;
		///////////////////////////////////////////////////

/*
		///////////////////////////////////////////////////
		//float temp = gTaskL3aqycq20Context.staMin.a01001_Avg;
		float temp = 20;
		//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
		memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		//itoa(number,string,16);
		sprintf(CStr,"%.1f",temp);
		printf("integer = %f CStr = %s\n", temp,CStr);

	   //把char*转换为wchar_t*
		len = strlen(CStr) + 1;
		wchar_t *temp_value_str;
		temp_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
		mbstowcs(temp_value_str,CStr,len);

		unsigned int *p_temp_value = (wchar_t*)temp_value_str;
		///////////////////////////////////////////////////


		///////////////////////////////////////////////////
		//float humid = gTaskL3aqycq20Context.staMin.a01002_Avg;
		float humid = 50;
		//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
		memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		//itoa(number,string,16);
		sprintf(CStr,"%.1f",humid);
		printf("integer = %f CStr = %s\n", humid,CStr);

	   //把char*转换为wchar_t*
		len = strlen(CStr) + 1;
		wchar_t *humid_value_str;
		humid_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
		mbstowcs(humid_value_str,CStr,len);

		unsigned int *p_humid_value = (wchar_t*)humid_value_str;
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		//float windspd = gTaskL3aqycq20Context.staMin.a01007_Avg;
		float windspd = 100;
		//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
		memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		//itoa(number,string,16);
		sprintf(CStr,"%.1f",windspd);
		printf("integer = %f CStr = %s\n", windspd,CStr);

	   //把char*转换为wchar_t*
		len = strlen(CStr) + 1;
		wchar_t *windspd_value_str;
		windspd_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
		mbstowcs(windspd_value_str,CStr,len);

		unsigned int *p_windspd_value = (wchar_t*)windspd_value_str;
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		//unsigned int winddir = (unsigned int)gTaskL3aqycq20Context.staMin.a01008_Avg;
		unsigned int winddir = 255;
		wchar_t *winddir_value_str;
		if(winddir>=0 && winddir<=11){
			winddir_value_str = L"北                      ";
		}
		else if(winddir>349 && winddir<=360){
			winddir_value_str = L"北                      ";
		}

		else if(winddir>11 && winddir<=34){
			winddir_value_str = L"东北偏北                  ";
		}

		else if(winddir>34 && winddir<=56){
			winddir_value_str = L"东北                      ";
		}

		else if(winddir>56 && winddir<=79){
			winddir_value_str = L"东北偏东                   ";
		}

		else if(winddir>79 && winddir<=101){
			winddir_value_str = L"东                        ";
		}

		else if(winddir>101 && winddir<=124){
			winddir_value_str = L"东南偏东                   ";
		}

		else if(winddir>124 && winddir<=146){
			winddir_value_str = L"东南                       ";
		}

		else if(winddir>146 && winddir<=169){
			winddir_value_str = L"东南偏南                    ";
		}

		else if(winddir>169 && winddir<=191){
			winddir_value_str = L"南                          ";
		}

		else if(winddir>191 && winddir<=214){
			winddir_value_str = L"西南偏南                     ";
		}

		else if(winddir>214 && winddir<=236){
			winddir_value_str = L"西南                          ";
		}

		else if(winddir>236 && winddir<=259){
			winddir_value_str = L"西南偏西                       ";
		}

		else if(winddir>259 && winddir<=281){
			winddir_value_str = L"西                            ";
		}

		else if(winddir>281 && winddir<=304){
			winddir_value_str = L"西北偏西                       ";
		}

		else if(winddir>304 && winddir<=326){
			winddir_value_str = L"西北                           ";
		}

		else if(winddir>326 && winddir<=349){
			winddir_value_str = L"西北偏北                        ";
		}

		unsigned int *p_winddir_value = (wchar_t*)winddir_value_str;
*/

/*
		///////////////////////////////////////////////////
		printf("Unicode: \n");
		printf("Chinese str len: %d\n",wcslen(chinese_str));

		unsigned int i = 0;

		printf("Noise str len: %d\n",wcslen(noise_value_str));
		for(i=0;i<wcslen(noise_value_str);i++)
		{
			printf("0x%x  ",p_noise_value[i]);
		}
		printf("\n");

		printf("tsp value len: %d\n",wcslen(tsp_value_str));
		for(i=0;i<wcslen(tsp_value_str);i++)
		{
			printf("0x%x  ",p_tsp_value[i]);
		}
		printf("\n\n\n\n\n");
		///////////////////////////////////////////////////
*/
		unsigned int curLen = 228;

		unsigned char sample1[] = {0x7A,0x01,0x00,0x06,0xFA,0x00,0x01,0x01,0x01,0x0C,0x05,0x03,0x01,0x00,0x3C,0x01,0x01,0x00,0x00,0x7F,0x0A,0x00,0x01,0x00,0x00,0x00,
				0x00,0x00,0x80,0x00,0x10,0x00,0x02,0x04,0x02,0x09,0xF4,0x01,0x00,0x00,0x05,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5C,0x5A,0x00,0x5C,0x54,0x01};

		unsigned char  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];

		//send the first frame start
		memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
		memcpy(curBuf, sample1, curLen);

		unsigned int i = 0;
		unsigned int j = 0;
/*				/////////////////////////////////////////
			for(i=0;i<wcslen(chinese_str); i++)
		{
			curBuf[curLen+j] = p_chinese[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_chinese[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n");
		curLen = curLen+2*wcslen(chinese_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////
*/
	/////////////////////////////////////////

		for(i=0;i<wcslen(noise_str); i++)
		{
			curBuf[curLen+j] = p_noise[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_noise[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n");
		curLen = curLen+2*wcslen(noise_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////

	/////////////////////////////////////////
		j = 0;
		for(i=0;i<wcslen(noise_value_str); i++)
		{
			curBuf[curLen+j] = p_noise_value[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_noise_value[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n\n\n\n\n\n\n\n\n");
		curLen = curLen+2*wcslen(noise_value_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////

	/////////////////////////////////////////
		j = 0;
		for(i=0;i<wcslen(noise_unit_str); i++)
		{
			curBuf[curLen+j] = p_noise_unit[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_noise_unit[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n");
		curLen = curLen+2*wcslen(noise_unit_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////

	/////////////////////////////////////////
		j = 0;
		for(i=0;i<wcslen(tsp_str); i++)
		{
			curBuf[curLen+j] = p_tsp[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_tsp[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n");
		curLen = curLen+2*wcslen(tsp_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////

	/////////////////////////////////////////
		j = 0;
		for(i=0;i<wcslen(tsp_value_str); i++)
		{
			curBuf[curLen+j] = p_tsp_value[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_tsp_value[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n\n\n\n\n\n\n\n\n");
		curLen = curLen+2*wcslen(tsp_value_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////

	/////////////////////////////////////////
		j = 0;
		for(i=0;i<wcslen(tsp_unit_str); i++)
		{
			curBuf[curLen+j] = p_tsp_unit[i] >> 8;
			printf("%02x  ",curBuf[curLen+j]);
			curBuf[curLen+j+1] = p_tsp_unit[i];
			printf("%02x  ",curBuf[curLen+j+1]);
			j=j+2;
		}

		printf("\n\n\n\n\n\n\n\n\n");
		curLen = curLen+2*wcslen(tsp_unit_str);


		for(i=0;i<curLen;i++)
		{

			printf("%02x  ",curBuf[i]);
		}
		printf("\n");
	/////////////////////////////////////////


		unsigned char sample3[] = {0x5C,0x00};
		memcpy(&curBuf[curLen], sample3, 2);

		unsigned int DataLen = curLen+2-6;
		printf("Total Length: %d\n",DataLen);
		printf("Total Length: %02x\n  ",DataLen);

		//第五位需要更新为DataLen的十六进制 unsigned char
		//memcpy(&curBuf[4], DataLen, 2);
		//curBuf[4] = DataLen;
		curBuf[4] = DataLen;
		curBuf[5] = DataLen >> 8;
		printf("Total Length: %02x\n  ",curBuf[4]);
		printf("Total Length: %02x\n  ",curBuf[5]);

		unsigned char checksum = 0;
		for(i=0;i<(curLen + 2);i++)
		{
			//printf("0x%x  ",sample[i]);
			printf("%02x  ",curBuf[i]);
			checksum = checksum + curBuf[i];

		}

		printf("\n");
		printf("checksum: 0x%2x  ", checksum);
		printf("\n");

		curBuf[curLen+2] = checksum;
		curBuf[curLen+3] = 0x1A;


		for(i=0;i<(curLen+4);i++)
		{
			printf("%02x  ",curBuf[i]);
		}

//////////////////////////////////////////////////////////

		//发送串口指令
		//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
		ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), curBuf, curLen+4);

		if (FAILURE == ret)
		{
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
			HcuErrorPrint("L3DAYCG20: Error send ZH command to serials port!\n");
			return FAILURE;
		}
		else
		{
			//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send ZH data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
			return SUCCESS;
		}

		//等待短时
		hcu_usleep(5);
		//send the first frame end

	}
}










