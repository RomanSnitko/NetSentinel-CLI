## NetSentinel-CLI

Система низкоуровневого сетевого аудита и мониторинга инфраструктуры. Реализует активное обнаружение хостов через ручную сборку пакетов (Raw Sockets L2/L3), асинхронный движок сканирования тысяч портов на базе Boost.Asio и детектор сетевых угроз (ARP Spoofing). Включает модули замера метрик качества канала и интеграцию с PostgreSQL (libpqxx) для отслеживания истории состояний и топологии сети.

---

# Features

* L2/L3 Discovery: Active host detection using manual ARP packet assembly (Raw Sockets).
* Async Port Audit: High-concurrency port scanning engine based on Boost.Asio (Proactor pattern).
* Security Watchdog: Detection of network threats such as ARP Spoofing.
* Quality Metrics: Real-time measurement of Latency, Jitter, and Download Speed.
* Persistent Storage: SQL-based tracking of network history and device status via libpqxx.

---

# Tech Stack

* C++ Development: Boost.Asio, Async Programming, RAII, C++17/20.
* Networking: L2-L4 Layers, TCP/UDP, ARP, ICMP, Raw Sockets (AF_PACKET).
* Database: PostgreSQL 13+, libpqxx (parameterized queries).
* Build System: CMake.

---

# Project Structure
```
NetSentinelCLI/
├── build/                # Build directory
├── include/              # Header files (.hpp)
│   ├── AuditEngine.hpp   # Port scanning logic
│   ├── CLIHandler.hpp    # Terminal interface & menu
│   ├── DatabaseManager.hpp# PostgreSQL integration
│   ├── DiscoveryEngine.hpp# ARP/Raw Sockets discovery
│   └── MetricsEngine.hpp # Speed & quality testing
├── src/                  # Source code (.cpp)
│   ├── main.cpp          # Application entry point
│   └── ...               # Implementations
├── sql/                  # Database schema & migrations
├── CMakeLists.txt        # Build configuration
└── README.md             # This file
```

---

# Quick Start
Ensure you have the following installed:

* C++ Compiler (GCC 11+ or Clang 13+)
* CMake (3.20+)
* Boost Libraries (system, asio)
* libpqxx & PostgreSQL

Build and Setup

Clone the repository:
```Bash
git clone https://github.com/RomanSnitko/NetSentinel-CLI.git
cd NetSentinel-CLI
```
  

Setup PostgreSQL:
```Bash
# Create database and user
sudo -u postgres psql -c "CREATE DATABASE netsentinel_db;"
sudo -u postgres psql -c "CREATE USER roman WITH PASSWORD 'your_password';"
```
  
Build the project:
```
mkdir build && cd build
cmake ..
make
```
      
Usage
```
# Show help menu
./NetSentinel --help

# Scan local network for devices (Requires sudo for Raw Sockets)
sudo ./NetSentinel -s wlo1

# Run asynchronous port audit on a specific IP
./NetSentinel -a 192.168.1.1

# Measure network quality (Speed, Latency)
./NetSentinel -t

# List all known devices from database
./NetSentinel -l
```
  

Example Output (Port Audit)
```    
====================================
      NetSentinel CLI v1.0.0
====================================
[DB] Connected to: netsentinel_db
[*] Starting audit on 192.168.10.1 for 14 ports...
[+] Port 53 is OPEN on 192.168.10.1
[+] Port 80 is OPEN on 192.168.10.1
[+] Port 443 is OPEN on 192.168.10.1
[*] Audit for 192.168.10.1 finished.
```
  

# Architecture

* DiscoveryEngine: Uses AF_PACKET and SOCK_RAW to bypass the standard TCP/IP stack, manually constructing Ethernet and ARP headers to map the local network.
* AuditEngine: Implements an asynchronous event loop. Instead of sequential scanning, it initiates hundreds of connection attempts simultaneously using Boost.Asio timers for sub-second fail-fast logic.
* DatabaseManager: Implements the RAII pattern for SQL connections, ensuring all resources are freed and transactions are committed safely.

# Requirements

* OS: Linux (Required for Raw Sockets and ioctl support)
* Permissions: sudo privileges are required for L2 network scanning.
* Libraries: libboost-all-dev, libpqxx-dev, libpq-dev.
