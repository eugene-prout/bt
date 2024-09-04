#include "utils.hpp"

#include <iomanip>
#include <map>
#include <format>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

std::string BT::UrlEncodeBytes(const std::vector<std::byte>& bytesToConvert)
{
    std::stringstream encodedBytes;
    encodedBytes << std::hex;

    for (auto b : bytesToConvert)
    {
        encodedBytes << "%" << std::setw(2) << std::setfill('0') << std::to_integer<int>(b);
    }

    return encodedBytes.str();
}

std::string BT::UrlEncodeByte(std::byte byte)
{
    return std::format("%{:02X}", std::to_integer<int>(byte));
}

std::string BT::DecodeBytesToHex(auto &&byteList)
{
    std::stringstream ss{};
    for (auto const b : byteList)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(b);
    }

    return ss.str();
}

std::vector<std::byte> BT::ConvertHexStringToBytes(const std::string &hex)
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

// https://stackoverflow.com/a/67604259
std::vector<std::byte> BT::EncodeStringAsBytes(std::string_view s)
{
    std::vector<std::byte> bytes;
    bytes.reserve(s.size());
      
   for (const auto& c : s)
        bytes.push_back(static_cast<std::byte>(c));
    
    return bytes;
}

// https://stackoverflow.com/a/67604259
std::vector<std::string> BT::ToHex(std::vector<std::byte> const& v)
{
    std::vector<std::string> bytes;
      
    std::transform(std::begin(v), std::end(v), std::back_inserter(bytes), [](std::byte byte){
        return std::format("{:02X}", std::to_integer<int>(byte));
    });

    return bytes;
}

std::string BT::EncodeBytesAsCharacters(const std::span<const std::byte> bytes) {
    std::string result;
    result.reserve(bytes.size());
    
    for (const auto& byte : bytes)
        result += static_cast<char>(std::to_integer<int>(byte));
    
    return result;
}


void BT::ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch)
                                    { return !std::isspace(ch); }));
}

void BT::rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

void BT::trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// https://stackoverflow.com/a/46931770
std::vector<std::string> BT::split(std::string_view sv, std::string delimiter)
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

std::vector<std::string> BT::split_on_first(std::string_view sv, std::string delimiter)
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

std::string BT::ConvertParametersToQueryString(const std::map<std::string, std::string> &query_parameters)
{
    return query_parameters | std::views::transform([](std::pair<std::string, std::string> pair)
                                                    { return std::format("{}={}", pair.first, pair.second); }) 
                            | std::views::join_with('&')
                            | std::ranges::to<std::string>();
}

std::string BT::IPAddressToString(const std::vector<std::byte>& ipAddressOctets)
{
    return ipAddressOctets
            | std::views::transform([](std::byte b) { return std::to_string(std::to_integer<int>(b)); })
            | std::views::join_with('.')
            | std::ranges::to<std::string>();
}

std::array<std::byte, 4> BT::EncodeIntegerForMessage(unsigned int integer)
{
    return std::array<std::byte, 4> {
        static_cast<std::byte>((integer >> 24) & 0xFF),
        static_cast<std::byte>((integer >> 16) & 0xFF),
        static_cast<std::byte>((integer >> 8) & 0xFF),
        static_cast<std::byte>(integer & 0xFF)
    };
}

long long int BT::DecodeIntegerFromMessage(std::span<std::byte> integerAsBytes)
{
    long long int result = 0;
    result |= std::to_integer<long long int>(integerAsBytes[0]) << 24;
    result |= std::to_integer<long long int >(integerAsBytes[1]) << 16;
    result |= std::to_integer<long long int>(integerAsBytes[2]) << 8;
    result |= std::to_integer<long long int>(integerAsBytes[3]);

    return result;
}