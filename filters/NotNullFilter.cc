/**
 *
 *  NotNullFilter.cc
 *
 */

#include "NotNullFilter.h"

using namespace drogon;

void NotNullFilter::doFilter(const HttpRequestPtr &req,
                             FilterCallback &&fcb,
                             FilterChainCallback &&fccb) {

    LOG_INFO << "enter in NotNullFilter";

    auto method = req->getMethod();
    if (method == Options || method == Get || method == Put) {
        fccb();
        return;
    }
    if (req->getJsonObject()) {
        fccb();
        return;
    }
    Json::Value result;
    result["code"] = -1;
    result["data"] = {};
    result["msg"] = "request params error";
    auto res = drogon::HttpResponse::newHttpJsonResponse(result);
    res->setStatusCode(k422UnprocessableEntity);
    fcb(res);
}
