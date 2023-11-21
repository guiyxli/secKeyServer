#pragma once
#include <string>
#include "TcpSocket.h"
//#define IPADDRESS "192.168.1.1"
using namespace std;
//#define IPADDRESS "127.0.0.1"
class TcpServer//通信前的处理
{
public:
	 TcpServer();
	~TcpServer();
	int createListen();//服务器创建监听
	//等待客户端连接
	TcpSocket* acceptConnect(int wait_seconds);
private:
	void initSocket();//初始化套接字结构体
	void error_die(char* name);
	void readConfigFile();
private:
	int m_lfd;//监听文件描述符
	struct sockaddr_in serveraddr;
	unsigned short mPort;
	string mIP;
};

