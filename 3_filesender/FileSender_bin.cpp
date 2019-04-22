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
	char filename[255] = "pic2.jpg";
	//C��� ���� �����
	FILE * fp = fopen(filename, "rb");//���� ����
	char buf[BUF_SIZE] = { 0 };
	int filesize = 0;
	//====================================================================
	fseek(fp, 0, SEEK_END);//������ ���������� �̵�
	filesize = ftell(fp);

	send(ClientSocket, filename, 255, 0);
	send(ClientSocket, (char*)&filesize, 4, 0);
	
	rewind(fp);//�����б� ��ġ�� ó������

	while (fread(buf, 1, BUF_SIZE, fp) != EOF) {
			send(ClientSocket, buf, BUF_SIZE, 0);//���Ͽ��� ���̳ʸ� �����͸� �о����!
	}

	fclose(fp);//���� �ݱ�

	getchar();
	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}