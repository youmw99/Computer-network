#pragma once
#include <winsock2.h>
#include <windows.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

typedef struct DrawPacket {
	int sX;
	int sY;
	int eX;
	int eY;
	int width;
	COLORREF color;
}DRAWPACK;