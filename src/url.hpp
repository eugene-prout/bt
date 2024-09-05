#pragma once

#include <map>
#include <string>
#include <optional>

namespace BT
{
    enum class TrackerProtocol
    {
        HTTP,
        HTTPS
    };

    const std::map<TrackerProtocol, std::string> TrackerProtocols{
        {TrackerProtocol::HTTP, "http"},
        {TrackerProtocol::HTTPS, "https"}};

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

    std::string ConvertParametersToQueryString(const std::map<std::string, std::string> &query_parameters);
}
