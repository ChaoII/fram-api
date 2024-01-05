#include "api_Settings.h"
#include "custom/utils.h"
#include "custom/timeInterhelper.h"
#include <filesystem>
#include "plugins/GlobalThreadPool.h"
#include "plugins/LoopTask.h"
#include <trantor/net/EventLoop.h>
#include "models/Settings.h"
#include "models/Attend.h"

using namespace api;
using namespace drogon::orm;
namespace fs = std::filesystem;
using SettingsModel = drogon_model::sqlite3::Settings;
using AttendModel = drogon_model::sqlite3::Attend;


void Settings::getSettings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value result, sub;
    Mapper<SettingsModel> mp(drogon::app().getDbClient());
    auto settings_ = mp.findAll()[0];
    sub["id"] = settings_.getValueOfId();
    sub["deleteIntervalNum"] = settings_.getValueOfDeleteIntervalNum();
    sub["deleteIntervalUnit"] = settings_.getValueOfDeleteIntervalUnit();
    sub["isEnableDelete"] = settings_.getValueOfIsEnableDelete();
    result = drogon::Custom::getJsonResult(0, sub, "success");
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void Settings::updateSettings(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value result, sub;
    auto obj = req->getJsonObject();
    uint64_t id = obj->get("id", "").asUInt64();
    int64_t delete_interval_num = obj->get("deleteIntervalNum", "").asInt64();
    std::string delete_interval_unit = obj->get("deleteIntervalUnit", "").asString();
    bool isEnableDelete = obj->get("isEnableDelete", "").asBool();
    Mapper<SettingsModel> mp(drogon::app().getDbClient());
    auto SettingsModels = mp.findBy(Criteria(SettingsModel::Cols::_id, CompareOperator::EQ, id));
    if (!SettingsModels.empty()) {
        auto SettingsModel = SettingsModels[0];
        SettingsModel.setDeleteIntervalNum(delete_interval_num);
        SettingsModel.setDeleteIntervalUnit(delete_interval_unit);
        SettingsModel.setIsEnableDelete(isEnableDelete);
        size_t size = mp.update(SettingsModel);
        if (size <= 0) {
            result = drogon::Custom::getJsonResult(-1, *obj, "update settings failed");
        } else {
            //更新定时任务
            if (isEnableDelete) {
                auto delete_interval = IntervalHelper::interValNumToSecondDouble(delete_interval_num,
                                                                                 delete_interval_unit);
                drogon::app().getPlugin<LoopTask>()->deleteAttendHistory(delete_interval, isEnableDelete);
            }
            result = drogon::Custom::getJsonResult(0, *obj, "update settings success");
        }
    } else {
        result = drogon::Custom::getJsonResult(-1, *obj, "settings is not existing");
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void Settings::restartProgram(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value result;
    std::string fram_app_name = drogon::app().getCustomConfig().get("FRAM_APP_name", "FRAM").asString();

#if __linux__
    //system("ps aux | grep "+ fram_app_name + "| awk '{print $2}' | xargs kill -9");
    system((std::string("pkill ")+ fram_app_name).c_str());
    result["code"] = 0;
    result["data"] = {};
    result["message"] = "restarting...";
    result = drogon::Custom::getJsonResult(0, {}, "restarting...");
#else
    result = drogon::Custom::getJsonResult(-1, {}, "only support linux system please check your operate system;");
#endif
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}