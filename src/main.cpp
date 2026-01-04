#include <iostream>
#include <memory>
#include "DatabaseManager.hpp"
#include "CLIHandler.hpp"
#include "AuditEngine.hpp"
#include <boost/asio.hpp>
#include "DiscoveryEngine.hpp"

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
            /*
            Что это: Уровень L4 (Transport). Когда мы уже знаем IP устройства (например, твоего телевизора), мы 
            хотим проверить его «двери» (порты). Открытый порт — это потенциальная лазейка.

            За какие файлы отвечает:
                include/AuditEngine.hpp
                src/AuditEngine.cpp

            Логика: Использует Boost.Asio. Пытается асинхронно подключиться к списку портов (80, 443, 22 и т.д.). 
            Если подключение удалось — порт открыт. Результат (какие порты открыты) записывается в базу.
            */
        }
        else if (mode == "-l" || mode == "--list") 
        {
            db->listDevices();
            return 0;
        }
        else if (mode == "-t" || mode == "--test-speed")
        {
            /*
            Что это: Модуль измерения качества связи (L3/L7). 
            Позволяет оценить реальную производительность сети и стабильность интернета.

            За какие файлы отвечает:
                include/MetricsEngine.hpp
                src/MetricsEngine.cpp

            Логика: 
                1. Latency & Jitter: Выполняет серию ICMP-запросов (Ping) до 8.8.8.8 
                   и вычисляет задержку и её колебания (стабильность канала).
                2. Speed Test: Использует Boost.Beast (HTTP) для загрузки тестового блока 
                   данных и вычисляет реальную скорость загрузки в Mbps.
                3. База данных: Сохраняет полученные метрики в таблицу 'metrics' 
                   для построения истории качества связи.
            */
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