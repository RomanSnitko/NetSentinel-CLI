#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class DatabaseManager;

class AuditEngine : std::enable_shared_from_this<AuditEngine>
{
public:
    explicit AuditEngine(boost::asio::io_context& io_context,  std::shared_ptr<DatabaseManager> db);

    void scanTarget(const std::string& ip, const std::vector<uint16_t>& ports);
private:
    void checkPort(const std::string& ip, uint16_t port);

    boost::asio::io_context& io_ctx;
    std::shared_ptr<DatabaseManager> db_ptr;

    const int timeout_ms = 1500; //try to connect
};