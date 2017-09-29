/*
 * ftp.h
 *
 *  Created on: 16 May 2016
 *      Author: shanchun
 */

#ifndef L0WEBSER_FTP_H_
#define L0WEBSER_FTP_H_

#include "../l0comvm/vmlayer.h"

/*FTP OPERATION CODE*/
typedef enum HCU_FTP_SVR_STATE
{
	HCU_FTP_SVR_UPLOAD_SUCCESS,
	HCU_FTP_SVR_UPLOAD_FAILED,
	HCU_FTP_SVR_DOWNLOAD_SUCCESS,
	HCU_FTP_SVR_DOWNLOAD_FAILED
}HCU_FTP_SVR_STATE;

/*FTP OPERATIONS OPTIONS*/
typedef struct HcuFtpServiceOptType
{
	char url[HCU_SYSDIM_FILE_NAME_LEN_MAX];		     /*url of ftp*/
	char user_key[HCU_SYSDIM_FILE_NAME_LEN_MAX];		 /*username:password*/
	char file[HCU_SYSDIM_FILE_NAME_LEN_MAX];		     /*filepath*/
}HcuFtpServiceOptType_t;


/*upload file to ftp server*/
HCU_FTP_SVR_STATE func_ftp_svr_upload(const HcuFtpServiceOptType_t ftp_option);

/*download file from ftp server*/
HCU_FTP_SVR_STATE func_ftp_svr_download(const HcuFtpServiceOptType_t ftp_option);

//向上提供的标准服务
OPSTAT hcu_service_ftp_sw_download_by_ftp(char *filename);
OPSTAT hcu_service_ftp_picture_upload_by_ftp(char *purefilename, char *dirfilename);

#endif /* L0WEBSER_FTP_H_ */
