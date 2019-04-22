//��Ʈ��ũ ��Ŷ(��������) ����
/*//=====================================================================
��ǥ(x,y)
�� �β�
�� ����
1. �������� ������
2. recv ������(recv ��� Ŭ���̾�Ʈ�� �����ϹǷ� send �ʿ����)
*///=====================================================================
#include "common.h"

//������ ����
typedef struct SOCKET_INFO {
	SOCKET ClientSocket;
	SOCKADDR_IN ClientAddr;
};

SOCKET_INFO Socketlist[32];
int SocketCount = 0;

//Ŭ���̾�Ʈ ������
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
	//winsock dll �ε�
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		puts("WSAStartup() error!!");
		return 0;
	}
	//====================================================================
	//socket ����(�ڵ��� �ܸ���)
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//-->�������� ������ ����� ����
	if (ListenSocket == INVALID_SOCKET) {
		puts("socket() error!");
		return 0;
	}
	//���� �ڵ� Ȯ��
	printf("ListenSocket: %d\n", ListenSocket);

	//�ּ� ����ü(����)
	//-->��ſ� �ʿ��� �ּ������� ���� ����ü
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(8000);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//socket(�ڵ���)+sockaddr(����)
	//socket�� ����ü�� ����!
	if (bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) {
		puts("bind()error!");
		return 0;
	}
	//���� ��� ����
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("Listen error!");
		return 0;
	}
	//����ó��
	SOCKADDR_IN ClientAddr;//������ Ŭ���̾�Ʈ�� �ּ�
	int len = sizeof(ClientAddr);
	//������ Ŭ���̾�Ʈ�� ������ ����
	while (1) {
		SOCKET ClientSocket =
			accept(ListenSocket,//������
			(SOCKADDR*)&ClientAddr,//Ŭ���̾�Ʈ�� �ּҰ��� ������
				&len);//�ּ��� ũ��
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
		printf("[ %s ]: %d ����\n", inet_ntoa(ClientAddr.sin_addr)//IP����
			, ClientAddr.sin_port);//port����
	}
	


	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}