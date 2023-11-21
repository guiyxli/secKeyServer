#pragma once
#include <string>
#include "TcpSocket.h"
//#define IPADDRESS "192.168.1.1"
using namespace std;
//#define IPADDRESS "127.0.0.1"
class TcpServer//ͨ��ǰ�Ĵ���
{
public:
	 TcpServer();
	~TcpServer();
	int createListen();//��������������
	//�ȴ��ͻ�������
	TcpSocket* acceptConnect(int wait_seconds);
private:
	void initSocket();//��ʼ���׽��ֽṹ��
	void error_die(char* name);
	void readConfigFile();
private:
	int m_lfd;//�����ļ�������
	struct sockaddr_in serveraddr;
	unsigned short mPort;
	string mIP;
};

