#include "Server.h"
#include <thread>
#include<iostream>
using namespace std;




CServer* CServer::getServer()
{
	static CServer *pGServer = NULL;
	if (!pGServer) {
		pGServer = new CServer();
	}
	return pGServer;
}



CServer::CServer()
{
	 m_ListemSock = NULL;
	 m_nPort = 0;
}


CServer::~CServer()
{
}

bool CServer::StartServer(int nPort)
{
	WinSockInit();//需要初始化socket库才能继续
	try
	{
		this->m_nPort = nPort;
		std::thread th(&CServer::acceptProc,this);
		th.join();
	}
	catch (...)
	{
		cout << "catched the exception!" << endl;
		return false;
	}
	return true;
}

/* 静态成员函数不含有this指针*/
int CServer::acceptProc()
{
	cout << this->m_nPort << endl;
	this->m_ListemSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->m_ListemSock == INVALID_SOCKET)
	{
		cout << "socket found fail!" << endl;
		return false;
	}

	sockaddr_in sServer;
	sServer.sin_family = AF_INET;
	//网络字节序转换
	sServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sServer.sin_port = htons(this->m_nPort);
	if (::bind(this->m_ListemSock, (sockaddr*)&sServer, sizeof(sServer)) == SOCKET_ERROR)
	{
		cout << "bind fail!" << endl;
		return false;
	}

	if(listen(this->m_ListemSock,SOMAXCONN) == SOCKET_ERROR) //第二个参数为能处理的最大并发量，不是能连接的最大客户端数目
	{
		cout << "listen fail!" << endl;
		return false;
	}

	while (true)
	{
		sockaddr_in clientAddr = { 0 };//用于保存客户端的地址信息
		int nLen = sizeof(sockaddr_in);
		if (SOCKET_Select(this->m_ListemSock))
		{
			SOCKET accSock = accept(this->m_ListemSock, (sockaddr*)&clientAddr, &nLen);
			if (accSock == INVALID_SOCKET) {
				continue;
			}
			CClientItem *pItem = new CClientItem();
			pItem->m_Socket = accSock;
			//网络字节序转换成本地字节序
			char *strIp = inet_ntoa(clientAddr.sin_addr);
			strcpy(pItem->m_szIpAddr, strIp);
			this->m_ClientList.push_back(pItem);
			std::thread th(&CServer::ClentThreadProc, this, pItem);
			th.detach();		
		}	
	}
		return true;
}


bool CServer::ClentThreadProc(CClientItem *item)
{
	CClientItem *pClient = item;
	while (true)
	{
		if (SOCKET_Select(pClient->m_Socket))
		{
			CMsgHead msg;//自定义协议的协议头(消息类型 + 包体字节数)
			int iRet = RecvData(pClient->m_Socket, (char *)&msg, sizeof(msg));
			if (iRet <= 0)
			{
				cout << "recv fail1!" << endl;
				getServer()->RemoveTargetClient(pClient);
				return false;
			}
			char *pszBuf = new char[msg.m_cntLen];
			memset(pszBuf, 0, sizeof(char)*msg.m_cntLen);
			iRet = RecvData(pClient->m_Socket, (char *)pszBuf, msg.m_cntLen);
			if (iRet <= 0)
			{
				cout << "recv fail2!" << endl;
				getServer()->RemoveTargetClient(pClient);
				delete[] pszBuf;
				return false;
			}
			ProcessMsg(msg, pszBuf, pClient);
			delete[] pszBuf;
		}

	}
}

