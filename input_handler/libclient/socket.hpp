#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class SocketManager {
    public:
        SocketManager(const std::string& address, const int port) : serverAddress(address), serverPort(port), socketFd(-1) {};
        ~SocketManager() { closeSocket(); };

        bool connectServer();
        bool sendData(std::string& data);
        bool reconnectServer();
        bool closeSocket();
        void monitoringConnection();

    private:
        std::string serverAddress;
        int serverPort;
        int socketFd;
        std::mutex socketMutex;
};