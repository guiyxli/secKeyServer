#pragma once
#include <string>
#include  <occi.h>
#include "SecKeyNodeInfo.h"
using namespace std;
using namespace oracle::occi;
class OCCIOP
{
public:

	OCCIOP();
	~OCCIOP();
	//初始化环境连接数据库
	bool connectDb(string user, string passwd, string connstr);
	//得到keyID->根据实际业务需求封装的小函数
	int getKeyID();
	bool updateKeyID(int keyID);//更新密钥ID
	bool writeSecKey(SecKeyNodeInfo* pNode);
	void closeDB();
private:
	string getCurTime();//获取当前时间
private:
	Environment* m_env;
	Connection* m_conn;
};

