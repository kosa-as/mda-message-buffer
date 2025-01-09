
## 1. `IProducer` 接口

- **文件位置**：`IProducer.h`
- **简要介绍**：
    - 定义了生产者的抽象接口，提供 `produceMessage(const Message&)` 纯虚函数。
    - 任何想要扮演“生产者”角色的类，都需要实现此接口，从而具备“生产 / 发送”一条消息的能力。

---

## 2. `IConsumer` 接口

- **文件位置**：`IConsumer.h`
- **简要介绍**：
    - 定义了消费者的抽象接口，提供 `consumeMessage()` 纯虚函数。
    - 任何想要扮演“消费者”角色的类，都需要实现此接口，从而具备“从消息缓冲中取消息并处理”的能力。

---

## 3. `ProducerThread` 类

- **文件位置**：`ProducerThread.h` / `.cpp`
- **继承关系**：实现了 `IProducer` 接口
- **简要介绍**：
    - 内部持有对 `MessageBufferTool` 的指针。
    - 实现 `produceMessage(const Message&)` 方法：在方法体中调用 `bufferTool->pushMessage(msg)`，将消息放入缓冲。
    - 通常与线程绑定，循环或批量地产生 `Message` 并调用 `produceMessage(...)`。

---

## 4. `ConsumerThread` 类

- **文件位置**：`ConsumerThread.h` / `.cpp`
- **继承关系**：实现了 `IConsumer` 接口
- **简要介绍**：
    - 内部持有对 `MessageBufferTool` 的指针。
    - 实现 `consumeMessage()` 方法：在其中不断调用 `bufferTool->popMessage()` 获取最高优先级消息，并进行处理（示例中是简单打印）。
    - 通常单独开一个线程或多个线程执行 `consumerObj.consumeMessage()`，直到 `MessageBufferTool` 被 `stop()`，消息取完后线程退出。

---

## 5. `Message` 类

- **文件位置**：`Message.h` / `.cpp`
- **简要介绍**：
    - 用于表示单条消息的实体，包含：
        - `seqId`, `priority`, `content`, `timestamp`, `enqueueTime` 等。
    - 重载了 `operator<` 以便在 `std::priority_queue<Message>` 中按优先级进行排序（数值越大，优先级越高，放在队列顶部）。
    - 生产者创建 `Message`，消费者取出并处理 `Message`。

---

## 6. `MessageBufferTool` 类

- **文件位置**：`MessageBufferTool.h` / `.cpp`
- **简要介绍**：
    - **核心**的生产-消费缓冲工具，内部使用 `std::priority_queue<Message>` 存储消息，并借助 `std::mutex`, `std::condition_variable` 等保证线程安全。
    - 提供 `pushMessage(Message)`, `popMessage()`, `stop()` 等核心方法：
        - `pushMessage(...)`：生产者调用，放入队列并通知消费者。
        - `popMessage()`：消费者调用，若队列为空则阻塞等待。取到最高优先级的消息后进行处理。
        - `stop()`：终止运行，唤醒所有阻塞的线程，让它们退出。
    - 支持“插件”机制：可 `addManager(...)` 注册一个或多个管理器（Manager），在 `push/pop/stop` 的前后触发钩子，让外部组件（如优先级衰减、限流、监控、持久化）执行相应逻辑。
    - 相比以往的版本，大量附加功能都拆到各自的 Manager 中，从而实现**低耦合**、**高扩展**。

---

## 7. `IBufferToolManager` 接口

- **文件位置**：`IBufferToolManager.h`
- **简要介绍**：
    - 定义了一套“钩子方法”（`onBeforePush`, `onAfterPush`, `onBeforePop`, `onAfterPop`, `onStop`），`MessageBufferTool` 在合适时机调用这些方法，帮助“附加功能组件”接入核心流程。
    - 所有的 Manager 都可以实现该接口，在 `MessageBufferTool` 的 `pushMessage()` / `popMessage()` / `stop()` 前后插入相应的业务逻辑（如限流检查、记录监控数据、写文件等）。

