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

    /*加载Winsock DLL*/
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        std::cout << "winsock 初始化失败！\n" << std::endl;
        return 1;
    }

    /*创建Socket*/
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET) {
        std::cout << "socket() 失败: " << WSAGetLastError() << std::endl;
        return 1;
    }

    /*指定服务器地址*/
    server.sin_family = AF_INET;
    port = atoi(argv[2]);
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if (server.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(argv[1]);    //输入的地址可能是域名等
        if (host == NULL) {
            std::cout << "无法解析服务端地址:" << argv[1] << std::endl;
            return 1;
        }
        CopyMemory(&server.sin_addr,
            host->h_addr_list[0],
            host->h_length);
    }
    /*与服务器建立连接*/
    if (connect(sClient, (sockaddr*)&server,
        sizeof(server)) == SOCKET_ERROR) {
        std::cout << "connect() 失败: " << WSAGetLastError() << std::endl;
        return 1;
    }

    /*发送、接收消息*/

    while(1) {
        //从标准输入读取要发送的数据
        //gets_s(Buffer,BUFSIZE);
        std::cout << "请输入需要发送的消息:" << std::endl;
        std::cin.get(Buffer, DEFAULT_BUFFER);
        //向服务器发送消息
        ret = send(sClient, Buffer, strlen(Buffer), 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            std::cerr << "send() 失败: " << WSAGetLastError() << std::endl;
            break;
        }
        //接收从服务器来的消息
        ret = recv(sClient, Buffer, DEFAULT_BUFFER, 0);
        if (ret == 0) {
            break;
        }
        else if (ret == SOCKET_ERROR) {
            std::cerr << "recv() 失败: " << WSAGetLastError() << std::endl;
            break;
        }
        Buffer[ret] = '\0';
        std::cout << "接收到的消息: " << Buffer << std::endl;
    }
    //用完了，关闭socket句柄(文件描述符)
    closesocket(sClient);
    WSACleanup();    //清理
    return 0;
}