#include <iostream>
#include <thread>
#include <csignal>
#include <mutex>
#include <condition_variable>

#include <../libclient/thread.hpp>

std::mutex exitMutex;
std::condition_variable exitCondition;
bool exitFlag = false;
ThreadManager threadManager;

void signalHandler(int signum) {
    std::cout << "\nПрограмма завершает работу (сигнал: " << signum << ")" << std::endl;
    {
        std::lock_guard<std::mutex> lock(exitMutex);
        exitFlag = true;
    }
    exitCondition.notify_one();

    threadManager.stopThreads();
}

int main() {
    std::signal(SIGINT, signalHandler);

    try {
        std::cout << "Программа запущена. Нажмите Ctrl+C для выхода.\n" << std::endl;

        {
            std::unique_lock<std::mutex> lock(exitMutex);
            exitCondition.wait(lock, [] { return exitFlag; });
        }

    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}