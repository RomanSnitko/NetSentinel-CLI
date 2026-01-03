#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>

class DatabaseManager 
{
public:
    DatabaseManager(const std::string& connection_str);
    void connect();
    void createTables();
    void saveDevice(const std::string& mac, const std::string& ip, const std::string& hostname);
    void logEvent(const std::string& device_mac, const std::string& event_type, const std::string& details);

private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> connection;
};