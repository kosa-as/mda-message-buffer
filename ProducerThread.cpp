//
// Created by 51275 on 25-1-9.
//
#include "ProducerThread.h"

ProducerThread::ProducerThread(MessageBufferTool* buffer)
    : bufferTool_(buffer) {}

void ProducerThread::produceMessage(const Message& msg) {
    if (bufferTool_) {
        bufferTool_->pushMessage(msg);
    }
}
