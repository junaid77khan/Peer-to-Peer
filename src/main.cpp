#include <Poco/Net/Socket.h>
#include <Poco/Net/SocketAddress.h>
#include <iostream>
#include "PeerManager.h"
#include "VideoManager.h"
#include "NetworkManager.h"
#include "WebServer.h"

int main(int argc, char* argv[]) {
    try {
        std::cout << "Starting P2P Video Streaming Platform...\n";

        uint16_t port = 8080;
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }

        PeerManager peerManager;
        VideoManager videoManager;
        NetworkManager networkManager(port);
        WebServer webServer(port + 1);

        if (!networkManager.startServer()) {
            std::cerr << "Failed to start network server\n";
            return 1;
        }

        webServer.start();

        std::cout << "Servers started successfully\n";
        std::cout << "P2P Server port: " << port << "\n";
        std::cout << "Web Interface: http://localhost:" << (port + 1) << "\n";

        std::string input;
        while (true) {
            std::getline(std::cin, input);
            if (input == "exit") {
                break;
            }
        }

        webServer.stop();
        networkManager.stopServer();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
