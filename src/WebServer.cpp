#include "WebServer.h"
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

std::mutex WebSocketRequestHandler::peersMutex;
std::map<std::string, std::shared_ptr<Poco::Net::WebSocket>> WebSocketRequestHandler::activePeers;
int WebSocketRequestHandler::peerCounter = 0;

void WebSocketRequestHandler::handleRequest(
    Poco::Net::HTTPServerRequest& request, 
    Poco::Net::HTTPServerResponse& response) 
{
    try {
        auto ws = std::make_shared<Poco::Net::WebSocket>(request, response);
        std::string peerId = "peer_" + std::to_string(++peerCounter);

        {
            std::lock_guard<std::mutex> lock(peersMutex);
            activePeers[peerId] = ws;
            json peerInfo;
            peerInfo["type"] = "peer_id";
            peerInfo["id"] = peerId;
            std::string peerInfoStr = peerInfo.dump();
            ws->sendFrame(peerInfoStr.data(), peerInfoStr.size(), Poco::Net::WebSocket::FRAME_TEXT);
            broadcastPeerList();
        }

        char buffer[8192];
        int flags;
        int n;
        
        do {
            try {
                n = ws->receiveFrame(buffer, sizeof(buffer), flags);
            
                if (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE) {
                    std::string message(buffer, n);
                    json msg = json::parse(message);
                    
                    if (msg.contains("type")) {
                        std::string msgType = msg["type"];
                        
                        if (msgType == "get_peers") {
                            handleGetPeers(peerId, ws);
                        }
                        else if (msgType == "connect_peer") {
                            handleConnectPeer(peerId, msg);
                        }
                        else if (msgType == "offer" || msgType == "answer") {
                            if (msg.contains("target") && msg.contains("sdp")) {
                                std::string targetPeerId = msg["target"];
                                std::lock_guard<std::mutex> lock(peersMutex);
                                auto it = activePeers.find(targetPeerId);
                                if (it != activePeers.end()) {
                                    json sdpMsg = {
                                        {"type", msgType},
                                        {"sdp", msg["sdp"]},
                                        {"from", peerId}
                                    };
                                    std::string sdpStr = sdpMsg.dump();
                                    it->second->sendFrame(sdpStr.data(), sdpStr.size(), 
                                                        Poco::Net::WebSocket::FRAME_TEXT);
                                }
                            }
                        }
                        else if (msgType == "ice-candidate") {
                            if (msg.contains("target") && msg.contains("candidate")) {
                                std::string targetPeerId = msg["target"];
                                std::lock_guard<std::mutex> lock(peersMutex);
                                auto it = activePeers.find(targetPeerId);
                                if (it != activePeers.end()) {
                                    json iceMsg = {
                                        {"type", "ice-candidate"},
                                        {"candidate", msg["candidate"]},
                                        {"from", peerId}
                                    };
                                    std::string iceStr = iceMsg.dump();
                                    it->second->sendFrame(iceStr.data(), iceStr.size(), 
                                                        Poco::Net::WebSocket::FRAME_TEXT);
                                }
                            }
                        }
                    }
                }
            } catch (const Poco::TimeoutException&) {
                continue;
            } catch (const json::parse_error& e) {
            }
        } while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);

        {
            std::lock_guard<std::mutex> lock(peersMutex);
            activePeers.erase(peerId);
            broadcastPeerList();
        }
    }
    catch (std::exception& e) {
    }
}

void WebSocketRequestHandler::handleGetPeers(const std::string& peerId, 
                                           std::shared_ptr<Poco::Net::WebSocket> ws) {
    json peerList;
    peerList["type"] = "peer_list";
    std::vector<std::string> peers;
    {
        std::lock_guard<std::mutex> lock(peersMutex);
        for (const auto& pair : activePeers) {
            if (pair.first != peerId) {
                peers.push_back(pair.first);
            }
        }
    }
    peerList["peers"] = peers;
    std::string peerListStr = peerList.dump();
    ws->sendFrame(peerListStr.data(), peerListStr.size(), 
                 Poco::Net::WebSocket::FRAME_TEXT);
}

