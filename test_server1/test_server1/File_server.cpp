//==================================================================================================
// -- 201343007_���ο�_��Ʈ��ũ_��������
//==================================================================================================

#include "common.h"

SOCKET_INFO Socketlist[32];
int SocketCount = 0;
FILE_LIST file_list[LIST_LIMIT];
int list_count = 0;
char buf[BUF_STAND] = { 0 };
FILE_INFO file_info;

LPVOID doListThread(LPVOID p) {

	SOCKET ClientSocket = (SOCKET)p;
	
	char check_buf[BUF_STAND] = { 0 };

	recv(ClientSocket, check_buf, BUF_STAND, 0);
	if (check_buf[0] == '1'&& list_count<LIST_LIMIT) {
		strcpy(buf, "success");
		send(ClientSocket, buf, BUF_STAND, 0);
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

		strcpy(file_list[list_count].filename, file_info.filename);
		list_count++;

		char msg_buf[BUF_STAND] = { 0 };

		strcpy(msg_buf, "File Uploaded");
		send(ClientSocket, msg_buf, BUF_STAND, 0);

		return 0;
	}
	else if (check_buf[0] == '2') {
		ShowList(ClientSocket);
		recv(ClientSocket, buf, 1, 0);
		FileSend(ClientSocket);
		strcpy(buf, "�ٿ�ε尡 �Ϸ�Ǿ����ϴ�.");
		send(ClientSocket, buf, BUF_STAND, 0);
	}
	else {
		send(ClientSocket, buf, BUF_STAND, 0);
		strcpy(buf, "fail");
		char msg_buf[BUF_STAND] = { 0 };
		strcpy(msg_buf, "Not enough Storage or Wrong Number");
		send(ClientSocket, msg_buf, BUF_STAND, 0);
	}


	
}

void ShowList(SOCKET p) {
	SOCKET ClientSocket =  p;
	char list_buf[1] = { list_count };
	send(ClientSocket, list_buf, 1, 0);
	for (int i = 0; i<list_count; i++) {
		strcpy(buf, file_list[i].filename);
		send(ClientSocket, buf, BUF_STAND, 0);
	}
	return;
}

void FileSend(SOCKET p) {
	SOCKET ClientSocket = p;
	FILE_INFO file_info;
	int selected_num = atoi(buf);
	strcpy(file_info.filename, file_list[selected_num-1].filename);
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
	char msg_buf[BUF_STAND] = { 0 };
	strcpy(msg_buf, "�ٿ�ε� �Ϸ�");
	send(ClientSocket, msg_buf, BUF_STAND, 0);
	return;
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

		if (ClientSocket == INVALID_SOCKET) {
			puts("accept() error!");
			return 0;
		}
		printf("ClientSocket : %d\n", ClientSocket);

		//
		printf("[ %s ]: %d ����\n", inet_ntoa(ClientAddr.sin_addr)//IP����
			, ClientAddr.sin_port);//port����

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)doListThread, (LPVOID)ClientSocket, 0, 0);
	}

	//====================================================================
	WSACleanup();//winsock dll ��ε�
	return 0;
}