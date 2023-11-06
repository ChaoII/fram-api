//
// Created by AC on 2023/11/6.
//

#include "custom/Socket.h"

CustomSocket::CustomSocket() {


#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        showError("WSAStartup");
    }
#endif
    // 创建套接字
    client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket_ == INVALID_SOCKET) {
        showError("socket");
    }
    // 设置服务器信息


}

CustomSocket::~CustomSocket() {

    close_connect();
}

bool CustomSocket::connect(const std::string &ip, int port) {

    // 连接到服务器
    sockaddr_in serverAddr{AF_INET, htons(port)};
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr); // 设置服务器 IP 地址
    if (::connect(client_socket_, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) ==
        SOCKET_ERROR) {
        showError("connect");
    }

}

void CustomSocket::close_connect() {

    // 关闭套接字和清理
#ifdef _WIN32
    closesocket(client_socket_);
    WSACleanup();
#else
    close(clientSocket);
#endif

}

void CustomSocket::showError(const std::string &message) {

    std::cerr << message << " failed with error: " << std::endl;
#ifdef _WIN32
    WSACleanup();
#endif
    exit(EXIT_FAILURE);

}

void CustomSocket::send_message(const std::string &str) {

    if (send(client_socket_, str.c_str(), static_cast<int>(str.length()), 0) == SOCKET_ERROR) {
        showError("send");
    }
    std::cout << "Message sent to server" << std::endl;

}

