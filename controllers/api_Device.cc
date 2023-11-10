#include "api_Device.h"
#include "custom/utils.h"

using namespace api;

void Device::updateTime(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value response_result, client_result;
    std::string host = drogon::app().getCustomConfig().get("time_synchronization_server_host", "").asString();
    std::string api_path = drogon::app().getCustomConfig().get("time_synchronization_server_api", "").asString();
    auto client = HttpClient::newHttpClient(host);
    auto client_req = HttpRequest::newHttpRequest();
    client_req->setPath(api_path);
    client_req->setMethod(drogon::Post);
    client_req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    client_req->setBody("{}");
    auto ret = client->sendRequest(client_req);
    if (ret.first != ReqResult::Ok) {
        response_result = drogon::Custom::getJsonResult(-1, {}, "time synchronization failed");
        auto res = HttpResponse::newHttpJsonResponse(response_result);
        callback(res);
    }
    Json::Reader reader;
    reader.parse(std::string(ret.second->getBody()), client_result);
#if defined(__linux__) || defined(__APPLE__)
    system((std::string("sudo date -s ") + client_result["result"].asString()).c_str());
    LOG_INFO<<"update datetime successfully!";
#else
    LOG_WARN << "your operator system is not linux,ony linux the datetime update is needed.";
#endif
    response_result = drogon::Custom::getJsonResult(0, {}, "time synchronization successfully");
    auto res = HttpResponse::newHttpJsonResponse(response_result);
    callback(res);
}

void Device::restartProgram(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

}