cmake_minimum_required(VERSION 3.29)
project(Code)

set(CMAKE_CXX_STANDARD 17)

add_executable(Code main.cpp
        IProducer.h
        IConsumer.h
        Message.h
        MessageBufferTool.h
        Message.cpp
        MessageBufferTool.cpp
        ProducerThread.h
        ConsumerThread.h
        ProducerThread.cpp
        ConsumerThread.cpp
        IBufferToolManager.h
        PriorityDecayManager.h
        RateLimiter.h
        MetricsManager.h
        PersistenceManager.h)
