#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <buffer.hpp>
#include <socket.hpp>

class ThreadManager {
    public:
        ThreadManager() : stopFlag(false) { startThreads(); }
        ~ThreadManager() { stopThreads(); }

        void startThreads();
        void stopThreads();

    private:
        Buffer buffer_;
        std::thread inputThread;
        std::thread processingThread;
        std::mutex threadMutex;
        std::condition_variable dataCV;
        bool stopFlag;

        void inputHandler();
        void processingHandler();
};