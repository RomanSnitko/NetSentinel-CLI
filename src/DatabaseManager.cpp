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
            std::cout << "[DB] connected to: " << connection->dbname() << std::endl;
        }
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "[DB] error: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::createTables() 
{
    pqxx::work W(*connection); //work with transaction
    
    W.exec(R"(
        CREATE TABLE IF NOT EXISTS devices (
            id SERIAL PRIMARY KEY,
            mac_address TEXT UNIQUE NOT NULL,
            ip_address TEXT,
            hostname TEXT,
            last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )");

    W.commit();
    std::cout << "[DB] tables initialized :)" << std::endl;
}

void DatabaseManager::insertDevice(const std::string& mac, const std::string& ip, const std::string& hostname)
{
    try
    {
        pqxx::work W(*connection);

        W.exec_params(
            "INSERT INTO devices (mac_address, ip_address, hostname) "
            "VALUES ($1, $2, $3) "
            "ON CONFLICT (mac_address) DO UPDATE SET "
            "ip_address = EXCLUDED.ip_address, "
            "hostname = EXCLUDED.hostname, "
            "last_seen = CURRENT_TIMESTAMP;",
            mac, ip, hostname
        );

        W.commit();
        std::cout << "[DB] device inserted/updated: " << mac << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[DB] insertDevice error: " << e.what() << std::endl;
        throw;
    }
}
