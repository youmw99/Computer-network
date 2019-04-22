#include <winsock2.h>
#include <windows.h>
#include <stdio.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

LPVOID SendThread(LPVOID p) {
	SOCKET ClientSocket = (SOCKET)p;
	char buf[32];
	while (1) {
		printf("[send] : ");
		gets_s(buf);
		send(ClientSocket, buf, 32, 0);
	}

	return NULL;
}
LPVOID ReceiveThread(LPVOID p) {
	SOCKET ClientSocket = (SOCKET)p;
	char buf[32] = { 0 };
	while (1) {
		recv(ClientSocket, buf, 32, 0);
		printf("[recv] : %s", buf);
	}
	return NULL;
}

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

	HANDLE thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendThread, (LPVOID)ClientSocket, 0, NULL);
	HANDLE thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveThread, (LPVOID)ClientSocket, 0, NULL);

	WaitForSingleObject(thread1, INFINITE);
	WaitForSingleObject(thread2, INFINITE);

	//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}