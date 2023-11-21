#include "MyRSA.h"
#include <openssl/pem.h>
#include <iostream>
using namespace std;
MyRSA::MyRSA()
{
	publicRsa = RSA_new();
	privateRsa = RSA_new();
}

MyRSA::MyRSA(string fileName,bool isPrivate)
{
	publicRsa = RSA_new();
	privateRsa = RSA_new();
	if (isPrivate) {
		initPrivateKey(fileName);
	}
	else {
		cout << "enter initPublickey" << endl;
		initPublicKey(fileName);
	}
}

MyRSA::~MyRSA()
{
	RSA_free(publicRsa);
	RSA_free(privateRsa);
}

void MyRSA::generateRsaKey()
{
	RSA* rsa = RSA_new();
	BIGNUM* e = BN_new();
	BN_set_word(e, 12345);
	//������Կ��
	RSA_generate_key_ex(rsa, 1024, e, NULL);
	//����Կ��д�����,��Ҫ����
#if 0
	FILE* p = fopen("public.pem", "w");
	PEM_write_RSAPublicKey(p, rsa);
	fclose(p);
	p = fopen("private.pem", "w");
	PEM_write_RSAPrivateKey(p, rsa, NULL, NULL,0, NULL, NULL);
#endif
	BIO* bio = BIO_new_file("public.pem", "w");
	PEM_write_bio_RSAPublicKey(bio, rsa);
	BIO_flush(bio);
	//�ͷ���Դ
	BIO_free(bio);
	bio = BIO_new_file("private.pem", "w");
	PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_flush(bio);
	//��ȡ��Կ��˽Կ
	publicRsa = RSAPublicKey_dup(rsa);
	privateRsa = RSAPrivateKey_dup(rsa);
	BIO_free(bio);
	RSA_free(rsa);
}

string MyRSA::encryptPublicKey(string data)
{
	//׼����������
	//string text = "��̸ı�����";
	//׼����Կ
	//FILE* p = fopen("public.pem", "r");
#if 0
	BIO* bio = BIO_new_file("public.pem", "r");
	//publicRsa = RSA_new();
	if (PEM_read_bio_RSAPublicKey(bio, &publicRsa, NULL, NULL) == NULL) {
		cout << "��ȡʧ��";
		return NULL;
	}
	BIO_free(bio);//�ͷ���Դ
#endif
	//���ݱ�����֮�󣬳��Ⱥ���Կ����һ��
	//ͨ������������Կ����
	//cout << "test publicRsa" << publicRsa<<endl;
	cout << "enter encodePublicKey" << endl;
	int keyLen = RSA_size(publicRsa);
	cout << "test publicRsa" << endl;
	char* buf = new char[keyLen+1];
	cout<<"test KeyLen:"<<keyLen<<endl;
	int len = RSA_public_encrypt((int)data.size(), (const unsigned char*)data.data(), (unsigned char*)buf,publicRsa,RSA_PKCS1_PADDING);
	cout << "test KeyLen2" << endl;
	RSA_free(publicRsa);
	delete[] buf;
	return string(buf,len);

}

string MyRSA::decryptPrivateKey(string enc)
{

	//��ȡ˽Կ
#if 0
	BIO* bio = BIO_new_file("private-1.pem", "r");
	//privateRsa = RSA_new();
	if (PEM_read_bio_RSAPrivateKey(bio, &privateRsa, NULL, NULL) == NULL) {
		cout << "��ȡ2ʧ��";
		return NULL;
	}
	BIO_free(bio);
#endif
	int keyLen = RSA_size(privateRsa);
	char* buf = new char[keyLen];
	//��ʼ����
	int len = RSA_private_decrypt((int)enc.size(), (const unsigned char*)enc.data(), (unsigned char*)buf, privateRsa, RSA_PKCS1_PADDING);
	RSA_free(privateRsa);
	return string(buf,len);
}

string MyRSA::sign(string data)
{
#if 0
	//1.ǩ������
	//string text = "��̸ı�����";
	BIO* bio = BIO_new_file("private-1.pem", "r");
	privateRsa = RSA_new();
	PEM_read_bio_RSAPrivateKey(bio, &privateRsa, NULL, NULL);
	BIO_free(bio);

	//2.��Կ
	//3.ǩ��
#endif
	int keyLen = RSA_size(privateRsa);
	unsigned char* out = new unsigned char[keyLen];
	unsigned int outLen = 0;
	RSA_sign(NID_sha1, (const unsigned char*)data.data(), (unsigned int)data.size(), out, &outLen, privateRsa);
	RSA_free(privateRsa);
	return string((char*)out, (int)outLen);

}
bool MyRSA::verifySign(string text, string enc)
{
#if 0
	BIO* bio = BIO_new_file("public-1.pem", "r");
	publicRsa = RSA_new();
	PEM_read_bio_RSAPublicKey(bio, &publicRsa, NULL, NULL);
	BIO_free(bio);
#endif
	int ret = RSA_verify(NID_sha1, (const unsigned char*)text.data(), (unsigned int)text.size(), (const unsigned char*)enc.data(), (unsigned int)enc.size(), publicRsa);
	if (ret) {
		cout << "data true" << endl;
	}
		RSA_free(publicRsa);
		return true;
}

bool MyRSA::initPrivateKey(string privateFile)
{

	BIO* privateBio= BIO_new_file(privateFile.data(), "r");
	//��ȡ����
	if (PEM_read_bio_RSAPrivateKey(privateBio, &privateRsa, NULL, NULL) == NULL) {
		return false;
	}
	BIO_free(privateBio);
	return true;
}

bool MyRSA::initPublicKey(string pubFile)
{
	BIO* pubBio = BIO_new_file(pubFile.data(), "r");
	//��ȡ����
	if (PEM_read_bio_RSAPublicKey(pubBio, &publicRsa, NULL, NULL) == NULL) {
		return false;
	}
	cout << "initPublicKey:" << publicRsa<<endl;
	BIO_free(pubBio);
	return true;

}
