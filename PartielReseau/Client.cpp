#include "Client.h"

#include <iostream>
#include <thread>

Client::Client()
{
    m_udp_comm = UDPCommunicator();
    if (!m_udp_comm.initSocket())
    {
        std::cerr << "Error during socket initialization" << '\n';
    }

    if (!m_udp_comm.bindSocket(0))
    {
        std::cerr << "No available port anymore ??" << '\n';
        return;
    }

    std::string serverIp = "127.0.0.1";
    int serverPort = 8080;

    std::string uname;
    std::cout << "Specify your Username: ";
    std::getline(std::cin, uname);

    // manque de temps, packet types en string (if it works, it works)
    if (!m_udp_comm.sendMessage(std::vector<char>(uname.begin(), uname.end()), serverIp, serverPort))
    {
        std::cerr << "Error during message sending" << '\n';
    }

    std::cout << "Welcome to the UDP Chat!" << '\n';
    std::cout << "You may now talk and send messages using enter!" << '\n';
    std::cout << "Send an empty message to disconnect" << '\n';


    auto receiveThread = std::thread(&Client::RecvLoop, this);
    auto h = receiveThread.native_handle();
    receiveThread.detach();


    std::string message;
    while (isRunning)
    {
        // broken when new message is received, not enough time to fix this
        // std::cout << uname << ": ";
        std::getline(std::cin, message);

        if (message.length() < 1)
        {
            std::cout << "Disconnected" << '\n';
            isRunning = false;
        }

        if (!m_udp_comm.sendMessage(std::vector<char>(message.begin(), message.end()), serverIp, serverPort))
        {
            std::cerr << "Error during message sending" << '\n';
            std::cerr << "(How can this ever happen ?)" << '\n';
            isRunning = false;
        }
    }
}

void Client::RecvLoop()
{
    std::vector<char> receivedMessage;
    char ip[23];
    int port = 0;
    while (isRunning)
    {
        auto n = m_udp_comm.receiveMessage(receivedMessage, ip, &port);
        if (n > 0)
        {
            std::vector<char> bytes(receivedMessage.begin(), receivedMessage.end());
            std::string str(bytes.begin(), bytes.end());
            std::cout << str << '\n';
        }
        else
        {
            if (n == 0)
            {
                std::cerr << "Server closed gracefully for maintenance" << '\n';
            }
            else
            {
                std::cerr << "Server crash :/" << '\n';
            }
            std::cerr << "You are now offline" << '\n';
            std::cerr << "Restart the client to try and reconnect" << '\n';
            isRunning = false;
        }
    }
}
