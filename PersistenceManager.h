//
// Created by 51275 on 25-1-9.
//
#pragma once
#include <fstream>
#include <iostream>
#include "IBufferToolManager.h"
/**
 * @brief 持久化管理器示例: 每次 pushMessage 后，将消息写到文件
 *        在 stop() 时可以flush或做额外操作
 */
class PersistenceManager : public IBufferToolManager {
public:
    void setPersistenceFile(const std::string& filePath) {
        filePath_ = filePath;
        if (!filePath_.empty()) {
            ofs_.open(filePath_, std::ios::app);
            if (!ofs_) {
                std::cerr << "Failed to open persistence file: " << filePath_ << std::endl;
            }
        }
    }
    void onAfterPush(const Message& msg) override {
        if (ofs_) {
            ofs_ << msg.seqId << " "
                 << msg.priority << " "
                 << msg.timestamp << " "
                 << msg.enqueueTime << " "
                 << msg.content << "\n";
        }
    }
    void onStop() override {
        // 可以在这里 flush 并关闭文件
        if (ofs_) {
            ofs_.flush();
            ofs_.close();
        }
    }
private:
    std::string filePath_;
    std::ofstream ofs_;
};
