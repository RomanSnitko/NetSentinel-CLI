# NetSentinel-CLI

Система низкоуровневого сетевого аудита и мониторинга инфраструктуры. Реализует активное обнаружение хостов через ручную сборку пакетов (Raw Sockets L2/L3), асинхронный движок сканирования тысяч портов на базе Boost.Asio и детектор сетевых угроз (ARP Spoofing). Включает модули замера метрик качества канала и интеграцию с PostgreSQL (libpqxx) для отслеживания истории состояний и топологии сети.

## Features

* L2/L3 Discovery: Active host detection using manual ARP packet assembly (Raw Sockets).
* Async Port Audit: High-concurrency port scanning engine based on Boost.Asio (Proactor pattern).
* Security Watchdog: Detection of network threats such as ARP Spoofing.
* Quality Metrics: Real-time measurement of Latency, Jitter, and Download Speed.
* Persistent Storage: SQL-based tracking of network history and device status via libpqxx.

## Tech Stack

* C++ Development: Boost.Asio, Async Programming, RAII, C++17/20.
* Networking: L2-L4 Layers, TCP/UDP, ARP, ICMP, Raw Sockets (AF_PACKET).
* Database: PostgreSQL 13+, libpqxx (parameterized queries).
* Build System: CMake.

* C++ Dev: Boost.Asio (Proactor pattern), Boost.Beast (HTTP), Async Programming, RAII, C++17/20, Smart Pointers.
* Networking: L2-L7 Layers, ARP, ICMP, TCP/UDP, Raw Sockets (AF_PACKET), System IO (ioctl), IP/MAC stack management.
* Security: ARP Spoofing Watchdog, Asynchronous Port Auditing, Network Event Logging.
* Data & Tools: PostgreSQL (libpqxx), PostgreSQL, Docker-compose (containerization), CMake, CLI formatting (iomanip).

## Project Structure

The project follows a modular architecture, separating the low-level networking logic from the asynchronous engine and the data persistence layer.

```text
NetSentinel-CLI/
├── build/                      # Build artifacts (binaries and object files)
├── include/                    # Header files (.hpp)
│   ├── AuditEngine.hpp         # Asynchronous L4 port scanning engine
│   ├── CLIHandler.hpp          # CLI argument parsing and terminal UI logic
│   ├── DatabaseManager.hpp     # PostgreSQL interface (libpqxx wrapper)
│   ├── DiscoveryEngine.hpp     # L2 Host discovery via Raw Sockets (ARP)
│   ├── MetricsEngine.hpp       # L7 Network quality (Speed, Latency, Jitter)
│   └── NetworkUtils.hpp        # Network helper functions (IP/MAC conversion)
├── src/                        # Source files (.cpp)
│   ├── main.cpp                # Application entry point & io_context dispatcher
│   ├── AuditEngine.cpp         # Implementation of async connects and timers
│   ├── CLIHandler.cpp          # Banner rendering, tables, and help menu
│   ├── DatabaseManager.cpp     # SQL logic, RAII connection, and event logging
│   ├── DiscoveryEngine.cpp     # Raw socket management and ioctl configuration
│   └── MetricsEngine.cpp       # HTTP client and ICMP logic implementation
├── sql/                        # Database scripts
│   ├── init.sql                # Initial schema (devices, events, metrics tables)
│   └── migrations/             # Incremental database updates
├── scripts/                    # Automation and testing utilities
│   ├── setup_db.sh             # Script to automate PostgreSQL user/DB creation
│   └── test_ports.py           # Python utility to simulate open ports for testing
├── docker-compose.yml          # Containerized PostgreSQL for rapid deployment
├── .gitignore                  # Git exclusion rules (build/, logs, sensitive data)
├── CMakeLists.txt              # Cross-platform build configuration
└── README.md                   # Project documentation
```

---

## Quick Start
Ensure you have the following installed:

* C++ Compiler (GCC 11+ or Clang 13+)
* CMake (3.20+)
* Boost Libraries (system, asio)
* libpqxx & PostgreSQL

### Build and Setup

### Clone the repository:
```Bash
git clone https://github.com/RomanSnitko/NetSentinel-CLI.git
cd NetSentinel-CLI
```
  

### Setup PostgreSQL:
```Bash
# Create database and user
sudo -u postgres psql -c "CREATE DATABASE netsentinel_db;"
sudo -u postgres psql -c "CREATE USER roman WITH PASSWORD 'your_password';"
```
  
### Build the project:
```
mkdir build && cd build
cmake ..
make
```
      
### Usage
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
  

### Example Output (Port Audit)
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

## Architecture

* DiscoveryEngine: Uses AF_PACKET and SOCK_RAW to bypass the standard TCP/IP stack, manually constructing Ethernet and ARP headers to map the local network.
* AuditEngine: Implements an asynchronous event loop. Instead of sequential scanning, it initiates hundreds of connection attempts simultaneously using Boost.Asio timers for sub-second fail-fast logic.
* DatabaseManager: Implements the RAII pattern for SQL connections, ensuring all resources are freed and transactions are committed safely.

## Requirements

* OS: Linux (Required for Raw Sockets and ioctl support)
* Permissions: sudo privileges are required for L2 network scanning.
* Libraries: libboost-all-dev, libpqxx-dev, libpq-dev.
