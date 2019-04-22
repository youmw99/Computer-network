#include <winsock2.h>
#include <stdio.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
int main(void) {
	WSADATA wsa;
	//winsock dll 로딩
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		puts("WSAStartup() error!!");
		return 0;
	}
	//====================================================================
	SOCKET ClientSocket
		= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		puts("socket error!");
		return 0;
	}
	//접속할 서버의 주소
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_port = htons(8000);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(ClientSocket, //클라이언트 소켓
		(SOCKADDR*)&ServerAddr, //접속할 서버주소
		sizeof(ServerAddr))//주소 크기
		== SOCKET_ERROR) {
		puts("connect error!");
		return 0;
	}
	char msg[32] = { 0 };

	recv(ClientSocket, msg, 32, 0);

	printf("[Welcome msg] %s\n", msg);
	
	
	while (1) {
		printf("[send msg] ");
		gets_s(msg);
		send(ClientSocket, msg, 32, 0);
		memset(msg, 0, 32);
		recv(ClientSocket, msg, 32, 0);
		printf("[echo msg] %s\n",msg);
	}
		//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}