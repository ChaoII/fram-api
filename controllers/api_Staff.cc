#include "api_Staff.h"
#include "custom/utils.h"
#include <filesystem>
#include "plugins/GlobalThreadPool.h"
#include <trantor/net/EventLoop.h>
#include "plugins/TrantorSocketClient.h"

using namespace api;
using namespace drogon::orm;
namespace fs = std::filesystem;
using StaffModel = drogon_model::sqlite3::Staff;

void Staff::addFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    MultiPartParser fileUpload;
    fileUpload.parse(req);
    Json::Value result, sub;
    std::string glob_file_path_str;
    LOG_INFO << "[start add face ]...";
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->submit([&]() {
        auto file = fileUpload.getFiles()[0];
        if (file.getFileExtension() == "jpg") {
            auto uid = fileUpload.getParameters().at("uid");
            auto name = fileUpload.getParameters().at("name");
            uint64_t uuid = drogon::Custom::getUuid();
            glob_file_path_str = app().getUploadPath() + "/" + uid + "/" + std::to_string(uuid) + ".jpg";
            fs::path file_path(glob_file_path_str);
            try {
                file.saveAs(file_path.string());
                LOG_INFO << "file save in [" << file_path.parent_path().string() << "]";
                std::string message = "1@" + fs::absolute(file_path).string() + "@" + uid + "@" + name;
                drogon::app().getPlugin<TrantorSocketClient>()->sendMessage(message);
                auto start = std::chrono::steady_clock::now();
                while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(500));
                    auto end = std::chrono::steady_clock::now();
                    auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                    if (interval >= 2000) {
                        LOG_INFO << "add face time_out...";
                        drogon::Custom::removeFileWithParentDir(glob_file_path_str);
                        return drogon::Custom::getJsonResult(-1, sub, "add face failed, for socket server time out");
                    }
                }
                auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
                LOG_INFO << receive_result;
                sub["error"] = receive_result;
                if (receive_result == "0x0001") {
                    result = drogon::Custom::getJsonResult(0, sub, "success");
                } else {
                    drogon::Custom::removeFileWithParentDir(glob_file_path_str);
                    result = drogon::Custom::getJsonResult(-1, sub, "add face failed");
                }
            }
            catch (std::exception &e) {
                LOG_INFO << "delete failed, detail:" << e.what();
                drogon::Custom::removeFileWithParentDir(glob_file_path_str);
                result = drogon::Custom::getJsonResult(-1, sub, "delete face failed");
            }
        } else {
            result = drogon::Custom::getJsonResult(-1, sub, "delete face failed");
        }
        return result;
    });
    auto resp = HttpResponse::newHttpJsonResponse(ret.get());
    callback(resp);
}

void Staff::deleteFace(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {

    Json::Value result, sub;
    auto obj = req->getJsonObject();
    std::string index_id = obj->get("index_id", "").asString();
    app().getPlugin<TrantorSocketClient>()->sendMessage("2@" + index_id);
    auto ret = app().getPlugin<GlobalThreadPool>()->getGlobalThreadPool()->submit([&]() {
        auto start = std::chrono::steady_clock::now();
        while (app().getPlugin<TrantorSocketClient>()->getReceiveMsg().empty()) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            // add timeout condition
            auto end = std::chrono::steady_clock::now();
            auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (interval >= 2000) {
                LOG_INFO << "delete face time_out...";
                return drogon::Custom::getJsonResult(-1, sub, "add face failed, for socket server time out");
            }
        }
        auto receive_result = app().getPlugin<TrantorSocketClient>()->getReceiveMsg();
        sub["error"] = receive_result;
        if (receive_result == "0x0002") {
            result = drogon::Custom::getJsonResult(0, sub, "delete success");
        } else {
            result = drogon::Custom::getJsonResult(-1, sub, "delete failed");
        }
        return result;
    });
    auto resp = HttpResponse::newHttpJsonResponse(ret.get());
    callback(resp);
}

void Staff::getFaceInfos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    Json::Value face_list, sub, result, root;
    auto obj = req->getJsonObject();
    std::string name = obj->get("query_name", "").asString();
    if (name.empty())
        name = "%";
    int page_size = obj->get("pageSize", "").asInt();
    int page_index = obj->get("pageIndex", "").asInt();
    Mapper<StaffModel> mp(drogon::app().getDbClient());
    auto conditions = Criteria(StaffModel::Cols::_name, CompareOperator::Like, name);
    auto face_infos = mp.limit(page_size).offset(
            (page_index - 1) * page_size).orderBy(StaffModel::Cols::_register_time, SortOrder::DESC).findBy(conditions);
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
    root["total"] = total;
    result = drogon::Custom::getJsonResult(0, root, "success");
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}



