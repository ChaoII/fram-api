//
// Created by aichao on 2022/5/5.
//

#ifndef BACKEND_UTILS_H
#define BACKEND_UTILS_H

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace drogon {

    class Custom {

    public:
        Custom() = default;

        void static update_time(const std::string host, const std::string &api_path);

    };
}


#endif //BACKEND_UTILS_H
