#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"
#include "tcp.h"
// 处理客户端发来的命令请求
int disClientCmd(int connfd, const char *clientIP)
{
	unsigned char cmd[100];
	int r = recvData(connfd, cmd, 100);
	if (r == -1)
	{
		perror("recvData error");
		return -1;
	}

	// 实际读取的命令数据为r个字节
	int len;
	int shutdown = 1;
	if (cmd[0] == 0xc0 && cmd[r - 1] == 0xc0)
	{
		// 有效数据
		// 根据命令号，进行不同的处理
		switch (cmd[1])
		{
		case FTP_CLOSE_CMD:
			printf("\n收到[%s]关闭连接请求\n", clientIP);
			shutdown = disCloseCmd(connfd, clientIP);
			if (shutdown == -1)
			{
				printf("[%s] 连接已断开\n", clientIP);
				return -1;
			}
			break;
		case FTP_LS_CMD:
			// 处理ls命令
			printf("\n收到[%s]查看文件列表请求\n", clientIP);
			disLsCmd(connfd, clientIP);
			break;
		case FTP_UPLOAD_CMD:
			// 处理上传文件命令
			len = cmd[2];
			printf("\n收到[%s]上传文件请求: %s\n", clientIP, &cmd[3]);
			disUpLoadCmd(connfd, &cmd[3], clientIP);
			break;

		case FTP_DOWNLOAD_CMD:
			// 处理下载文件命令
			len = cmd[2];
			printf("\n收到[%s]下载文件请求: %s\n", clientIP, &cmd[3]);
			disDownLoadCmd(connfd, &cmd[3], clientIP);
			break;
		}
	}
}

void disLsCmd(int connfd, const char *clientIP)
{
	// 获取服务端 指定路径下的所有文件名字
	unsigned char data[32];
	int ok = OK;
	unsigned char filename[1024] = "";
	int len = 0;

	printf("[%s] 正在获取文件列表...\n", clientIP);
	DIR *p = opendir(PATHNAME);
	if (p == NULL)
	{
		ok = ERROR;
		printf("[%s] 错误: 无法打开目录 %s\n", clientIP, PATHNAME);
	}
	else
	{
		struct dirent *entp;
		while (1)
		{
			entp = readdir(p);
			if (entp == NULL)
			{
				break;
			}
			// 过滤掉"."和".."目录
			if (strcmp(entp->d_name, ".") == 0 || strcmp(entp->d_name, "..") == 0)
			{
				continue;
			}
			// 把文件名存到 filename数组中
			// 并且要记录下 所有文件名的总长度
			len += sprintf(filename + len, "%s\n", entp->d_name);
		}
		printf("[%s] 成功获取文件列表\n", clientIP);
	}
	closedir(p);
	// 回复客户端一个数据包，表示是否成功
	int i = 0;
	data[i] = 0xc0;
	i++;
	data[i] = ok;
	i++;
	data[i] = 0xc0;
	i++;

	sendData(connfd, data, i);

	if (ok == OK) // 只有成功了，才需要继续回复，回复内容是文件名
	{
		sendData(connfd, filename, len);
	}
}

void disDownLoadCmd(int connfd, const char *filename, const char *clientIP)
{
	unsigned char data[32];
	int ok = OK;
	char pathname[100] = "";
	sprintf(pathname, "%s/%s", PATHNAME, filename);

	printf("正在处理[%s]的下载请求: %s\n", clientIP, filename);
	printf("文件路径: %s\n", pathname);

	int fd = open(pathname, O_RDONLY);
	if (fd == -1)
	{
		perror(" 错误");
		ok = ERROR;
		printf("[%s] 无法打开文件: %s\n", clientIP, pathname);
	}
	else
	{
		printf("[%s] 成功打开文件，准备发送...\n", clientIP);
	}

	int i = 0;
	data[i] = 0xc0;
	i++;
	data[i] = ok;
	i++;
	data[i] = 0xc0;
	i++;
	sendData(connfd, data, i); // 回复是否成功

	if (ok == OK) // 如果成功继续发送文件内容
	{
		char filedata[1024];
		int total_bytes = 0;
		while (1) // 循环发送数据，直到全部发送完毕
		{
			int i = 0;
			filedata[i++] = 0xc0; // 包头
			filedata[i++] = FTP_DOWNLOAD_CMD;
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
				printf("[%s] 文件发送完成，总字节数: %d\n", clientIP, total_bytes);
			}

			filedata[r + 3] = 0xc0;

			sendData(connfd, filedata, r + 4);

			if (r < 1020)
			{
				break;
			}
		}
		close(fd);
	}
}
void disUpLoadCmd(int connfd, const char *filename, const char *clientIP)
{
	unsigned char data[32];
	int ok = OK;
	char pathname[100] = "";
	sprintf(pathname, "%s/%s", PATHNAME, filename);

	printf("正在处理[%s]的上传请求: %s\n", clientIP, filename);
	printf("文件路径: %s\n", pathname);

	int fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd == -1)
	{
		perror(" 错误");
		ok = ERROR;
		printf("无法创建文件: %s\n", pathname);
	}
	else
	{
		printf("成功创建文件，准备接收...\n");
	}

	int i = 0;
	data[i] = 0xc0;
	i++;
	data[i] = ok;
	i++;
	data[i] = 0xc0;
	i++;
	int r = sendData(connfd, data, i); // 回复是否成功

	unsigned char resp[32];
	r = recvData(connfd, resp, 3); // 接收服务端的是否成功的数据包

	unsigned char filedata[1024];
	if (resp[0] == 0xc0 && resp[r - 1] == 0xc0)
	{
		// 有效数据
		if (resp[1] == OK)
		{
			// 接着接收客户端发送过来的数据
			// 把收到的数据存到服务端本地文件中
			int total_bytes = 0;
			while (1) // 循环接收，直到收到客户端发来的最后一个包
			{
				char filedata[1024];
				int r = recvData(connfd, filedata, 1024);
				total_bytes += (r - 4);

				write(fd, filedata + 3, r - 4); // 真正文件数据是从filename[3]开始的，并且最后一个字节是包尾

				if (filedata[2] == 1)
				{
					printf("文件接收完成，总字节数: %d\n", total_bytes);
					break;
				}
			}
			close(fd);
		}
		else
		{
			printf("[%s] 客户端取消上传\n", clientIP);
			return;
		}
	}

	close(fd);
}
int disCloseCmd(int connfd, const char *clientIP)
{
	// 获取服务端 指定路径下的所有文件名字
	unsigned char data[32];
	int ok = OK;
	// 回复客户端一个数据包，表示是否成功
	int i = 0;
	data[i] = 0xc0;
	i++;
	data[i] = ok;
	i++;
	data[i] = 0xc0;
	i++;
	sendData(connfd, data, i);
	printf("[%s] 正在断开连接...\n", clientIP);
	return -1;
}