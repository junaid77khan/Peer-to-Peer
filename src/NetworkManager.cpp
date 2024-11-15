#include "NetworkManager.h"
#include <Poco/Net/NetException.h>
#include <fmt/format.h>
#include <iostream>
#include <vector>

NetworkManager::NetworkManager(uint16_t port) 
    : port_(port), isRunning_(false) {
    std::cout << fmt::format("NetworkManager initialized on port {}\n", port);
}

bool NetworkManager::startServer() {
    try {
        if (isRunning_) {
            std::cout << "Server already running\n";
            return true;
        }

        serverSocket_.bind(Poco::Net::SocketAddress("0.0.0.0", port_));
        serverSocket_.listen(64);  // Backlog size
        isRunning_ = true;
        
        std::cout << fmt::format("Server started on port {}\n", port_);
        return true;

    } catch (const Poco::Net::NetException& e) {
        std::cerr << fmt::format("Failed to start server: {}\n", e.what());
        return false;
    }
}

bool NetworkManager::stopServer() {
    try {
        if (!isRunning_) {
            std::cout << "Server already stopped\n";
            return true;
        }

        serverSocket_.close();
        isRunning_ = false;
        std::cout << "Server stopped\n";
        return true;

    } catch (const Poco::Net::NetException& e) {
        std::cerr << fmt::format("Failed to stop server: {}\n", e.what());
        return false;
    }
}

bool NetworkManager::sendData(const Poco::Net::SocketAddress& address, const std::string& data) {
    try {
        Poco::Net::StreamSocket socket;
        socket.connect(address);
        socket.sendBytes(data.data(), static_cast<int>(data.size()));
        socket.close();
        return true;

    } catch (const Poco::Net::NetException& e) {
        std::cerr << fmt::format("Failed to send data: {}\n", e.what());
        return false;
    }
}

std::string NetworkManager::receiveData() {
    try {
        if (!isRunning_) {
            throw std::runtime_error("Server not running");
        }

        Poco::Net::StreamSocket socket = serverSocket_.acceptConnection();
        std::vector<char> buffer(8192);  // 8KB buffer
        int n = socket.receiveBytes(buffer.data(), static_cast<int>(buffer.size()));
        
        return std::string(buffer.data(), n);

    } catch (const Poco::Net::NetException& e) {
        std::cerr << fmt::format("Failed to receive data: {}\n", e.what());
        return "";
    }
}