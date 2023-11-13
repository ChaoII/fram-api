#include "api_Attend.h"
#include "custom/utils.h"


using namespace drogon::orm;
using namespace api;
using AttendModel = drogon_model::sqlite3::Attend;

void Attend::getAttendInfos(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value attend_list, sub, result, root;
    auto obj = req->getJsonObject();
    std::string attend_name = obj->get("attend_name", "").asString();
    std::string start_time = obj->get("start_time", "").asString();
    std::string end_time = obj->get("end_time", "").asString();
    if (start_time.empty()) {
        start_time = trantor::Date::now().roundDay().toCustomedFormattedStringLocal("%Y-%m-%dT%H:%M:%S", true);
    }
    if (end_time.empty()) {
        end_time = trantor::Date::now().roundDay().after(24 * 60 * 60).toCustomedFormattedStringLocal(
                "%Y-%m-%dT%H:%M:%S", true);
    }
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
    root["total"] = total;
    result = drogon::Custom::getJsonResult(0, root, "success");
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}


void Attend::clearHistoryAttendData(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback) const {
    auto time_obj = req->getJsonObject();
    std::string start_time = (*time_obj)["start_time"].asString();
    std::string end_time = (*time_obj)["end_time"].asString();
    Mapper<AttendModel> mp(drogon::app().getDbClient());
    mp.deleteBy((Criteria(AttendModel::Cols::_attend_time, CompareOperator::GE, start_time) &&
                 Criteria(AttendModel::Cols::_attend_time, CompareOperator::LE, end_time)));

    auto result = drogon::Custom::getJsonResult(0, {}, "clear attend history data success");
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}




