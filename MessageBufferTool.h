//
// Created by 51275 on 25-1-9.
//

#ifndef MESSAGEBUFFERTOOL_H
#define MESSAGEBUFFERTOOL_H
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include "Message.h"
#include "IBufferToolManager.h"

/**
 * @brief 核心工具类 (去除原先的优先级衰减、限流、监控、持久化逻辑),
 *        只负责 push / pop / stop 以及队列同步.
 *        额外功能通过注入 managers 来实现.
 */
class MessageBufferTool {
public:
 MessageBufferTool();
 ~MessageBufferTool();

 // ========== Manager注册与管理 ==========
 void addManager(std::shared_ptr<IBufferToolManager> mgr);

 // ========== 核心生产/消费接口 ==========
 void pushMessage(Message msg);
 Message popMessage();

 void stop();
 bool isRunning() const;

 // ========== 仅供 Manager 回调使用的接口 (可选) ==========
 size_t getQueueSize() const;
 void decayPriorities(); // 供 PriorityDecayManager 调用, 若需要

private:
 std::priority_queue<Message> queue_;
 bool running_;
 mutable std::mutex mtx_;
 std::condition_variable cv_;

 // 可以再加一个 "队列满" 的 condition_variable, 如果你想要阻塞式限流
 // ...

 // 所有功能组件(Manager)的集合
 std::vector<std::shared_ptr<IBufferToolManager>> managers_;

 long getCurrentTimeMillis() const;
};

#endif //MESSAGEBUFFERTOOL_H
