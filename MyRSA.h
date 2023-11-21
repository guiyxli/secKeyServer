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
	void generateRsaKey();//生成密钥对
	string  encryptPublicKey(string data);//使用公钥加密
	string decryptPrivateKey(string enc);//私钥解密
	string sign(string data);//签名
	bool verifySign(string text,string enc);//验证签名
private:
	bool initPrivateKey(string privateFile);
	bool initPublicKey(string pubFile);
private:
	RSA* publicRsa;
	RSA* privateRsa;
};

