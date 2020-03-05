#pragma once

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <list>

#define MAX_IP_LEN 16
#define MAX_USER_LEN 20
#define MAX_PASS_LEN 20


bool WinSockInit(); //socket库初始化
void WinSockUnLoad();//socket库卸载
bool SOCKET_Select(SOCKET sock);
int SendData(SOCKET sock, char *buf, unsigned int len);
int RecvData(SOCKET sock, char *buf, unsigned int len);


/////////message type////////////////
#define MSG_TYPE_HEATBATE  0x0
#define MSG_TYPE_LOGIN  0x1 
#define MSG_TYPE_CHAT  0x2
#define MSG_TYPE_CHATS  0x3
#define MSG_TYPE_REQUEST_ID  0x4
#define MSG_TYPE_Client_Enter  0x5


class CMsgHead//协议头
{
public:
	char m_type;//消息类型
	unsigned int m_cntLen;//消息长度

	CMsgHead()
	{
		m_type = -1;
		m_cntLen = 0;
	}
	CMsgHead(char nType)
	{
		m_type = nType;
		m_cntLen = 0;
	}
	CMsgHead(char nType,unsigned int cntLen)
	{
		m_type = nType;
		m_cntLen = cntLen;
	}

};

class CLoginInfo
{
public:
	char m_User[MAX_USER_LEN];
	char m_Pass[MAX_PASS_LEN];

	CLoginInfo()
	{
		ZeroMemory(m_User, MAX_USER_LEN);
		ZeroMemory(m_Pass, MAX_PASS_LEN);
	}

	CLoginInfo(char *user,char *pass)
	{
		if (!user || !pass)
		{
			return;
		}
		ZeroMemory(m_User, MAX_USER_LEN);
		ZeroMemory(m_Pass, MAX_PASS_LEN);
		strncpy(m_User, user, MAX_USER_LEN);
		strncpy(m_Pass, pass, MAX_PASS_LEN);
	}


};


struct Heatbeat_packet
{

};
