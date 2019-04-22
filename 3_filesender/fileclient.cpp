#include "common.h"

int main(void) {
	WSADATA wsa;
	FILE_INFO file_info;
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
	//C��� ���� �����
	strcpy((char*)(file_info.filename),"pic2.jpg");
	FILE * fp = fopen(file_info.filename, "rb");//���� ����
	char buf[BUF_SIZE] = { 0 };
	//====================================================================
	fseek(fp, 0, SEEK_END);//������ ���������� �̵�
	file_info.filesize = ftell(fp);

	send(ClientSocket, (char*)&file_info, sizeof(file_info), 0);

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