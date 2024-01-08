/**
 *
 *  LoopTask.cc
 *
 */

#include "LoopTask.h"
#include <drogon/drogon.h>
#include <chrono>
#include "plugins/GlobalThreadPool.h"
#include "models/Settings.h"
#include "custom/timeInterhelper.h"

using namespace drogon::orm;
using SettingsModel = drogon_model::sqlite3::Settings;

using namespace std::chrono_literals;

using namespace drogon;

void LoopTask::initAndStart(const Json::Value &config) {
    host_ = config.get("time_synchronization_server_host", "").asString();
    api_path_ = config.get("time_synchronization_server_api", "").asString();
    app().getLoop()->runAfter(20s, [&]() {
        deleteAttendAtStart();
    });
    startUpdateDateTimeTask();
}


void LoopTask::shutdown() {
    /// Shutdown the plugin
    drogon::app().getLoop()->invalidateTimer(updateTimeTimerId_);
    drogon::app().getLoop()->invalidateTimer(deleteAttendTimerId_);
}

void LoopTask::deleteAttendHistory(double interval, bool enable_delete) {
    if (not enable_delete) {
        drogon::app().getLoop()->invalidateTimer(deleteAttendTimerId_);
        return;
    }
    // 立即删除
    deleteAttendHistorySub(interval);
    // 开启定时器定期删除
    updateDeleteAttendTimer(interval);
}

void LoopTask::updateDeleteAttendTimer(double interval) {
    // 关闭定时器
    drogon::app().getLoop()->invalidateTimer(deleteAttendTimerId_);
    trantor::TimerId timer_id = drogon::app().getLoop()->runEvery(interval, [&]() {
        deleteAttendHistorySub(interval);
    });
    deleteAttendTimerId_ = timer_id;
}

void LoopTask::deleteAttendHistorySub(double interval) {
    auto endTime = trantor::Date::now().after((-1 * interval)).
            toCustomedFormattedStringLocal("%Y-%m-%dT%H:%M:%S", true);
    LOG_INFO << "======== before: " << endTime << " the attend history will be deleted ========";
    app().getPlugin<TrantorSocketClient>()->sendMessage("3@" + endTime);
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->commit([&]() {
        auto start = std::chrono::steady_clock::now();
        while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // add timeout condition
            auto end = std::chrono::steady_clock::now();
            auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (interval >= 2000) {
                LOG_ERROR << "delete face time_out...";
                return;
            }
        }
        auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
        if (receive_result == "0x0003") {
            LOG_INFO << "delete successfully...";
        } else {
            LOG_ERROR << "delete failed...";
        }
    });
}

void LoopTask::deleteAttendAtStart() {
    Mapper<SettingsModel> mp(drogon::app().getDbClient());
    auto settings_ = mp.findAll()[0];
    auto delete_interval_num = (int64_t) settings_.getValueOfDeleteIntervalNum();
    const auto &delete_interval_unit = settings_.getValueOfDeleteIntervalUnit();
    bool is_enable_delete = settings_.getValueOfIsEnableDelete();
    if (is_enable_delete) {
        auto delete_interval = IntervalHelper::interValNumToSecondDouble(delete_interval_num, delete_interval_unit);
        deleteAttendHistory(delete_interval, is_enable_delete);
    }
}

void LoopTask::startUpdateDateTimeTask() {
    // 立即执行
    startUpdateDateTimeTaskSub();
    updateTimeTimerId_ = drogon::app().getLoop()->runEvery(10min, [&]() {
        startUpdateDateTimeTaskSub();
    });
}

void LoopTask::startUpdateDateTimeTaskSub() {
    /// Initialize and start the plugin
    auto client = HttpClient::newHttpClient(host_);
    auto client_req = HttpRequest::newHttpRequest();
    client_req->setPath(api_path_);
    client_req->setMethod(drogon::Post);
    client_req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    client_req->setBody("{}");
    client->sendRequest(
            client_req, [&](ReqResult r, const HttpResponsePtr &resp) {
                if (r != ReqResult::Ok) {
                    LOG_ERROR << r;
                    return;
                }
                std::string time_str = resp->getJsonObject()->get("result", "").asString();
                if (time_str.empty()) return;
                LOG_INFO << time_str;
#if defined(__linux__) || defined(__APPLE__)
                system((std::string("sudo date -s ") + time_str).c_str());
                LOG_INFO << "update datetime successfully!";
#else
                LOG_WARN << "your operator system is not linux,ony linux the datetime update is needed.";
#endif
            }, 3);
}
