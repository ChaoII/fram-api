/**
 *
 *  GlobalParameter.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <map>
#include "drogon/drogon.h"


class GlobalParameter : public drogon::Plugin<GlobalParameter> {
    using IntMap = std::map<std::string, int64_t>;
    using StringMap = std::map<std::string, std::string>;

public:
    GlobalParameter() {}

    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

    template<class T>
    auto &getGlobalParametersMap() {
        if (std::is_same<T, int>::value) {
            return globalParametersMapInt;
        } else if (std::is_same<T, std::string>::value) {
            return globalParametersMapString;
        } else {
            return globalParametersMapString;
        }
    }

    trantor::TimerId getDeleteAttendTimerId() {
        return deleteAttendTimerId;
    }

    void setDeleteAttendTimerId(trantor::TimerId timerId) {
        deleteAttendTimerId = timerId;
    }

private:
    IntMap globalParametersMapInt;
    IntMap globalParametersMapString;
    trantor::TimerId deleteAttendTimerId = -1;
};

