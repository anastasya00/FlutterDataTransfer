#include <iostream>
#include <thread>
#include <csignal>
#include <mutex>
#include <condition_variable>

#include <../libclient/thread.hpp>

// Механизм завершения работы программы
std::mutex exitMutex;
std::condition_variable exitCondition;
bool exitFlag = false;

// Обработчик сигнала для завершения программы
void signalHandler(int signum) {
    std::cout << "\nПрограмма завершает работу (сигнал: " << signum << ")" << std::endl;
    {
        std::lock_guard<std::mutex> lock(exitMutex);
        exitFlag = true;
    }
    exitCondition.notify_one();
}

int main() {
    std::signal(SIGINT, signalHandler);

    try {
        ThreadManager threadManager;
        threadManager.startThreads();

        std::cout << "Программа запущена. Нажмите Ctrl+C для выхода." << std::endl;

        {
            std::unique_lock<std::mutex> lock(exitMutex);
            exitCondition.wait(lock, [] { return exitFlag; });
        }

        threadManager.stopThreads();
        std::cout << "Работа потоков завершена." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}