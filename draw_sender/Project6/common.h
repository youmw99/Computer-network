#pragma once

#include <winsock2.h>//socket API사용
#include <windows.h>//Thread 사용
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

//==========================================================================
typedef struct DrawPacket {
	int sX;
	int sY;
	int eX;
	int eY;
	int width;
	COLORREF color;
}DRAWPACK;