void CServer::RemoveTargetClient(CClientItem *pItem)
{
	if (!pItem) return;

	for (list<CClientItem*>::iterator it = m_ClientList.begin(); it != m_ClientList.end(); it++)
	{
		if ((*it)->m_Socket = pItem->m_Socket)
		{
			m_ClientList.erase(it);
			closesocket(pItem->m_Socket);
			delete pItem;
		}
	}
}
bool CServer::ProcessMsg(CMsgHead &msg, const char *pCnt, CClientItem *pClient)
{
	switch (msg.m_type)
	{
	case MSG_TYPE_HEATBATE:
	{
			SYSTEMTIME time = { 0 };
			GetLocalTime(&time);
			cout << pClient->m_Socket<< "的心跳包:" << time.wHour << ":" << time.wMinute << ":" << time.wSecond << endl;
			Heatbeat_packet heat_packet;
			CMsgHead msgHead(MSG_TYPE_HEATBATE, sizeof(Heatbeat_packet));
			int iHeadSend = SendData(pClient->m_Socket, (char *)&msgHead, sizeof(msgHead));
			int iBodySend = SendData(pClient->m_Socket, (char *)&heat_packet, sizeof(Heatbeat_packet));
			if (iHeadSend > 0 && iBodySend > 0)
			{
				return true;
			}
			else
			{
				getServer()->RemoveTargetClient(pClient);
				cout << "客户端已断开" << endl;
				return false;
			}
	}
		break;
	case MSG_TYPE_LOGIN:
	{
		CLoginInfo *info = (CLoginInfo *)pCnt;
		if (strcmp(info->m_User, "abc") == 0 && strcmp(info->m_Pass, "123") == 0)
		{
			bool result = true;
			CMsgHead msgHead(MSG_TYPE_LOGIN, sizeof(bool));
			int iHeadSend = SendData(pClient->m_Socket, (char *)&msgHead, sizeof(msgHead));
			int iBodySend = SendData(pClient->m_Socket, (char*)&result, sizeof(bool));
			if (iHeadSend > 0 && iBodySend > 0)
			{
				return true;
			}
		}
		else
		{
			bool result = false;
			CMsgHead msgHead(MSG_TYPE_LOGIN, sizeof(bool));
			int iHeadSend = SendData(pClient->m_Socket, (char *)&msgHead, sizeof(msgHead));
			int iBodySend = SendData(pClient->m_Socket, (char*)&result, sizeof(bool));
			if (iHeadSend > 0 && iBodySend > 0)
			{
				return true;
			}
			else
			{
				getServer()->RemoveTargetClient(pClient);
				cout << "客户端已断开" << endl;
				return false;
			}
		}
	}
		break;
	case MSG_TYPE_REQUEST_ID:
	{
		sendClientId(pClient);
			//std::list<CClientItem*> m_list = getClientList();
			//CMsgHead msgHead(MSG_TYPE_REQUEST_ID, sizeof(std::list<CClientItem*>));
			//int iHeadSend = SendData(pClient->m_Socket, (char *)&msgHead, sizeof(msgHead));
			//int iBodySend = SendData(pClient->m_Socket, (char*)&m_list, sizeof(std::list<CClientItem*>));
			//if (iHeadSend > 0 && iBodySend > 0)
			//{
			//	return true;
			//}
			//else
			//{
			//	getServer()->RemoveTargetClient(pClient);
			//	cout << "客户端已断开" << endl;
			//	return false;
			//}
	}
		break;
	}
	return true;
}

std::list<CClientItem*> CServer::getClientList()
{
	return m_ClientList;
}


bool CServer::sendClientId(CClientItem *pClient)
{
	cout << pClient->m_Socket << endl;
	std::list<CClientItem*> m_list = getClientList();


	for (std::list<CClientItem*>::iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		int num = (int)((*it)->m_Socket);
		CMsgHead msgHead(MSG_TYPE_REQUEST_ID, sizeof(int));
		int iHeadSend = SendData(pClient->m_Socket, (char *)&msgHead, sizeof(msgHead));
		int iBodySend = SendData(pClient->m_Socket, (char*)&num, sizeof(int));
		if (iHeadSend > 0 && iBodySend > 0)
		{
			return true;
		}
		else
		{
			getServer()->RemoveTargetClient(pClient);
			cout << "客户端已断开" << endl;
			return false;
		}
	}



}