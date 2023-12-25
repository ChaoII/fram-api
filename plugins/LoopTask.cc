/**
 *
 *  LoopTask.cc
 *
 */

#include "LoopTask.h"
#include <drogon/drogon.h>
#include <chrono>
#include "plugins/GlobalThreadPool.h"

using namespace std::chrono_literals;

using namespace drogon;

void LoopTask::initAndStart(const Json::Value &config) {
    /// Initialize and start the plugin
    drogon::app().getLoop()->runEvery(10min, [&]() {
        Json::Value client_result;
        std::string host = config.get("time_synchronization_server_host", "").asString();
        std::string api_path = config.get("time_synchronization_server_api", "").asString();
        LOG_INFO << host;
        LOG_INFO << api_path;
        auto client = HttpClient::newHttpClient(host);
        auto client_req = HttpRequest::newHttpRequest();
        client_req->setPath(api_path);
        client_req->setMethod(drogon::Post);
        client_req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
        client_req->setBody("{}");
        client->sendRequest(
                client_req,
                [&](ReqResult r, const HttpResponsePtr &resp) {
                    auto c= resp->getJsonObject()->get("name","").asString();
                    if (r != ReqResult::Ok) {
                        LOG_ERROR << r;
                        return;
                    }
                    Json::Reader reader;
                    reader.parse(std::string(resp->getBody()), client_result);
#if defined(__linux__) || defined(__APPLE__)
                    system((std::string("sudo date -s ") + client_result["result"].asString()).c_str());
                    LOG_INFO<<"update datetime successfully!";
#else
                    LOG_WARN << "your operator system is not linux,ony linux the datetime update is needed.";
#endif
                },
                3);
    });
}

trantor::TimerId LoopTask::getUpdateTimerId() {
    return updateTimeTimerId;
}

void LoopTask::shutdown() {
    /// Shutdown the plugin
    drogon::app().getLoop()->invalidateTimer(updateTimeTimerId);
}

std::future<bool> LoopTask::startDeleteAttendTimer(double internal, const std::string &endTime) {
    drogon::app().getLoop()->invalidateTimer(deleteAttendTimerId);
    LOG_INFO << "开启定时任务,任务间隔" << internal << "s";
    trantor::TimerId timer_id = drogon::app().getLoop()->runEvery(internal, [&]() {
        app().getPlugin<TrantorSocketClient>()->sendMessage("3@" + endTime);
        app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->commit([&]() {
            auto start = std::chrono::steady_clock::now();
            while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
                std::this_thread::sleep_for(std::chrono::microseconds(500));
                auto end = std::chrono::steady_clock::now();
                auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                if (interval >= 2000) {
                    LOG_INFO << "delete attend history time out...";
                }
            }
            auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
            if (receive_result == "0x0003") {
                LOG_INFO << "delete history data success end time is [" + endTime + "]";
            } else {
                LOG_ERROR << "delete history data failed";
            }
        });

    });
    deleteAttendTimerId = timer_id;
    app().getPlugin<TrantorSocketClient>()->sendMessage("3@" + endTime);
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->commit([&]() {
        auto start = std::chrono::steady_clock::now();
        while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            // add timeout condition
            auto end = std::chrono::steady_clock::now();
            auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (interval >= 2000) {
                LOG_INFO << "delete face time_out...";
            }
        }
        auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
        if (receive_result == "0x0002") {
            return true;
        } else {
            return false;
        }
    });
    return ret;
}
