//
// Created by 51275 on 25-1-9.
//

#ifndef MESSAGE_H
#define MESSAGE_H
#pragma once

#include <string>

/**
 * @brief 表示一条消息，包括优先级、内容、时间戳等
 */
class Message {
public:
    long        seqId       = 0;  // 全局或自增 ID
    int         priority    = 0;  // 优先级：数值越大，越先取
    std::string content;          // 消息内容
    long        timestamp   = 0;  // （可选）接收或创建时间
    long        enqueueTime = 0;  // 进入队列的时间, 用于优先级衰减

    /**
     * @brief 重载 operator< 以用于 std::priority_queue 的排序
     *        这里让 priority 大的先弹出
     */
    bool operator<(const Message& other) const;
};

#endif //MESSAGE_H
