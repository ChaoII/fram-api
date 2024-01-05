//
// Created by AC on 2024/1/5.
//

#include "custom/timeInterhelper.h"


std::string IntervalHelper::intervalUnitToString(IntervalUnit unit) {
    static const std::unordered_map<IntervalUnit, std::string> deleteInterValUnitMap = {
            {IntervalUnit::Hour,  "hour"},
            {IntervalUnit::Day,   "day"},
            {IntervalUnit::Week,  "week"},
            {IntervalUnit::Month, "month"},
            {IntervalUnit::Year,  "year"},
    };
    auto it = deleteInterValUnitMap.find(unit);
    return (it != deleteInterValUnitMap.end()) ? it->second : "Unknown";
}


IntervalUnit IntervalHelper::stringToInterValUnit(const std::string &str) {
    static const std::unordered_map<std::string, IntervalUnit> stringToGenreMap = {
            {"hour",  IntervalUnit::Hour},
            {"day",   IntervalUnit::Day},
            {"week",  IntervalUnit::Week},
            {"month", IntervalUnit::Month},
            {"year",  IntervalUnit::Year},
    };
    auto it = stringToGenreMap.find(str);
    return (it != stringToGenreMap.end()) ? it->second : IntervalUnit::Month; // 默认为 Montn
}

double IntervalHelper::interValNumToSecondDouble(int64_t num, const std::string &unit_str) {
    IntervalUnit unit = stringToInterValUnit(unit_str);
    auto delete_interval = std::chrono::seconds(0);  // 初始化，以防没有匹配的情况
    switch (unit) {
        case IntervalUnit::Hour:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::hours(num));
            break;
        case IntervalUnit::Day:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::days(num));
            break;
        case IntervalUnit::Week:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::weeks(num));
            break;
        case IntervalUnit::Month:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::months(num));
            break;
        case IntervalUnit::Year:
            delete_interval = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::years(num));
            break;
        default:
            break;
    }
    double seconds_as_double = std::chrono::duration_cast<std::chrono::duration<double>>(
            delete_interval).count();
    return seconds_as_double;
}
