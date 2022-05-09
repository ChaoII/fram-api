#pragma once

#include <drogon/orm/Mapper.h>
#include <drogon/HttpController.h>
#include "models/Staff.h"

using namespace drogon;

class Html : public drogon::HttpController<Html> {
public:
    METHOD_LIST_BEGIN

        ADD_METHOD_TO(Html::index, "/index", Get);
        ADD_METHOD_TO(Html::attendinfo, "/attendinfo", Get);
        ADD_METHOD_TO(Html::faceinfo, "/faceinfo", Get);
    METHOD_LIST_END

    // your declaration of processing function maybe like this:
    void index(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void attendinfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void faceinfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
