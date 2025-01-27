#include <../libclient/buffer.hpp>

// Метод для изменения данных буфера
void Buffer::setData(const std::string& newData) {
    std::unique_lock<std::mutex> lock(mx);
    data = newData;
    ready = true;
    cv.notify_one();
}

// Метод для получения данных буфера 
std::string Buffer::getData() {
    std::unique_lock<std::mutex> lock(mx);  
    cv.wait(lock, [this]() { return ready; });
    ready = false;
    return data;
}

// Метод для затирки данных буфера
void Buffer::clearData() {
    std::unique_lock<std::mutex> lock(mx);
    data.clear();
    ready = false;
}