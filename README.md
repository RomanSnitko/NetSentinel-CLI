# NetSentinel-CLI

A low-level network audit and infrastructure monitoring system. It implements active host discovery through manual packet assembly (Raw Sockets L2/L3), an asynchronous scanning engine for thousands of ports based on Boost.Asio, and a network threat detector (ARP Spoofing). Includes modules for measuring link quality metrics and PostgreSQL integration (libpqxx) for tracking network status and topology history.

* C++ Dev: Boost.Asio, RAII, libpqxx (PostgreSQL).
* Networking: L2-L4 Layers, TCP/UDP, ARP, ICMP, IPv4/IPv6, Raw Sockets.
* Security: ARP Spoofing Detection, Network Auditing.
* Tools: CMake, PostgreSQL, CLI.
  
---
```
====================================
      NetSentinel CLI v1.0.0
====================================
Usage: ./NetSentinel [OPTIONS]

Options:
  -h, --help       Show this help message
  -s, --scan       Scan local network for devices
  -a, --audit <IP> Run port audit on target IP
  -t, --test-speed Latency, Jitter, Download Speed
  -l, --list       Show all devices from DB
```
