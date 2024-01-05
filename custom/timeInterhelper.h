//
// Created by AC on 2024/1/5.
//

#pragma once

#include <iostream>
#include <unordered_map>
#include <chrono>

enum class IntervalUnit {
    Hour,
    Day,
    Week,
    Month,
    Year,
};

class IntervalHelper {

public:
    IntervalHelper() = default;

    ~IntervalHelper() = default;

    static double interValNumToSecondDouble(int64_t num, const std::string &unit_str);

private:
    static std::string intervalUnitToString(IntervalUnit unit);

    static IntervalUnit stringToInterValUnit(const std::string &str);

};

