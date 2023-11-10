/**
 *
 *  NotNullFilter.cc
 *
 */

#include "NotNullFilter.h"
#include "custom/utils.h"

using namespace drogon;

void NotNullFilter::doFilter(const HttpRequestPtr &req,
                             FilterCallback &&fcb,
                             FilterChainCallback &&fccb) {
    auto method = req->getMethod();
    if (method == Options || method == Get || method == Put) {
        fccb();
        return;
    }
    if (req->getJsonObject()) {
        fccb();
        return;
    }
    auto result = drogon::Custom::getJsonResult(-1, {}, "request params error");
    auto res = drogon::HttpResponse::newHttpJsonResponse(result);
    res->setStatusCode(k422UnprocessableEntity);
    fcb(res);
}
