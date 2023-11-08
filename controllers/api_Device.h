#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
class Device : public drogon::HttpController<Device>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(Device::updateTime, "/api/device/updateTime", Get);
        ADD_METHOD_TO(Device::restartDevice, "/api/device/restartDevice", Get);
    METHOD_LIST_END



    void updateTime(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void restartDevice(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
};
}
