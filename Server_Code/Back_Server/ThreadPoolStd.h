#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "BaseTask.h"

// Lightweight std::thread based thread pool (Windows friendly).
class ThreadPoolStd {
public:
    explicit ThreadPoolStd(int num);
    ~ThreadPoolStd();

    void add_task(BaseTask* task);

private:
    void worker_loop();

    int min_num;
    std::vector<std::thread> workers;
    std::queue<BaseTask*> task_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    bool stop;
};
