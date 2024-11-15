#pragma once

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <map>
#include <memory>
#include <string>

using json = nlohmann::json;

inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

class WebSocketRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, 
                      Poco::Net::HTTPServerResponse& response) override;

private:
    static std::mutex peersMutex;
    static std::map<std::string, std::shared_ptr<Poco::Net::WebSocket>> activePeers;
    static int peerCounter;
    
    void handleGetPeers(const std::string& peerId, 
                       std::shared_ptr<Poco::Net::WebSocket> ws);
    void handleConnectPeer(const std::string& peerId, const json& msg);
    void broadcastPeerList();
};

class PageRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, 
                      Poco::Net::HTTPServerResponse& response) override;
};

class StaticFileHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, 
                      Poco::Net::HTTPServerResponse& response) override;
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override;
};

class WebServer {
public:
    explicit WebServer(uint16_t port);
    ~WebServer();

    void start();
    void stop();

private:
    uint16_t port_;
    std::unique_ptr<Poco::Net::HTTPServer> server_;
};