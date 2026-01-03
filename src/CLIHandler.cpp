#include "CLIHandler.hpp"
#include <iostream>

void CLIHandler::printBanner()
{
    std::cout << "\033[1;36m" << "====================================\n"
              << "      NetSentinel CLI v1.0.0\n"
              << "====================================\033[0m\n";
}

void CLIHandler::printHelp() 
{
    printBanner();
    std::cout << "Usage: ./NetSentinel [OPTIONS]\n\n"
              << "Options:\n"
              << "  -h, --help       Show this help message\n"
              << "  -s, --scan       Scan local network for devices\n"
              << "  -a, --audit <IP> Run port audit on target IP\n"
              << "  -t, --test-speed Latency, Jitter, Download Speed\n"
              << "  -l, --list       Show all devices from DB\n\n";
}