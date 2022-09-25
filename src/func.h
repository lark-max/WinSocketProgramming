#pragma once
#include<WinSock2.h>
#include<cstdlib>
#include<iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

const int DEFAULT_BUFFER = 4096;

/*服务器函数*/
DWORD WINAPI clientThread(LPVOID);
int main_server(int, char* []);

/*客户端函数*/
int main_client(int, char* []);