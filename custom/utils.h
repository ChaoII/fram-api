//
// Created by aichao on 2022/5/5.
//

#pragma once

#include <drogon/drogon.h>
#include <ctime>
#include <type_traits>
#include <opencv2/opencv.hpp>
#include "snowflake.h"

namespace drogon {

    class Custom {

    public:
        Custom() = default;

        static uint64_t get_uuid();

        template<class T>
        static std::vector<char> toByte(T *buffer, size_t n) {
            std::vector<char> vec;
            size_t bytes_num = sizeof(T) * n;
            vec.resize(bytes_num);
            memcpy(vec.data(), buffer, bytes_num);
            return vec;
        }

        static void JsDateToDrogonDate(std::string &js_date);

        static void replace_str(std::string &str, const std::string &before, const std::string &after);

        static bool json_array_2_vector(const Json::Value &array, std::vector<std::string> &vec);

        static trantor::Date format_date_time(const std::string &data_time);

        static void tolower_string(std::string &str);

        static void toupper_string(std::string &str);

        static cv::Mat concat_mat_horizontal(const cv::Mat &m1, const cv::Mat &m2);

        static cv::Mat concat_mat_vertical(const cv::Mat &m1, const cv::Mat &m2);

        static std::string front_time_to_backend(const std::string &fronted_time);

        template<class T>
        static bool target_in_vector(std::vector<T> &vec, T target) {
            auto it = std::find(vec.begin(), vec.end(), target);
            if (it == vec.end()) {
                return false;
            }
            return true;
        }
    };
}