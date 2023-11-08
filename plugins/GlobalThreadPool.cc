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
    pool_ = std::make_shared<wsp::workbranch>(thread_pool_size);
}

std::shared_ptr<wsp::workbranch> GlobalThreadPool::getGlobalThreadPool() {
    return pool_;
}

void GlobalThreadPool::shutdown() {
    /// Shutdown the plugin
    pool_->del_worker();
}
