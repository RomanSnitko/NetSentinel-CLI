#include <iostream>
#include "DatabaseManager.hpp"

int main() 
{
    std::string conn_str = "dbname=netsentinel_db user=roman";

    try 
    {
        DatabaseManager db(conn_str);
        db.connect();
        db.createTables();
        
        std::cout << "NetSentinel is ready!" << std::endl;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Startup failed: " << e.what() << std::endl;
        return 1;
    }
}