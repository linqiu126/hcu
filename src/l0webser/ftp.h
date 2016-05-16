/*
 * ftp.h
 *
 *  Created on: 16 May 2016
 *      Author: pi
 */

#ifndef L0WEBSER_FTP_H_
#define L0WEBSER_FTP_H_


/*FTP OPERATION CODE*/
typedef enum FTP_STATE
{
	FTP_UPLOAD_SUCCESS,
	FTP_UPLOAD_FAILED,
	FTP_DOWNLOAD_SUCCESS,
	FTP_DOWNLOAD_FAILED
}FTP_STATE;

/*FTP OPERATIONS OPTIONS*/
typedef struct FTP_OPT
{
	char *url;		     /*url of ftp*/
	char *user_key;		 /*username:password*/
	char *file;		     /*filepath*/
}FTP_OPT;


/*upload file to ftp server*/
FTP_STATE ftp_upload(const FTP_OPT ftp_option);

/*download file from ftp server*/
FTP_STATE ftp_download(const FTP_OPT ftp_option);



#endif /* L0WEBSER_FTP_H_ */
