#include"func.h"

/*与客户机通信的线程函数*/
DWORD WINAPI clientThread(LPVOID lparam)
{
	SOCKET sock = (SOCKET)lparam;
	char Buffer[DEFAULT_BUFFER];
	int ret;
	//int nNetTimeout = 60000; //60秒
	//setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int)); //设置发送时限
	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int)); //设置接收时限

	/*接受来自客户端的消息*/
	while (1) {
		ret = recv(sock, Buffer, DEFAULT_BUFFER, 0);
		if (!ret)
			break;
		if (ret == SOCKET_ERROR) {
			std::cout << "recv() 失败: " << WSAGetLastError() << std::endl;
			break;
		}
		Buffer[ret] = '\0';
		std::cout << "接收到的消息: " << Buffer << std::endl;
		/*向客户端发送回应消息*/
		std::cout << "回应消息: ACK" << std::endl;
		ret = send(sock, "ACK", 4, 0);
		if (ret == SOCKET_ERROR) {
			std::cerr << "send() 失败: " << WSAGetLastError() << std::endl;
		}
	}

	return 0;
}

int main_server(int argc, char* argv[])
{
	WSADATA wsd;
	HANDLE	hThread;
	DWORD dwThread;
	SOCKET sListen, sClient;
	int	AddrSize;
	unsigned short port;
	sockaddr_in local, client;

	if (argc < 2) {
		std::cout << "Usage:" << argv[0] << " Port\n" << std::endl;
		return -1;
	}

	/*加载winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		std::cerr << "winsock 初始化失败！\n" << std::endl;
		return 1;
	}

	/*创建socket*/
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR) {
		std::cerr << "socket() 失败: " << WSAGetLastError() << std::endl;
		return 1;
	}
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	port = atoi(argv[1]); //获取端口值
	local.sin_port = htons(port);

	/*绑定socket*/
	if (bind(sListen,
		(sockaddr*)&local,
		sizeof(local)) == SOCKET_ERROR) {
		std::cerr << "bind() 失败: " << WSAGetLastError() << std::endl;
		return 1;
	}
	
	/*打开监听*/
	std::cout << "服务器启动,开始监听请求:" << std::endl;
	listen(sListen, 8);

	/*在端口进行监听，一旦有客户端发起连接请求，
	就建立与客户端进行通信的线程
	*/
	while (1) {
		AddrSize = sizeof(client);
		/*监听是否有连接请求*/
		sClient = accept(sListen, (sockaddr*)&client, &AddrSize);
		if (sClient == INVALID_SOCKET) {
			std::cerr << "accept() 失败: " << WSAGetLastError() << std::endl;
			break;
		}
		std::cout << "客户端请求连接,IP: " << inet_ntoa(client.sin_addr) << ":"
			<< ntohs(client.sin_port) << std::endl;
		//创建一个线程去处理
		hThread = CreateThread(NULL, 0, clientThread,
			(LPVOID)sClient, 0, &dwThread);
		if (!hThread) {
			std::cerr << "CreateThread() 失败: " << GetLastError() << std::endl;
			break;
		}
		//处理后关闭
		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();  //用完后清理
	return 0;
}