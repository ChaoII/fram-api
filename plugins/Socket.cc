/**
 *
 *  Socket.cc
 *
 */

#include "Socket.h"

using namespace drogon;

void Socket::initAndStart(const Json::Value &config) {
    /// Initialize and start the plugin
    address_ = config.get("address", "0.0.0.0").asString();
    port_ = config.get("port", 8899).asInt();
    custom_socket_ptr = std::make_unique<CustomSocket>();
    custom_socket_ptr->connect(address_, port_);
}

void Socket::send_message(const std::string &message) {
    custom_socket_ptr->send_message(message);
}

void Socket::shutdown() {
    /// Shutdown the plugin
    custom_socket_ptr->close_connect();
}
