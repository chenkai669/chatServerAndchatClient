#include"Client.h"

int main()
{
	WinSockInit();
	CClient client;
	client.ConnectToServer(9090);
	return 0;
}