//==================================================================================================
// -- 201343007_유민우_네트워크_최종과제
//==================================================================================================

#include "common.h"

SOCKET_INFO Socketlist[32];
int SocketCount = 0;
FILE_LIST file_list[LIST_LIMIT];
int list_count = 0;
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

		if (ClientSocket == INVALID_SOCKET) {
			puts("accept() error!");
			return 0;
		}
		printf("ClientSocket : %d\n", ClientSocket);

		//
		printf("[ %s ]: %d 접속\n", inet_ntoa(ClientAddr.sin_addr)//IP얻어옴
			, ClientAddr.sin_port);//port얻어옴

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)doListThread, (LPVOID)ClientSocket, 0, 0);
	}

	//====================================================================
	WSACleanup();//winsock dll 언로딩
	return 0;
}

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
		strcpy(buf, "다운로드가 완료되었습니다.");
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
	char msg_buf[BUF_STAND] = { 0 };
	strcpy(msg_buf, "다운로드 완료");
	send(ClientSocket, msg_buf, BUF_STAND, 0);
	return;
}
