#pragma once
#include <iostream>
#include <string>
#include <arpa/inet.h>
#define TIMEOUT 10000
using namespace std;
class TcpSocket {
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	//TcpSocket();
	// 使用一个可以用于通信的套接字实例化套接字对象
	TcpSocket(int connfd);
	~TcpSocket();
	// 发送数据
	int sendMsg(string sendData, unsigned int timeout = TIMEOUT);
	// 接收数据
	string recvMsg(unsigned int timeout = TIMEOUT);
	void disConnect();//断开连接
private:
	// 读超时检测函数，不含读操作
	int readTimeout(unsigned int wait_seconds);
	// 写超时检测函数, 不包含写操作
	int writeTimeout(unsigned int wait_seconds);
	// 带连接超时的connect函数
	int readn(void* buf, int count);
	int writen(const void* buf, int count);
private:
	int mConnfd;

};
