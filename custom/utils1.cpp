//
// Created by aichao on 2022/5/5.
//

#include "utils.h"

using namespace drogon;

bool Custom::update_time(const std::string &host, const std::string &api_path) {

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


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string Custom::currentDateTime() {
    time_t now = time(nullptr);
    struct tm t_struct{};
    char buf[80];
    t_struct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &t_struct);
    return buf;
}

// time calculate
std::string Custom::time_delta(int d, int h, int m, int s) {
    time_t now = time(nullptr);
    now += d * 24 * 60 * 60 + h * 60 * 60 + m * 60 + s;
    struct tm t_struct{};
    char buf[80];
    t_struct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &t_struct);
    return buf;


}
