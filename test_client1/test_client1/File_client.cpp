//==================================================================================================
// -- 201343007_���ο�_��Ʈ��ũ_��������
//==================================================================================================
#include "common.h"

char buf[BUF_STAND] = { 0 };
FILE_INFO file_info;

void Upload(SOCKET p) {
	SOCKET ClientSocket = p;
	recv(ClientSocket, buf, BUF_STAND, 0);
	if (buf[0] == 's') {
		strcpy(file_info.filename, "shinjuku1.jpg");
		FILE * fp = fopen(file_info.filename, "rb");//���� ����
		char file_buf[BUF_SIZE] = { 0 };
		//====================================================================
		fseek(fp, 0, SEEK_END);//������ ���������� �̵�
		file_info.filesize = ftell(fp);

		send(ClientSocket, (char*)&file_info, sizeof(file_info), 0);

		rewind(fp);//�����б� ��ġ�� ó������

		int sendtotal = 0;

		while (fread(file_buf, 1, BUF_SIZE, fp) != EOF && sendtotal <= file_info.filesize) {
			int sent = send(ClientSocket, file_buf, BUF_SIZE, 0);
			sendtotal += sent;
		}//���Ͽ��� ���̳ʸ� �����͸� �о����!

		fclose(fp);//���� �ݱ�
		recv(ClientSocket, buf, BUF_STAND, 0);
		printf("%s\n", buf);
	}
	else if (buf[0] == 'f') {
		puts("���ε� �� ������ �����մϴ�. �ٽ� �������ּ���.");
		WSACleanup();//winsock dll ��ε�
		return;
	}
	return;
}

int Download(const SOCKET &ClientSocket, bool &retflag)
{
	retflag = true;
	char list_buf[1] = { 0 };
	recv(ClientSocket, list_buf, 1, 0);
	for (int i = 0; i < list_buf[0]; i++) {
		recv(ClientSocket, buf, BUF_STAND, 0);
		printf("%d. %s\n", i + 1, buf);
	}
	puts("�� �� ������ �ٿ�ε� �Ͻðڽ��ϱ�?");
	fflush(stdin);
	char d_buf[BUF_STAND];
	scanf("%s", &d_buf);
	fflush(stdin);
	send(ClientSocket, d_buf, BUF_STAND, 0);


	char filename[BUF_STAND] = { 0 };
	int filesize = 0;

	recv(ClientSocket, (char*)&file_info, sizeof(FILE_INFO), 0);

	//���� ����

	FILE *fp = fopen(file_info.filename, "wb");
	if (fp == NULL) {
		puts("fopen failed");
		return 0;
	}
	char file_buf[BUF_SIZE] = { 0 };

	int recvtotal = 0;

	while (recvtotal <= file_info.filesize) {
		int ret = recv(ClientSocket, file_buf, BUF_SIZE, 0);
		recvtotal += ret;
		fwrite(file_buf, 1, ret, fp);
	}
	char msg_buf[BUF_STAND] = { 0 };
	recv(ClientSocket, msg_buf, BUF_STAND, 0);
	puts(msg_buf);
	retflag = false;
	return 0;
}

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
	//C��� ���� �����

	puts("� �۾��� �����Ͻðڽ��ϱ�? 1.���ε� 2.�ٿ�ε�");

	char check_buf[BUF_STAND] = { getchar() };

	send(ClientSocket, check_buf, BUF_STAND, 0);

	if (check_buf[0] == '1') {
		Upload(ClientSocket);
	}
	else if (check_buf[0] == '2') {
		bool retflag;
		int retval = Download(ClientSocket, retflag);
		if (retflag) return retval;
	}
	else {
		puts("�߸��� ���� ����Դϴ�. ���α׷��� �ٽ� ��������ּ���.");
	}

	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}