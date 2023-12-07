/**
 *
 *  GlobalThreadPool.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <custom/threadpool.hpp>


class GlobalThreadPool : public drogon::Plugin<GlobalThreadPool> {
public:
    GlobalThreadPool() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    std::shared_ptr<ThreadPool> getGlobalThreadPool();

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

private:
    std::shared_ptr<ThreadPool> pool_;
};

