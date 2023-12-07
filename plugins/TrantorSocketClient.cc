/**
 *
 *  TrantorSocketClient.cc
 *
 */

#include "TrantorSocketClient.h"

using namespace drogon;

void TrantorSocketClient::initAndStart(const Json::Value &config) {
    /// Initialize and start the plugin

    std::string address = config.get("address", "0.0.0.0").asString();
    int port = config.get("port", 9088).asInt();
    tcp_client_ = std::make_shared<trantor::TcpClient>(drogon::app().getLoop(),
                                                       trantor::InetAddress(address, port), "tcp_client");

    tcp_client_->setConnectionCallback([&](const trantor::TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG_INFO << "Connected to server successful";
        } else {
            tcp_client_->connect();
        }
    });

    tcp_client_->setConnectionErrorCallback([this]() {
        drogon::app().getLoop()->runAfter(std::chrono::seconds(3), [&]() {
            LOG_WARN << "try to reconnect...";
            tcp_client_->connect();
        });
    });
    // 设置收到数据时的回调函数
    tcp_client_->setMessageCallback([this](const trantor::TcpConnectionPtr &conn, trantor::MsgBuffer *buffer) {
        receive_msg_ = buffer->read(1024);
        LOG_INFO << "Received message from server: " << receive_msg_;
    });
    tcp_client_->connect();
}

bool TrantorSocketClient::sendMessage(const std::string &msg) {
    receive_msg_ = "";
    if (tcp_client_->connection() && tcp_client_->connection()->connected()) {
        LOG_INFO << "sned message" << msg;
        tcp_client_->connection()->send(msg);
        return true;
    } else {
        return false;
    }
}

void TrantorSocketClient::shutdown() {
    /// Shutdown the plugin
    if (tcp_client_->connection()->connected()) {
        tcp_client_->disconnect();
    }
}

std::string TrantorSocketClient::getReceiveMsg() {
    return receive_msg_;
}
