#pragma once
#include"../../../Common/Common.h"
#include<vector>

class CClientItem//客户端节点
{
public:
	SOCKET m_Socket;
	char m_szIpAddr[MAX_IP_LEN];

	CClientItem()
	{
		m_Socket = INVALID_SOCKET;
		memset(m_szIpAddr, 0, 0);
	}
};


class CClient
{
public:
	CClient();
	~CClient();

public:
	bool ConnectToServer(int port);
	bool LoginProcess();
	void Heatbeat();
	bool ProcessMsg(CMsgHead &msg, const char *pCnt);
	void WritrThread();
	//std::list<CClientItem*> getClientList();
	//void printClient(vector<int> v);

private:
	char m_szServerIp[MAX_IP_LEN];
	int m_nPort;
	char m_szUser[MAX_USER_LEN];
	char m_szPass[MAX_PASS_LEN];
	SOCKET m_ConnectSocket;
	bool heatflag;
	std::vector<int> Clist;
};


