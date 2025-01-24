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
bool SocketManager::sendData(std::string& data) {
    std::lock_guard<std::mutex> lock(socketMutex);

    if (socketFd == -1) {
        std::cerr << "Ошибка: нет подключения к серверу." << std::endl;
        return false;
    }

    ssize_t byteSend = send(socketFd, data.c_str(), data.size(), 0);
    if (byteSend == -1) {
        std::cerr << "Ошибка: данные не отправлены." << std::endl;
        return false;
    }

    std::cout << "Данные отправлены на сервер." << std::endl;
    return true;
}

// Метод переподключения к серверу
bool SocketManager::reconnectServer() {
    int maxRecconectAttemp = 10;
    int delayMsec = 2000;

    std::lock_guard<std::mutex> lock(socketMutex);

    for (int attemp = 0; attemp <= maxRecconectAttemp; attemp++) {
        std::cout << "Попытка переподключения " << attemp + 1 << " из " << maxRecconectAttemp << std::endl;

        if (connectServer()) {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMsec));
    }

    std::cerr << "Ошибка: не удалось установить подключение к серверу " << maxRecconectAttemp << " попыток." << std::endl;
    return false;
    
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

// Метод мониторинга соединения с сервером
void SocketManager::monitoringConnection() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if (socketFd == -1) {
            reconnectServer();
        }
    }
}