---

## 8. `PriorityDecayManager` 类

- **文件位置**：`PriorityDecayManager.h` / `.cpp`
- **继承关系**：实现了 `IBufferToolManager` 接口
- **简要介绍**：
    - 用于实现**优先级衰减/提升**逻辑。
    - 示例中，当 `onBeforePop()` 被调用时，它会执行一个回调（`decayCallback`），让 `MessageBufferTool` 做批量衰减——比如对等待过久的消息提升 `priority` 值，防止低优先级消息长期饿死。
    - 提供 `setEnabled(bool)` 来启用或禁用此功能。

---

## 9. `RateLimiter` 类

- **文件位置**：`RateLimiter.h` / `.cpp`
- **继承关系**：实现了 `IBufferToolManager` 接口
- **简要介绍**：
    - 用于实现**限流**，防止队列无限膨胀。
    - 在 `onBeforePush()` 中检查当前队列大小是否超过设定 `maxQueueSize`。若超过则可选择**抛异常**、**阻塞**、**丢弃**等。
    - 通过回调 `getQueueSizeCallback` 获取队列长度，与 `maxQueueSize` 做比较。

---

## 10. `MetricsManager` 类

- **文件位置**：`MetricsManager.h` / `.cpp`
- **继承关系**：实现了 `IBufferToolManager` 接口
- **简要介绍**：
    - 用于**监控**生产和消费的数量（或其他指标）。
    - 在 `onAfterPush()` 中对“生产计数”自增，在 `onAfterPop()` 中对“消费计数”自增。
    - 通过 `getProducedCount()`、`getConsumedCount()` 对外提供统计结果，方便监控和调试。

---

## 11. `PersistenceManager` 类

- **文件位置**：`PersistenceManager.h` / `.cpp`
- **继承关系**：实现了 `IBufferToolManager` 接口
- **简要介绍**：
    - 用于**持久化**消息（示例中是简单地写到文件）。
    - 在 `onAfterPush()` 被调用时，把刚写入队列的消息追加到文件中，防止系统意外崩溃导致消息丢失。
    - 在 `onStop()` 时做必要的 `flush()` 和文件关闭操作，保证持久化数据落盘。

---

## 12. `main.cpp`

- **简要介绍**：
    - 演示如何组合以上所有类，形成一个“多生产者、单消费者 + 插件式功能组件”的完整流程：
        1. 创建 `MessageBufferTool`；
        2. 创建并初始化若干 Manager（`PriorityDecayManager`, `RateLimiter`, `MetricsManager`, `PersistenceManager`），并添加到 `MessageBufferTool`；
        3. 启动若干生产者线程（`ProducerThread`），并发地调用 `pushMessage(...)`；
        4. 启动一个消费者线程（`ConsumerThread`），循环调用 `popMessage()` 消费消息；
        5. 等生产者结束后，调用 `stop()`，并等待消费者退出；
        6. 打印监控统计，结束程序。

---

### 总体关系

- **`MessageBufferTool`**：核心并发队列 + 插件式扩展点
- **`IBufferToolManager`**：接口，定义常见钩子方法
- **各个 Manager（组件）**：
    - `PriorityDecayManager` (优先级衰减)
    - `RateLimiter` (限流)
    - `MetricsManager` (监控)
    - `PersistenceManager` (持久化)
- **生产者 / 消费者**：
    - `ProducerThread` (实现 `IProducer`，调用 `pushMessage`)
    - `ConsumerThread` (实现 `IConsumer`，调用 `popMessage`)
- **`Message`**：存放消息字段并参与队列排序

通过这样“插件化”的设计，**核心逻辑**（`MessageBufferTool` 里的并发安全队列）和**扩展功能**（衰减、限流、监控、持久化）之间的耦合度大幅降低，更方便后期维护与拓展。