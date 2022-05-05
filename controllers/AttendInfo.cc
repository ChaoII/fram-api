#include "AttendInfo.h"

using namespace drogon::orm;

using Attend = drogon_model::sqlite3::Attend;
// Add definition of your processing function here

void
AttendInfo::add_face_libs(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {


}

void
AttendInfo::get_attend_infos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    auto time_obj = req->getJsonObject();
    std::string start_time = (*time_obj)["start_time"].asString();
    std::string end_time = (*time_obj)["end_time"].asString();
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Attend> mp(clientPtr);
    auto attend_infos = mp.findBy((Criteria(Attend::Cols::_attend_time, CompareOperator::GE, start_time) &&
                                   Criteria(Attend::Cols::_attend_time, CompareOperator::LE, end_time)));

    Json::Value root, sub;

    for (auto &attend_info: attend_infos) {
        sub["staff_id"] = attend_info.getValueOfStaffId();
        sub["name"] = attend_info.getValueOfName();
        sub["attend_time"] = attend_info.getValueOfAttendTime();
        root.append(sub);
    }
    auto resp = HttpResponse::newHttpJsonResponse(root);
    callback(resp);
}
