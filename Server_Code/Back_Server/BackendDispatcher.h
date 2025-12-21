#pragma once

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

#include "FrontBridgeListener.h"
#include "Protocol.h"
#include "TaskList.h"
#include "ThreadPoolStd.h"

class BackendDispatcher {
public:
    BackendDispatcher();
    ~BackendDispatcher();

    void run();
    void stop();

private:
    void processMessage(const ReadResult& readResult);

    FrontBridgeListener* reader;
    ThreadPoolStd* threadPool;
    std::atomic<bool> isRunning;
};
