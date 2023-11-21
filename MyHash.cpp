#include "MyHash.h"
#include <string.h>
MyHash::MyHash(HashType hash_type)
{
	switch (hash_type) {
	case T_MD5:
		MD5_Init(&md5_ctx);
		m_hash_type = T_MD5;
		//cout << 1;
		break;
	case T_SHA1:
		SHA1_Init(&sha1_ctx);
		m_hash_type = T_SHA1;
		break;
	case T_SHA224:
		SHA224_Init(&sha224_ctx);
		m_hash_type = T_SHA224;
		break;
	case T_SHA256:
		SHA256_Init(&sha256_ctx);
		m_hash_type = T_SHA256;
		break;
	case T_SHA384:
		SHA384_Init(&sha384_ctx);
		m_hash_type = T_SHA384;
		break;
	case T_SHA512:
		SHA512_Init(&sha512_ctx);
		m_hash_type = T_SHA512;
		break;
	}
}

MyHash::~MyHash()
{
}

void MyHash::addData(string hashString)
{
	switch (m_hash_type) {
	case T_MD5:
		MD5_Update(&md5_ctx, hashString.data(), strlen(hashString.data()));
		//cout << 2<<endl;
		break;
	case T_SHA1:
		SHA1_Update(&sha1_ctx, hashString.data(), strlen(hashString.data()));
		break;
	case T_SHA224:
		SHA224_Update(&sha224_ctx, hashString.data(), strlen(hashString.data()));
		break;
	case T_SHA256:
		SHA256_Update(&sha256_ctx, hashString.data(), strlen(hashString.data()));
		break;
	case T_SHA384:
		SHA384_Update(&sha384_ctx, hashString.data(), strlen(hashString.data()));
		break;
	case T_SHA512:
		SHA512_Update(&sha512_ctx, hashString.data(), strlen(hashString.data()));
		break;
	}
}

string MyHash::encode()
{
	switch (m_hash_type) {
	case T_MD5: {
		unsigned char* md = new unsigned char[MD5_DIGEST_LENGTH];
		char* res = new char[MD5_DIGEST_LENGTH * 2 + 1];
		MD5_Final(md, &md5_ctx);
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break;
	}
	case T_SHA1:
	{
		unsigned char* md = new unsigned char[SHA_DIGEST_LENGTH];
		char* res = new char[SHA_DIGEST_LENGTH * 2 + 1];
		SHA1_Final(md, &sha1_ctx);
		for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break;
	}
	case T_SHA224:
	{
		unsigned char* md = new unsigned char[SHA224_DIGEST_LENGTH];
		char* res = new char[SHA224_DIGEST_LENGTH * 2 + 1];
		SHA224_Final(md, &sha224_ctx);
		for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break;
	}
	case T_SHA256:
	{
		unsigned char* md = new unsigned char[SHA256_DIGEST_LENGTH];
		char* res = new char[SHA256_DIGEST_LENGTH * 2 + 1];
		SHA256_Final(md, &sha256_ctx);
		for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break;
	}
	case T_SHA384: {
		unsigned char* md = new unsigned char[SHA384_DIGEST_LENGTH];
		char* res = new char[SHA384_DIGEST_LENGTH * 2 + 1];
		SHA384_Final(md, &sha384_ctx);
		for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break; 
	}
	case T_SHA512: {
		unsigned char* md = new unsigned char[SHA512_DIGEST_LENGTH];
		char* res = new  char[SHA512_DIGEST_LENGTH * 2 + 1];
		SHA512_Final(md, &sha512_ctx);
		for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
			sprintf(&res[i * 2], "%02x", md[i]);
		}

		//cout << res << endl;
		return string(res);
		//break;
	}
	default:
		return NULL;
	}
	//return NULL;
}
