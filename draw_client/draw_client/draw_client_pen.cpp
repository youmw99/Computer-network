#include "common.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
const char* lpszClass = "First";
HWND gHWnd;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

DWORD RecvThread(LPVOID p) {
	SOCKET ClientSocket = (SOCKET)p;
	while (1) {
		DRAWPACK Pack;
		recv(ClientSocket, (char*)&Pack, sizeof(Pack), 0);
		
		char buf[255];
		wsprintf(buf,"x: %d y : %d width: %d\n",Pack.sX,Pack.sY,Pack.width);

		HDC hdc = GetDC(gHWnd);
		HPEN pen = CreatePen(PS_SOLID, Pack.width, Pack.color);
		SelectObject(hdc, pen);

		MoveToEx(hdc, Pack.sX, Pack.sY, NULL);
		LineTo(hdc, Pack.eX, Pack.eY);

		ReleaseDC(gHWnd, hdc);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int startX;
	static int startY;
	static int endX;
	static int endY;
	static int width;
	static COLORREF color;

	static bool mode = false;

	static SOCKET ClientSocket;

	switch (iMessage) {
	case WM_CREATE: {
		WSADATA wsa;
		//winsock dll 로딩
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			return 0;
		}
		//====================================================================
		ClientSocket
			= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ClientSocket == INVALID_SOCKET) {
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
			return 0;
		}
		gHWnd = hWnd;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, (LPVOID)ClientSocket, 0, NULL);
	}
	return 0;
	case WM_LBUTTONDOWN:
	{
		startX = LOWORD(lParam);
		startY = HIWORD(lParam);
		mode = true;
	}
	return 0;
	case WM_LBUTTONUP: {
		mode = false;
	}
					   return 0;
	case WM_MOUSEMOVE: {
		if (mode == true) {
			HDC hdc =GetDC(hWnd);

			width = rand() % 10 + 1;
			color = RGB(rand() % 255, rand() % 255, rand() % 255);
			
			HPEN pen = CreatePen(PS_SOLID, width, color);
			
			SelectObject(hdc, pen);//pen속성 변경

			endX = LOWORD(lParam);
			endY = HIWORD(lParam);
			
			//SetPixel(hdc, endX, endY, RGB(255, 0, 0));//점으로 할 경우 입력속도를 따라오지 못하여 드문드문 찍힘

			MoveToEx(hdc, startX, startY, NULL);
			LineTo(hdc, endX, endY);

			DRAWPACK Pack;
			Pack.sX = startX;
			Pack.sY = startY;
			Pack.eX = endX;
			Pack.eY = endY;
			Pack.width = width;
			Pack.color = color;

			send(ClientSocket, (char*)&Pack, sizeof(Pack), 0);

			startX = endX;
			startY = endY;

			ReleaseDC(hWnd, hdc);
		}
	}
					   return 0;
	
	case WM_DESTROY:
		WSACleanup();
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}