#include <stdio.h>
#include <stdlib.h>
#include "tcp.h"
#include "session.h"

//./ftpClient 服务端ip 服务端端口号
int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		printf("[错误] 请提供服务器IP和端口号\n");
		printf("用法: %s <服务器IP> <端口号>\n", argv[0]);
		return -1;
	}

	printf("正在连接服务器 %s:%s...\n", argv[1], argv[2]);

	int sockfd = connect_server(argv[1], atoi(argv[2]));
	if (sockfd == -1)
	{
		printf("[错误] 连接服务器失败\n");
		return -1;
	}

	printf("成功连接到服务器\n");
	printf("========================================\n");

	int cmd;
	int shutdown = 0;
	while (1)
	{
		printf("\n请选择操作:\n");
		printf("========================================\n");
		printf("  0 - 退出FTP客户端\n");
		printf("  1 - 查看服务器文件列表\n");
		printf("  2 - 上传文件到服务器\n");
		printf("  3 - 从服务器下载文件\n");
		printf("========================================\n");
		printf("请输入选项编号: ");

		scanf("%d", &cmd);
		getchar();

		switch (cmd)
		{
		case 0:
			// 退出操作
			printf("\n正在断开与服务器的连接...\n");
			shutdown = Closecmd(sockfd);
			if (shutdown == -1)
			{
				printf("\n感谢使用FTP客户端,再见!\n");
				printf("========================================\n");
				return 0;
			}
			break;
		case 1:
			// ls操作
			printf("\n");
			lsCmd(sockfd);
			break;
		case 2:
			// 上传操作
			printf("\n请输入要上传的文件名: ");
			char Uploadfilename[100];
			scanf("%s", Uploadfilename);
			getchar(); // 消耗换行符
			uploadFile(sockfd, Uploadfilename);
			break;
		case 3:
			// 下载操作
			printf("\n请输入要下载的文件名: ");
			char Downloadfilename[100];
			scanf("%s", Downloadfilename);
			getchar(); // 消耗换行符
			downloadFile(sockfd, Downloadfilename);
			break;
		default:
			printf("\n[错误] 无效的选项，请重新输入\n");
			break;
		}
	}
}
