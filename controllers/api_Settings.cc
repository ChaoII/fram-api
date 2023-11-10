#include "api_Settings.h"
#include "custom/utils.h"
#include <filesystem>
#include "plugins/GlobalThreadPool.h"
#include <trantor/net/EventLoop.h>
#include "plugins/TrantorSocketClient.h"
#include "models/Settings.h"

using namespace api;
using namespace drogon::orm;
namespace fs = std::filesystem;
using SettingsModel = drogon_model::sqlite3::Settings;

void Settings::getSettings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value result, sub;
    Mapper<SettingsModel> mp(drogon::app().getDbClient());
    auto settings_ = mp.findAll()[0];
    sub["deleteIntervalNum"] = settings_.getValueOfDeleteIntervalNum();
    sub["deleteIntervalUnit"] = settings_.getValueOfDeleteIntervalUnit();
    sub["isEnableDelete"] = settings_.getValueOfIsEnableDelete();
    result = drogon::Custom::getJsonResult(0, sub, "success");
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void
Settings::updateSettings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

}

void
Settings::restartProgram(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
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




