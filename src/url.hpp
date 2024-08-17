#pragma once

#include <map>
#include <string>
#include <optional>

enum class TrackerProtocol
{
    HTTP,
    HTTPS
};

const std::map<TrackerProtocol, std::string> TrackerProtocols{
    {TrackerProtocol::HTTP, "HTTP"},
    {TrackerProtocol::HTTPS, "HTTPS"}};

const std::map<TrackerProtocol, int> TrackerProtocolsPorts{
    {TrackerProtocol::HTTP, 80},
    {TrackerProtocol::HTTPS, 443}};

struct Url
{
    TrackerProtocol protocol;
    std::string hostname;
    int port;
    std::string path;
};


Url ParseUrl(std::string_view urlView);

std::string ToString(Url url);
