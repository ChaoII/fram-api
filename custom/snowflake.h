//
// Created by AC on 2023/6/16.
//

#pragma once

#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>

using namespace std;

class Snowflake {
public:
    // 时间戳位数
    static const uint64_t TIMESTAMP_BIT = 41;
    // 机器位数
    static const uint64_t WORKER_ID_BIT = 5;
    // 序列号位数
    static const uint64_t SEQUENCE_BIT = 18;
    // 最大机器 ID
    static const uint64_t MAX_WORKER_ID = (1 << WORKER_ID_BIT) - 1;
    // 最大序列号
    static const uint64_t MAX_SEQUENCE = (1 << SEQUENCE_BIT) - 1;

    // 时间戳左移位数
    static const uint64_t TIMESTAMP_LEFT_SHIFT = SEQUENCE_BIT + WORKER_ID_BIT;
    // 机器 ID 左移位数
    static const uint64_t WORKER_ID_LEFT_SHIFT = SEQUENCE_BIT;

    // 开始时间戳（2021-01-01 00:00:00）
    static const uint64_t START_TIMESTAMP = 1609430400000;

    Snowflake(uint64_t worker_id) :
            worker_id_(worker_id),
            sequence_(0),
            last_timestamp_(0) {
        if (worker_id > MAX_WORKER_ID) {
            throw "worker id overflow";
        }
    }

    uint64_t NextId() {
        // 对 NextId() 函数加锁以保证同一时间只有一个线程在调用
        std::lock_guard<std::mutex> lock_guard(mutex_);

        // 获取当前时间戳
        uint64_t timestamp = GetCurrentTimestamp();

        // 如果当前时间小于上一次生成 ID 的时间，说明系统时钟回拨过，抛出异常
        if (timestamp < last_timestamp_) {
            throw "clock moved backwards";
        }

        // 如果是同一毫秒内生成的 ID，那么序列号加一
        if (last_timestamp_ == timestamp) {
            sequence_ = (sequence_ + 1) & MAX_SEQUENCE;

            // 如果序列号超过最大值，那么等待下一毫秒再重新生成
            if (sequence_ == 0) {
                timestamp = WaitForNextTimestamp(last_timestamp_);
            }
        }
            // 如果不是同一毫秒生成的 ID，那么序列号从零开始计数
        else {
            sequence_ = 0;
        }

        // 保存上一次生成 ID 的时间戳
        last_timestamp_ = timestamp;

        // 组装 ID 并返回
        uint64_t id = ((timestamp - START_TIMESTAMP) << TIMESTAMP_LEFT_SHIFT)
                      | (worker_id_ << WORKER_ID_LEFT_SHIFT)
                      | sequence_;

        return id;
    }

private:
    // 当前机器 ID
    uint64_t worker_id_;
    // 当前序列号
    uint64_t sequence_;
    // 上一次生成 ID 的时间戳
    uint64_t last_timestamp_;
    // 保证下一毫秒仍能生成唯一 ID 的计数器
    std::atomic<uint64_t> counter_{0};
    // 加锁保证同一时间只有一个线程生成 ID
    std::mutex mutex_;

    /**
     * 获取当前时间戳（毫秒）
     */
    uint64_t GetCurrentTimestamp() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }

    /**
     * 等待下一毫秒并返回时间戳
     */
    uint64_t WaitForNextTimestamp(uint64_t last_timestamp) {
        uint64_t timestamp = GetCurrentTimestamp();
        while (timestamp <= last_timestamp) {
            timestamp = GetCurrentTimestamp();
        }
        return timestamp;
    }
};
