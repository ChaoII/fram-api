//
// Created by aichao on 2022/5/5.
//

#include "utils.h"

using namespace drogon;

void Custom::update_time(const std::string host, const std::string &api_path) {

    auto client = HttpClient::newHttpClient(host);
    auto req = HttpRequest::newHttpRequest();
    req->setPath(api_path);
    req->setMethod(drogon::Post);
    req->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
    req->setBody("{}");
    client->sendRequest(
            req, [](ReqResult result, const HttpResponsePtr &response) {
                if (result != ReqResult::Ok) {
                    std::cout
                            << "error while sending request to server! result: "
                            << result << std::endl;
                    return;
                }
                std::cout << "receive response!" << std::endl;
                // auto headers=response.
                Json::Reader reader;
                Json::Value root;
                reader.parse(std::string(response->getBody()), root);
#ifdef __linux__
                system((std::string("sudo date -s ") + root["result"].asString()).c_str());
                spdlog::info("update datetime successfully!");
#elif
                spdlog::warn("your operator system is not linux,ony linux the datetime update is needed.")
#endif
            });
}
