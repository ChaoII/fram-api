#include "AttendInfo.h"

using namespace drogon::orm;

using Attend = drogon_model::sqlite3::Attend;
using Staff = drogon_model::sqlite3::Staff;
// Add definition of your processing function here

void save_face_info(Json::Value &staff_info) {
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto staff_id = staff_info["staff_id"].asString();
    auto name = staff_info["name"].asString();
    auto file_path = staff_info["file_path"].asString();
    auto staff_ = mp.findBy(Criteria(Staff::Cols::_staff_id, CompareOperator::EQ, staff_id));
    Staff staff;
    staff.setStaffId(staff_id);
    staff.setName(name);
    staff.setFilePath(file_path);
    staff.setUpdateTime(Custom::currentDateTime());
    if (staff_.empty()) {
        mp.insert(staff);
    } else {
        std::vector<std::string> update_col_names;
        update_col_names.emplace_back("update_time");
        mp.updateBy(update_col_names, Criteria(Staff::Cols::_staff_id, CompareOperator::EQ, staff_id),
                    Custom::currentDateTime());
    }
}


void db_to_json_file() {
    Json::Value root;
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto staffs = mp.findAll();
    for (auto &staff: staffs) {
        root.append(staff.toJson());
    }
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, root);
    std::ofstream ofs;
    ofs.open("facelib/facelib_1.json");
    ofs << json_file;
    ofs.close();
}

void
AttendInfo::add_face_libs(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

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
        auto file_name = file.getFileName();
        auto staff_info = drogon::utils::splitString(file_name, "_");
        if (staff_info.size() >= 2) {
            if (file.getFileExtension() == "jpg") {
                auto staff_id = staff_info[0];
                auto name = drogon::utils::splitString(staff_info[1], ".")[0];
                auto file_path = std::string("./face/").append(staff_id).append(".jpg");
                sub["staff_id"] = staff_id;
                sub["name"] = name;
                sub["file_path"] = file_path;
                try {
                    file.saveAs(file_path);
                    save_face_info(sub);
                    sub["message"] = "file upload successfully!";
                }
                catch (std::exception &e) {
                    sub["message"] = "file save failed.";
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
    db_to_json_file();
    auto resp = HttpResponse::newHttpJsonResponse(root);
    callback(resp);
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

void AttendInfo::delete_face(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    auto resp = HttpResponse::newHttpResponse();
    auto staff_info = req->getJsonObject();
    std::string staff_id = (*staff_info)["staff_id"].asString();
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto staff = mp.findOne(Criteria(Staff::Cols::_staff_id, CompareOperator::EQ, staff_id));
    //delete face_img
    if (0 == remove(staff.getFilePath()->data())) {
        //delete face_info_db
        mp.deleteBy(Criteria(Staff::Cols::_staff_id, CompareOperator::EQ, staff_id));
        //update json file
        db_to_json_file();
        resp->setStatusCode(drogon::k200OK);
        resp->setBody("删除成功");
    } else {
        resp->setStatusCode(drogon::k403Forbidden);
        resp->setBody("删除失败");
    }
    callback(resp);
}

void
AttendInfo::download_img(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    auto staff_info = req->getJsonObject();
    std::string staff_id = (*staff_info)["staff_id"].asString();
    auto clientPtr = drogon::app().getDbClient();
    Mapper<Staff> mp(clientPtr);
    auto staff = mp.findOne(Criteria(Staff::Cols::_staff_id, CompareOperator::EQ, staff_id));
    //delete face_img
    auto file_path = staff.getFilePath()->data();
    auto resp = HttpResponse::newFileResponse(file_path);

    callback(resp);
}

void AttendInfo::clear_data(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    std::string start_time;
    std::string end_time;
    auto resp = HttpResponse::newHttpResponse();
    try {
        auto time_obj = req->getJsonObject();
        if (time_obj->empty()) {
            start_time = Custom::time_delta(-30000);
            end_time = Custom::time_delta(-7);
        } else {
            start_time = (*time_obj)["start_time"].asString();
            end_time = (*time_obj)["end_time"].asString();
        }
        auto clientPtr = drogon::app().getDbClient();
        Mapper<Attend> mp(clientPtr);
        mp.deleteBy((Criteria(Attend::Cols::_attend_time, CompareOperator::GE, start_time) &&
                     Criteria(Attend::Cols::_attend_time, CompareOperator::LE, end_time)));

        resp->setBody("数据清除成功");
        callback(resp);
    }
    catch (std::exception &e) {
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody("数据请求错误，请添加json请求体");
        callback(resp);
    }
}

void AttendInfo::update_time(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

}


