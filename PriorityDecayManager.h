//
// Created by 51275 on 25-1-9.
//
#pragma once
#include <chrono>
#include <functional>

#include "IBufferToolManager.h"

/**
 * @brief 优先级衰减管理器示例：
 *        在 onBeforePop() 时，对队列中的消息做一次衰减处理
 *        （或根据实际需要在 onAfterPush() / 定时任务 中执行）
 */
class PriorityDecayManager : public IBufferToolManager {
public:
    // 简单控制是否启用衰减
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }

    // 每次消费者 popMessage 前，先做一次衰减（仅示例）
    void onBeforePop() override {
        if (!enabled_) return;

        // 在这里执行衰减逻辑
        // 实际上，需要访问队列本身
        // 可通过在 MessageBufferTool 提供专用接口 (如 decayPriorities()) 来实现
        if (decayCallback) {
            decayCallback();
        }
    }

    // 注册一个函数指针或 lambda，用于实际更新队列优先级
    std::function<void()> decayCallback;

private:
    bool enabled_ = false;
};
