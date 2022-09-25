#include"func.h"

/*��ͻ���ͨ�ŵ��̺߳���*/
DWORD WINAPI clientThread(LPVOID lparam)
{
	SOCKET sock = (SOCKET)lparam;
	char Buffer[DEFAULT_BUFFER];
	int ret;
	//int nNetTimeout = 60000; //60��
	//setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int)); //���÷���ʱ��
	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int)); //���ý���ʱ��

	/*�������Կͻ��˵���Ϣ*/
	while (1) {
		ret = recv(sock, Buffer, DEFAULT_BUFFER, 0);
		if (!ret)
			break;
		if (ret == SOCKET_ERROR) {
			std::cout << "recv() ʧ��: " << WSAGetLastError() << std::endl;
			break;
		}
		Buffer[ret] = '\0';
		std::cout << "���յ�����Ϣ: " << Buffer << std::endl;
		/*��ͻ��˷��ͻ�Ӧ��Ϣ*/
		std::cout << "��Ӧ��Ϣ: ACK" << std::endl;
		ret = send(sock, "ACK", 4, 0);
		if (ret == SOCKET_ERROR) {
			std::cerr << "send() ʧ��: " << WSAGetLastError() << std::endl;
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

	/*����winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		std::cerr << "winsock ��ʼ��ʧ�ܣ�\n" << std::endl;
		return 1;
	}

	/*����socket*/
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR) {
		std::cerr << "socket() ʧ��: " << WSAGetLastError() << std::endl;
		return 1;
	}
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	port = atoi(argv[1]); //��ȡ�˿�ֵ
	local.sin_port = htons(port);

	/*��socket*/
	if (bind(sListen,
		(sockaddr*)&local,
		sizeof(local)) == SOCKET_ERROR) {
		std::cerr << "bind() ʧ��: " << WSAGetLastError() << std::endl;
		return 1;
	}
	
	/*�򿪼���*/
	std::cout << "����������,��ʼ��������:" << std::endl;
	listen(sListen, 8);

	/*�ڶ˿ڽ��м�����һ���пͻ��˷�����������
	�ͽ�����ͻ��˽���ͨ�ŵ��߳�
	*/
	while (1) {
		AddrSize = sizeof(client);
		/*�����Ƿ�����������*/
		sClient = accept(sListen, (sockaddr*)&client, &AddrSize);
		if (sClient == INVALID_SOCKET) {
			std::cerr << "accept() ʧ��: " << WSAGetLastError() << std::endl;
			break;
		}
		std::cout << "�ͻ�����������,IP: " << inet_ntoa(client.sin_addr) << ":"
			<< ntohs(client.sin_port) << std::endl;
		//����һ���߳�ȥ����
		hThread = CreateThread(NULL, 0, clientThread,
			(LPVOID)sClient, 0, &dwThread);
		if (!hThread) {
			std::cerr << "CreateThread() ʧ��: " << GetLastError() << std::endl;
			break;
		}
		//�����ر�
		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();  //���������
	return 0;
}