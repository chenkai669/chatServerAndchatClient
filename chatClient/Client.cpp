#include "Client.h"
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

std::mutex m_lock;

CClient::CClient()
{
	int m_nPort = 0;
	m_ConnectSocket = NULL;
	ZeroMemory(m_szServerIp, MAX_IP_LEN);
	ZeroMemory(m_szUser, MAX_USER_LEN);
	ZeroMemory(m_szPass, MAX_PASS_LEN);
	heatflag = true;
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
	std::thread h_th(&CClient::Heatbeat, this);
	//std::thread w_th(&CClient::WritrThread, this);
	h_th.detach();
	//w_th.detach();
	std::chrono::microseconds dura1(1000);
	std::this_thread::sleep_for(dura1);
	while (true && heatflag)
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
			iRet = RecvData(m_ConnectSocket, (char *)pszBuf, msg.m_cntLen);
			if (iRet <= 0)
			{
				cout << "recv fail2!" << endl;
				closesocket(m_ConnectSocket);
				delete[] pszBuf;
				return false;
			}
			ProcessMsg(msg, pszBuf);
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
		//Sleep(50);
		while (true)
		{
			if (SOCKET_Select(m_ConnectSocket))
			{
				CMsgHead msg;
				int iRet = RecvData(m_ConnectSocket, (char *)&msg, sizeof(msg));//先接收协议头
				if (iRet <= 0)
				{
					cout << "recv fail1!" << endl;
					closesocket(m_ConnectSocket);
					return false;
				}
				char *pszBuf = new char[msg.m_cntLen];
				memset(pszBuf, 0, 0);
				iRet = RecvData(m_ConnectSocket, (char *)pszBuf, msg.m_cntLen);//在接收协议体
				if (iRet <= 0)
				{
					cout << "recv fail2!" << endl;
					closesocket(m_ConnectSocket);
					delete[] pszBuf;
					return false;
				}
				if (*pszBuf == true)
				{
					return true;
				}
				else
					return false;
			}
		}			
	}
	return false;
}


void CClient::Heatbeat()
{
	Heatbeat_packet heat_packet;
	std::chrono::milliseconds dura(2000);
	while (true)
	{
		CMsgHead msgHead(MSG_TYPE_HEATBATE, sizeof(Heatbeat_packet));
		int iHeadSend = SendData(m_ConnectSocket, (char *)&msgHead, sizeof(msgHead));
		int iBodySend = SendData(m_ConnectSocket, (char *)&heat_packet, sizeof(heat_packet));
		if (iHeadSend > 0 && iBodySend > 0)
		{
			std::this_thread::sleep_for(dura);//1000ms发一次
		}
		else
		{
			heatflag = false;
			cout << "服务器已断开" << endl;
			return ;
		}
	}

}

bool CClient::ProcessMsg(CMsgHead &msg, const char *pCnt)
{
	switch (msg.m_type)
	{
	case MSG_TYPE_HEATBATE:
		return true;

	case MSG_TYPE_LOGIN:

		break;

	case MSG_TYPE_REQUEST_ID:
		//int s1 = *((int*)(pCnt));
		//m_lock.lock();
		//Clist.push_back(s1);
		//m_lock.unlock();
		break;
	}
	return true;
}


void CClient::WritrThread()
{
	//写之前查看在线ID
	system("start");
	m_lock.lock();
	if (!Clist.empty())
	{
		//printClient(Clist);
	}
	m_lock.unlock();

	CMsgHead msgHead(MSG_TYPE_REQUEST_ID, sizeof(0));
	int iHeadSend = SendData(m_ConnectSocket, (char *)&msgHead, sizeof(msgHead));
}

//void CClient::printClient(vector<int> v)
//{
//	m_lock.lock();
//	for (vector<int>::iterator it = v.begin(); it != v.end(); it++)
//	{
//		cout << *it<<" ";
//	}
//	m_lock.unlock();
//	cout << endl;
//}