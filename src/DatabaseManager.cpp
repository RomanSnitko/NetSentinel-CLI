#include "DatabaseManager.hpp"
#include <iostream>
#include <iomanip>

DatabaseManager::DatabaseManager(const std::string& connection_str) : conn_str(connection_str) {}

void DatabaseManager::connect() 
{
    try 
    {
        connection = std::make_unique<pqxx::connection>(conn_str);
        if (connection->is_open()) 
        {
            std::cout << "[DB] Connected to: " << connection->dbname() << std::endl;
        }
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "[DB] Error: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::createTables() 
{
    pqxx::work W(*connection);

    W.exec
    (
        R"(
        CREATE TABLE IF NOT EXISTS devices (
            id SERIAL PRIMARY KEY,
            mac_address TEXT UNIQUE NOT NULL,
            ip_address TEXT,
            hostname TEXT,
            last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
        CREATE TABLE IF NOT EXISTS network_events (
            id SERIAL PRIMARY KEY,
            mac_address TEXT,
            event_type TEXT,
            details TEXT,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
        )"
    );

    W.commit();
}

void DatabaseManager::saveDevice(const std::string& mac, const std::string& ip, const std::string& hostname) 
{
    pqxx::work W(*connection);
    
    W.exec_params
    (
        "INSERT INTO devices (mac_address, ip_address, hostname, last_seen) "
        "VALUES ($1, $2, $3, CURRENT_TIMESTAMP) "
        "ON CONFLICT (mac_address) DO UPDATE SET ip_address = $2, hostname = $3, last_seen = CURRENT_TIMESTAMP",
        mac, ip, hostname
    );
    W.commit();
}

void DatabaseManager::listDevices()
{
    try 
    {
        pqxx::nontransaction N(*connection);

        pqxx::result R = N.exec("SELECT mac_address, ip_address, hostname, last_seen FROM devices ORDER BY last_seen DESC");
    
        if (R.empty()) 
        {
            std::cout << "\033[1;33m[!] No devices found in database.\033[0m" << std::endl;
            return;
        }

        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << std::left 
                  << std::setw(20) << "MAC ADDRESS" 
                  << std::setw(18) << "IP ADDRESS" 
                  << std::setw(20) << "HOSTNAME" 
                  << "LAST SEEN" << "\n";
        std::cout << std::string(80, '-') << "\n";

        for (auto const &row : R) 
        {
            std::cout << std::left 
                      << std::setw(20) << row[0].c_str()
                      << std::setw(18) << row[1].c_str()
                      << std::setw(20) << row[2].c_str()
                      << row[3].c_str() << "\n";
        }
        std::cout << std::string(80, '=') << "\n\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[DB] ListDevices error: " << e.what() << std::endl;
    }
}

void DatabaseManager::logEvent(const std::string& mac, const std::string& type, const std::string& details) 
{
    try 
    {
        pqxx::work W(*connection);
        
        W.exec_params
        (
            "INSERT INTO network_events (mac_address, event_type, details, timestamp) "
            "VALUES ($1, $2, $3, CURRENT_TIMESTAMP)",
            mac, type, details
        );
        
        W.commit();
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "[DB] LogEvent error: " << e.what() << std::endl;
    }
}