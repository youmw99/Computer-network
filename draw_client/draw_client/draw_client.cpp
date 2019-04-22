#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
const char* lpszClass = "First";
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int startX;
	static int startY;
	static int endX;
	static int endY;
	static bool mode;
	
	switch (iMessage) {

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
			endX = LOWORD(lParam);
			endY = HIWORD(lParam);
			InvalidateRect(hWnd, NULL, true);//����� ��ǥ ������ �ٽ� �׸���
		}
	}
	return 0;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		//HDC hdc = GetDC(hWnd);//DC:�׸��� �׸����� �ϴ� ������ ����
		HDC hdc = BeginPaint(hWnd, &ps);

		MoveToEx(hdc, startX, startY, NULL);//���� ��ġ �̵�
		LineTo(hdc, endX, endY);		   //���� ��ġ�������� �� �׸���
		//�簢�� �׸���
		//Rectangle(hdc, 100, 100, 200, 200);

		EndPaint(hWnd, &ps);
	}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}