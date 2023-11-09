#pragma once

#include <drogon/HttpController.h>
#include "models/Attend.h"
#include <drogon/orm/Mapper.h>

using namespace drogon;

namespace api {
    class Attend : public drogon::HttpController<Attend> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Attend::clearHistoryAttendData, "/api/attend/clearHistoryAttendData", Options, Delete,
                          "LoginFilter");
            ADD_METHOD_TO(Attend::getAttendInfos, "/api/attend/getAttendInfos", Options, Post, "LoginFilter");
        METHOD_LIST_END

        void getAttendInfos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void clearHistoryAttendData(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback) const;
    };
}
