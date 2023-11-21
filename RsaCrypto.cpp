#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <string.h>
#include <iostream>

Cryptographic::Cryptographic()
{
	m_publicKey = RSA_new();
	m_privateKey = RSA_new();
}

Cryptographic::Cryptographic(string fileName, bool isPrivate)
{
	m_publicKey = RSA_new();
	m_privateKey = RSA_new();
	if (isPrivate)
	{
		initPrivateKey(fileName);
	}
	else
	{
		initPublicKey(fileName);
	}
}


Cryptographic::~Cryptographic()
{
	RSA_free(m_publicKey);
	RSA_free(m_privateKey);
}

void Cryptographic::generateRsakey(int bits, string pub, string pri)
{
	RSA* r = RSA_new();
	// 生成RSA密钥对
	// 创建bignum对象
	BIGNUM* e = BN_new();
	// 初始化bignum对象
	BN_set_word(e, 456787);
	RSA_generate_key_ex(r, bits, e, NULL);

	// 创建bio文件对象
	BIO* pubIO = BIO_new_file(pub.data(), "w");
	// 公钥以pem格式写入到文件中
	PEM_write_bio_RSAPublicKey(pubIO, r);
	// 缓存中的数据刷到文件中
	BIO_flush(pubIO);
	BIO_free(pubIO);

	// 创建bio对象
	BIO* priBio = BIO_new_file(pri.data(), "w");
	// 私钥以pem格式写入文件中
	PEM_write_bio_RSAPrivateKey(priBio, r, NULL, NULL, 0, NULL, NULL);
	BIO_flush(priBio);
	BIO_free(priBio);

	// 得到公钥和私钥
	m_privateKey = RSAPrivateKey_dup(r);
	m_publicKey = RSAPublicKey_dup(r);
	
	// 释放资源
	BN_free(e);
	RSA_free(r);
}

bool Cryptographic::initPublicKey(string pubfile)
{
	// 通过BIO读文件
	BIO* pubBio = BIO_new_file(pubfile.data(), "r");
	// 将bio中的pem数据读出
	if (PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL)
	{
		ERR_print_errors_fp(stdout);
		return false;
	}
	BIO_free(pubBio);
	return true;
}

bool Cryptographic::initPrivateKey(string prifile)
{
	// 通过bio读文件
	BIO* priBio = BIO_new_file(prifile.data(), "r");
	// 将bio中的pem数据读出
	if (PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL)
	{
		ERR_print_errors_fp(stdout);
		return false;
	}
	BIO_free(priBio);
	return true;
}

string Cryptographic::rsaPubKeyEncrypt(string data)
{
	// 计算公钥长度
	int keyLen = RSA_size(m_publicKey);
	cout << "pubKey len: " << keyLen << endl;
	// 申请内存空间
	char* encode = new char[keyLen + 1];
	// 使用公钥加密
	int ret = RSA_public_encrypt((int)data.size(), (const unsigned char*)data.data(),
		(unsigned char*)encode, m_publicKey, RSA_PKCS1_PADDING);
	string retStr = string();
	if (ret >= 0)
	{
		// 加密成功
		retStr = toBase64(encode, ret);
	}
	// 释放资源
	delete[]encode;
	return retStr;
}

string Cryptographic::rsaPriKeyDecrypt(string encData)
{
	cout << "enter private decode" << endl;
	// 计算私钥长度
	char* text = fromBase64(encData);
	int keyLen = RSA_size(m_privateKey);
	// 使用私钥解密
	char* decode = new char[keyLen + 1];
	int ret = RSA_private_decrypt(keyLen, (const unsigned char*)text,
		(unsigned char*)decode, m_privateKey, RSA_PKCS1_PADDING);
	string retStr = string();
	if (ret >= 0)
	{
		retStr = string(decode, ret);
	}
	delete[]decode;
	cout << "retStr:" << retStr << endl;
	return retStr;
}

string Cryptographic::rsaSign(string data, SignLevel level)
{

	unsigned int len;
	char* signBuf = new char[1024];
	memset(signBuf, 0, 1024);
	int ret  = RSA_sign(NID_sha1, (const unsigned char*)data.data(), data.size(), (unsigned char*)signBuf,
			&len, m_privateKey);
	cout << "sign len: " << len << endl;
	string retStr = toBase64(signBuf, len);
	delete[] signBuf;
	return retStr;
}

bool Cryptographic::rsaVerify(string data, string signData, SignLevel level)
{
	// 验证签名
	int keyLen = RSA_size(m_publicKey);
	char* sign = fromBase64(signData);
	int ret = RSA_verify(NID_sha1, (const unsigned char*)data.data(), (unsigned int)data.size(),
		(const unsigned char*)sign, (unsigned int)keyLen, m_publicKey);
	delete[] sign;
	cout << "ret:" << ret<<endl;
	if (ret != 1)
	{
		return false;
	}
	return true;
}
string Cryptographic::toBase64(const char* str, int len)
{
	BIO* mem = BIO_new(BIO_s_mem());
	BIO* bs64 = BIO_new(BIO_f_base64());
	// mem添加到bs64中
	bs64 = BIO_push(bs64, mem);
	// 写数据
	BIO_write(bs64, str, len);
	BIO_flush(bs64);
	// 得到内存对象指针
	BUF_MEM* memPtr;
	BIO_get_mem_ptr(bs64, &memPtr);
	string retStr = string(memPtr->data, memPtr->length - 1);
	BIO_free_all(bs64);
	return retStr;
}

char* Cryptographic::fromBase64(string str)
{
	int length = str.size();
	BIO* bs64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new_mem_buf(str.data(), length);
	BIO_push(bs64, mem);
	char* buffer = new char[length];
	memset(buffer, 0, length);
	BIO_read(bs64, buffer, length);
	BIO_free_all(bs64);

	return buffer;
}

