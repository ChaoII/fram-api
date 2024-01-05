/**
 *
 *  LoopTask.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include "plugins/TrantorSocketClient.h"
#include "plugins/GlobalThreadPool.h"


class LoopTask : public drogon::Plugin<LoopTask> {
public:
    LoopTask() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

    void deleteAttendHistory(double interval, bool enable_delete);


private:
    void deleteAttendHistorySub(double interval);

    void updateDeleteAttendTimer(double interval);

    void deleteAttendAtStart();

    void startUpdateDateTimeTask();

    void startUpdateDateTimeTaskSub();

private:

    std::string host_;
    std::string api_path_;
    trantor::TimerId updateTimeTimerId_ = -1;
    trantor::TimerId deleteAttendTimerId_ = -1;
};

