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
		HcuErrorPrint("FTP: Init curl failed!\n");
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
		HcuErrorPrint("FTP: Perform curl failed!\n");
		//curl_exit(curl);
		//exit(1);
	}
	return ret;
}

/****************ftp upload & download api******************/
FTP_STATE ftp_upload(const FTP_OPT ftp_option)
{
	FTP_STATE state;
	CURL *curl;
	CURLcode curlRes;

	FILE *fp = fopen(ftp_option.file, "r");
	if(NULL == fp)
	{
		//HcuErrorPrint("FTP: Open file failed at %s:%d\n\n\n\n\n\n", __FILE__, __LINE__);
		HcuErrorPrint("FTP: Open file failed at %s\n\n\n", ftp_option.file);
		return FTP_UPLOAD_FAILED;
	}
	else
	{
		//HcuDebugPrint("FTP: Open file succeed at %s:%d\n\n\n\n\n\n", __FILE__, __LINE__);
		HcuDebugPrint("FTP: Open file succeed at %s\n\n\n", ftp_option.file);

	}

	curl = curl_init();
	curl_set_upload_opt(curl, ftp_option.url, ftp_option.user_key, fp);
	curlRes = curl_perform(curl);
	HcuDebugPrint("FTP: curl perform result [%d]\n\n\n", curlRes);

	if(CURLE_OK == curlRes)
		state = FTP_UPLOAD_SUCCESS;
	else
		state = FTP_DOWNLOAD_FAILED;

	curl_exit(curl);
	fclose(fp);
	return state;
}

FTP_STATE ftp_download(const FTP_OPT ftp_option)
{
	FTP_STATE state;
	CURL *curl;

	CURLcode curlRes;

	FILE *fp = fopen(ftp_option.file, "w");
	if(NULL == fp)
	{
		//HcuErrorPrint("FTP: Open file failed at %s:%d\n\n\n\n\n\n", __FILE__, __LINE__);
		HcuErrorPrint("FTP: Open file failed at %s\n\n\n", ftp_option.file);
		return FTP_UPLOAD_FAILED;
	}
	else
	{
		//HcuDebugPrint("FTP: Open file succeed at %s:%d\n\n\n\n\n\n", __FILE__, __LINE__);
		HcuDebugPrint("FTP: Open file succeed at %s\n\n\n", ftp_option.file);

	}

	curl = curl_init();
	curl_set_download_opt(curl, ftp_option.url, ftp_option.user_key, fp);

	curlRes = curl_perform(curl);
	HcuDebugPrint("FTP: curl perform result [%d]\n\n\n", curlRes);

	if(CURLE_OK == curlRes)
		state = FTP_DOWNLOAD_SUCCESS;
	else
		state = FTP_DOWNLOAD_FAILED;

	curl_exit(curl);
	fclose(fp);
	return state;
}

OPSTAT hcu_service_ftp_sw_download_by_ftp(char *filename)
{
	FTP_OPT ftp_opt;

	char usrtmp[3] = ":";

	memset( (void *)&ftp_opt, 0, sizeof(FTP_OPT));

	//ftp_opt.user_key = zHcuSysEngPar.cloud.cloudFtpUser;
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpUser);
	strcat(ftp_opt.user_key, usrtmp);
	strcat(ftp_opt.user_key, zHcuSysEngPar.cloud.cloudFtpPwd);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.user_key: %s \n", ftp_opt.user_key);

	//char filetmp[64] = "swdownload.txt";
	//ftp_opt.url = zHcuSysEngPar.cloud.cloudFtpAdd;
	strcat(ftp_opt.url, zHcuSysEngPar.cloud.cloudFtpAdd);
	strcat(ftp_opt.url, filename);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.url: %s \n", ftp_opt.url);

	//ftp_opt.file = zHcuSysEngPar.swDownload.hcuSwDownloadDir;
	strcat(ftp_opt.file, zHcuSysEngPar.swm.hcuSwDownloadDir);
	strcat(ftp_opt.file, filename);
	HCU_DEBUG_PRINT_NOR("CLOUDVELA: ftp_opt.file: %s \n", ftp_opt.file);

	if(FTP_DOWNLOAD_SUCCESS == ftp_download(ftp_opt)){
		HCU_DEBUG_PRINT_NOR("CLOUDVELA: HCU SW Download success.\n");
		return SUCCESS;
	}else{
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "CLOUDVELA: HCU SW Download failed.\n");
	}

	return SUCCESS;
}



