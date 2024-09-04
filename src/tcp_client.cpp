#include "tcp_client.hpp"

#include <format>
#include <string>
#include <sstream>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

BT::TCPConnection::TCPConnection(int fd) : _fd(fd) {}

BT::TCPConnection::~TCPConnection()
{
    CloseConnection();
}

int BT::TCPConnection::SendMessage(std::string& message)
{
    return send(_fd, message.c_str(), message.length(), 0);
}

int BT::TCPConnection::SendMessage(std::vector<std::byte>& message)
{
    
    return send(_fd, message.data(), message.size(), 0);
}

std::string BT::TCPConnection::ReceiveMessage()
{
    int bytes_read;
    std::vector<char> buf(1000);
    std::stringstream response;

    while ((bytes_read = recv(_fd, buf.data(), buf.size(), 0)))
    {
        if (bytes_read < 0)
            throw std::runtime_error("Error reading from socket.");
        response.write(buf.data(), bytes_read);
    }

    return response.str();
}

std::string BT::TCPConnection::ReceiveMessage(int size)
{
    std::vector<char> buf(size);
    std::stringstream response;

    auto bytes_read = recv(_fd, buf.data(), buf.size(), 0);
    if (bytes_read < 0)
        throw std::runtime_error("Error reading from socket.");
    response.write(buf.data(), bytes_read);

    return response.str();
}

void BT::TCPConnection::CloseConnection()
{
    // We need to prevent a double-close of a file descriptor as the kernel reuses file descriptors and
    // we don't want to close a different fd if we've already closed the original fd.
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

BT::TCPConnection BT::TCPConnectionFactory::OpenConnection(std::string hostname, int port)
{
    addrinfo hints = {};
    addrinfo *result;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    auto port_string = std::to_string(port);
    auto status = getaddrinfo(hostname.c_str(), port_string.c_str(), &hints, &result);

    if (status != 0)
    {
        auto error_message = std::format("getaddrinfo error: {}.", gai_strerror(status));
        throw std::runtime_error(error_message);
    }

    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        auto error_message = std::format("Error creating socket.");
        throw std::runtime_error(error_message);
    }

    auto connection = connect(sockfd, result->ai_addr, result->ai_addrlen);
    if (connection < 0)
    {
        auto error_message = std::format("Error connecting to remote.");
        throw std::runtime_error(error_message);
    }

    freeaddrinfo(result);

    return { sockfd };
}