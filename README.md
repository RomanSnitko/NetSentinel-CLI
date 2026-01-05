# NetSentinel-CLI

Высокопроизводительная система низкоуровневого сетевого аудита и мониторинга инфраструктуры. Реализует активное обнаружение хостов через ручную сборку пакетов (Raw Sockets L2/L3) и прямое взаимодействие с ядром Linux (ioctl). Включает асинхронный движок сканирования тысяч портов на базе Boost.Asio (паттерн Proactor), детектор сетевых угроз (ARP Spoofing Watchdog) и модули замера метрик качества канала на уровне L7 (Boost.Beast). Интегрирована с PostgreSQL (libpqxx) и Docker-compose для персистентного отслеживания истории состояний сети и её топологии.

## Features

* L2/L3 Discovery: Active host detection using manual ARP packet assembly (Raw Sockets).
* Async Port Audit: High-concurrency port scanning engine based on Boost.Asio (Proactor pattern).
* Security Watchdog: Detection of network threats such as ARP Spoofing.
* Quality Metrics: Real-time measurement of Latency, Jitter, and Download Speed.
* Persistent Storage: SQL-based tracking of network history and device status via libpqxx.

## Tech Stack

* C++ Dev: Boost.Asio (Proactor pattern), Boost.Beast (HTTP), Async Programming, RAII, C++17/20, Smart Pointers.
* Networking: L2-L7 Layers, ARP, ICMP, TCP/UDP, Raw Sockets (AF_PACKET), System IO (ioctl), IP/MAC stack management.
* Security: ARP Spoofing Watchdog, Asynchronous Port Auditing, Network Event Logging.
* Data & Tools: PostgreSQL (libpqxx), Parameterized SQL, Docker-compose (containerization), CMake, CLI formatting (iomanip).

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
### (Scan Devices)
```
[DB] Connected to: netsentinel_db
[*] initializing Discovery Engine...
[*] Starting scan on wlo1...
[*] waiting for arp replies...
[FOUND] IP: 192.168.10.1    | MAC: 80:af:ca:6a:e2:18
[+] scan completed successfully.
```
### (Test Speed)
```
[DB] Connected to: netsentinel_db
[*] Starting Network Quality Test...
[*] Starting network quality metrics...
[Metrics] Ping 8.8.8.8: 19.23 ms
[Metrics] Ping 8.8.8.8: 350.40 ms
[Metrics] Ping 8.8.8.8: 301.70 ms
[Metrics] Ping 8.8.8.8: 17.47 ms
[Metrics] Ping 8.8.8.8: 399.87 ms
[Metrics] Ping 8.8.8.8: 99.76 ms
[Metrics] Ping 8.8.8.8: 401.44 ms
[Metrics] Ping 8.8.8.8: 18.00 ms
[Metrics] Ping 8.8.8.8: 21.65 ms
[Metrics] Ping 8.8.8.8: 18.17 ms
[+] Latency: 164.77 ms
[+] Jitter: 226.54 ms
[*] Moving to download speed test...
[Metrics] Step 1: Resolving www.google.com...
[Metrics] Step 2: Connecting...
[Metrics] Step 3: Requesting data...
[Metrics] Step 4: Receiving data...
[+] Download Speed: 2.22 Mbps
[DB] Metrics saved to history.
[DB] Metrics saved successfully.
[+] All tests completed.
```

## Architecture

* DiscoveryEngine: Uses AF_PACKET and SOCK_RAW to bypass the standard TCP/IP stack, manually constructing Ethernet and ARP headers to map the local network.
* AuditEngine: Implements an asynchronous event loop. Instead of sequential scanning, it initiates hundreds of connection attempts simultaneously using Boost.Asio timers for sub-second fail-fast logic.
* DatabaseManager: Implements the RAII pattern for SQL connections, ensuring all resources are freed and transactions are committed safely.

## Requirements

* OS: Linux (Required for Raw Sockets and ioctl support)
* Permissions: sudo privileges are required for L2 network scanning.
* Libraries: libboost-all-dev, libpqxx-dev, libpq-dev.
