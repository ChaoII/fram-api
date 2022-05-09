#include "Html.h"

// Add definition of your processing function here
void Html::index(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

    auto resp = HttpResponse::newHttpViewResponse("dashboard");
//    auto resp = HttpResponse::newHttpViewResponse("dashboard");
    callback(resp);

}

void Html::attendinfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpViewResponse("attendinfo");
//    auto resp = HttpResponse::newHttpViewResponse("dashboard");
    callback(resp);

}

void Html::faceinfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    HttpViewData data;
    Json::Value root;
    auto clientPtr = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Staff> mp(clientPtr);
    auto staffs = mp.findAll();
    for (auto &staff: staffs) {
        root.append(staff.toJson());
    }
    data.insert("staffs", root);
    auto resp = HttpResponse::newHttpViewResponse("faceinfo", data);
//    auto resp = HttpResponse::newHttpViewResponse("dashboard");
    callback(resp);

}