//==================================================================================================
// -- 201343007_유민우_네트워크_최종과제
//==================================================================================================
#include "common.h"

char buf[BUF_STAND] = { 0 };
FILE_INFO file_info;

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
	//C언어 파일 입출력

	puts("어떤 작업을 수행하시겠습니까? 1.업로드 2.다운로드");

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
		puts("잘못된 접근 경로입니다. 프로그램을 다시 실행시켜주세요.");
	}

	//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}

void Upload(SOCKET p) {
	SOCKET ClientSocket = p;
	recv(ClientSocket, buf, BUF_STAND, 0);
	if (buf[0] == 's') {
		strcpy(file_info.filename, "shinjuku1.jpg");
		FILE * fp = fopen(file_info.filename, "rb");//파일 열기
		char file_buf[BUF_SIZE] = { 0 };
		//====================================================================
		fseek(fp, 0, SEEK_END);//파일의 마지막으로 이동
		file_info.filesize = ftell(fp);

		send(ClientSocket, (char*)&file_info, sizeof(file_info), 0);

		rewind(fp);//파일읽기 위치를 처음으로

		int sendtotal = 0;

		while (fread(file_buf, 1, BUF_SIZE, fp) != EOF && sendtotal <= file_info.filesize) {
			int sent = send(ClientSocket, file_buf, BUF_SIZE, 0);
			sendtotal += sent;
		}//파일에서 바이너리 데이터를 읽어오기!

		fclose(fp);//파일 닫기
		recv(ClientSocket, buf, BUF_STAND, 0);
		printf("%s\n", buf);
	}
	else if (buf[0] == 'f') {
		puts("업로드 할 공간이 부족합니다. 다시 접속해주세요.");
		WSACleanup();//winsock dll 언로딩
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
	puts("몇 번 파일을 다운로드 하시겠습니까?");
	fflush(stdin);
	char d_buf[BUF_STAND];
	scanf("%s", &d_buf);
	fflush(stdin);
	send(ClientSocket, d_buf, BUF_STAND, 0);


	char filename[BUF_STAND] = { 0 };
	int filesize = 0;

	recv(ClientSocket, (char*)&file_info, sizeof(FILE_INFO), 0);

	//파일 열기

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
