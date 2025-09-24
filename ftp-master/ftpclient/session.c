#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "session.h"
#include "tcp.h"
/*
命令包数据格式：（协议）
0xc0(包头)	命令号(1个字节) 参数长度(1个字节) 参数内容(N个字节)  0xc0(包尾)
*/

// 发送ls命令给服务端，并接收服务端回复的数据
void lsCmd(int sockfd)
{
	printf("正在请求文件列表...\n");

	unsigned char cmd[32];
	int i = 0;
	cmd[i] = 0xc0; // 包头
	i++;
	cmd[i] = FTP_LS_CMD; // 命令号
	i++;
	cmd[i] = 0; // 参数长度
	i++;
	// 参数内容为空
	cmd[i] = 0xc0; // 包尾
	i++;
	int r = sendData(sockfd, cmd, i);

	unsigned char resp[32];
	r = recvData(sockfd, resp, 3); // 接收服务端的是否成功的数据包

	unsigned char pathname[1024];
	if (resp[0] == 0xc0 && resp[r - 1] == 0xc0)
	{
		// 有效数据
		if (resp[1] == OK)
		{
			// 接着接收服务端发送过来的数据
			int r = recvData(sockfd, pathname, 1024);
			pathname[r] = '\0';
			printf("\n服务器文件列表:\n");
			printf("----------------------------------------\n");
			printf("%s", pathname);
			printf("----------------------------------------\n");
		}
		else
		{
			printf("错误: 无法获取文件列表\n");
			return;
		}
	}
}

void downloadFile(int sockfd, const char *filename)
{
	printf("正在请求下载文件: %s\n", filename);

	unsigned char cmd[32];
	int i = 0;
	cmd[i] = 0xc0; // 包头
	i++;
	cmd[i] = FTP_DOWNLOAD_CMD; // 命令号
	i++;
	int len = strlen(filename);
	cmd[i] = len + 1; // 参数长度
	i++;

	strncpy(&cmd[i], filename, len + 1); // 参数内容
	i += len + 1;
	cmd[i] = 0xc0; // 包尾
	i++;
	int r = sendData(sockfd, cmd, i);

	unsigned char resp[32];
	r = recvData(sockfd, resp, 3); // 接收服务端的是否成功的数据包

	unsigned char filedata[1024];
	if (resp[0] == 0xc0 && resp[r - 1] == 0xc0)
	{
		// 有效数据
		if (resp[1] == OK)
		{
			// 接着接收服务端发送过来的数据
			// 把收到的数据存到客户端本地文件中
			char pathname[100] = "";
			sprintf(pathname, "%s/%s", PATHNAME, filename);
			printf("保存路径: %s\n", pathname);

			int fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0664);
			if (fd == -1)
			{
				perror("错误");
				printf("无法创建文件: %s\n", pathname);
				return;
			}

			printf("开始接收文件...\n");
			int total_bytes = 0;
			while (1) // 循环接收，直到收到服务端发来的最后一个包
			{
				char filedata[1024];
				int r = recvData(sockfd, filedata, 1024);
				total_bytes += (r - 4);

				write(fd, filedata + 3, r - 4); // 真正文件数据是从filename[3]开始的，并且最后一个字节是包尾

				if (filedata[2] == 1)
				{
					printf("文件接收完成，总字节数: %d\n", total_bytes);
					break;
				}
			}
			close(fd);
			printf("文件已成功保存: %s\n", pathname);
		}
		else
		{
			printf("错误: 服务器无法找到文件 %s\n", filename);
			return;
		}
	}
}

void uploadFile(int sockfd, const char *filename)
{
	// 上传文件的话
	// 肯定首先要告诉服务器我要上传文件的名字
	// 服务器发送一个ok后，开始上传
	printf("正在请求上传文件: %s\n", filename);

	unsigned char cmd[32];
	int i = 0;
	cmd[i] = 0xc0; // 包头
	i++;
	cmd[i] = FTP_UPLOAD_CMD; // 命令号
	i++;
	int len = strlen(filename);
	cmd[i] = len + 1; // 参数长度
	i++;

	strncpy(&cmd[i], filename, len + 1); // 参数内容
	i += len + 1;
	cmd[i] = 0xc0; // 包尾
	i++;
	int r = sendData(sockfd, cmd, i);

	unsigned char resp[32];
	r = recvData(sockfd, resp, 3); // 接收服务端的是否成功的数据包

	// 如果接收到了ok信号
	if (resp[0] == 0xc0 && resp[r - 1] == 0xc0)
	{
		// 有效数据
		if (resp[1] == OK)
		{
			printf("服务器已准备接收文件\n");
			char pathname[100] = "";
			unsigned char data[32];
			int ok = OK;
			sprintf(pathname, "%s/%s", PATHNAME, filename);
			printf("本地文件路径: %s\n", pathname);

			int fd = open(pathname, O_RDONLY);
			if (fd == -1)
			{
				perror("错误");
				ok = ERROR;
				printf("无法打开文件: %s\n", pathname);
			}

			int i = 0;
			data[i] = 0xc0;
			i++;
			data[i] = ok;
			i++;
			data[i] = 0xc0;
			i++;
			sendData(sockfd, data, i); // 回复是否成功

			if (ok == OK) // 如果成功继续发送文件内容
			{
				printf("开始上传文件...\n");
				char filedata[1024];
				int total_bytes = 0;
				while (1) // 循环发送数据，直到全部发送完毕
				{
					int i = 0;
					filedata[i++] = 0xc0; // 包头
					filedata[i++] = FTP_UPLOAD_CMD;
					// data[2] 表示是否为最后一个包，为1时表示是最后一个
					// 为0时表示不是最后一个
					int r = read(fd, &filedata[3], 1020);
					total_bytes += r;

					if (r == 1020)
					{
						filedata[2] = 0; //
					}
					else if (r < 1020)
					{
						filedata[2] = 1;
						printf("文件上传完成，总字节数: %d\n", total_bytes);
					}

					filedata[r + 3] = 0xc0;

					sendData(sockfd, filedata, r + 4);

					if (r < 1020)
					{
						break;
					}
				}
				close(fd);
				printf("文件上传成功: %s\n", filename);
			}
			else
			{
				printf("上传失败: 无法打开本地文件\n");
			}
		}
		else
		{
			printf("错误: 服务器拒绝上传文件\n");
			return;
		}
	}
}

int Closecmd(int sockfd)
{
	// 关闭命令，很明显肯定双方都要断开连接
	// 所以客户先发送命令到服务端，然后服务端根据这个命令断开连接并退出
	// 在退出之前，回复服务端一个ok指令，然后客户端也断开连接
	printf("正在请求断开连接...\n");

	unsigned char cmd[32];
	int i = 0;
	cmd[i] = 0xc0; // 包头
	i++;
	cmd[i] = FTP_CLOSE_CMD; // 命令号
	i++;
	cmd[i] = 0; // 参数长度
	i++;
	// 参数内容为空
	cmd[i] = 0xc0; // 包尾
	i++;
	int r = sendData(sockfd, cmd, i);

	unsigned char resp[32];
	r = recvData(sockfd, resp, 3); // 接收服务端的是否成功的数据包

	if (resp[0] == 0xc0 && resp[r - 1] == 0xc0)
	{
		// 有效数据
		if (resp[1] == OK)
		{
			// 接着接收服务端发送过来的数据
			printf("连接已断开\n");
			close(sockfd);
			return -1;
		}
		else
		{
			printf("错误: 断开连接失败\n");
			return -1;
		}
	}
}
