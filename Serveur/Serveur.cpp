#include <iostream>
#include <map>
#include <ostream>

#include "framework.h"
#include "UDPCommunicator.h"

int main()
{
    auto udpComm = UDPCommunicator();
    if (!udpComm.initSocket())
    {
        std::cerr << "Error during socket initialization" << '\n';
    }

    // Binding the socket to port 8080
    if (!udpComm.bindSocket(8080))
    {
        std::cerr << "Error during binding the socket to port 8080" << '\n';
    }

    std::cout << "Server Up" << '\n';

    std::map<std::pair<char*, int>, std::string> users = std::map<std::pair<char*, int>, std::string>();


    std::vector<char> receivedMessage;
    char ip[23];
    int port = 0;
    while (true)
    {
        auto n = udpComm.receiveMessage(receivedMessage, ip, &port);
        auto pair = std::pair<char*, int>(ip, port);
        if (n > 0)
        {
            if (users.find(pair) == users.end())
            {
                std::string uname(receivedMessage.begin(), receivedMessage.end());
                users[pair] = uname;
                std::cout << "New Connection From : " << pair.first << ":" << pair.second << " as " << uname << '\n';
                continue;
            }

            std::vector<char> bytes(receivedMessage.begin(), receivedMessage.end());
            std::string str(bytes.begin(), bytes.end());
            std::string parsed_message = users[pair] + ": " + str;
            std::cout << "[DEBUG] Got message | " << parsed_message << '\n';

            std::vector<char> newdata(parsed_message.begin(), parsed_message.end());

            for (auto& user : users)
            {
                // no need to thread as it will only add complexity for nothing
                // (maybe spawn n thread to send to n clients but that does not seem useful)
                if (user.first != pair)
                {
                    if (!udpComm.sendMessage(newdata, user.first.first, user.first.second))
                    {
                        // for safety
                        std::cout << "Connection Closed From " << pair.first << ":" << pair.second << " (" <<
                        users[pair] << ")" << '\n';
                        users.erase(pair);
                    }
                }
            }
        }
        else if (n == 0)
        {
            if (users.find(pair) != users.end()) {
                std::cout << "Connection Closed From " << pair.first << ":" << pair.second << " (" <<
                        users[pair] << ")" << '\n';
                users.erase(pair);
            }
        }
        {
            
        }
    }

    return 0;
}
