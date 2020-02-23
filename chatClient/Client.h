#pragma once
#include"../../../Common/Common.h"

class CClient
{
public:
	CClient();
	~CClient();

public:
	bool ConnectToServer(int port);
	bool LoginProcess();

private:
	char m_szServerIp[MAX_IP_LEN];
	int m_nPort;
	char m_szUser[MAX_USER_LEN];
	char m_szPass[MAX_PASS_LEN];
	SOCKET m_ConnectSocket;
};

