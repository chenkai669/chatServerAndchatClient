#pragma once
#include"../../Common/Common.h"




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


class CServer
{
public:
	CServer();
	~CServer();
	CServer *getServer();


	bool StartServer(int port);
	int  acceptProc();
	bool ClentThreadProc(CClientItem *);
	void RemoveTargetClient(CClientItem *);
	bool ProcessMsg(CMsgHead&, const char *, CClientItem *);
	//bool CheckUserInfo(LPCTSTR, LPCTSTR);
	//bool SendOtherUserInfo(CClientItem *);

private:
	SOCKET m_ListemSock;
	int m_nPort;
	std::list<CClientItem*> m_ClientList;
};

