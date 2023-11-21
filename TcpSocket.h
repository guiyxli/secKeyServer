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
	// ʹ��һ����������ͨ�ŵ��׽���ʵ�����׽��ֶ���
	TcpSocket(int connfd);
	~TcpSocket();
	// ��������
	int sendMsg(string sendData, unsigned int timeout = TIMEOUT);
	// ��������
	string recvMsg(unsigned int timeout = TIMEOUT);
	void disConnect();//�Ͽ�����
private:
	// ����ʱ��⺯��������������
	int readTimeout(unsigned int wait_seconds);
	// д��ʱ��⺯��, ������д����
	int writeTimeout(unsigned int wait_seconds);
	// �����ӳ�ʱ��connect����
	int readn(void* buf, int count);
	int writen(const void* buf, int count);
private:
	int mConnfd;

};
