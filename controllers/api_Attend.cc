#include "api_Attend.h"
#include "custom/utils.h"


using namespace drogon::orm;
using namespace api;
using AttendModel = drogon_model::sqlite3::Attend;

void Attend::getAttendInfos(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value attend_list, sub, result, root;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string attend_name = obj->get("attend_name", "").asString();
    std::string start_time = drogon::Custom::front_time_to_backend(obj->get("start_time", "").asString());
    std::string end_time = drogon::Custom::front_time_to_backend(obj->get("end_time", "").asString());
    if (attend_name.empty())
        attend_name = "%";
    int page_size = obj->get("pageSize", "").asInt();
    int page_index = obj->get("pageIndex", "").asInt();

    Mapper<AttendModel> mp(drogon::app().getDbClient());
    auto conditions = (Criteria(AttendModel::Cols::_attend_time, CompareOperator::GE, start_time) &&
                       Criteria(AttendModel::Cols::_attend_time, CompareOperator::LE, end_time)) &&
                      Criteria(AttendModel::Cols::_name, CompareOperator::Like, attend_name);
    auto attend_infos = mp.limit(page_size).offset(
            (page_index - 1) * page_size).findBy(conditions);
    size_t total = mp.count(conditions);

    for (auto &attend_info: attend_infos) {
        sub["id"] = std::to_string(attend_info.getValueOfId());
        sub["uid"] = attend_info.getValueOfUid();
        sub["name"] = attend_info.getValueOfName();
        sub["pic_url"] = attend_info.getValueOfPicUrl();
        sub["attend_time"] = attend_info.getValueOfAttendTime();
        attend_list.append(sub);
    }
    root["attend_infos"] = attend_list;
    result["code"] = 0;
    root["total"] = total;
    result["data"] = root;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}


void Attend::clearHistoryAttendData(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback) const {
    std::string start_time;
    std::string end_time;
    auto resp = HttpResponse::newHttpResponse();
    try {
        auto time_obj = req->getJsonObject();
        if (time_obj == nullptr) {
            resp->setBody("请传入正确的json对象，{}被允许");
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        if (time_obj->empty()) {

        } else {
            start_time = (*time_obj)["start_time"].asString();
            end_time = (*time_obj)["end_time"].asString();
        }
        auto clientPtr = drogon::app().getDbClient();
        Mapper<AttendModel> mp(clientPtr);
        mp.deleteBy((Criteria(AttendModel::Cols::_attend_time, CompareOperator::GE, start_time) &&
                     Criteria(AttendModel::Cols::_attend_time, CompareOperator::LE, end_time)));

        resp->setStatusCode(k200OK);
        resp->setBody("数据清除成功");
        callback(resp);
    }
    catch (std::exception &e) {
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody(std::string("数据清除错误") + e.what());
        callback(resp);
    }
}




