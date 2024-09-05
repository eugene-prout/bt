#pragma once

#include <string>
#include <vector>

namespace BT
{
    class TCPConnection
    {
    public:
        TCPConnection(int fd);
        ~TCPConnection();

        TCPConnection(const TCPConnection&&) = delete;
        TCPConnection& operator=(const TCPConnection&&) = delete;

        void SendMessage(std::string& message);
        void SendMessage(std::vector<std::byte>& message);

        std::string ReceiveMessageUntilBufferEmpty();
        std::string ReceiveMessage(int size);

        void CloseConnection();

    private:
        int _fd;
    };

    class TCPConnectionFactory
    {
    public:
        TCPConnection static OpenConnection(std::string hostname, int port);
    };
}
