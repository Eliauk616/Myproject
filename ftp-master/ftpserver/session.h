#ifndef __SESSION_H__
#define __SESSION_H__
#define PATHNAME "/mnt/hgfs/daima/ftp-master/severfile"

#define FTP_CLOSE_CMD 0
#define FTP_LS_CMD 1
#define FTP_UPLOAD_CMD 2
#define FTP_DOWNLOAD_CMD 3
//......


#define OK 0
#define ERROR 1

//处理客户端发来的命令请求
int disClientCmd(int connfd, const char *clientIP);

void disLsCmd(int connfd, const char *clientIP);

void disDownLoadCmd(int connfd, const char *filename, const char *clientIP);
void disUpLoadCmd(int connfd, const char *filename, const char *clientIP);

int disCloseCmd(int connfd, const char *clientIP);
#endif
