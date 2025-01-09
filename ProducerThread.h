//
// Created by 51275 on 25-1-9.
//

#ifndef PRODUCERTHREAD_H
#define PRODUCERTHREAD_H
#pragma once

#include "IProducer.h"
#include "MessageBufferTool.h"

/**
 * @brief 生产者线程类，实现 IProducer 接口
 */
class ProducerThread : public IProducer {
public:
    explicit ProducerThread(MessageBufferTool* buffer);

    // 实现 IProducer 接口的方法
    void produceMessage(const Message& msg) override;

private:
    MessageBufferTool* bufferTool_;
};

#endif //PRODUCERTHREAD_H
