#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

class DatabaseManager;

class MetricsEngine : public std::enable_shared_from_this<MetricsEngine> 
{
public:
    explicit MetricsEngine(boost::asio::io_context& io, std::shared_ptr<DatabaseManager> db);
    void runAllTests();
private:
    void measureLatency(const std::string& host, int count);
    void measureDownloadSpeed(const std::string& host, const std::string& target_path);
    
    void finalizeResults();

    boost::asio::io_context& io_ctx;
    std::shared_ptr<DatabaseManager> db_ptr;

    std::vector<double> rtt_results;
    double avg_latency = 0.0;
    double jitter = 0.0;
    double download_speed = 0.0;

    const int ping_count = 10;
    const std::string test_host = "8.8.8.8";
};