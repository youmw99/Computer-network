#include <winsock2.h>
#include <stdio.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define BUF_SIZE 4096
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
	SOCKET ClientSocket =
		accept(ListenSocket,//������
		(SOCKADDR*)&ClientAddr,//Ŭ���̾�Ʈ�� �ּҰ��� ������
			&len);//�ּ��� ũ��
	if (ClientSocket == INVALID_SOCKET) {
		puts("accept() error!");
		return 0;
	}
	printf("ClientSocket : %d\n", ClientSocket);

	//
	printf("[ %s ]: %d\n", inet_ntoa(ClientAddr.sin_addr)//IP����
		, ClientAddr.sin_port);//port����
	//====================================================================
	
	char filename[255] = { 0 };
	int filesize = 0;
	
	recv(ClientSocket, filename, 255, 0);
	printf("�����̸� : %s\n",filename);
	recv(ClientSocket, (char*)&filesize, 4, 0);
	printf("����ũ�� : %d byte\n",filesize);
	//���� ����
	
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		puts("fopen failed");
		return 0;
	}
	char buf[BUF_SIZE] = { 0 };

	int recvtotal = 0;

	while (recvtotal <= filesize) {
		int ret = recv(ClientSocket,buf,BUF_SIZE,0);
		recvtotal += ret;
		fwrite(buf, 1, ret, fp);
	}
	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}