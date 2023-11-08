//
// Created by aichao on 2022/5/5.
//

#include "utils1.h"

using namespace drogon;

bool Customs::update_time(const std::string &host, const std::string &api_path) {

    auto client = HttpClient::newHttpClient(host);
    auto req = HttpRequest::newHttpRequest();
    req->setPath(api_path);
    req->setMethod(drogon::Post);
    req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    req->setBody("{}");
    auto ret = client->sendRequest(req);
    if (ret.first != ReqResult::Ok) {
        return false;
    }
    Json::Reader reader;
    Json::Value root;
    reader.parse(std::string(ret.second->getBody()), root);

#ifdef __linux__
    system((std::string("sudo date -s ") + root["result"].asString()).c_str());
    LOG_INFO<<"update datetime successfully!";
#else
    LOG_WARN<<"your operator system is not linux,ony linux the datetime update is needed.";
#endif
    return true;
}

