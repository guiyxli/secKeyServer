#pragma once
#include <openssl/rsa.h>
#include <string>

using namespace std;
class MyRSA
{
public:
	MyRSA();
	MyRSA(string flleName,bool isPrivate=true);
	~MyRSA();
	void generateRsaKey();//������Կ��
	string  encryptPublicKey(string data);//ʹ�ù�Կ����
	string decryptPrivateKey(string enc);//˽Կ����
	string sign(string data);//ǩ��
	bool verifySign(string text,string enc);//��֤ǩ��
private:
	bool initPrivateKey(string privateFile);
	bool initPublicKey(string pubFile);
private:
	RSA* publicRsa;
	RSA* privateRsa;
};

