#include "api_Settings.h"
#include "custom/utils.h"
#include <filesystem>
#include "plugins/GlobalThreadPool.h"
#include "plugins/TrantorSocketClient.h"
#include "plugins/LoopTask.h"
#include <trantor/net/EventLoop.h>
#include "models/Settings.h"
#include "models/Attend.h"
#include "plugins/GlobalParameter.h"

using namespace api;
using namespace drogon::orm;
namespace fs = std::filesystem;
using SettingsModel = drogon_model::sqlite3::Settings;
using AttendModel = drogon_model::sqlite3::Attend;


enum class DeleteInterValUnit {
    Hour,
    Day,
    Week,
    Month,
    Year,
};

std::string deleteInterValUnitToString(DeleteInterValUnit unit) {
    static const std::unordered_map<DeleteInterValUnit, std::string> deleteInterValUnitMap = {
            {DeleteInterValUnit::Hour,  "hour"},
            {DeleteInterValUnit::Day,   "day"},
            {DeleteInterValUnit::Week,  "week"},
            {DeleteInterValUnit::Month, "month"},
            {DeleteInterValUnit::Year,  "year"},
    };
    auto it = deleteInterValUnitMap.find(unit);
    return (it != deleteInterValUnitMap.end()) ? it->second : "Unknown";
}


DeleteInterValUnit stringToDeleteInterValUnit(const std::string &str) {
    static const std::unordered_map<std::string, DeleteInterValUnit> stringToGenreMap = {
            {"hour",  DeleteInterValUnit::Hour},
            {"day",   DeleteInterValUnit::Day},
            {"week",  DeleteInterValUnit::Week},
            {"month", DeleteInterValUnit::Month},
            {"year",  DeleteInterValUnit::Year},
    };
    auto it = stringToGenreMap.find(str);
    return (it != stringToGenreMap.end()) ? it->second : DeleteInterValUnit::Month; // 默认为 Montn
}

double deleteInterValNumToSecondDouble(int64_t num, const std::string &unit_str) {
    DeleteInterValUnit unit = stringToDeleteInterValUnit(unit_str);
    auto delete_interval = std::chrono::seconds(0);  // 初始化，以防没有匹配的情况
    switch (unit) {
        case DeleteInterValUnit::Hour:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::hours(num));
            break;
        case DeleteInterValUnit::Day:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::days(num));
            break;
        case DeleteInterValUnit::Week:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::weeks(num));
            break;
        case DeleteInterValUnit::Month:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::months(num));
            break;
        case DeleteInterValUnit::Year:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::years(num));
            break;
        default:
            break;
    }
    double seconds_as_double = std::chrono::duration_cast<std::chrono::duration<double>>(
            delete_interval).count();
    return seconds_as_double;
}


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
                auto delete_interval = deleteInterValNumToSecondDouble(delete_interval_num, delete_interval_unit);
                // 获取当前时间立即删除
                std::string end_time = trantor::Date::now().after(
                        (-1 * delete_interval + 8 * 60 * 60)).toCustomedFormattedStringLocal("%Y-%m-%dT%H:%M:%S", true);

                auto ret = drogon::app().getPlugin<LoopTask>()->startDeleteAttendTimer(delete_interval, end_time);
                if (ret.get()) {
                    result = drogon::Custom::getJsonResult(0, *obj, "updaet setting success");
                } else {
                    result = drogon::Custom::getJsonResult(0, *obj, "update setting failed");
                }
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

#if __LINUX__
    system("ps aux | grep "+ fram_app_name + "| awk '{print $2}' | xargs kill -9");
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