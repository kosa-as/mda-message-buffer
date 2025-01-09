//
// Created by 51275 on 25-1-9.
//

#ifndef IPRODUCER_H
#define IPRODUCER_H
#include "Message.h"
class IProducer {
public:
    virtual ~IProducer() = default;
    /**
     * @brief 生产消息的接口
     * @param msg 要生产/发送的消息对象
     */
    virtual void produceMessage(const Message& msg) = 0;
};
#endif //IPRODUCER_H
