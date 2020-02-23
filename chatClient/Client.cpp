#include "Client.h"
#include<iostream>
using namespace std;

CClient::CClient()
{
	int m_nPort = 0;
	m_ConnectSocket = NULL;
	ZeroMemory(m_szServerIp, MAX_IP_LEN);
	ZeroMemory(m_szUser, MAX_USER_LEN);
	ZeroMemory(m_szPass, MAX_PASS_LEN);
}


CClient::~CClient()
{
}


bool CClient::ConnectToServer(int port)
{
	if (port < 1 || port>65535)
	{
		return false;
	}
	m_nPort = port;
	m_ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ConnectSocket == INVALID_SOCKET)
	{
		cout << "socket found fail!" << endl;
		return false;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	//网络字节序转换
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(this->m_nPort);
	if (::connect(m_ConnectSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		cout << "connect fail!" << endl;
		closesocket(m_ConnectSocket);
		return false;
	}

	//连接成功
	bool bRet = LoginProcess();
	if (!bRet)
	{
		cout << "login fail" << endl;
		closesocket(m_ConnectSocket);
		return false;
	}


	while (true)
	{
		if (SOCKET_Select(m_ConnectSocket))
		{
			CMsgHead msg;//自定义协议的协议头(消息类型 + 包体字节数)
			int iRet = RecvData(m_ConnectSocket, (char *)&msg, sizeof(msg));
			if (iRet <= 0)
			{
				cout << "recv fail1!" << endl;
				closesocket(m_ConnectSocket);
				return false;
			}
			char *pszBuf = new char[msg.m_cntLen];
			memset(pszBuf, 0, 0);
			iRet = RecvData(m_ConnectSocket, (char *)&pszBuf, msg.m_cntLen);
			if (iRet <= 0)
			{
				cout << "recv fail2!" << endl;
				closesocket(m_ConnectSocket);
				delete[] pszBuf;
				return false;
			}
		}

		
	}



	return true;
}

bool CClient::LoginProcess()
{
	strcpy(m_szUser, "abc");
	strcpy(m_szPass, "123");
	CMsgHead msgHead(MSG_TYPE_LOGIN, sizeof(CLoginInfo));
	CLoginInfo loginInfo(m_szUser, m_szPass);

	int iHeadSend = SendData(m_ConnectSocket, (char *)&msgHead, sizeof(msgHead));
	int iBodySend = SendData(m_ConnectSocket, (char *)&loginInfo, sizeof(loginInfo));
	if (iHeadSend > 0 && iBodySend > 0)
	{
		return true;
	}
	return false;
}
