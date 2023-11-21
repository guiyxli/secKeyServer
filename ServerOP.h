#pragma once
#include <map>
#include <string>
#include "Message.pb.h"
#include "TcpSocket.h"
#include "TcpServer.h"
#include "OCCIOP.h"
#include "SecKeyShm.h"
using namespace std;
class ServerOP
{
public:
#if 0
	enum KeyLen {
		LEN16 = 16,
		LEN24 = 24,
		LEN32=32
	};
#endif
	 ServerOP();
	~ServerOP();
	void startSever();//启动服务器

private:
	static void* working(void* arg);//回调函数
	string secKeyAgree(RequestMsg* msg);//
	void secKeyCheck();
	void secKeyDelete();
	string getRandKey(int len);
private:
	map<pthread_t, TcpSocket*> mMap;
	TcpServer* m_server =NULL;
	string m_serverID;
	string m_dbUser;
	string m_dbPwd;
	string m_dbconStr;
	string m_pathName;
	int  m_maxNode;
	//创建数据库的实例对象
	OCCIOP m_occi;
	SecKeyShm* m_shm;
};

