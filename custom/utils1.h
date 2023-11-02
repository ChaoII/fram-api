//
// Created by aichao on 2022/5/5.
//

#ifndef BACKEND_UTILS_H
#define BACKEND_UTILS_H

#include <drogon/drogon.h>
#include <ctime>

namespace drogon {

    class Custom {

    public:
        Custom() = default;

        static bool update_time(const std::string &host = "172.168.100.79:8112",
                                const std::string &api_path = "/api/services/assistant/UserConfig/GetNow");

        static std::string currentDateTime();

        static std::string time_delta(int d = 7, int h = 0, int m = 0, int s = 0);
    };
}


#endif //BACKEND_UTILS_H
