/**
 *
 *  LoopTask.cc
 *
 */

#include "LoopTask.h"
#include <drogon/drogon.h>
#include <chrono>

using namespace std::chrono_literals;

using namespace drogon;

void LoopTask::initAndStart(const Json::Value &config) {
    /// Initialize and start the plugin
    updateTimeTimerId = drogon::app().getLoop()->runEvery(10min, [&]() {
        Json::Value client_result;
        std::string host = config.get("time_synchronization_server_host", "").asString();
        std::string api_path = config.get("time_synchronization_server_api", "").asString();
        auto client = HttpClient::newHttpClient(host);
        auto client_req = HttpRequest::newHttpRequest();
        client_req->setPath(api_path);
        client_req->setMethod(drogon::Post);
        client_req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
        client_req->setBody("{}");
        auto ret = client->sendRequest(client_req, 3);
        if (ret.first != ReqResult::Ok) {
            LOG_ERROR << "update time failed";
            return;
        }
        Json::Reader reader;
        reader.parse(std::string(ret.second->getBody()), client_result);
#if defined(__linux__) || defined(__APPLE__)
        system((std::string("sudo date -s ") + client_result["result"].asString()).c_str());
    LOG_INFO<<"update datetime successfully!";
#else
        LOG_WARN << "your operator system is not linux,ony linux the datetime update is needed.";
#endif
    });
}

trantor::TimerId LoopTask::getUpdateTimerId() {
    return updateTimeTimerId;
}

void LoopTask::shutdown() {
    /// Shutdown the plugin
    drogon::app().getLoop()->invalidateTimer(updateTimeTimerId);
}
