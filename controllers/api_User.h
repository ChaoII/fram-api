#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;

namespace api
{
class User : public drogon::HttpController<User>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(User::login, "/api/user/login", Options, Post);
        ADD_METHOD_TO(User::add_user, "/api/user/addUser", Options, Put, "LoginFilter");
        ADD_METHOD_TO(User::get_user, "/api/user/getUser", Options, Get, "LoginFilter");
        ADD_METHOD_TO(User::del_user, "/api/user/delUser", Options, Delete, "LoginFilter");
        ADD_METHOD_TO(User::edit_user, "/api/user/editUser", Options, Post, "LoginFilter");
        ADD_METHOD_TO(User::modify_password, "/api/user/modify_password", Options, Post, "LoginFilter");
    METHOD_LIST_END

    void login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void add_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void get_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void del_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void edit_user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void modify_password(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

};
}
