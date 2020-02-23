
#include "Common.h"

bool WinSockInit() //socket库初始化
{
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR)
	{
		return true;
	}
	else {
		return false;
	}
}

void WinSockUnLoad()//socket库卸载
{
	WSACleanup();
}

int SendData(SOCKET sock, char *buf, unsigned int len)
{
	unsigned int offset = 0;//读取缓冲区偏移量
	while (offset < len)
	{
		int nSend = send(sock, buf + offset, len - offset, 0);
		if (nSend == SOCKET_ERROR)
		{
			return -1;
		}
		if (nSend == 0)
			return 0;
		offset += nSend;
	}
	return 1;
}

int RecvData(SOCKET sock, char *buf, unsigned int len)
{
	unsigned int offset = 0;//读取缓冲区偏移量
	while (offset < len)
	{
		int nRecv = recv(sock, buf + offset, len - offset, 0); 
		if (nRecv == SOCKET_ERROR)
		{
			return -1;
		}
		if (nRecv == 0)
			return 0;
		offset += nRecv;
	}
	return 1;
}

bool SOCKET_Select(SOCKET  m_sock)
{
	FD_SET fdRead;
	int nRet = 0;
	TIMEVAL tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	//初始化fd_set
	FD_ZERO(&fdRead);
	//将套接字设置到集合里
	FD_SET(m_sock, &fdRead);

	nRet = select(0, &fdRead, NULL, NULL, &tv);
	if (!FD_ISSET(m_sock, &fdRead))//检查 s是否s e t集合的一名成员；如答案是肯定的是，则返回 T R U E。
	{
		return false;
	}
	return true;
}
