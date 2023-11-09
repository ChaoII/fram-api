//
// Created by aichao on 2022/5/5.
//

#pragma once

#include <drogon/drogon.h>
#include <ctime>
#include <type_traits>
#include "snowflake.h"

namespace drogon {

    class Custom {

    public:
        Custom() = default;

        static uint64_t getUuid();

        template<class T>
        static std::vector<char> toByte(T *buffer, size_t n) {
            std::vector<char> vec;
            size_t bytes_num = sizeof(T) * n;
            vec.resize(bytes_num);
            memcpy(vec.data(), buffer, bytes_num);
            return vec;
        }

        static void jsDateToDrogonDate(std::string &js_date);

        static void replaceString(std::string &str, const std::string &before, const std::string &after);

        static bool jsonArrayToVector(const Json::Value &array, std::vector<std::string> &vec);

        static trantor::Date formatDateTime(const std::string &data_time);

        static void toLower(std::string &str);

        static void toUpper(std::string &str);

        static std::string frontTimeToBackend(const std::string &fronted_time);

        template<class T>
        static bool targetInVector(std::vector<T> &vec, T target) {
            auto it = std::find(vec.begin(), vec.end(), target);
            if (it == vec.end()) {
                return false;
            }
            return true;
        }
    };
}