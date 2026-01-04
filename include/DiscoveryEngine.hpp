#pragma once
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <netinet/if_ether.h>

class DiscoveryEngine 
{
public:
    explicit DiscoveryEngine(std::shared_ptr<class DatabaseManager> db);

    void runScan(const std::string& interface_name);

private:
    std::shared_ptr<class DatabaseManager> db_ptr;

    void sendArpRequest(int sock, const std::string& iface, const std::string& target_ip);
    
    void listenForReplies(int sock);
};