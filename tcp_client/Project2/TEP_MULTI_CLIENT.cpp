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
	char nickname[32] = { 0 };
	printf("대화명 : ");
	gets_s(nickname);
	send(ClientSocket, nickname, 32, 0);

	while (1) {
		if (recv(ClientSocket, msg, 32, 0) == 0||SOCKET_ERROR) {
			break;//SOCKETERROR의 경우 에러의 가지 수가 많기 때문에
				  //-1일 경우 여러가지의 에러처리를 넣어 자세히 설명 가능
		}
		printf("[msg]:[%d] 님 입장\n", ServerAddr);
	}
	//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}