#pragma once
#include "UDPCommunicator.h"

class Client
{
public:
    Client();

private:

    void RecvLoop();

    bool isRunning = true;
    
    UDPCommunicator m_udp_comm;
};
