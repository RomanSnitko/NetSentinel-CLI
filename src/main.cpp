#include <iostream>
#include <memory>
#include "DatabaseManager.hpp"
#include "CLIHandler.hpp"
#include "AuditEngine.hpp"
#include <boost/asio.hpp>
#include "DiscoveryEngine.hpp"
#include "MetricsEngine.hpp"

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        CLIHandler::printHelp();
        return 0;
    }

    std::string mode = argv[1];
    std::string conn_str = "dbname=netsentinel_db user=roman";

    try 
    {
        auto db = std::make_shared<DatabaseManager>(conn_str);
        db->connect();
        db->createTables();

        if (mode == "-h" || mode == "--help")
        {
            CLIHandler::printHelp();
            return 0;
        } 
        else if (mode == "-s" || mode == "--scan")
        {
            std::string interface = (argc >= 3) ? argv[2] : "eth0";

            std::cout << "[*] initializing Discovery Engine..." << std::endl;
            
            try 
            {
                DiscoveryEngine discovery(db);

                discovery.runScan(interface);

                std::cout << "[+] scan completed successfully." << std::endl;
            } 
            catch (const std::exception& e) 
            {
                // sudo or forgot interface problem
                std::cerr << "\033[1;31m[!] Discovery Error: " << e.what() << "\033[0m" << std::endl;
                std::cerr << "[?] Hint: Check if you ran the program with 'sudo' and provided the correct interface name (e.g., eth0, wlan0, enp3s0)." << std::endl;
            }
        }
        else if (mode == "-a" || mode == "--audit") 
        {
            if (argc < 3) 
            {
                std::cerr << "Usage: ./NetSentinel --audit <IP>\n";
                return 1;
            }
            std::string target_ip = argv[2];
    
            boost::asio::io_context io_ctx;

            auto auditor = std::make_shared<AuditEngine>(io_ctx, db);

            std::vector<uint16_t> common_ports = {21, 22, 23, 25, 53, 80, 110, 143, 443, 445, 3306, 3389, 5432, 6666, 7777, 9999, 8080};

            auditor->scanTarget(target_ip, common_ports);

            //async cycle
            io_ctx.run();

            std::cout << "[*] audit for " << target_ip << " finished." << std::endl;
        }
        else if (mode == "-l" || mode == "--list") 
        {
            db->listDevices();
            return 0;
        }
        else if (mode == "-t" || mode == "--test-speed")
        {
            std::cout << "\033[1;35m[*] Starting Network Quality Test...\033[0m" << std::endl;
            
            try 
            {
                boost::asio::io_context io_ctx;

                auto metrics = std::make_shared<MetricsEngine>(io_ctx, db);

                metrics->runAllTests();

                io_ctx.run();

                std::cout << "\033[1;32m[+] All tests completed.\033[0m" << std::endl;
            } 
            catch (const std::exception& e) 
            {
                std::cerr << "[!] Metrics Error: " << e.what() << std::endl;
            }
        }
        else 
        {
            std::cout << "Unknown option. Use --help\n";
        }

    } catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}