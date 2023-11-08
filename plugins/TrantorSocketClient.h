/**
 *
 *  TrantorSocketClient.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <trantor/net/TcpClient.h>
#include <drogon/drogon.h>

class TrantorSocketClient : public drogon::Plugin<TrantorSocketClient> {
public:
    TrantorSocketClient() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;


    bool sendMessage(const std::string &msg);


    std::string getReceiveMsg();

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;




private:
    std::shared_ptr<trantor::TcpClient> tcp_client_;

    std::string receive_msg_;
};

