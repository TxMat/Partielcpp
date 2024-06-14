#include "UDPCommunicator.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

UDPCommunicator::UDPCommunicator() : sockfd(-1) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error during Winsock initialization" << std::endl;
    }
#endif
}

UDPCommunicator::~UDPCommunicator() {
    if (sockfd != -1) {
#ifdef _WIN32
        closesocket(sockfd);
        WSACleanup();
#else
        close(sockfd);
#endif
    }
}

bool UDPCommunicator::initSocket() {
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        std::cerr << "Error during socket creation" << std::endl;
        return false;
    }
    return true;
}

bool UDPCommunicator::bindSocket(int port) const {
    sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error during socket binding" << std::endl;
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        return false;
    }
    return true;
}

bool UDPCommunicator::sendMessage(const std::vector<char>& message, const std::string& address, int port) {
    sockaddr_in servaddr{};
    std::memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(address.c_str());

    if (sendto(sockfd, message.data(), message.size(), 0, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error during message sending" << std::endl;
        return false;
    }
    return true;
}

long UDPCommunicator::receiveMessage(std::vector<char>& message, char* ip, int* port, int bufferSize) const {
    // Initialize buffer with the given buffer size
    char buffer[1400];
    sockaddr_in cliaddr{};
    socklen_t len = sizeof(cliaddr);

    // Receive the message
    long n = recvfrom(sockfd, buffer, 1400 - 1, 0, (sockaddr*)&cliaddr, &len);

    // Check for errors during reception
    if (n < 0) {
        // std::cerr << "Error during message reception : " << WSAGetLastError() << std::endl;
        return false;
    }

    inet_ntop(AF_INET, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN);
    *port = htons(cliaddr.sin_port);

    // Ensure buffer is null-terminated if n < bufferSize
    if (n < bufferSize) {
        buffer[n] = '\0';
    }

    message.clear();
    message.insert(message.end(), buffer, buffer + n);

    return n;
}
