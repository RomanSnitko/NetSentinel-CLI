#pragma once
#include <string>
#include <iomanip>
#include <sstream>

class NetworkUtils 
{
public:
    static std::string formatMac(const uint8_t* mac) 
    {
        std::stringstream ss;
        for (int i = 0; i < 6; ++i) 
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
            if (i < 5) ss << ":";
        }
        return ss.str();
    }
};