
#ifndef __TCP_H__
#define __TCP_H__
//头文件中一般是写声明语句，如类型声明，函数声明.....

/*
	功能：服务端初始化
	参数：
		@serverIp 服务端自己的ip地址
		@port	服务端自己的端口号
	返回值：
		失败返回-1
		成功返回 套接字文件描述符
*/
int init_client(const char *serverIp,short int port);

/*
	功能：发送数据到客户端
	参数：
		@fd  套接字描述符
		@data	指向一段内存空间，这段空间保存了你要发送的数据
		@len 	你想要发送的数据的字节数
*/
int sendData(int fd,const unsigned char * data,int len);

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
int recvData(int fd,unsigned char * data,int len);
#endif