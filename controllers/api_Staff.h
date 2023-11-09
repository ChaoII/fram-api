#pragma once

#include <fstream>
#include <cstdio>
#include <drogon/drogon.h>
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include "models/Staff.h"

using namespace drogon;

namespace api {
    class Staff : public drogon::HttpController<Staff> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(Staff::addFace, "/api/staff/addFace", Options, Put, "LoginFilter");
            ADD_METHOD_TO(Staff::deleteFace, "/api/staff/deleteFace", Options, Delete, "LoginFilter");
            ADD_METHOD_TO(Staff::getFaceInfos, "/api/staff/getFaceInfos", Options, Post, "LoginFilter");
        METHOD_LIST_END

        void addFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void deleteFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

        void getFaceInfos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
    };
}
