//
// Created by 51275 on 25-1-9.
//
#pragma once
#include <functional>
#include <stdexcept>

#include "IBufferToolManager.h"

/**
 * @brief 限流管理器示例
 *        在 onBeforePush() 时，检查队列大小是否达到上限，若达到则阻塞或抛异常
 */
class RateLimiter : public IBufferToolManager {
public:
    void setMaxQueueSize(size_t maxSize) {
        maxQueueSize_ = maxSize;
    }

    // 在 pushMessage 前检查队列是否已满
    void onBeforePush(Message& msg) override {
        if (maxQueueSize_ > 0 && getQueueSizeCallback) {
            auto qSize = getQueueSizeCallback();
            if (qSize >= maxQueueSize_) {
                // 示例：简单抛异常或做其他阻塞逻辑
                throw std::runtime_error("RateLimiter: Queue is full.");
            }
        }
    }

    // 注册获取队列大小的回调
    std::function<size_t()> getQueueSizeCallback;

private:
    size_t maxQueueSize_ = 0;
};
