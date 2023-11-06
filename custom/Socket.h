//
// Created by AC on 2023/11/6.
//

#pragma once

#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET  ((SOCKET)(~0))
#define SOCKET_ERROR            (-1)
#endif

class CustomSocket {
public:
    explicit CustomSocket();

    ~CustomSocket();

    bool connect(const std::string &ip, int port);

    void send_message(const std::string &str);

    void close_connect();

private:
    /// 错误处理函数
    void showError(const std::string &message);


private:
    SOCKET client_socket_;
};



