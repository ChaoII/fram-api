#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;

namespace api {
    class User : public drogon::HttpController<User> {
    public:
        METHOD_LIST_BEGIN
            ADD_METHOD_TO(User::login, "/api/user/login", Options, Post);
            ADD_METHOD_TO(User::addUser, "/api/user/addUser", Options, Put, "LoginFilter");
            ADD_METHOD_TO(User::getUser, "/api/user/getUser", Options, Get, "LoginFilter");
            ADD_METHOD_TO(User::delUser, "/api/user/delUser", Options, Delete, "LoginFilter");
            ADD_METHOD_TO(User::editUser, "/api/user/editUser", Options, Post, "LoginFilter");
            ADD_METHOD_TO(User::modifyPassword, "/api/user/modifyPassword", Options, Post, "LoginFilter");
        METHOD_LIST_END

        void login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void addUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void getUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void delUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void editUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

        void modifyPassword(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    };
}
