# 基于linux的FTP文件传输

#### 介绍
基于c语言，linux网络编程，socket套接字的简单文件传输。客户端和服务端进行文件通信，实现互传小文件的简单功能

#### 软件架构
两个模块，ftpclient和ftpserver
ftpclient文件夹
session.c  服务端命令的处理函数
tcp.c 建立服务端套接字连接
ftpCilent.c 主函数

#### 使用说明

> session.h中的PATHNAME 设置为你自己需要存放文件的路径

客户端
> gcc ftpclient.c session.c tcp.c -o ftpclient

./ftpclient 你的ip地址 你的网络端口号  

例如 
>  ./ftpclient 192.168.8.250 6666

服务端
 > gcc ftpServer.c session.c tcp.c -o ftpServer

./ftpServer 你的ip地址 你的网络端口号 

 例如 
> ./ftpServer 192.168.8.250 6666
