#include <iostream>
#include <memory>
#include "DatabaseManager.hpp"
#include "CLIHandler.hpp"
#include "AuditEngine.hpp"
#include <boost/asio.hpp>

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
            /*
                Что это: Уровень L2 (Data Link) и L3 (Network) модели OSI. Самая сложная и «хакерская» часть. 
                Программа ищет, кто вообще подключен к твоему Wi-Fi или Ethernet.

                За какие файлы отвечает:
                include/DiscoveryEngine.hpp
                src/DiscoveryEngine.cpp

                Логика: Здесь будет использоваться ARP (Address Resolution Protocol). Программа отправляет 
                широковещательный запрос («Кто здесь?»), собирает MAC-адреса и IP-адреса всех устройств и 
                передает их в DatabaseManager для сохранения.
            */
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
            /*
            Что это: Работа с данными. Это твоя «память». Если ты сканировал сеть вчера, 
            тебе не нужно сканировать её снова, чтобы увидеть список устройств — они уже в базе.

            За какие файлы отвечает:
                include/DatabaseManager.hpp
                src/DatabaseManager.cpp

            Логика: Программа делает SQL-запрос SELECT * FROM devices; к PostgreSQL. 
            Затем она красиво выводит этот список в терминал.
            */
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