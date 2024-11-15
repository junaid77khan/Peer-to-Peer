#pragma once
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/StreamSocket.h>
#include <string>

class NetworkManager {
public:
    NetworkManager(uint16_t port = 8080);
    bool startServer();
    bool stopServer();
    bool sendData(const Poco::Net::SocketAddress& address, const std::string& data);
    std::string receiveData();

private:
    Poco::Net::ServerSocket serverSocket_;
    uint16_t port_;
    bool isRunning_;
};