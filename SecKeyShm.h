#pragma once
#include "BaseShm.h"
#include "SecKeyNodeInfo.h"
#include <string.h>


class SecKeyShm : public BaseShm
{
public:
	// 打开或创建一块共享内存
	// 这个操作是在父类中做的
	SecKeyShm(int key, int maxNode);
	SecKeyShm(string pathName, int maxNode);
	~SecKeyShm();

	void shmInit();
	int shmWrite(SecKeyNodeInfo* pNodeInfo);
	SecKeyNodeInfo shmRead(string clientID, string serverID);

private:
	int m_maxNode;
};

