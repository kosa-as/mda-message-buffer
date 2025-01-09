//
// Created by 51275 on 25-1-9.
//

#ifndef IBUFFERTOOLMANAGER_H
#define IBUFFERTOOLMANAGER_H
#pragma once
#include <string>

class MessageBufferTool; // 前向声明
#include "Message.h"

/**
 * @brief 统一的管理器接口（可选的抽象层），
 *        各种功能模块(优先级衰减、限流、监控、持久化)都可以实现这个接口，
 *        并在 MessageBufferTool 的关键节点（onBeforePush、onAfterPush 等）执行相应逻辑。
 */
class IBufferToolManager {
public:
    virtual ~IBufferToolManager() = default;

    /**
     * @brief 在 pushMessage(...) 调用之前触发，可对将要入队的 msg 做处理
     */
    virtual void onBeforePush(Message& msg) {}

    /**
     * @brief 在 pushMessage(...) 完成后触发，可记录统计信息或做其他处理
     */
    virtual void onAfterPush(const Message& msg) {}

    /**
     * @brief 在 popMessage(...) 取消息之前触发，
     *        可以在这里执行优先级衰减、检查队列状态等
     */
    virtual void onBeforePop() {}

    /**
     * @brief 在 popMessage(...) 完成后（取到 msg）触发，
     *        可记录监控统计或做其他处理
     */
    virtual void onAfterPop(const Message& msg) {}

    /**
     * @brief 在 stop() 调用时触发，可做一些资源清理或持久化刷盘等
     */
    virtual void onStop() {}
};

#endif //IBUFFERTOOLMANAGER_H
