#include "utils.hpp"

#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

std::string UrlEncodeBytes(const std::vector<std::byte> bytesToConvert)
{
    std::stringstream encodedBytes;
    encodedBytes << std::hex; 

    for (auto& b : bytesToConvert)
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

void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch) { return !std::isspace(ch); }));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch) { return !std::isspace(ch); }).base(), s.end());
}

void trim(std::string& s)
{
    rtrim(s);
    ltrim(s);
}