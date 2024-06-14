#ifndef UDPCOMMUNICATOR_H
#define UDPCOMMUNICATOR_H

#include <string>
#include <vector>

// Definition des includes et des types pour Windows et Unix/Linux
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <unistd.h>

typedef int socket_t;
#endif

class UDPCommunicator {
public:
    UDPCommunicator();
    ~UDPCommunicator();

    bool initSocket();
    bool bindSocket(int port) const;
    bool sendMessage(const std::vector<char>& message, const std::string& address, int port);
    long receiveMessage(std::vector<char>& message, char* ip, int* port, int bufferSize = 1024) const;

private:
    socket_t sockfd;
};

#endif // UDPCOMMUNICATOR_H