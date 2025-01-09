//
// Created by 51275 on 25-1-9.
//
#pragma once
#include <atomic>
#include "IBufferToolManager.h"

/**
 * @brief 监控管理器：记录生产/消费总量，用于对外提供统计
 */
class MetricsManager : public IBufferToolManager {
public:
    void onAfterPush(const Message& msg) override {
        producedCount_++;
    }

    void onAfterPop(const Message& msg) override {
        consumedCount_++;
    }

    size_t getProducedCount() const {
        return producedCount_.load();
    }

    size_t getConsumedCount() const {
        return consumedCount_.load();
    }

private:
    std::atomic<size_t> producedCount_{0};
    std::atomic<size_t> consumedCount_{0};
};
