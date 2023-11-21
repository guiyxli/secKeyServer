#include "OCCIOP.h"
#include <iostream>
#include <time.h>
OCCIOP::OCCIOP()
{
}

OCCIOP::~OCCIOP()
{
}

bool OCCIOP::connectDb(string user, string passwd, string connstr)
{
	//1.初始化连接环境
	m_env = Environment::createEnvironment("utf-8","utf-8");
	//创建连接
	m_conn = m_env->createConnection(user, passwd, connstr);
	if (m_conn == NULL) {
		cout << "failly" << endl;
		return false;
	}
	cout << "database connect successfully" << endl;
	return true;
}

int OCCIOP::getKeyID()
{
	//查询数据库
	//for update:临时加锁
	string sql = "select ikeysn from SECMNG.KEYSN for update";
	Statement* stat = m_conn->createStatement(sql);
	//执行数据库操作
	ResultSet* resSet = stat->executeQuery();
	//该表只有一条数据
	int keyID = 1;
	if (resSet->next()) {
		keyID = resSet->getInt(1);
	}
	stat->closeResultSet(resSet);
	m_conn->terminateStatement(stat);
	return keyID;
}
//密钥ID在插入的时候会自动更新，也可以手动更新
bool OCCIOP::updateKeyID(int keyID)
{
	//更新数据库
	string sql = "update SECMNG.KEYSN set ikeysn= " + to_string(keyID);
	Statement* stat = m_conn->createStatement(sql);
	//设置自动提交
	stat->setAutoCommit(true);
	//执行数据查询
	int ret = stat->executeUpdate();
	m_conn->terminateStatement(stat);
	if (ret <= 0) {
		return false;
	}
	return true;
}

bool OCCIOP::writeSecKey(SecKeyNodeInfo* pNode)
{
	// 组织待插入的sql语句
	char sql[1024] = { 0 };
	sprintf(sql, "Insert Into SECMNG.SECKEYINFO(clientid, serverid, keyid, createtime, state, seckey) \
					values ('%s', '%s', %d, to_date('%s', 'yyyy-mm-dd hh24:mi:ss') , %d, '%s') ",
		pNode->clientID, pNode->serverID, pNode->seckeyID,
		getCurTime().data(), 1, pNode->secKey);
	cout << "insert sql: " << sql << endl;
	Statement* stat = m_conn->createStatement();
	// 数据初始化
	stat->setSQL(sql);
	// 执行sql
	stat->setAutoCommit(true);
	bool bl = stat->executeUpdate(sql);
	m_conn->terminateStatement(stat);
	if (bl)
	{
		return true;
	}
	return false;
}

void OCCIOP::closeDB()
{
	m_env->terminateConnection(m_conn);
	Environment::terminateEnvironment(m_env);
}

string OCCIOP::getCurTime()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m%d %H:%M:%S", localtime(&timep));
	return tmp;
}
