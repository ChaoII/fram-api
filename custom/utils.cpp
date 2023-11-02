//
// Created by aichao on 2022/5/5.
//

#include "utils.h"

using namespace drogon;

uint64_t Custom::get_uuid() {
    return Snowflake(0).NextId();
}


/**
 * 字符串替换函数
 * #function name   : replace_str()
 * #param str       : 操作之前的字符串
 * #param before    : 将要被替换的字符串
 * #param after     : 替换目标字符串
 * #return          : void
 */
void Custom::replace_str(std::string &str, const std::string &before, const std::string &after) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += after.length()) {
        pos = str.find(before, pos);
        if (pos != std::string::npos)
            str.replace(pos, before.length(), after);
        else
            break;
    }
}

/// 将前端类似于2023-06-21T23:59:59.000Z 转化为2023-06-21 23:59:59.000
/// \param js_date
/// \return
void Custom::JsDateToDrogonDate(std::string &js_date) {
    replace_str(js_date, "T", " ");
    replace_str(js_date, "Z", "");
}

bool Custom::json_array_2_vector(const Json::Value &array, std::vector<std::string> &vec) {
    if (!array.isArray()) { return false; }
    for (auto &value: array) {
        std::string val = value.asString();
        vec.emplace_back(val);
    }
    return true;
}

///
/// \param data_time 20220615071032977
/// \return
trantor::Date Custom::format_date_time(const string &data_time) {
    std::string year = data_time.substr(0, 4);
    std::string month = data_time.substr(4, 2);
    std::string day = data_time.substr(6, 2);
    std::string hour = data_time.substr(8, 2);
    std::string minute = data_time.substr(10, 2);
    std::string second = data_time.substr(12, 2);
    std::string msecond = data_time.substr(14, 3);

    std::string date_time_str =
            year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second + "." + msecond;
    return trantor::Date::fromDbStringLocal(date_time_str);
}

void Custom::tolower_string(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
}

void Custom::toupper_string(string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
}

cv::Mat Custom::concat_mat_horizontal(const cv::Mat &m1, const cv::Mat &m2) {
    if (m1.empty() || m2.empty()) {
        LOG_ERROR << "mat is empty please check the image is correct";
    }
    int w1 = m1.cols;
    int h1 = m1.rows;
    int w2 = m2.cols;
    int h2 = m2.rows;
    if (!(w1 == w2 && h1 == h2)) {
        LOG_ERROR << "two images' shape must be equal current is : h1 = " << h1 << "h2=" << h2 << "w1=" << w1 << "w2="
                  << w2;
    }
    int width = w1 + w2;
    int height = max(h1, h2);
    cv::Mat resultImg = cv::Mat(height, width, CV_8UC3, cv::Scalar::all(5));

    cv::Mat ROI_1 = resultImg(cv::Rect(0, 0, w1, h1));
    cv::Mat ROI_2 = resultImg(cv::Rect(w1, 0, w2, h2));
    m1.copyTo(ROI_1);
    m2.copyTo(ROI_2);
    return resultImg;
}


cv::Mat Custom::concat_mat_vertical(const cv::Mat &m1, const cv::Mat &m2) {
    if (m1.empty() || m2.empty()) {
        LOG_ERROR << "mat is empty please check the image is correct";
    }
    int w1 = m1.cols;
    int h1 = m1.rows;
    int w2 = m2.cols;
    int h2 = m2.rows;
    if (!(w1 == w2 && h1 == h2)) {
        LOG_ERROR << "two images' shape must be equal current is : h1 = " << h1 << "h2=" << h2 << "w1=" << w1 << "w2="
                  << w2;
    }
    int width = max(w1, w2);
    int height = h1 + h2;
    cv::Mat resultImg = cv::Mat(height, width, CV_8UC3, cv::Scalar::all(5));

    cv::Mat ROI_1 = resultImg(cv::Rect(0, 0, w1, h1));
    cv::Mat ROI_2 = resultImg(cv::Rect(0, h1, w2, h2));
    m1.copyTo(ROI_1);
    m2.copyTo(ROI_2);
    return resultImg;
}

std::string Custom::front_time_to_backend(const string &fronted_time) {
    //2023-07-31T15:59:59.000Z
    auto parts = drogon::utils::splitString(fronted_time, "T");
    if (parts.size() != 2) {
        LOG_ERROR << "time format error";
        return "";
    }
    std::string time = parts[1];
    replace_str(time, "Z", "");
    return parts[0] + "" + time;
}
