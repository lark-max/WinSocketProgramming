#include"func.h"

int main_client(int argc, char* argv[])
{
    WSADATA wsd;
    SOCKET sClient;
    char Buffer[DEFAULT_BUFFER];
    int ret;
    sockaddr_in server;
    unsigned short port;
    hostent* host = NULL;

    if (argc < 3) {
        std::cout << "Usage:" << argv[0] << "IP Port\n" << std::endl;
        return -1;
    }

    /*����Winsock DLL*/
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        std::cout << "winsock ��ʼ��ʧ�ܣ�\n" << std::endl;
        return 1;
    }

    /*����Socket*/
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET) {
        std::cout << "socket() ʧ��: " << WSAGetLastError() << std::endl;
        return 1;
    }

    /*ָ����������ַ*/
    server.sin_family = AF_INET;
    port = atoi(argv[2]);
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if (server.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(argv[1]);    //����ĵ�ַ������������
        if (host == NULL) {
            std::cout << "�޷���������˵�ַ:" << argv[1] << std::endl;
            return 1;
        }
        CopyMemory(&server.sin_addr,
            host->h_addr_list[0],
            host->h_length);
    }
    /*���������������*/
    if (connect(sClient, (sockaddr*)&server,
        sizeof(server)) == SOCKET_ERROR) {
        std::cout << "connect() ʧ��: " << WSAGetLastError() << std::endl;
        return 1;
    }

    /*���͡�������Ϣ*/

    while(1) {
        //�ӱ�׼�����ȡҪ���͵�����
        //gets_s(Buffer,BUFSIZE);
        std::cout << "��������Ҫ���͵���Ϣ:" << std::endl;
        std::cin.get(Buffer, DEFAULT_BUFFER);
        //�������������Ϣ
        ret = send(sClient, Buffer, strlen(Buffer), 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            std::cerr << "send() ʧ��: " << WSAGetLastError() << std::endl;
            break;
        }
        //���մӷ�����������Ϣ
        ret = recv(sClient, Buffer, DEFAULT_BUFFER, 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            std::cerr << "recv() ʧ��: " << WSAGetLastError() << std::endl;
            break;
        }
        Buffer[ret] = '\0';
        std::cout << "���յ�����Ϣ: " << Buffer << std::endl;
    }
    //�����ˣ��ر�socket���(�ļ�������)
    closesocket(sClient);
    WSACleanup();    //����
    return 0;
}