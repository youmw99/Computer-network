//네트워크 패킷(프로토콜) 설계
/*//=====================================================================
좌표(x,y)
선 두께
선 색상
1. 다중접속 스레드
2. recv 스레드(recv 즉시 클라이언트에 전송하므로 send 필요없음)
*///=====================================================================
#include "common.h"

//접속자 정보
typedef struct SOCKET_INFO {
	SOCKET ClientSocket;
	SOCKADDR_IN ClientAddr;
};

SOCKET_INFO Socketlist[32];
int SocketCount = 0;

//클라이언트 쓰레드
DWORD ClientThread(LPVOID p) {
	SOCKET ClientSocket = (SOCKET)p;

	while (1) {
		DRAWPACK pack;
		recv(ClientSocket,(char*)&pack,sizeof(pack),0);
		printf("x:%d y:%d width:%d\n", pack.sX, pack.sY, pack.width);

		for (int i = 0; i < SocketCount; i++) {
			if (Socketlist[i].ClientSocket != ClientSocket) {
				send(Socketlist[i].ClientSocket, (char*)&pack, sizeof(pack), 0);
			}
		}
	}
}

int main(void) {
	WSADATA wsa;
	//winsock dll 로딩
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		puts("WSAStartup() error!!");
		return 0;
	}
	//====================================================================
	//socket 생성(핸드폰 단말기)
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//-->서버에서 접속을 대기할 소켓
	if (ListenSocket == INVALID_SOCKET) {
		puts("socket() error!");
		return 0;
	}
	//소켓 핸들 확인
	printf("ListenSocket: %d\n", ListenSocket);

	//주소 구조체(유심)
	//-->통신에 필요한 주소정보를 갖는 구조체
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(8000);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//socket(핸드폰)+sockaddr(유심)
	//socket과 구조체를 연결!
	if (bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {
		puts("bind()error!");
		return 0;
	}
	//접속 대기 상태
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("Listen error!");
		return 0;
	}
	//접속처리
	SOCKADDR_IN ClientAddr;//접속한 클라이언트의 주소
	int len = sizeof(ClientAddr);
	//접속한 클라이언트의 소켓을 리턴
	while (1) {
		SOCKET ClientSocket =
			accept(ListenSocket,//대기소켓
			(SOCKADDR*)&ClientAddr,//클라이언트의 주소값을 가져옴
				&len);//주소의 크기
		Socketlist[SocketCount].ClientSocket = ClientSocket;
		Socketlist[SocketCount].ClientAddr = ClientAddr;
		SocketCount++;

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientThread, (LPVOID)ClientSocket, 0,0);

		if (ClientSocket == INVALID_SOCKET) {
			puts("accept() error!");
			return 0;
		}
		printf("ClientSocket : %d\n", ClientSocket);

		//
		printf("[ %s ]: %d 접속\n", inet_ntoa(ClientAddr.sin_addr)//IP얻어옴
			, ClientAddr.sin_port);//port얻어옴
	}
	


	//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}