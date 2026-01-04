#include "AuditEngine.hpp"
#include "DatabaseManager.hpp"
#include <iostream>

using boost::asio::ip::tcp;

AuditEngine::AuditEngine(boost::asio::io_context& io_context, std::shared_ptr<DatabaseManager> db)
    : io_ctx(io_context), db_ptr(db) {}

void AuditEngine::scanTarget(const std::string& ip, const std::vector<uint16_t>& ports) 
{
    std::cout << "[*] starting audit on " << ip << " for " << ports.size() << " ports..." << std::endl;
    
    for (uint16_t port : ports) 
    {
        checkPort(ip, port);
    }
}

//check, open/dumbling port with socket
void AuditEngine::checkPort(const std::string& ip, uint16_t port)
{
    auto socket = std::make_shared<tcp::socket>(io_ctx);
    auto timer = std::make_shared<boost::asio::steady_timer>(io_ctx);

    tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port); //inet_pton

    timer->expires_after(std::chrono::milliseconds(timeout_ms));
    timer->async_wait([socket, port, ip](const boost::system::error_code& ec) 
    {
        if (!ec) //timer to 0
        {
            boost::system::error_code ignore_ec;
            socket->close(ignore_ec);
        }
    });

    socket->async_connect(endpoint, [this, socket, timer, port, ip](const boost::system::error_code& ec) {
        timer->cancel();

        if (!ec) 
        {
            std::cout << "\033[1;32m[+]\033[0m Port " << port << " is OPEN on " << ip << std::endl;

            if (db_ptr) 
            {
                db_ptr->logEvent(ip, "PORT_OPEN", "TCP Port " + std::to_string(port) + " is accessible");
            }
        } else {
            //close / ECONNREFUSED
            //or timer more fast
        }
    });
}