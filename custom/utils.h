//
// Created by aichao on 2022/5/5.
//

#ifndef BACKEND_UTILS_H
#define BACKEND_UTILS_H

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <ctime>

namespace drogon {

    class Custom {

    public:
        Custom() = default;

        void static update_time(const std::string &host, const std::string &api_path);

        static std::string currentDateTime();
    };
}


#endif //BACKEND_UTILS_H
