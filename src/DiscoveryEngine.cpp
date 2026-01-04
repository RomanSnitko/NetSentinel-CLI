#include "DiscoveryEngine.hpp"
#include "DatabaseManager.hpp"

#include <iostream>
#include <array>
#include <stdexcept>
#include <cstring>
#include <iomanip>
#include <sstream>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

DiscoveryEngine::DiscoveryEngine(std::shared_ptr<DatabaseManager> db) : db_ptr(db) {}

void DiscoveryEngine::sendArpRequest(int sock, const std::string& iface, const std::string& target_ip) 
{
    //for no buffer overflow when copy
    if (iface.size() >= IFNAMSIZ)
    {
        throw std::invalid_argument("interface name is long");
    }


    ifreq ifr_mac{};
    std::strncpy(ifr_mac.ifr_name, iface.c_str(), IFNAMSIZ - 1);

    //SIOCGIFHWADDR - get hardware address mac
    if (ioctl(sock, SIOCGIFHWADDR, &ifr_mac) < 0)
    {
        throw std::runtime_error("could not get macaddress for " + iface);
    }
    std::array<uint8_t, 6> my_mac{};
    //size of mac - 6 bytes
    std::memcpy(my_mac.data(), ifr_mac.ifr_hwaddr.sa_data, 6);


    ifreq ifr_ip{};
    std::strncpy(ifr_ip.ifr_name, iface.c_str(), IFNAMSIZ - 1);

    //SIOCGIFADDR - get ip intrface
    if (ioctl(sock, SIOCGIFADDR, &ifr_ip) < 0)
    {
        throw std::runtime_error("could not get IP address for " + iface);
    }

    std::array<uint8_t, 4> my_ip{};
    auto* sin = reinterpret_cast<sockaddr_in*>(&ifr_ip.ifr_addr); //offset by 2 bytes :(
    //std::memcpy(my_ip.data(), ifr_ip.ifr_addr.sa_data + 2, 4);

    //IPv4 - 4 bytes
    std::memcpy(my_ip.data(), &sin->sin_addr, 4);

    
    sockaddr_ll device{};
    device.sll_family = AF_PACKET;
    device.sll_ifindex = if_nametoindex(iface.c_str());
    device.sll_halen = 6;
    std::fill(std::begin(device.sll_addr), std::begin(device.sll_addr) + 6, 0xFF); //broadcast

    //arp = header ethernet(14) + header arp(28)
    std::array<uint8_t, 42> buffer{};
    auto* eth = reinterpret_cast<ether_header*>(buffer.data());
    auto* arp = reinterpret_cast<ether_arp*>(buffer.data() + sizeof(ether_header));

    //ethernet header
    std::fill(std::begin(eth->ether_dhost), std::end(eth->ether_dhost), 0xFF); //broadcast
    std::memcpy(eth->ether_shost, my_mac.data(), 6);
    eth->ether_type = htons(ETHERTYPE_ARP); //-> network order bytes

    //apr header
    arp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp->ea_hdr.ar_pro = htons(ETH_P_IP);
    arp->ea_hdr.ar_hln = 6;
    arp->ea_hdr.ar_pln = 4;
    arp->ea_hdr.ar_op  = htons(ARPOP_REQUEST);

    std::memcpy(arp->arp_sha, my_mac.data(), 6);
    std::memcpy(arp->arp_spa, my_ip.data(), 4);
    std::fill(std::begin(arp->arp_tha), std::end(arp->arp_tha), 0x00); //target mac

    if (inet_pton(AF_INET, target_ip.c_str(), arp->arp_tpa) != 1) 
    {  //ip -> 4 bytes and write in target
        throw std::invalid_argument("invalid target ip format: " + target_ip);
    }

    if (sendto(sock, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr*>(&device), sizeof(device)) < 0) 
    {
        throw std::runtime_error("failed to send arp request to " + target_ip);
    }
}

void DiscoveryEngine::listenForReplies(int sock) 
{
    //timeout 2 sec
    timeval tv{.tv_sec = 2, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    std::array<uint8_t, 65536> buffer{};
    std::cout << "[*] waiting for arp replies..." << std::endl;

    while (true) 
    {
        //recvfrom wait while come packet
        ssize_t length = recvfrom(sock, buffer.data(), buffer.size(), 0, nullptr, nullptr);
        if (length < 0) 
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) 
            {
                break; 
            }
            throw std::runtime_error("error receiving packets");
        }
        
        //remove all trash
        if (length < static_cast<ssize_t>(sizeof(ether_header) + sizeof(ether_arp))) continue;

        //трафарет ethernet header
        auto* eth = reinterpret_cast<ether_header*>(buffer.data());
        if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
            auto* arp = reinterpret_cast<ether_arp*>(buffer.data() + sizeof(ether_header)); //+14 bytes
            
            if (ntohs(arp->ea_hdr.ar_op) == ARPOP_REPLY) 
            {
                //sender ip
                std::array<char, INET_ADDRSTRLEN> ip_buf{};
                inet_ntop(AF_INET, arp->arp_spa, ip_buf.data(), ip_buf.size());
                std::string sender_ip(ip_buf.data());

                //mac to 16number (255->ff)
                std::stringstream mac_ss;
                for (int i = 0; i < 6; ++i) 
                {
                    mac_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(arp->arp_sha[i]);
                    if (i < 5) mac_ss << ":";
                }
                std::string sender_mac = mac_ss.str();

                std::cout << "\033[1;32m[FOUND]\033[0m IP: " << std::left << std::setw(15) << sender_ip 
                          << " | MAC: " << sender_mac << std::endl;

                if (db_ptr) 
                {
                    db_ptr->saveDevice(sender_mac, sender_ip, "LAN Device");
                    db_ptr->logEvent(sender_mac, "DEVICE_DISCOVERED", "IP: " + sender_ip);
                }
            }
        }
    }
}

void DiscoveryEngine::runScan(const std::string& interface_name) 
{
    //raw socket
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0) 
    {
        throw std::runtime_error("raw socket failed, try: sudo ./NetSentinel -s");
    }

    try 
    {
        ifreq ifr{};  //name interface (wlo1)
        std::strncpy(ifr.ifr_name, interface_name.c_str(), IFNAMSIZ - 1);
        
        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) 
        {
            throw std::runtime_error("failed to get local ip");
        }
        uint32_t local_ip = reinterpret_cast<sockaddr_in*>(&ifr.ifr_addr)->sin_addr.s_addr;

        //netmask for define range between local and network
        if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0) 
        {
            throw std::runtime_error("failed to get netmask");
        }
        uint32_t netmask = reinterpret_cast<sockaddr_in*>(&ifr.ifr_addr)->sin_addr.s_addr;

        //range ip addresov
        uint32_t network_addr = ntohl(local_ip & netmask); //network(big-endian) -> computer(little-endian)
        uint32_t broadcast_addr = network_addr | (~ntohl(netmask));

        std::cout << "[*] Starting scan on " << interface_name << "..." << std::endl;

        //send arp request
        for (uint32_t i = network_addr + 1; i < broadcast_addr; ++i) 
        {
            in_addr addr{.s_addr = htonl(i)}; //htonl: to big-endian
            sendArpRequest(sock, interface_name, inet_ntoa(addr)); //inet_ntoa: to string
        }

        listenForReplies(sock);

    } 
    catch (...) 
    {
        ::close(sock);
        throw;
    }

    ::close(sock);
}