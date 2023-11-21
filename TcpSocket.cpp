#include "TcpSocket.h"
#include <string.h>
#include <unistd.h>
TcpSocket::TcpSocket(int connfd)
{
	mConnfd = connfd;
}

TcpSocket::~TcpSocket()
{
}





int TcpSocket::sendMsg(string sendData, unsigned int timeout)
{
	int ret;
	if (timeout > 0) {
		
		ret = writeTimeout(timeout);
		if (ret != 0) {
			if (ret == -1 || errno == ETIMEDOUT) {
				return -1;
			}
			else {
				return -1;

			}
		}
		int writed = 0;
		int dataLen = (int)sendData.size() + 4;
		// 添加的4字节作为数据头, 存储数据块长度
		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL)
		{
			//ret = MallocError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		// 转换为网络字节序
		int netlen = htonl((int)sendData.size());
		memcpy(netdata, &netlen, 4);
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// 没问题返回发送的实际字节数, 应该 == 第二个参数: dataLen
		// 失败返回: -1
		writed = writen(netdata, dataLen);
		if (writed < dataLen)	// 发送失败
		{
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		// 释放内存
		if (netdata != NULL)
		{
			free(netdata);
			netdata = NULL;
		}
		
	}
	return ret;
}
#if 0
string TcpSocket::recvMsg(unsigned int timeout)
{
	if (timeout > 0) {
		int ret;
		ret = readTimeout(timeout);
		if (ret != 0) {
			if (ret == -1 || errno == ETIMEDOUT) {
				printf("timeout");
				return string();
			}
			else {
				printf("timeout");
				return string();
			}
		}
		int dataLen = 0;
		ret = readn(&dataLen, 4);
		if (ret == -1)
		{
			printf("func readn() err:%d \n", ret);
			return string();
		}
		else if (ret < 4)
		{
			printf("func readn() err peer closed:%d \n", ret);
			return string();
		}

		//int n = ntohl(netdatalen);
		// 根据包头中记录的数据大小申请内存, 接收数据
		char* tmpBuf = (char*)malloc(dataLen + 1);
		if (tmpBuf == NULL)
		{
			//ret = MallocError;
			printf("malloc() err \n");
			return NULL;
		}

		ret = readn(tmpBuf, dataLen); //根据长度读数据
		if (ret == -1)
		{
			printf("func readn() err:%d \n", ret);
			return string();
		}
		else if (ret < dataLen)
		{
			printf("func readn() err peer closed:%d \n", ret);
			return string();
		}

		tmpBuf[dataLen] = '\0'; //多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
		string data = string(tmpBuf);
		// 释放内存
		free(tmpBuf);

		return data;
	}
}
#endif
string TcpSocket::recvMsg(unsigned int timeout)
{
	// 返回0 -> 没超时就接收到了数据, -1, 超时或有异常
	int ret = readTimeout(timeout);
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			printf("readTimeout(timeout) err: TimeoutError \n");
			return string();
		}
		else
		{
			printf("readTimeout(timeout) err: %d \n", ret);
			return string();
		}
	}

	int netdatalen = 0;
	ret = readn(&netdatalen, 4); //读包头 4个字节
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return string();
	}
	else if (ret < 4)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}

	int n = ntohl(netdatalen);
	// 根据包头中记录的数据大小申请内存, 接收数据
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL)
	{
		ret = MallocError;
		printf("malloc() err \n");
		return NULL;
	}

	ret = readn(tmpBuf, n); //根据长度读数据
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return string();
	}
	else if (ret < n)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}

	tmpBuf[n] = '\0'; //多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
	string data = string(tmpBuf);
	// 释放内存
	free(tmpBuf);

	return data;
}

void TcpSocket::disConnect()
{
	close(mConnfd);
}

int TcpSocket::readTimeout(unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset);
		FD_SET(mConnfd, &read_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;

		//select返回值三态
		//1 若timeout时间到（超时），没有检测到读事件 ret返回=0
		//2 若ret返回<0 &&  errno == EINTR 说明select的过程中被别的信号中断（可中断睡眠原理）
		//2-1 若返回-1，select出错
		//3 若ret返回值>0 表示有read事件发生，返回事件发生的个数

		do
		{
			ret = select(mConnfd + 1, &read_fdset, NULL, NULL, &timeout);

		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
		{
			ret = 0;
		}
	}

	return ret;
}

int TcpSocket::writeTimeout(unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set write_fdset;
		struct timeval timeout;

		FD_ZERO(&write_fdset);
		FD_SET(mConnfd, &write_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(mConnfd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		// 超时
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
		{
			ret = 0;	// 没超时
		}
	}

	return ret;
}

int TcpSocket::readn(void* buf, int count)
{
	size_t nleft = count;
	ssize_t nread;
	char* bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nread = read(mConnfd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nread == 0)
		{
			return (int)(count - nleft);
		}

		bufp += nread;
		nleft -= nread;
	}

	return count;
}

int TcpSocket::writen(const void* buf, int count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char* bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nwritten = write(mConnfd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)	// 被信号打断
			{
				continue;
			}
			return -1;
		}
		else if (nwritten == 0)
		{
			continue;
		}

		bufp += nwritten;
		nleft -= nwritten;
	}

	return count;
}
