#include "api_User.h"
#include "models/Users.h"
#include "jwt-cpp/jwt.h"
#include <drogon/orm/Mapper.h>

using namespace api;
using namespace drogon::orm;
using Users = drogon_model::sqlite3::Users;

void User::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error, must be {'username':'xx','password':'xx'}";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string username = obj->get("username", "").asString();
    std::string password = drogon::utils::getMd5(obj->get("password", "").asString());
    if (!username.empty() && !password.empty()) {
        Mapper<Users> mp(drogon::app().getDbClient());
        auto user_ = mp.findBy(Criteria(Users::Cols::_username, CompareOperator::EQ, username) &&
                               Criteria(Users::Cols::_password, CompareOperator::EQ, password));

        if (!user_.empty()) {
            auto now = std::chrono::system_clock::now();
            auto exp = now + std::chrono::hours(7 * 24); // 过期时间为当前时间 + 7 天
            auto token = jwt::create()
                    .set_issuer("aichao")
                    .set_subject(username)
                    .set_issued_at(now)
                    .set_expires_at(exp)
                    .sign(jwt::algorithm::hs256{"zhongmingdingshi"});
            sub["token"] = token;
            sub["nickname"] = *user_[0].getNickname();
            sub["username"] = username;
            result["code"] = 0;
            result["data"] = sub;
            result["msg"] = "login success";
        } else {
            result["code"] = -1;
            result["data"] = {};
            result["msg"] = "login please checkout username or password, if you dont register please register a account firstly";
        }
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void User::add_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value result, sub;
    auto obj = req->getJsonObject();
    if (obj == nullptr) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error, must be {'username':'xx','password':'xx'}";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    std::string username = obj->get("username", "").asString();
    std::string nickname = obj->get("nickname", "").asString();
    std::string password = drogon::utils::getMd5(obj->get("password", "").asString());
    if (username.empty() || password.empty()) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "username or password can not be empty";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    Mapper<Users> mp(drogon::app().getDbClient());
    auto user_ = mp.findBy(Criteria(Users::Cols::_username, CompareOperator::EQ, username));
    if (user_.empty()) {
        Users user;
        user.setUsername(username);
        user.setPassword(password);
        user.setNickname(nickname);
        mp.insert(user);
        result["code"] = 0;
        result["data"] = {username};
        result["msg"] = "register success";
    } else {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "register failed, username has been used, please change your username";
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void User::get_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

    std::string search_name = req->getParameter("searchName");
    int page_size = std::stoi(req->getParameter("pageSize"));
    int page_index = std::stoi(req->getParameter("pageIndex"));
    Json::Value result, root, user_list, sub;
    Mapper<Users> mp(drogon::app().getDbClient());
    if (search_name.empty())
        search_name = "%";
    auto conditions = Criteria(Users::Cols::_username, CompareOperator::Like, search_name);
    auto users = mp.limit(page_size).offset(
            (page_index - 1) * page_size).findBy(conditions);
    size_t count = mp.count(conditions);
    for (auto &user: users) {
        sub["id"] = user.getValueOfId();
        sub["username"] = user.getValueOfUsername();
        sub["nickname"] = user.getValueOfNickname();
        user_list.append(sub);
    }
    root["user"] = user_list;
    root["total"] = count;
    result["code"] = 0;
    result["data"] = root;
    result["msg"] = "success";
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void User::del_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

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
    std::string id = obj->get("id", "").asString();
    if (id.empty()) {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "request params error";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
        return;
    }
    Mapper<Users> mp(drogon::app().getDbClient());
    size_t size = mp.deleteBy(Criteria(Users::Cols::_id, CompareOperator::EQ, id));
    if (size > 0) {
        result["code"] = 0;
        result["data"] = {};
        result["msg"] = "delete success";
    } else {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "delete failed";
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void User::edit_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

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
    std::string id = obj->get("id", "").asString();
    std::string username = obj->get("username", "").asString();
    std::string nickname = obj->get("nickname", "").asString();
    Mapper<Users> mp(drogon::app().getDbClient());
    auto users = mp.findBy(Criteria(Users::Cols::_id, CompareOperator::EQ, id));
    if (!users.empty()) {
        auto user = users[0];
        user.setUsername(username);
        user.setNickname(nickname);
        size_t size = mp.update(user);
        if (size <= 0) {
            result["code"] = -1;
            result["data"] = {};
            result["msg"] = "update failed";
        } else {
            result["code"] = 0;
            result["data"] = *obj;
            result["msg"] = "update success";
        }
    } else {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "delete failed";
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void User::modify_password(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
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
    std::string username = obj->get("username", "").asString();
    std::string old_password = drogon::utils::getMd5(obj->get("oldPassword", "").asString());
    LOG_INFO << username;
    LOG_INFO << old_password;
    std::string new_password = drogon::utils::getMd5(obj->get("newPassword", "").asString());
    Mapper<Users> mp(drogon::app().getDbClient());
    auto users = mp.findBy(Criteria(Users::Cols::_username, CompareOperator::EQ, username) &&
                           Criteria(Users::Cols::_password, CompareOperator::EQ, old_password));
    sub["username"] = username;
    if (!users.empty()) {
        Users user = users[0];
        user.setPassword(new_password);
        mp.update(user);
        result["code"] = 0;
        result["data"] = sub;
        result["msg"] = "password modify successful!";
    } else {
        result["code"] = -1;
        result["data"] = {};
        result["msg"] = "password modify failed,please ensure old password is correct!";
    }
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);

}
