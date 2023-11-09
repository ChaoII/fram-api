/**
 *
 *  LoginFilter.cc
 *
 */

#include "LoginFilter.h"
#include <jwt-cpp/jwt.h>

using namespace drogon;

void LoginFilter::doFilter(const HttpRequestPtr &req,
                           FilterCallback &&fcb,
                           FilterChainCallback &&fccb) {
    //Edit your logic here
    if (req->getMethod() == HttpMethod::Options) {
        fccb();
        return;
    }
    auto token = req->getHeader("token");
    auto verifier = jwt::verify()
            //钟鸣鼎食
            .allow_algorithm(jwt::algorithm::hs256{"zhongmingdingshi"})
            .with_issuer("aichao");
    try {
        auto decoded_token = jwt::decode(token);
        verifier.verify(decoded_token);
        auto ret = decoded_token.get_subject();
        // 如果有权限问题，可以根据用户名查找权限，该项目没有角色权限问题，可以直接设置
        req->addHeader("User-Info", ret);
        fccb();
        return;
    } catch (const std::exception &ex) {
        LOG_WARN << ex.what();
        Json::Value result;
        result["code"] = -2;
        result["data"] = ex.what();
        result["msg"] = "login error please login again";
        auto res = drogon::HttpResponse::newHttpJsonResponse(result);
        res->setStatusCode(k401Unauthorized);
        fcb(res);
    }
    //Check failed
}
