#include <../libclient/thread.hpp>

// Метод для запуска потоков
void ThreadManager::startThreads() {
    inputThread = std::thread(&ThreadManager::inputHandler, this);
    processingThread = std::thread(&ThreadManager::processingHandler, this);
}

// Метод для ожидания завершения потоков
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
            std::cout << "Введите строку: ";
            std::getline(std::cin, inputUser);

            if (std::cin.eof() || stopFlag) {
                std::cout << "Ввод завершен. Выходим из inputHandler." << std::endl;
                break;
            }

            if (inputUser.empty() || inputUser.size() > 64 || 
                !std::all_of(inputUser.begin(), inputUser.end(), ::isdigit)) {
                std::cerr << "Ошибка ввода: пустая строка, длина > 64 или недопустимые символы." << std::endl;
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
            }
            dataCV.notify_one();

            std::cout << "Данные добавлены в буфер: " << inputProcessed << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Исключение в inputHandler: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Неизвестное исключение в inputHandler." << std::endl;
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
                dataCV.wait(lock, [this]() { return !buffer_.getData().empty() || stopFlag; });

                if (stopFlag && buffer_.getData().empty()) {
                    std::cout << "Завершаем processingHandler. Нет данных для обработки." << std::endl;
                    break;
                }

                dataBuffer = buffer_.getData();
                buffer_.clearData();
            }

            std::cout << "Обработка данных: " << dataBuffer << std::endl;

            long sum = 0;
            for (char elem : dataBuffer) {
                if (isdigit(elem)) {
                    sum += elem - '0';
                }
            }

            std::cout << "Сумма чисел: " << sum << std::endl;

            if (!socket.sendDataWithRetry("SUM: " + std::to_string(sum))) {
                std::cerr << "Ошибка: не удалось отправить данные после 5 попыток." << std::endl;
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Исключение в processingHandler: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Неизвестное исключение в processingHandler." << std::endl;
    }
}