#include <fstream>
#include <cstdio>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include <custom/utils.h>
#include "models/Attend.h"
#include "models/Staff.h"

using namespace drogon;

class api : public drogon::HttpController<api> {
public:
    METHOD_LIST_BEGIN
        // use METHOD_ADD to add your custom processing function here;
        ADD_METHOD_TO(api::add_face_libs, "/api/add_face_libs", Put);
        ADD_METHOD_TO(api::get_attend_infos, "/api/get_attend_infos", Post);
        ADD_METHOD_TO(api::delete_face, "/api/delete_face", Delete);
        ADD_METHOD_TO(api::download_img, "/api/download_img", Post);
        ADD_METHOD_TO(api::clear_data, "/api/clear_data", Delete);
        ADD_METHOD_TO(api::update_time, "/api/update_time", Get);
    METHOD_LIST_END

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void add_face_libs(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void get_attend_infos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void delete_face(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void download_img(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void clear_data(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void update_time(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
};
