#include <../libclient/socket.hpp>

// Метод для подключения к серверу
bool SocketManager::connectServer() {
    std::lock_guard<std::mutex> lock(socketMutex);

    if (socketFd != -1) {
        closeSocket();
    }

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "Ошибка: сокет не создан." << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Ошибка: неверный адрес сервера." << std::endl;
        closeSocket();
        return false;
    }

    if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка: не удалось установаить подключение к серверу." << std::endl;
        closeSocket();
        return false;
    }

    std::cout << "Подключение к серверу установлено." << std::endl;
    return true;
}

// Метод отправки данных на сервер
bool SocketManager::sendData(const std::string& data) {
    std::lock_guard<std::mutex> lock(socketMutex);

    if (socketFd == -1) {
        std::cerr << "Ошибка: нет подключения к серверу." << std::endl;
        return false;
    }

    ssize_t byteSend = send(socketFd, (data + "\n").c_str(), data.size() + 1, 0);
    if (byteSend == -1) {
        std::cerr << "Ошибка: данные не отправлены." << std::endl;
        return false;
    }

    std::cout << "Данные отправлены на сервер." << std::endl;
    return true;
}

// Метод отправки данных с повторными попытками
bool SocketManager::sendDataWithRetry(const std::string& data) {
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (sendData(data)) {
            return true;
        }
        std::cerr << "Попытка отправки данных не удалась. Повтор через 2 секунды." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        reconnectServer();
    }
    return false;
}

// Метод переподключения к серверу
bool SocketManager::reconnectServer() {
    return connectServer();
}

// Метод закрытия сокета
bool SocketManager::closeSocket() {
    std::lock_guard<std::mutex> lock(socketMutex);

    if (socketFd != -1) {
        close(socketFd);
        socketFd = -1;
        std::cout << "Сокет закрыт." << std::endl;
        return true;
    }
    return false;
}