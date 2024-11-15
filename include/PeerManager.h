#pragma once
#include <Poco/Net/SocketAddress.h>
#include <string>
#include <unordered_map>

class PeerManager {
public:
    PeerManager();
    bool registerPeer(const std::string& peerId, const Poco::Net::SocketAddress& address);
    bool removePeer(const std::string& peerId);
    Poco::Net::SocketAddress getPeerAddress(const std::string& peerId);
    void displayActivePeers();

private:
    std::unordered_map<std::string, Poco::Net::SocketAddress> peers_;
};