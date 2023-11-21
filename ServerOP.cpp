#include "ServerOP.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include "Codec.h"
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondCodec.h"
#include "RespondFactory.h"
#include "TcpSocket.h"
#include "MyRSA.h"
#include "RsaCrypto.h"
#include "MyHash.h"
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <json/reader.h>
using namespace Json;
ServerOP::ServerOP()
{
	ifstream ifs("serverOP_config.json");
	Reader r;
	Value root;
	r.parse(ifs, root);
	//将root的值取出
	m_serverID = root["ServerID"].asString();
	m_dbUser = root["UserDB"].asString();
	m_dbPwd = root["PwdDB"].asString();
	m_dbconStr = root["ConnStrDB"].asString();
	m_pathName = root["SecKey"].asString();
	m_maxNode = root["MaxNode"].asInt();
	ifs.close();
	//实例化一个oracle对象，进行数据库操作
	m_occi.connectDb(m_dbUser, m_dbPwd, m_dbconStr);
	m_shm = new SecKeyShm(m_pathName, m_maxNode);

}

ServerOP::~ServerOP()
{
	if (m_server) {
		delete m_server;
	}
	if (m_shm) {
		delete m_shm;
	}
}

void ServerOP::startSever()
{
	m_server = new TcpServer();
	m_server->createListen();
	//等待并接受客户端连接
	while (1) {
		cout << "wait connecting........." << endl;
		TcpSocket* tcp = m_server->acceptConnect(1000);
		if (tcp == NULL) {
			continue;
		}
		//开始通信
		//创建子线程
		pthread_t tid;
		pthread_create(&tid, NULL, working, this);
		mMap.insert(make_pair(tid,tcp));
	}
}

void* ServerOP::working(void* arg)
{
	cout << "server working" << endl;
	sleep(2);
	string data = string();
	//参数转化
	ServerOP* serverOP = (ServerOP*)arg;
	//从op中将通信的套接字取出
	TcpSocket* tcp = serverOP->mMap[pthread_self()];
	//接受客户端的数据
	string msg = tcp->recvMsg();

	//反序列化
	CodecFactory* fac = new RequestFactory(msg);
	Codec* c = fac->createCodec();
	RequestMsg* req  =(RequestMsg*)(c->decodeMsg());
	//取出数据
	//判断客户端是请求做什么工作
	cout <<"cmdtype:"<< req->cmdtype() << endl;
	switch (req->cmdtype()) {
	case 1:
		data = serverOP->secKeyAgree(req);
		if (tcp->sendMsg(data) == 0)
			cout << "server send information successfully" << endl;
		break;
		//密钥协商
	case 2:
		//密钥校验
		break;
	case 3:
		//密钥删除
		break;
	default:
		break;
	}
	delete fac;
	delete c;
	//tcp对象如何处理
	tcp->disConnect();
	serverOP->mMap.erase(pthread_self());
	delete tcp;
	return nullptr;
}

string ServerOP::secKeyAgree(RequestMsg* msg)
{
	//对签名进行校验
	//MyRSA rsa;
	//将收到的公钥写入本地磁盘
	ofstream ofs("public.pem");
	//stringstream ret;
	//cout << "msg->data len:" << strlen(msg->data().c_str())<<endl;
	//ret << msg->data();
	ofs << msg->data();
	ofs.close();

	Cryptographic rsa("public.pem",false);
	MyHash hash(T_MD5);
	hash.addData(msg->data());
	cout << "------------------------------------" << endl;
	cout << "hash.encode:" << hash.encode() << endl;
	cout << "seckey:" << msg->data()<<endl;
	cout << "sign" << msg->sign()<<endl;
	bool ret1 = rsa.rsaVerify(hash.encode(), msg->sign());
	RespondInfo info;
	if (ret1 == false) {
		cout << "sign false" << endl;
		info.status = false;
		
	}
	else {
		//生成随机字符串->对此加密的密钥
		//cout << "start generate string" << endl;
		string key = getRandKey(16);
		cout << "key:" << key << endl;
		string secKey = rsa.rsaPubKeyEncrypt(key);
		//对此加密算法->AES
		cout << "generate successfully-2" << endl;
		//cout << "secKey:" << secKey<<endl;
		info.clientID = msg->clientid();
		info.data = secKey;
		info.secKeyID = m_occi.getKeyID();//需要数据库操作
		info.serverID = m_serverID;
		info.status = true;
	//生成密钥需要用到数据库->操作SECKINFO
		SecKeyNodeInfo Secnode;
		//strcpy(node.clientID, msg->clientid().data());
		//strcpy(node.serverID, msg->serverid().data());
		strcpy(Secnode.clientID, msg->clientid().data());
		strcpy(Secnode.serverID, msg->serverid().data());
		strcpy(Secnode.secKey, key.data());
		Secnode.seckeyID = m_occi.getKeyID();//密钥ID
		Secnode.status = 1;
		if (m_occi.writeSecKey(&Secnode)) {
			cout << "wirte secKey successfully" << endl;
			m_occi.updateKeyID(Secnode.seckeyID + 1);
			m_shm->shmWrite(&Secnode);
		}
		else {
			info.status = false;
			cout << "fail" << endl;
		}

	//用公钥加密
	//初始化加密数据

	}
	CodecFactory* fac = new RespondFactory(&info);
	Codec* c = fac->createCodec();
	string encMsg = c->encodeMsg();
	return encMsg;
}

void ServerOP::secKeyCheck()
{
}

void ServerOP::secKeyDelete()
{
}
//要求字符串包含a-z A-Z 0-9 特殊字符
string ServerOP::getRandKey(int len)
{
	//设置种子
	srand(time(NULL));
	int flag = 0;
	char* cs = "~!@#$%^&*()_+}{|;[]";
	string randStr = string();
	for (int i = 0; i < len; i++) {
		flag = rand() % 4;//4中情况
		switch (flag) {
		case 0://a-z
			randStr.append(1, 'a' + rand() % 26);
			break;
		case 1://A-Z
			randStr.append(1, 'A' + rand() % 26);
			break;
		case 2://0-9
			randStr.append(1, '0'+ rand() % 10);
			break;
		case 3://特殊字符
			randStr.append(1, cs[rand() % strlen(cs)]);
			break;
		default:
			break;
		}
	}
	return randStr;
}
