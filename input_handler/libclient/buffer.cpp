#include <../libclient/buffer.hpp>

// Метод для изменения данных буфера
void Buffer::setData(const std::string& newData) {
    {
        std::lock_guard<std::mutex> lock(mx);
        data = newData;
        ready = true;
    }
    cv.notify_one();
}

// Метод для получения данных буфера 
std::string Buffer::getData() {
    std::unique_lock<std::mutex> lock(mx);
    cv.wait(lock, [this]() { return ready; });
    std::string result = data;
    ready = false;
    return result;
}

// Метод для затирки данных буфера
void Buffer::clearData() {
    std::unique_lock<std::mutex> lock(mx);
    data.clear();
    ready = false;
}

// Метод, проверяющий готовность буфера к выполнению операций
bool Buffer::isReady() {
    std::lock_guard<std::mutex> lock(mx);
    return ready;
}