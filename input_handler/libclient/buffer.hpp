#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

class Buffer {
    public:
        Buffer() = default;
        
        void setData(const std::string& newData);
        std::string getData();
        void clearData();
        bool isReady();

    private:
        std::string data;
        bool ready = false;
        std::mutex mx;
        std::condition_variable cv;
};