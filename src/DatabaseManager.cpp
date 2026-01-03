#include "DatabaseManager.hpp"
#include <iostream>

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
    
    W.exec_params(
        "INSERT INTO devices (mac_address, ip_address, hostname, last_seen) "
        "VALUES ($1, $2, $3, CURRENT_TIMESTAMP) "
        "ON CONFLICT (mac_address) DO UPDATE SET ip_address = $2, hostname = $3, last_seen = CURRENT_TIMESTAMP",
        mac, ip, hostname
    );
    W.commit();
}