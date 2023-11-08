//
// Created by aichao on 2022/5/5.
//

#ifndef BACKEND_UTILS1_H
#define BACKEND_UTILS1_H

#include <drogon/drogon.h>
#include <ctime>

namespace drogon {

    class Customs {

    public:
        Customs() = default;

        static bool update_time(const std::string &host = "172.168.100.79:8112",
                                const std::string &api_path = "/api/services/assistant/UserConfig/GetNow");
    };
}


#endif //BACKEND_UTILS1_H
