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
    void listDevices();
    void logEvent(const std::string& mac, const std::string& type, const std::string& details);
    void saveMetrics(double latency, double jitter, double speed);
private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> connection;
};