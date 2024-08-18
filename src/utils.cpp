#include "utils.hpp"

#include <iomanip>
#include <map>
#include <ranges>
#include <string>
#include <sstream>
#include <vector>

std::string UrlEncodeBytes(const std::vector<std::byte> bytesToConvert)
{
    std::stringstream encodedBytes;
    encodedBytes << std::hex;

    for (auto &b : bytesToConvert)
    {
        encodedBytes << "%" << std::setw(2) << std::setfill('0') << std::to_integer<int>(b);
    }

    return encodedBytes.str();
}

std::string GetHexFromBytes(auto &&byteList)
{
    std::stringstream ss{};
    for (auto const b : byteList)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(b);
    }

    return ss.str();
}

std::vector<std::byte> HexStringToBytes(const std::string &hex)
{
    // TODO: string_view to a range and then group and transform.
    // TODO: test this somehow.

    std::vector<std::byte> bytes;
    bytes.reserve(hex.length() / 2); // Reserve space for efficiency

    for (size_t i = 0; i < hex.length(); i += 2)
    {
        std::byte byte{static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16))};
        bytes.push_back(byte);
    }

    return bytes;
}

void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch)
                                    { return !std::isspace(ch); }));
}

void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// https://stackoverflow.com/a/46931770
std::vector<std::string> split(std::string_view sv, std::string delimiter)
{
    std::size_t pos_start = 0;
    std::size_t pos_end;

    std::size_t delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = sv.find(delimiter, pos_start)) != std::string::npos)
    {
        token = std::string(sv.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(std::string(sv.substr(pos_start)));

    return res;
}

std::vector<std::string> split_on_first(std::string_view sv, std::string delimiter)
{
    std::size_t pos_start = 0;
    std::size_t pos_end;

    std::size_t delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    pos_end = sv.find(delimiter, pos_start);

    if (pos_end == std::string::npos)
    {
        throw std::runtime_error("String does not contain delimiter.");
    }

    token = std::string(sv.substr(pos_start, pos_end - pos_start));
    pos_start = pos_end + delim_len;
    res.push_back(token);
    res.push_back(std::string(sv.substr(pos_start)));

    return res;
}

std::string ConvertParametersToQueryString(const std::map<std::string, std::string> &query_parameters)
{
    return query_parameters | std::views::transform([](std::pair<std::string, std::string> pair)
                                                    { return std::format("{}={}", pair.first, pair.second); }) 
                            | std::views::join_with('&') | std::ranges::to<std::string>();
}