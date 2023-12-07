/**
 *
 *  GlobalThreadPool.cc
 *
 */

#include "GlobalThreadPool.h"

using namespace drogon;

void GlobalThreadPool::initAndStart(const Json::Value &config) {
    /// Initialize and start the plugin
    int thread_pool_size = config.get("thread_pool_size", 5).asInt();
    pool_ = std::make_shared<ThreadPool>(thread_pool_size);
}

std::shared_ptr<ThreadPool> GlobalThreadPool::getGlobalThreadPool() {
    return pool_;
}

void GlobalThreadPool::shutdown() {
    /// Shutdown the plugin
}

