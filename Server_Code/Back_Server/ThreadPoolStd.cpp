#include "ThreadPoolStd.h"
#include <iostream>

ThreadPoolStd::ThreadPoolStd(int num) : min_num(num), stop(false) {
    if (min_num <= 0) {
        min_num = 1;
    }
    for (int i = 0; i < min_num; ++i) {
        workers.emplace_back(&ThreadPoolStd::worker_loop, this);
    }
}

ThreadPoolStd::~ThreadPoolStd() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop = true;
    }
    queue_cv.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPoolStd::add_task(BaseTask* task) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        task_queue.push(task);
    }
    queue_cv.notify_one();
}

void ThreadPoolStd::worker_loop() {
    while (true) {
        BaseTask* task = nullptr;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this]() { return stop || !task_queue.empty(); });
            if (stop && task_queue.empty()) {
                return;
            }
            task = task_queue.front();
            task_queue.pop();
        }

        if (task) {
            try {
                task->do_service();
            } catch (const std::exception& e) {
                std::cerr << "task exception: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "task exception: unknown" << std::endl;
            }
            delete task;
        }
    }
}
