#include "MessageBufferTool.h"
#include <chrono>
#include <algorithm>
#include <iostream>

MessageBufferTool::MessageBufferTool()
    : running_(true)
{}

MessageBufferTool::~MessageBufferTool() {
    // 在析构时, 若还有 manager, 调用其 onStop()?
    // 也可以在外部先调用 stop()
    stop();
}

void MessageBufferTool::addManager(std::shared_ptr<IBufferToolManager> mgr) {
    std::lock_guard<std::mutex> lock(mtx_);
    managers_.push_back(mgr);
}

void MessageBufferTool::pushMessage(Message msg) {
    // manager 钩子: onBeforePush
    for (auto& m : managers_) {
        m->onBeforePush(msg);
    }

    {
        std::lock_guard<std::mutex> lock(mtx_);
        // 记录消息进入时间 (若衰减或统计用)
        msg.enqueueTime = getCurrentTimeMillis();
        queue_.push(msg);
    }

    // manager 钩子: onAfterPush
    for (auto& m : managers_) {
        m->onAfterPush(msg);
    }

    // 通知消费线程
    cv_.notify_one();
}

Message MessageBufferTool::popMessage() {
    // manager 钩子: onBeforePop
    for (auto& m : managers_) {
        m->onBeforePop();
    }

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] {
        return !running_ || !queue_.empty();
    });
    if (queue_.empty()) {
        // manager 钩子: onAfterPop(空消息) -- 可选
        return Message{};
    }

    auto topMsg = queue_.top();
    queue_.pop();
    lock.unlock();

    // manager 钩子: onAfterPop
    for (auto& m : managers_) {
        m->onAfterPop(topMsg);
    }

    return topMsg;
}

void MessageBufferTool::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!running_) return;  // 避免重复 stop
        running_ = false;
    }
    cv_.notify_all();

    // manager 钩子: onStop
    for (auto& m : managers_) {
        m->onStop();
    }
}

bool MessageBufferTool::isRunning() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return running_;
}

// ========== 仅供 Manager 回调使用 (示例) ==========
size_t MessageBufferTool::getQueueSize() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.size();
}

// 如果优先级衰减需要访问队列来更新优先级, 可以做个简单示例:
void MessageBufferTool::decayPriorities() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (queue_.empty()) return;

    // 把所有消息取出来, 更新 priority, 再放回
    std::vector<Message> tmp;
    tmp.reserve(queue_.size());
    while (!queue_.empty()) {
        auto msg = queue_.top();
        queue_.pop();

        long waitMs = getCurrentTimeMillis() - msg.enqueueTime;
        // 例如: 每等1秒, priority+1
        msg.priority += static_cast<int>(waitMs / 1000);

        tmp.push_back(msg);
    }
    for (auto& m : tmp) {
        queue_.push(m);
    }
}

long MessageBufferTool::getCurrentTimeMillis() const {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}
