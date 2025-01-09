//
// Created by 51275 on 25-1-9.
//
#include <iostream>
#include "ConsumerThread.h"

ConsumerThread::ConsumerThread(MessageBufferTool* buffer)
    : bufferTool_(buffer) {}

void ConsumerThread::consumeMessage() {
    while (true) {
        Message msg = bufferTool_->popMessage();

        // 若取到空消息且不再运行，说明已经停止并且没有剩余消息
        if (msg.content.empty() && !bufferTool_->isRunning()) {
            break;
        }

        // 如果消息确实有内容，则处理
        if (!msg.content.empty()) {
            std::cout << "[Consumer] seqId=" << msg.seqId
                      << ", priority=" << msg.priority
                      << ", content=\"" << msg.content << "\"\n";
        }
    }
    std::cout << "[Consumer] No more messages, thread exit.\n";
}
