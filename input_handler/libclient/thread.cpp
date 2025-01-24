#include <../libclient/thread.hpp>

// Метод для запуска потоков
void ThreadManager::startThreads() {
    inputThread = std::thread(&ThreadManager::inputHandler, this);
    processingThread = std::thread(&ThreadManager::stopThreads, this);
}

// Метод для ожидания заверщения потоков
void ThreadManager::stopThreads() {
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        isRunnig = false;
    }
    stopCV.notify_all();

    if (inputThread.joinable()) {
        inputThread.join();
    }
    if (processingThread.joinable()) {
        processingThread.join();
    }
}

// Поток ввода (1)
void ThreadManager::inputHandler() {
    while (isRunnig) {
        std::unique_lock<std::mutex> lock(threadMutex);
        if (!isRunnig) break;
        lock.unlock();

        std::string inputUser;
        std::cout << "Введите строку: " << std::endl;
        std::getline(std::cin, inputUser);

        if (inputUser.size() > 64 || !std::all_of(inputUser.begin(), inputUser.end(), ::isdigit)) {
            std::cerr << "Ошибка: длина строки должна быть не более 64 символов и содержать только цифры." << std::endl;
            continue;
        }

        std::sort(inputUser.begin(), inputUser.end(), std::greater<char>());

        std::string inputProcessed;
        for (char elem : inputUser) {
            inputProcessed += (elem - '0') % 2 == 0 ? "КВ" : std::string(1, elem);
        }
        buffer_.setData(inputProcessed);
    }
}

// Поток обработки (2)
void ThreadManager::processingHandler() {
    SocketManager socket("127.0.0.1", 5000);
    socket.connectServer();

    while (isRunnig) {
        std::unique_lock<std::mutex> lock(threadMutex);
        stopCV.wait_for(lock, std::chrono::milliseconds(100), [this]() { return !isRunnig; });
        if (!isRunnig) break;

        lock.unlock();

        std::string dataBuffer = buffer_.getData();
        if (dataBuffer.empty()) continue;

        std::cout << "Полученные данные: " << dataBuffer << std::endl;

        long sum = 0;
        for (char elem : dataBuffer) {
            if (isdigit(elem)) {
                sum += elem - '0';
            }
        }

        std::cout << "Сумма числовых элементов: " << sum << std::endl;

        std::string sumData = "SUM: " + std::to_string(sum);
        socket.sendData(sumData);
    }
}

