
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include "models/Attend.h"

using namespace drogon;

class AttendInfo : public drogon::HttpController<AttendInfo> {
public:
    METHOD_LIST_BEGIN
        // use METHOD_ADD to add your custom processing function here;
        // METHOD_ADD(Attend::get, "/{2}/{1}", Get); // path is /Attend/{arg2}/{arg1}
        METHOD_ADD(AttendInfo::add_face_libs, "/api/add_face_libs", Put); // path is /Attend/{arg1}/{arg2}/list
        METHOD_ADD(AttendInfo::get_attend_infos, "/api/get_attend_infos", Post); // path is /Attend/{arg1}/{arg2}/list
        // ADD_METHOD_TO(Attend::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END

    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void add_face_libs(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;

    void get_attend_infos(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
};
