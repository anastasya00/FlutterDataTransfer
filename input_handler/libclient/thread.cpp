#include <../libclient/thread.hpp>

// Метод для запуска потоков
void ThreadManager::startThreads() {
    std::lock_guard<std::mutex> lock(threadMutex);
    if (inputThread.joinable() || processingThread.joinable()) {
        throw std::runtime_error("Потоки уже запущены.");
    }
    inputThread = std::thread(&ThreadManager::inputHandler, this);
    processingThread = std::thread(&ThreadManager::processingHandler, this);
}

// Метод для завершения потоков
void ThreadManager::stopThreads() {
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        stopFlag = true;
    }
    dataCV.notify_all();

    if (inputThread.joinable()) {
        inputThread.join();
    }
    if (processingThread.joinable()) {
        processingThread.join();
    }
    std::cout << "Все потоки завершены." << std::endl;
}

// Поток ввода (1)
void ThreadManager::inputHandler() {
    try {
        while (true) {
            std::string inputUser;
            {
                std::lock_guard<std::mutex> lock(threadMutex);
                if (stopFlag) break;
                std::cout << "Введите строку: ";
            }

            if (!std::getline(std::cin, inputUser)) {
                if (stopFlag) break;
                std::cerr << "Ошибка ввода: EOF или некорректный ввод." << std::endl;
                break;
            }

            if (stopFlag) break;

            if (inputUser.empty() || inputUser.size() > 64 ||
                !std::all_of(inputUser.begin(), inputUser.end(), ::isdigit)) {
                std::lock_guard<std::mutex> lock(threadMutex);
                std::cerr << "Ошибка ввода: длина строки должна быть от 1 до 64 символов и содержать только цифры." << std::endl;
                continue;
            }

            std::sort(inputUser.begin(), inputUser.end(), std::greater<char>());
            std::string inputProcessed;
            for (char elem : inputUser) {
                inputProcessed += (elem - '0') % 2 == 0 ? "КВ" : std::string(1, elem);
            }

            {
                std::lock_guard<std::mutex> lock(threadMutex);
                buffer_.setData(inputProcessed);
                std::cout << "Данные записаны в буфер: " << inputProcessed << std::endl;
            }
            dataCV.notify_one();
        }
    } catch (const std::exception& ex) {
        std::lock_guard<std::mutex> lock(threadMutex);
        std::cerr << "Исключение в inputHandler: " << ex.what() << std::endl;
    }
}

// Поток обработки и отправки данных (2)
void ThreadManager::processingHandler() {
    try {
        SocketManager socket("127.0.0.1", 5000);
        while (true) {
            std::string dataBuffer;

            {
                std::unique_lock<std::mutex> lock(threadMutex);
                dataCV.wait(lock, [this]() {
                    return stopFlag || buffer_.isReady();
                });

                if (stopFlag && !buffer_.isReady()) {
                    break;
                }

                if (!buffer_.isReady()) continue;

                dataBuffer = buffer_.getData();
                buffer_.clearData();
            }

            if (dataBuffer.empty()) continue;

            {
                std::lock_guard<std::mutex> lock(threadMutex);
                std::cout << "Обработка данных: " << dataBuffer << std::endl;
            }

            long sum = 0;
            for (char elem : dataBuffer) {
                if (isdigit(elem)) {
                    sum += elem - '0';
                }
            }

            {
                std::lock_guard<std::mutex> lock(threadMutex);
                std::cout << "Сумма чисел: " << sum << std::endl;
            }

            if (!socket.sendDataWithRetry(std::to_string(sum))) {
                std::lock_guard<std::mutex> lock(threadMutex);
                std::cerr << "Ошибка: не удалось отправить данные после 5 попыток." << std::endl;
            }
        }
    } catch (const std::exception& ex) {
        std::lock_guard<std::mutex> lock(threadMutex);
        std::cerr << "Исключение в processingHandler: " << ex.what() << std::endl;
    }
}