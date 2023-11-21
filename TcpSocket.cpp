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
		// ��ӵ�4�ֽ���Ϊ����ͷ, �洢���ݿ鳤��
		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL)
		{
			//ret = MallocError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		// ת��Ϊ�����ֽ���
		int netlen = htonl((int)sendData.size());
		memcpy(netdata, &netlen, 4);
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// û���ⷵ�ط��͵�ʵ���ֽ���, Ӧ�� == �ڶ�������: dataLen
		// ʧ�ܷ���: -1
		writed = writen(netdata, dataLen);
		if (writed < dataLen)	// ����ʧ��
		{
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}
		// �ͷ��ڴ�
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
		// ���ݰ�ͷ�м�¼�����ݴ�С�����ڴ�, ��������
		char* tmpBuf = (char*)malloc(dataLen + 1);
		if (tmpBuf == NULL)
		{
			//ret = MallocError;
			printf("malloc() err \n");
			return NULL;
		}

		ret = readn(tmpBuf, dataLen); //���ݳ��ȶ�����
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

		tmpBuf[dataLen] = '\0'; //�����һ���ֽ����ݣ����ݿɼ��ַ��� �ַ�������ʵ������ȻΪn
		string data = string(tmpBuf);
		// �ͷ��ڴ�
		free(tmpBuf);

		return data;
	}
}
#endif
string TcpSocket::recvMsg(unsigned int timeout)
{
	// ����0 -> û��ʱ�ͽ��յ�������, -1, ��ʱ�����쳣
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
	ret = readn(&netdatalen, 4); //����ͷ 4���ֽ�
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
	// ���ݰ�ͷ�м�¼�����ݴ�С�����ڴ�, ��������
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL)
	{
		ret = MallocError;
		printf("malloc() err \n");
		return NULL;
	}

	ret = readn(tmpBuf, n); //���ݳ��ȶ�����
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

	tmpBuf[n] = '\0'; //�����һ���ֽ����ݣ����ݿɼ��ַ��� �ַ�������ʵ������ȻΪn
	string data = string(tmpBuf);
	// �ͷ��ڴ�
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

		//select����ֵ��̬
		//1 ��timeoutʱ�䵽����ʱ����û�м�⵽���¼� ret����=0
		//2 ��ret����<0 &&  errno == EINTR ˵��select�Ĺ����б�����ź��жϣ����ж�˯��ԭ��
		//2-1 ������-1��select����
		//3 ��ret����ֵ>0 ��ʾ��read�¼������������¼������ĸ���

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

		// ��ʱ
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
		{
			ret = 0;	// û��ʱ
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
			if (errno == EINTR)	// ���źŴ��
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
