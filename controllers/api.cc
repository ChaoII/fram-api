#include "api.h"
#include "custom/utils.h"
#include <filesystem>

namespace fs = std::filesystem;

using namespace drogon::orm;

using Attend = drogon_model::sqlite3::Attend;
using Staff = drogon_model::sqlite3::Staff;
// Add definition of your processing function here

void save_face_info(Json::Value &staff_info) {
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto uid = staff_info["uid"].asString();
    auto name = staff_info["name"].asString();
    auto file_path = staff_info["file_path"].asString();
    auto staff_ = mp.findBy(Criteria(Staff::Cols::_uid, CompareOperator::EQ, uid));
    Staff staff;
    staff.setUid(uid);
    staff.setName(name);
    staff.setPicUrl(file_path);
    staff.setRegisterTime("");
    if (staff_.empty()) {
        mp.insert(staff);
    } else {
        std::vector<std::string> update_col_names;
        update_col_names.emplace_back("update_time");
        mp.updateBy(update_col_names, Criteria(Staff::Cols::_uid, CompareOperator::EQ, uid),
                    "");
    }
}


void
api::add_face_libs(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    MultiPartParser fileUpload;
    fileUpload.parse(req);
    Json::Value root, sub;
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    for (auto &file: fileUpload.getFiles()) {
        sub["staff_id"] = "";
        sub["name"] = "";
        sub["file_path"] = "";
        sub["message"] = "";
        const std::string &file_name = file.getFileName();
        auto staff_info = drogon::utils::splitString(file_name, "_");
        if (staff_info.size() >= 2) {
            if (file.getFileExtension() == "jpg") {
                auto staff_id = staff_info[0];
                auto name = drogon::utils::splitString(staff_info[1], ".")[0];
                auto file_path = std::string("./static/facelib/").append(staff_id).append(".jpg");
                sub["staff_id"] = staff_id;
                sub["name"] = name;
                sub["file_path"] = file_path;
                try {
                    file.saveAs(file_path);
                    save_face_info(sub);
                    sub["message"] = "file upload successfully!";
                }
                catch (std::exception &e) {
                    sub["message"] = "file save failed detail is : .";
                }
            } else {
                sub["message"] = "file's extension must be '.jpg'.";
            }
        } else {
            sub["file_path"] = file_name;
            sub["message"] = "file name must like 'staffid_name.jpg'.";
        }
        root.append(sub);
    }
    //save staff info into sqlite3
    auto resp = HttpResponse::newHttpJsonResponse(root);
    callback(resp);
}

void api::get_face_infos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value face_list, sub, result, root;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }

    std::string name = obj->get("name", "").asString();
    if (name.empty())
        name = "%";
    int page_size = obj->get("pageSize", "").asInt();
    int page_index = obj->get("pageIndex", "").asInt();
    Mapper<Staff> mp(drogon::app().getDbClient());

    auto conditions = Criteria(Staff::Cols::_name, CompareOperator::Like, name);
    auto face_infos = mp.limit(page_size).offset(
            (page_index - 1) * page_size).findBy(conditions);
    size_t total = mp.count(conditions);
    for (auto &face_info: face_infos) {
        sub["id"] = std::to_string(face_info.getValueOfId());
        sub["uid"] = face_info.getValueOfUid();
        sub["name"] = face_info.getValueOfName();
        sub["pic_url"] = face_info.getValueOfPicUrl();
        sub["register_time"] = face_info.getValueOfRegisterTime();
        face_list.append(sub);
    }
    root["face_infos"] = face_list;
    result["code"] = 0;
    root["total"] = total;
    result["data"] = root;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void
api::get_attend_infos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
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

    Mapper<Attend> mp(drogon::app().getDbClient());
    auto conditions = (Criteria(Attend::Cols::_attend_time, CompareOperator::GE, start_time) &&
                       Criteria(Attend::Cols::_attend_time, CompareOperator::LE, end_time)) &&
                      Criteria(Attend::Cols::_name, CompareOperator::Like, attend_name);
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

void api::delete_face(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    Json::Value result;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string uid = obj->get("uid", "").asString();
    Mapper<Staff> mp(drogon::app().getDbClient());
    auto staffs = mp.findBy(Criteria(Staff::Cols::_uid, CompareOperator::EQ, uid));
    //delete face_img
    bool is_removed = true;
    for (auto staff: staffs) {
        if (0 == remove(staff.getPicUrl()->data())) {
            LOG_INFO << "delete file [" << staff.getValueOfPicUrl() << "] success";
            fs::path file_path(staff.getValueOfPicUrl());
            if (file_path.has_parent_path()) {
                if (fs::is_empty(file_path.parent_path())) {
                    LOG_INFO << "directory [" << file_path.parent_path().string()
                             << "] is empty, will delete this directory";
                    fs::remove(file_path.parent_path());
                    LOG_INFO << "delete directory [" << file_path.parent_path().string() << "] success";
                }
            }
            is_removed &= true;

        } else {
            LOG_ERROR << "delete file [" << staff.getValueOfPicUrl() << "] failed";
            is_removed &= false;
        }
    }
    if (is_removed) {
        //delete face_info_db
        mp.deleteBy(Criteria(Staff::Cols::_uid, CompareOperator::EQ, uid));
        //update json file
        result["code"] = 0;
        result["data"] = {};
        result["msg"] = "success";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
    } else {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "删除失败";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
    }
}

void
api::download_img(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    auto staff_info = req->getJsonObject();
    std::string staff_id = (*staff_info)["staff_id"].asString();
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto staff = mp.findOne(Criteria(Staff::Cols::_uid, CompareOperator::EQ, staff_id));
    //delete face_img
    auto file_path = staff.getPicUrl()->data();
    auto resp = HttpResponse::newFileResponse(file_path);
    callback(resp);
}

void api::clear_data(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
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
        Mapper<Attend> mp(clientPtr);
        mp.deleteBy((Criteria(Attend::Cols::_attend_time, CompareOperator::GE, start_time) &&
                     Criteria(Attend::Cols::_attend_time, CompareOperator::LE, end_time)));

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

void api::update_time(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

//    bool ret = Custom::update_time();
//    auto resp = HttpResponse::newHttpResponse();
//    if (ret) {
//        resp->setStatusCode(drogon::k200OK);
//        resp->setBody("success");
//    } else {
//        resp->setStatusCode(drogon::k408RequestTimeout);
//        resp->setBody("failed，时间同步接口访问超时ß");
//    }
//    callback(resp);

}


void api::restart_frame(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    auto resp = HttpResponse::newHttpResponse();

#if __LINUX__
    system("ps aux | grep main.py | awk '{print $2}' | xargs kill -9");
    resp->setStatusCode(drogon::k200OK);
    resp->setBody("更新成功");
#elif __APPLE__
    resp->setStatusCode(drogon::k200OK);
    resp->setBody("this is apple group which has been unsupported this program");
    LOG_WARN << "this is apple group which has been unsupported this program";
#else
    resp->setStatusCode(drogon::k400BadRequest);
    resp->setBody("tplease check your operate system");
    LOG_WARN << "please check your operate system";
#endif
    callback(resp);
}
