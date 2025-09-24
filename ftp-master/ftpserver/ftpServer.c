#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tcp.h"
#include "session.h"

int main(int argc, char *argv[])
{
	int fd = init_client(argv[1], atoi(argv[2]));
	if (fd == -1)
	{
		printf("init_client error\n");
		return -1;
	}
	int shutdown = 0;
	while (1)
	{
		// 一直等待客户端的连接
		struct sockaddr_in clientAddr;
		socklen_t len = sizeof(clientAddr);
		int connfd = accept(fd, (struct sockaddr *)&clientAddr, &len);
		if (connfd == -1)
		{
			printf("accept error\n");
		}
		else
		{
			// 获取客户端IP地址
			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
			printf("新客户端连接: %s\n", clientIP);

			// 连接成功
			int pid = fork();
			if (pid == 0)
			{
				// 子进程
				while (1)
				{
					shutdown = disClientCmd(connfd, clientIP);
					if (shutdown == -1)
					{
						close(connfd);
						printf("客户端[%s] 已断开连接\n", clientIP);
						return 0;
					}
				}
			}
			else if (pid > 0)
			{
				// 父进程
			}
			else
			{
				// fork出错
				break;
			}
		}
	}
}
