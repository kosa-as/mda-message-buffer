//
// Created by 51275 on 25-1-9.
//

#ifndef CONSUMERTHREAD_H
#define CONSUMERTHREAD_H
#pragma once

#include "IConsumer.h"
#include "MessageBufferTool.h"

/**
 * @brief 消费者线程类，实现 IConsumer 接口
 */
class ConsumerThread : public IConsumer {
public:
    explicit ConsumerThread(MessageBufferTool* buffer);

    // 实现 IConsumer 接口的方法
    void consumeMessage() override;

private:
    MessageBufferTool* bufferTool_;
};

#endif //CONSUMERTHREAD_H
