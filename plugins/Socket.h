/**
 *
 *  Socket.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "custom/Socket.h"


class Socket : public drogon::Plugin<Socket> {
public:
    Socket() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;


    void send_message(const std::string &message);

private:
    std::string address_;
    int port_;
    std::unique_ptr<CustomSocket> custom_socket_ptr;
};

