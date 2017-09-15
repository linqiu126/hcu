/*
 * ftp.c
 *
 *  Created on: 16 May 2016
 *      Author: shanchun
 */


//#include <stdio.h>
//#include <stdlib.h>

#include <curl/curl.h>
#include "../l0service/trace.h"

#include "ftp.h"

/*****************local api******************/
int get_file_size(FILE *file)
{
	int size = 0;
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}

/******************curl api****************/
CURL *curl_init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if(NULL == curl)
	{
		HcuErrorPrint("L0SVRFTP: Init curl failed!\n");
		return NULL;
		//exit(1);
	}

	return curl;
}

void curl_set_upload_opt(CURL *curl, const char *url, const char *user_key, FILE *file)
{
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERPWD, user_key);
	curl_easy_setopt(curl, CURLOPT_READDATA, file);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(curl, CURLOPT_INFILESIZE, get_file_size(file));
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}

void curl_set_download_opt(CURL *curl, const char *url, const char *user_key, FILE *file)
{
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERPWD, user_key);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}

void curl_exit(CURL *curl)
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

CURLcode curl_perform(CURL *curl)
{
	CURLcode ret = curl_easy_perform(curl);
	if(ret != 0)
	{
		HcuErrorPrint("L0SVRFTP: Perform curl failed!\n");
		//curl_exit(curl);
		//exit(1);
	}
	return ret;
}

/****************ftp upload & download api******************/
HCU_FTP_SVR_STATE func_ftp_svr_upload(const HcuFtpServiceOptType_t ftp_option)
{
	HCU_FTP_SVR_STATE state;
	CURL *curl;
	CURLcode curlRes;

	FILE *fp = fopen(ftp_option.file, "r");
	if(NULL == fp)
	{
		//HcuErrorPrint("L0SVRFTP: Open file failed at %s:%d\n\n\n\n\n\n", __FILE__, __LINE__);
		HcuErrorPrint("L0SVRFTP: Open file failed at %s\n\n\n", ftp_option.file);
		return HCU_FTP_SVR_UPLOAD_FAILED;
	}

	curl = curl_init();
	curl_set_upload_opt(curl, ftp_option.url, ftp_option.user_key, fp);
	curlRes = curl_perform(curl);
	HcuDebugPrint("L0SVRFTP: curl perform result [%d]\n", curlRes);

	if(CURLE_OK == curlRes)
		state = HCU_FTP_SVR_UPLOAD_SUCCESS;
	else
		state = HCU_FTP_SVR_DOWNLOAD_FAILED;

	curl_exit(curl);
	fclose(fp);
	return state;
}

HCU_FTP_SVR_STATE func_ftp_svr_download(const HcuFtpServiceOptType_t ftp_option)
{
	HCU_FTP_SVR_STATE state;
	CURL *curl;
	CURLcode curlRes;

	//打开本地文件
	FILE *fp = fopen(ftp_option.file, "w");
	if(NULL == fp){
		HcuErrorPrint("L0SVRFTP: Open file failed at %s\n", ftp_option.file);
		return HCU_FTP_SVR_UPLOAD_FAILED;
	}

	//初始化CURL连接
	curl = curl_init();
	if (curl == NULL){
		fclose(fp);
		HcuErrorPrint("L0SVRFTP: Open curl error!\n");
		return HCU_FTP_SVR_DOWNLOAD_FAILED;
	}

	//下载
	curl_set_download_opt(curl, ftp_option.url, ftp_option.user_key, fp);
	curlRes = curl_perform(curl);
	if(CURLE_OK == curlRes){
		state = HCU_FTP_SVR_DOWNLOAD_SUCCESS;
	}
	else{
		HcuErrorPrint("L0SVRFTP: curl perform error result [%d]\n", curlRes);
		state = HCU_FTP_SVR_DOWNLOAD_FAILED;
	}

	//CLOSE
	curl_exit(curl);
	fclose(fp);
	return state;
}

OPSTAT hcu_service_ftp_sw_download_by_ftp(char *filename)
{
	HcuFtpServiceOptType_t ftp_opt;
	char usrtmp[3] = ":";

	//初始化
	memset( (void *)&ftp_opt, 0, sizeof(HcuFtpServiceOptType_t));
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpUser);
	strcat(ftp_opt.user_key, usrtmp);
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpPwd);
	HCU_DEBUG_PRINT_INF("L0SVRFTP: ftp_opt.user_key: %s \n", ftp_opt.user_key);

	strcat(ftp_opt.url, zHcuSysEngPar.cloud.cloudFtpAdd);
	strcat(ftp_opt.url, filename);
	HCU_DEBUG_PRINT_INF("L0SVRFTP: ftp_opt.url: %s \n", ftp_opt.url);

	//直接使用上层传来的目录结构
	strcat(ftp_opt.file, zHcuSysEngPar.swm.hcuSwActiveDir);
	strcat(ftp_opt.file, filename);
	HCU_DEBUG_PRINT_INF("L0SVRFTP: ftp_opt.file: %s \n", ftp_opt.file);

	if(HCU_FTP_SVR_DOWNLOAD_SUCCESS == func_ftp_svr_download(ftp_opt)){
		HCU_DEBUG_PRINT_INF("L0SVRFTP: HCU SW Download success.\n");
		return SUCCESS;
	}else{
		HCU_ERROR_PRINT_TASK(TASK_ID_SVRCON, "L0SVRFTP: HCU SW Download failed.\n");
	}

	return SUCCESS;
}