void WebSocketRequestHandler::handleConnectPeer(const std::string& peerId, 
                                              const json& msg) {
    if (msg.contains("peerId")) {
        std::string targetPeerId = msg["peerId"];
        json connectionRequest = {
            {"type", "connection_request"},
            {"from", peerId}
        };
        
        std::string requestStr = connectionRequest.dump();
        std::lock_guard<std::mutex> lock(peersMutex);
        auto it = activePeers.find(targetPeerId);
        if (it != activePeers.end()) {
            it->second->sendFrame(requestStr.data(), requestStr.size(), 
                                Poco::Net::WebSocket::FRAME_TEXT);
        }
    }
}

void WebSocketRequestHandler::broadcastPeerList() {
    json peerList;
    peerList["type"] = "peer_list";
    std::vector<std::string> peers;
    for (const auto& pair : activePeers) {
        peers.push_back(pair.first);
    }
    peerList["peers"] = peers;
    
    std::string peerListStr = peerList.dump();
    
    for (const auto& pair : activePeers) {
        try {
            pair.second->sendFrame(peerListStr.data(), peerListStr.size(), 
                                 Poco::Net::WebSocket::FRAME_TEXT);
        } catch (std::exception& e) {
        }
    }
}


void PageRequestHandler::handleRequest(
    Poco::Net::HTTPServerRequest& request, 
    Poco::Net::HTTPServerResponse& response) 
{
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    response.add("Content-Security-Policy", 
                "default-src 'self'; "
                "connect-src 'self' ws: wss:; "
                "media-src 'self' blob:; "
                "script-src 'self'; "
                "style-src 'self';");

    std::ostream& ostr = response.send();
    ostr << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "    <title>P2P Video Streaming</title>\n"
         << "    <meta charset=\"utf-8\">\n"
         << "    <link rel=\"stylesheet\" href=\"/styles.css\">\n"
         << "</head>\n"
         << "<body>\n"
         << "    <div class=\"container\">\n"
         << "        <h1>P2P Video Streaming Platform</h1>\n"
         << "        <div class=\"video-container\">\n"
         << "            <video id=\"localVideo\" autoplay playsinline muted></video>\n"
         << "            <video id=\"remoteVideo\" autoplay playsinline></video>\n"
         << "        </div>\n"
         << "        <div class=\"controls\">\n"
         << "            <button id=\"startStreamBtn\">Start Stream</button>\n"
         << "            <button id=\"stopStreamBtn\">Stop Stream</button>\n"
         << "            <button id=\"connectPeerBtn\">Connect to Peer</button>\n"
         << "            <button id=\"showPeersBtn\">Show Active Peers</button>\n"
         << "        </div>\n"
         << "        <div id=\"peerList\"></div>\n"
         << "        <div id=\"status\"></div>\n"
         << "    </div>\n"
         << "    <script src=\"/app.js\"></script>\n"
         << "</body>\n"
         << "</html>\n";
}

void StaticFileHandler::handleRequest(
    Poco::Net::HTTPServerRequest& request, 
    Poco::Net::HTTPServerResponse& response) 
{
    std::string path = request.getURI();
    std::string contentType;
    
    if (endsWith(path, ".css")) {
        contentType = "text/css";
    } else if (endsWith(path, ".js")) {
        contentType = "application/javascript";
    } else {
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send();
        return;
    }
    
    std::string execPath = std::filesystem::current_path().string();
    std::string projectRoot = execPath;
    projectRoot = projectRoot.substr(0, projectRoot.find("build"));
    std::string fullPath = projectRoot + "static" + path;
    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        response.send();
        return;
    }
    
    response.setContentType(contentType);
    response.setChunkedTransferEncoding(true);
    
    std::ostream& out = response.send();
    out << file.rdbuf();
}
Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request) 
{
    std::string uri = request.getURI();
    if (uri == "/ws") {
        return new WebSocketRequestHandler;
    } else if (endsWith(uri, ".css") || endsWith(uri, ".js")) {
        return new StaticFileHandler;
    }
    return new PageRequestHandler;
}

WebServer::WebServer(uint16_t port) : port_(port) {}

WebServer::~WebServer() {
    stop();
}

void WebServer::start() {
    try {
        Poco::Net::ServerSocket socket(port_);
        auto* params = new Poco::Net::HTTPServerParams;
        params->setMaxQueued(100);
        params->setMaxThreads(16);
        
        server_ = std::make_unique<Poco::Net::HTTPServer>(
            new RequestHandlerFactory, socket, params);
        server_->start();
    }
    catch (const std::exception& e) {
    }
}

void WebServer::stop() {
    if (server_) {
        try {
            server_->stop();
            server_.reset();
        }
        catch (const std::exception& e) {
        }
    }
}
