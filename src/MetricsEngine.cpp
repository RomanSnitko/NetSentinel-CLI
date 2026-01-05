#include "MetricsEngine.hpp"
#include "DatabaseManager.hpp"
#include <iostream>
#include <numeric>
#include <cmath>
#include <iomanip>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

MetricsEngine::MetricsEngine(boost::asio::io_context& io, std::shared_ptr<DatabaseManager> db)
    : io_ctx(io), db_ptr(db) {}

void MetricsEngine::runAllTests() 
{
    std::cout << "\033[1;34m[*] starting network quality metrics...\033[0m" << std::endl;
    rtt_results.clear();
    measureLatency(test_host, ping_count);
}

void MetricsEngine::measureLatency(const std::string& host, int count) 
{
    //++count ref while async reply come
    auto self = shared_from_this();

    if (count <= 0) 
    {
        if (!rtt_results.empty()) 
        {
            double sum = std::accumulate(rtt_results.begin(), rtt_results.end(), 0.0);
            avg_latency = sum / rtt_results.size();

            double jitter_sum = 0.0;
            for (size_t i = 1; i < rtt_results.size(); ++i) 
            {
                jitter_sum += std::abs(rtt_results[i] - rtt_results[i - 1]);
            }
            jitter = jitter_sum / (rtt_results.size() - 1);

            std::cout << "[+] latency: " << std::fixed << std::setprecision(2) << avg_latency << " ms" << std::endl;
            std::cout << "[+] jitter: " << jitter << " ms" << std::endl;
        }

        std::cout << "\033[1;34m[*] moving to download speed test...\033[0m" << std::endl;
        this->measureDownloadSpeed("www.google.com", "/");
        return;
    }

    auto socket = std::make_shared<tcp::socket>(io_ctx);
    auto start_time = std::chrono::high_resolution_clock::now();
    auto resolver = std::make_shared<tcp::resolver>(io_ctx); //google -> ip

    resolver->async_resolve(host, "53",[this, self, socket, resolver, start_time, host, count](const boost::system::error_code& ec, tcp::resolver::results_type results) 
        {
        if (ec) 
        {
            this->measureLatency(host, 0);
            return;
        }
        boost::asio::async_connect(*socket, results, [this, self, socket, start_time, host, count](const boost::system::error_code& ec, const tcp::endpoint&) 
        {
            if (!ec) 
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> duration = end_time - start_time;
                rtt_results.push_back(duration.count());
                std::cout << "[Metrics] Ping " << host << ": " << std::fixed << std::setprecision(2) << duration.count() << " ms" << std::endl;
            }
            boost::system::error_code ignore_ec;
            socket->close(ignore_ec);
            this->measureLatency(host, count - 1);
        });
    });
}

void MetricsEngine::measureDownloadSpeed(const std::string& host, const std::string& target_path) 
{
    auto self = shared_from_this();
    
    auto resolver = std::make_shared<tcp::resolver>(io_ctx);
    //advanced socket with timeout
    auto stream = std::make_shared<beast::tcp_stream>(io_ctx);
    auto buffer = std::make_shared<beast::flat_buffer>();
    //to store the response
    auto res = std::make_shared<http::response<http::dynamic_body>>();

    std::cout << "[Metrics] Step 1: Resolving " << host << "..." << std::endl;

    resolver->async_resolve(host, "80", 
        [this, self, resolver, stream, buffer, res, host, target_path](const boost::system::error_code& ec, tcp::resolver::results_type results) 
        {
            if (ec) 
            {
                std::cerr << "\033[1;31m[!] DNS Error: " << ec.message() << "\033[0m" << std::endl;
                this->finalizeResults();
                return;
            }

            std::cout << "[Metrics] Step 2: Connecting..." << std::endl;

            stream->expires_after(std::chrono::seconds(10));

            stream->async_connect(results, 
                [this, self, stream, buffer, res, host, target_path](const boost::system::error_code& ec, tcp::endpoint) {
                    if (ec) {
                        std::cerr << "\033[1;31m[!] Connect Error: " << ec.message() << "\033[0m" << std::endl;
                        this->finalizeResults();
                        return;
                    }

                    std::cout << "[Metrics] Step 3: Requesting data..." << std::endl;
                    
                    //GET
                    auto req = std::make_shared<http::request<http::empty_body>>(http::verb::get, target_path, 11);
                    req->set(http::field::host, host);
                    req->set(http::field::user_agent, "NetSentinel/1.0");

                    http::async_write(*stream, *req, 
                        [this, self, stream, buffer, res, req](const boost::system::error_code& ec, std::size_t) 
                        {
                            if (ec) 
                            {
                                std::cerr << "\033[1;31m[!] Write Error: " << ec.message() << "\033[0m" << std::endl;
                                this->finalizeResults();
                                return;
                            }

                            std::cout << "[Metrics] Step 4: Receiving data..." << std::endl;
                            auto start_time = std::chrono::high_resolution_clock::now();

                            http::async_read(*stream, *buffer, *res, 
                                [this, self, start_time, stream, res](const boost::system::error_code& ec, std::size_t bytes_transferred) 
                                {
                                    if (ec && ec != http::error::end_of_stream) 
                                    {
                                        std::cerr << "\033[1;31m[!] Read Error: " << ec.message() << "\033[0m" << std::endl;
                                    } 
                                    else 
                                    {
                                        auto end_time = std::chrono::high_resolution_clock::now();
                                        std::chrono::duration<double> diff = end_time - start_time;
                                        
                                        if (diff.count() > 0) 
                                        {
                                            this->download_speed = (static_cast<double>(bytes_transferred) * 8.0) / (diff.count() * 1000000.0);
                                            std::cout << "\033[1;32m[+]\033[0m Download Speed: " << std::fixed << std::setprecision(2) 
                                                      << this->download_speed << " Mbps" << std::endl;
                                        }
                                    }
                                    
                                    boost::system::error_code ec_ignore;
                                    stream->socket().close(ec_ignore);
                                    
                                    this->finalizeResults();
                                });
                        });
                });
        });
}
void MetricsEngine::finalizeResults() 
{
    if (db_ptr) 
    {
        db_ptr->saveMetrics(avg_latency, jitter, download_speed);
        db_ptr->logEvent("SELF", "QUALITY_TEST", "Lat: " + std::to_string(avg_latency) + "ms, Speed: " + std::to_string(download_speed) + "Mbps");
    }
    std::cout << "\033[1;32m[DB] metrics saved successfully.\033[0m" << std::endl;
}