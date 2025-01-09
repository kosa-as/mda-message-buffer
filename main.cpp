#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>
#include <functional>

// ========== 引入核心与插件 ==========
#include "MessageBufferTool.h"
#include "PriorityDecayManager.h"
#include "RateLimiter.h"
#include "MetricsManager.h"
#include "PersistenceManager.h"

// ========== 引入生产者/消费者相关 ==========
#include "IProducer.h"
#include "IConsumer.h"
#include "ProducerThread.h"
#include "ConsumerThread.h"
#include "Message.h"

int main() {
    std::cout << "[Main] Program start.\n";

    // (1) 创建核心缓冲工具
    MessageBufferTool bufferTool;

    // (2) 创建并配置各个 Manager
    auto decayMgr = std::make_shared<PriorityDecayManager>();
    decayMgr->setEnabled(true);
    // decayCallback: 在 Manager 触发 onBeforePop() 时, 调用这个函数做衰减
    decayMgr->decayCallback = [&bufferTool]() {
        bufferTool.decayPriorities();
    };

    auto limiter = std::make_shared<RateLimiter>();
    limiter->setMaxQueueSize(10);
    // 让它能获取实时队列大小
    limiter->getQueueSizeCallback = [&bufferTool]() {
        return bufferTool.getQueueSize();
    };

    auto metrics = std::make_shared<MetricsManager>();

    auto persistence = std::make_shared<PersistenceManager>();
    persistence->setPersistenceFile("messages.log");

    // (3) 将 Manager 注入 bufferTool
    bufferTool.addManager(decayMgr);
    bufferTool.addManager(limiter);
    bufferTool.addManager(metrics);
    bufferTool.addManager(persistence);

    // (4) 创建并启动消费者线程 (单消费者)
    //     ConsumerThread 实现了 IConsumer::consumeMessage()，内部会循环调用 bufferTool.popMessage()
    ConsumerThread consumerObj(&bufferTool);
    std::thread consumerThread([&consumerObj]() {
        consumerObj.consumeMessage();
        // 直到 bufferTool.stop() 触发或消息耗尽时退出
    });

    // (5) 创建多个生产者线程并发产生日志 / 消息
    const int PRODUCER_COUNT = 3;             // 3个生产者
    const int MESSAGE_COUNT_PER_PRODUCER = 5; // 每个生产者发送5条消息

    std::vector<std::thread> producerThreads;
    producerThreads.reserve(PRODUCER_COUNT);

    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        // 每个 ProducerThread 对象都持有指向同一个 bufferTool 的指针
        ProducerThread* producerObj = new ProducerThread(&bufferTool);

        // 将 producerObj 放到 thread 中运行
        producerThreads.emplace_back([producerObj, i]() {
            for (int j = 0; j < MESSAGE_COUNT_PER_PRODUCER; ++j) {
                Message msg;
                msg.seqId    = j;
                msg.priority = j;  // 用 j 模拟优先级
                msg.timestamp = 0; // 可用 std::chrono 获取真实时间
                msg.content  = "Msg from Producer " + std::to_string(i)
                               + " # " + std::to_string(j);

                // 调用 producerObj->produceMessage(msg)
                producerObj->produceMessage(msg);

                // 模拟发送间隔
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }

    // (6) 等待所有生产者结束
    for (auto &t : producerThreads) {
        t.join();
    }
    std::cout << "[Main] All producers done.\n";

    // (7) 通知 bufferTool 停止, 唤醒可能阻塞的消费者, 让它退出
    bufferTool.stop();
    std::cout << "[Main] bufferTool stopped.\n";

    // (8) 等待消费者线程结束
    consumerThread.join();
    std::cout << "[Main] Consumer thread joined.\n";

    // (9) 打印监控结果
    std::cout << "[Main] Total produced: " << metrics->getProducedCount() << "\n";
    std::cout << "[Main] Total consumed: " << metrics->getConsumedCount() << "\n";

    std::cout << "[Main] Program exit.\n";
    return 0;
}
