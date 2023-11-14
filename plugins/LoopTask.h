/**
 *
 *  LoopTask.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>


class LoopTask : public drogon::Plugin<LoopTask> {
public:
    LoopTask() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;


    trantor::TimerId getUpdateTimerId();

private:
    trantor::TimerId updateTimeTimerId = -1;
};

