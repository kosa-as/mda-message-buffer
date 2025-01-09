//
// Created by 51275 on 25-1-9.
//

#ifndef ICONSUMER_H
#define ICONSUMER_H
#include "Message.h"
class IConsumer {
public:
    virtual ~IConsumer() = default;

    /**
     * @brief 消费消息的接口
     *        具体实现中会去缓冲工具中取消息并处理
     */
    virtual void consumeMessage() = 0;
};
#endif //ICONSUMER_H
