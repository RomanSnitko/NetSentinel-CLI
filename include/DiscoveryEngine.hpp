#pragma once
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <netinet/if_ether.h>

class DatabaseManager;

class DiscoveryEngine 
{
public:
    explicit DiscoveryEngine(std::shared_ptr<DatabaseManager> db);

    void runScan(const std::string& interface_name);

private:
    std::shared_ptr<DatabaseManager> db_ptr;

    void sendArpRequest(int sock, const std::string& iface, const std::string& target_ip);
    
    void listenForReplies(int sock);
};