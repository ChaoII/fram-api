#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
    class Device : public drogon::HttpController<Device> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Device::updateTime, "/api/device/updateTime", Get, "LoginFilter");
            ADD_METHOD_TO(Device::restartProgram, "/api/device/restartProgram", Get, "LoginFilter");
        METHOD_LIST_END

        void updateTime(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void restartProgram(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
    };
}
