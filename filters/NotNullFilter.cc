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

    if (req->getMethod() == HttpMethod::Options) {
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
