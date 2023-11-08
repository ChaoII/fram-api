#include "api_Staff.h"
#include "custom/utils.h"
#include <filesystem>
#include "plugins/GlobalThreadPool.h"
#include <trantor/net/EventLoop.h>
#include "plugins/TrantorSocketClient.h"

namespace fs = std::filesystem;
using namespace drogon::orm;
using namespace api;
using StaffModel = drogon_model::sqlite3::Staff;

void Staff::addFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    MultiPartParser fileUpload;
    fileUpload.parse(req);
    Json::Value result, sub;
    LOG_INFO << "add face...";
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->submit([&]() {
        auto file = fileUpload.getFiles()[0];
        if (file.getFileExtension() == "jpg") {
            auto uid = fileUpload.getParameters().at("uid");
            auto name = fileUpload.getParameters().at("name");
            uint64_t uuid = drogon::Custom::get_uuid();
            std::string file_path_str = app().getUploadPath() + "/" + uid + "/" + std::to_string(uuid) + ".jpg";
            fs::path file_path(file_path_str);
            try {
                file.saveAs(file_path.string());
                LOG_INFO << "file save in [" << file_path.parent_path().string() << "]";
                std::string message = "1@" + fs::absolute(file_path).string() + "@" + uid + "@" + name;
                drogon::app().getPlugin<TrantorSocketClient>()->sendMessage(message);
                while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(500));
                }
                auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
                LOG_INFO << receive_result;
                sub["error"] = receive_result;
                if (receive_result == "0x0001") {
                    //update json file
                    result["code"] = 0;
                    result["data"] = sub;
                    result["msg"] = "success";
                } else {
                    result["code"] = -1;
                    result["data"] = sub;
                    result["msg"] = "delete failed";
                }
            }
            catch (std::exception &e) {
                result["code"] = -1;
                result["data"] = {};
                result["msg"] = "file save failed detail is : .";
            }
        } else {
            result["code"] = -1;
            result["data"] = {};
            result["message"] = "file's extension must be '.jpg'.";
        }
        return result;
    });
    auto resp = HttpResponse::newHttpJsonResponse(ret.get());
    callback(resp);
}

void Staff::deleteFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    Json::Value result, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string index_id = obj->get("index_id", "").asString();
    app().getPlugin<TrantorSocketClient>()->sendMessage("-1@" + index_id);
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->submit([&]() {
        while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
        auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
        sub["error"] = receive_result;
        if (receive_result == "0x0002") {
            //update json file
            result["code"] = 0;
            result["data"] = sub;
            result["msg"] = "success";
        } else {
            result["code"] = -1;
            result["data"] = sub;
            result["msg"] = "delete failed";
        }
        return result;
    });
    auto resp = HttpResponse::newHttpJsonResponse(ret.get());
    callback(resp);
}

void Staff::getFaceInfos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
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
    std::string name = obj->get("query_name", "").asString();
    if (name.empty())
        name = "%";
    int page_size = obj->get("pageSize", "").asInt();
    int page_index = obj->get("pageIndex", "").asInt();
    Mapper<StaffModel> mp(drogon::app().getDbClient());

    auto conditions = Criteria(StaffModel::Cols::_name, CompareOperator::Like, name);
    auto face_infos = mp.limit(page_size).offset(
            (page_index - 1) * page_size).findBy(conditions);
    size_t total = mp.count(conditions);
    for (auto &face_info: face_infos) {
        sub["id"] = std::to_string(face_info.getValueOfId());
        sub["index_id"] = std::to_string(face_info.getValueOfIndexId());
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



