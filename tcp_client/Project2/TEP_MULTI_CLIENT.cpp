#include <winsock2.h>
#include <stdio.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)


int main(void) {
	WSADATA wsa;
	//winsock dll �ε�
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
	//������ ������ �ּ�
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_port = htons(8000);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(ClientSocket, //Ŭ���̾�Ʈ ����
		(SOCKADDR*)&ServerAddr, //������ �����ּ�
		sizeof(ServerAddr))//�ּ� ũ��
		== SOCKET_ERROR) {
		puts("connect error!");
		return 0;
	}
	char msg[32] = { 0 };
	char nickname[32] = { 0 };
	printf("��ȭ�� : ");
	gets_s(nickname);
	send(ClientSocket, nickname, 32, 0);

	while (1) {
		if (recv(ClientSocket, msg, 32, 0) == 0||SOCKET_ERROR) {
			break;//SOCKETERROR�� ��� ������ ���� ���� ���� ������
				  //-1�� ��� ���������� ����ó���� �־� �ڼ��� ���� ����
		}
		printf("[msg]:[%d] �� ����\n", ServerAddr);
	}
	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}