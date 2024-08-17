#include "url.hpp"

#include <format>
#include <stdexcept>
#include <string>

Url ParseUrl(std::string_view urlView)
{
    int protocolLength;
    TrackerProtocol protocol;

    if (urlView.starts_with("https://"))
    {
        protocolLength = 8;
        protocol = TrackerProtocol::HTTPS;
    }
    else if (urlView.starts_with("http://"))
    {
        protocolLength = 7;
        protocol = TrackerProtocol::HTTP;
    }
    else
    {
        throw std::runtime_error{"Unsupported URL protocol."};
    }

    auto startOfPath = urlView.find_first_of('/', protocolLength);
    std::string path;
    if (startOfPath == std::string_view::npos)
    {
        startOfPath = urlView.length();
        path = "/";
    }
    else
    {
        path = urlView.substr(startOfPath);
    }

    // Parse port
    auto startOfPort = urlView.find_first_of(':', protocolLength);
    auto urlHasPort = startOfPort != std::string_view::npos;

    std::string hostname;
    int port;
    if (urlHasPort)
    {
        auto portString = std::string(urlView.substr(startOfPort + 1, startOfPath - startOfPort));
        hostname = std::string{urlView.substr(protocolLength, startOfPort - protocolLength)};
        port = std::stoi(portString);
    }
    else
    {
        hostname = std::string{urlView.substr(protocolLength, startOfPath - protocolLength)};
        port = TrackerProtocolsPorts.at(protocol);
    }

    return {protocol, hostname, port, path};
}

std::string ToString(Url url)
{
    // Url{TrackerProtocol::HTTP, "localhost", 3000, "/"}
    return std::format("Url{{{}, {}, {}, {}}}", TrackerProtocols.at(url.protocol), url.hostname, url.port, url.path);
}
