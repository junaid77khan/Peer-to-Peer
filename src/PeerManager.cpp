#include "PeerManager.h"
#include <Poco/Net/NetException.h>
#include <iostream>
#include <fmt/format.h>

PeerManager::PeerManager() {
    std::cout << "PeerManager initialized\n";
}

bool PeerManager::registerPeer(const std::string& peerId, const Poco::Net::SocketAddress& address) {
    try {
        peers_[peerId] = address;
        std::cout << fmt::format("Peer {} registered with address {}:{}\n", 
            peerId, address.host().toString(), address.port());
        return true;
    } catch (const Poco::Net::NetException& e) {
        std::cerr << fmt::format("Failed to register peer: {}\n", e.what());
        return false;
    }
}

bool PeerManager::removePeer(const std::string& peerId) {
    auto it = peers_.find(peerId);
    if (it != peers_.end()) {
        peers_.erase(it);
        std::cout << fmt::format("Peer {} removed\n", peerId);
        return true;
    }
    return false;
}

Poco::Net::SocketAddress PeerManager::getPeerAddress(const std::string& peerId) {
    auto it = peers_.find(peerId);
    if (it != peers_.end()) {
        return it->second;
    }
    throw std::runtime_error(fmt::format("Peer {} not found", peerId));
}

void PeerManager::displayActivePeers() {
    if (peers_.empty()) {
        std::cout << "No active peers\n";
        return;
    }
    
    std::cout << "\nActive Peers:\n";
    std::cout << "-------------\n";
    for (const auto& [id, addr] : peers_) {
        std::cout << fmt::format("ID: {}, Address: {}:{}\n", 
            id, addr.host().toString(), addr.port());
    }
}