#ifndef __SESSION_H__
#define __SESSION_H__
#define PATHNAME "/mnt/hgfs/daima/ftp-master/clientfile"

#define FTP_CLOSE_CMD 0
#define FTP_LS_CMD 1
#define FTP_UPLOAD_CMD 2
#define FTP_DOWNLOAD_CMD 3
//......

#define OK 0
#define ERROR 1
/*
    函数功能：ls命令，查看服务端文件内容
    参数
    @sockfd 连接套接字
*/
void lsCmd(int sockfd);

/*
    函数功能：下载命令，下载服务端的内容到客户端
    参数
    @sockfd 连接套接字
    @filename 需要下载的文件名
*/
void downloadFile(int sockfd,const char * filename);

/*
    函数功能：上传命令，上传客户端的文件到服务端
    参数
    @sockfd 连接套接字
    @filename 需要上传的文件名
*/
void uploadFile(int sockfd,const char * filename);

/*
    函数功能：断开连接命令
    参数
    @sockfd 连接套接字
*/
int Closecmd(int sockfd);
#endif

