#pragma once
#include <string>
#include <iostream>
#include <openssl/md5.h>
#include <openssl/sha.h>
using namespace std;
enum HashType {
	T_MD5,
	T_SHA1,
	T_SHA224,
	T_SHA256,
	T_SHA384,
	T_SHA512
};
class MyHash
{
public:

	MyHash(HashType hash_type);
	~MyHash();
	void addData(string hashString);
	string encode();
private:
	HashType m_hash_type;
	MD5_CTX md5_ctx;
	SHA_CTX sha1_ctx;
	SHA256_CTX sha224_ctx, sha256_ctx;
	SHA512_CTX sha384_ctx, sha512_ctx;

};

