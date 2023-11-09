//
// Created by aichao on 2022/5/5.
//

#include "utils.h"

using namespace drogon;

uint64_t Custom::getUuid() {
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
void Custom::replaceString(std::string &str, const std::string &before, const std::string &after) {
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
void Custom::jsDateToDrogonDate(std::string &js_date) {
    replaceString(js_date, "T", " ");
    replaceString(js_date, "Z", "");
}

bool Custom::jsonArrayToVector(const Json::Value &array, std::vector<std::string> &vec) {
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
trantor::Date Custom::formatDateTime(const string &data_time) {
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

void Custom::toLower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
}

void Custom::toUpper(string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
}


std::string Custom::frontTimeToBackend(const string &fronted_time) {
    //2023-07-31T15:59:59.000Z
    auto parts = drogon::utils::splitString(fronted_time, "T");
    if (parts.size() != 2) {
        LOG_ERROR << "time format error";
        return "";
    }
    std::string time = parts[1];
    replaceString(time, "Z", "");
    return parts[0] + "" + time;
}
