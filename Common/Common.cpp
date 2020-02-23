
#include "Common.h"

bool WinSockInit() //socket���ʼ��
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

void WinSockUnLoad()//socket��ж��
{
	WSACleanup();
}

int SendData(SOCKET sock, char *buf, unsigned int len)
{
	unsigned int offset = 0;//��ȡ������ƫ����
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
	unsigned int offset = 0;//��ȡ������ƫ����
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

	//��ʼ��fd_set
	FD_ZERO(&fdRead);
	//���׽������õ�������
	FD_SET(m_sock, &fdRead);

	nRet = select(0, &fdRead, NULL, NULL, &tv);
	if (!FD_ISSET(m_sock, &fdRead))//��� s�Ƿ�s e t���ϵ�һ����Ա������ǿ϶����ǣ��򷵻� T R U E��
	{
		return false;
	}
	return true;
}
