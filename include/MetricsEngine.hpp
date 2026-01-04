#pragma once
#include <boost/asio.hpp>
#include <memory>
#include "DatabaseManager.hpp"

class MetricsEngine 
{
public:
    MetricsEngine(boost::asio::io_context& io, std::shared_ptr<DatabaseManager> db);
    void runAllTests();
private:
    boost::asio::io_context& io_ctx;
    std::shared_ptr<DatabaseManager> db_ptr;
};