#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <arpa/inet.h>
#include"tcp.h"

int init_client(const char *serverIp,short int port)
{
	//创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket failed");
		return -1;
	}

	//绑定服务端自己的网络地址
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.s_addr = inet_addr(serverIp);
	int r = bind(sockfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
	if(r==-1)
	{
		perror("bind failed");
		close(sockfd);
		return -1;
	}
	
	r = listen(sockfd,5);
	if(r==-1)
	{
		perror("listen failed");
		close(sockfd);
		return -1;
	}

	return sockfd;
}


/*
	功能：发送数据到客户端
	参数：
		@fd  套接字描述符
		@data	指向一段内存空间，这段空间保存了你要发送的数据
		@len 	你想要发送的数据的字节数
*/
int sendData(int fd,const unsigned char * data,int len)
{
	
	int r = send(fd,data,len,0);
	if(r==-1)
	{
		perror("send failed");
		return -1;
	}
	return 0;
}

/*
	功能:接收客户端端发过来的数据
	参数：
		@fd  套接字描述符
		@data	指向一段内存空间，这段空间用来保存接收到的数据
		@len 	表示data指向的空间最多只能保存 len个字节
	返回值：
		失败返回-1
		成功返回实际读取到的数据的字节数目
*/
int recvData(int fd,unsigned char * data,int len)
{
	int r = recv(fd,data,len,0);
	if(r==-1)
	{
		perror("recv failed");
		return -1;
	}
	
	return r;
}









