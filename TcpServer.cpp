#include "TcpServer.h"
#include <fstream>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json/reader.h>
using namespace Json;
TcpServer::TcpServer()
{
	ifstream ifo("config_connect.json");
	Reader r;
	Value root;
	r.parse(ifo, root);
	mPort = root["PORT"].asInt();
	mIP = root["IP"].asString();
	ifo.close();
	
}

TcpServer::~TcpServer()
{
	close(m_lfd);
}

int TcpServer::createListen()
{
	int ret = 0;
	
	initSocket();
	m_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_lfd == -1) {
		//perror("socket");
		//return -1;
		error_die("socket");
	}
	int on = 1;
	//设置端口复用
	ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret == -1) {
		//perror("setsockopt");
		//return -1;
		error_die("setsockopt");
	}
	//监听套接字绑定在本地IP和PORT
	ret = bind(m_lfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (ret == -1) {
		perror("bind");
		return -1;
	}
	ret = listen(m_lfd, 128);
	if (ret == -1) {
		error_die("listen");
	}
	//cout << "create successfully" << endl;
	return ret;
	
}



TcpSocket* TcpServer::acceptConnect(int wait_seconds)
{
	int ret;//此处利用IOzh
	if (wait_seconds > 0) {
		fd_set accept_fdset;
		struct timeval timeout;//阻塞时间结构体
		FD_ZERO(&accept_fdset);
		FD_SET(m_lfd, &accept_fdset);
		timeout.tv_sec = wait_seconds;//秒
		timeout.tv_usec = 0;//微秒
		do
		{
			// 检测读集合
			ret = select(m_lfd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);	// 被信号中断, 再次进入循环
		if (ret <= 0)
		{
			return NULL;
		}
	}
	// 一但检测出 有select事件发生，表示对等方完成了三次握手，客户端有新连接建立
	// 此时再调用accept将不会堵塞
	//cout << "connect successfully" << endl;
	struct sockaddr_in clientAddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int connfd = accept(m_lfd, (struct sockaddr*)&clientAddr, &addrlen); //返回已连接套接字
	if (connfd == -1)
	{
		error_die("accept");
		return NULL;
	}

	return new TcpSocket(connfd);
}

void TcpServer::initSocket()
{
	//readConfigFile();
	//cout << "test 1" << endl;
	//cout << mPort << endl;
	//cout << mIP << endl;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(mPort);
	inet_pton(AF_INET, mIP.c_str(), &serveraddr.sin_addr.s_addr);
}

void TcpServer::error_die(char* name)
{
	perror(name);
	exit(-1);
}

void TcpServer::readConfigFile()
{
	ifstream ifs("config.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	mIP = root["IP"].asString();
	mPort = root["PORT"].asInt();
	ifs.close();
}
