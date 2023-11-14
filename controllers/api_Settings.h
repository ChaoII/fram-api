#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api {
    class Settings : public drogon::HttpController<Settings> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Settings::getSettings, "/api/settings/getSettings", Options, Get, "LoginFilter");
            ADD_METHOD_TO(Settings::updateSettings, "/api/settings/updateSettings", Options, Post, "LoginFilter");
            ADD_METHOD_TO(Settings::restartProgram, "/api/settings/restartProgram", Options, Post, "LoginFilter");
        METHOD_LIST_END

        void getSettings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void updateSettings(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void restartProgram(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    private:
        inline static std::map<std::string, trantor::TimerId> globalCache_{{"deleteAttendTimerId", -1}};
    };
}
