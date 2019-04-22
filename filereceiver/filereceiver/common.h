#pragma once
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>


#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define BUF_SIZE 4096

typedef struct FILE_INFO {
	char filename[255];
	int filesize = 0;
};