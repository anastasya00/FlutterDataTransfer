#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <cctype>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <buffer.hpp>
#include <socket.hpp>

class ThreadManager {
    public:
        ThreadManager() : isRunnig(true) { startThreads(); }
        ~ThreadManager() { stopThreads(); }

        void startThreads();
        void stopThreads();

    private:
        Buffer buffer_;
        std::atomic<bool> isRunnig;
        std::thread inputThread;
        std::thread processingThread;
        std::mutex threadMutex;
        std::condition_variable stopCV;

        void inputHandler();
        void processingHandler();
};