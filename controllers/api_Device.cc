#include "api_Device.h"
#include "custom/utils.h"

using namespace api;

void Device::updateTime(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

//    bool ret = Custom::update_time();
//    auto resp = HttpResponse::newHttpResponse();
//    if (ret) {
//        resp->setStatusCode(drogon::k200OK);
//        resp->setBody("success");
//    } else {
//        resp->setStatusCode(drogon::k408RequestTimeout);
//        resp->setBody("failed，时间同步接口访问超时ß");
//    }
//    callback(resp);

}


void Device::restartDevice(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    auto resp = HttpResponse::newHttpResponse();

#if __LINUX__
    system("ps aux | grep main.py | awk '{print $2}' | xargs kill -9");
    resp->setStatusCode(drogon::k200OK);
    resp->setBody("更新成功");
#elif __APPLE__
    resp->setStatusCode(drogon::k200OK);
    resp->setBody("this is apple group which has been unsupported this program");
    LOG_WARN << "this is apple group which has been unsupported this program";
#else
    resp->setStatusCode(drogon::k400BadRequest);
    resp->setBody("tplease check your operate system");
    LOG_WARN << "please check your operate system";
#endif
    callback(resp);
}