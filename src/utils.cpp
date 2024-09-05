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

std::string BT::UrlEncodeBytes(const std::span<const std::byte> bytesToConvert)
{
    std::string encodedBytes;
    encodedBytes.reserve(bytesToConvert.size() * 3);

    for (auto b : bytesToConvert)
        encodedBytes.append(UrlEncodeByte(b));

    return encodedBytes;
}

std::string BT::UrlEncodeByte(const std::byte byte)
{
    return std::format("%{:02X}", std::to_integer<int>(byte));
}

// https://stackoverflow.com/a/67604259
std::vector<std::byte> BT::EncodeStringAsBytes(const std::string_view s)
{
    std::vector<std::byte> bytes;
    bytes.reserve(s.size());
      
   for (const auto& c : s)
        bytes.push_back(static_cast<std::byte>(c));
    
    return bytes;
}

// https://stackoverflow.com/a/67604259
std::vector<std::string> BT::ToHex(const std::span<const std::byte> v)
{
    std::vector<std::string> bytes;
    bytes.reserve(v.size());
      
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


void ltrim(std::string &s)
{
    auto start_of_text = std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); });
    s.erase(s.begin(), start_of_text);
}

void rtrim(std::string &s)
{
    auto end_of_text = std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base();
    s.erase(end_of_text, s.end());
}

void BT::TrimInPlace(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

std::string BT::Trim(const std::string &s)
{
    auto output_string = std::string{s};
    rtrim(output_string);
    ltrim(output_string);
    return output_string;
}

// https://stackoverflow.com/a/46931770
std::vector<std::string> BT::Split(const std::string_view sv, const std::string& delimiter)
{
    std::size_t pos_start = 0;
    std::size_t pos_end;

    std::size_t delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = sv.find(delimiter, pos_start)) != std::string::npos)
    {
        res.emplace_back(sv.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
    }

    res.emplace_back(sv.substr(pos_start));

    return res;
}

std::tuple<std::string, std::string> BT::SplitOnFirst(const std::string_view sv, const std::string& delimiter)
{
    auto delimiter_length = delimiter.length();
    auto delimiter_position = sv.find(delimiter, 0);

    if (delimiter_position == std::string::npos)
        throw std::runtime_error("String does not contain delimiter.");

    auto second_half = delimiter_position + delimiter_length;

    return { std::string(sv.substr(0, delimiter_position)), std::string(sv.substr(second_half)) };
}

std::string BT::IPAddressToString(const std::span<const std::byte> ipAddressOctets)
{
    return ipAddressOctets
            | std::views::transform([](std::byte b) { return std::to_string(std::to_integer<int>(b)); })
            | std::views::join_with('.')
            | std::ranges::to<std::string>();
}

std::array<std::byte, 4> BT::EncodeIntegerForMessage(const unsigned int integer)
{
    return std::array<std::byte, 4> {
        static_cast<std::byte>((integer >> 24) & 0xFF),
        static_cast<std::byte>((integer >> 16) & 0xFF),
        static_cast<std::byte>((integer >> 8) & 0xFF),
        static_cast<std::byte>(integer & 0xFF)
    };
}

long long int BT::DecodeIntegerFromMessage(const std::span<const std::byte> integerAsBytes)
{
    long long int result = 0;
    result |= std::to_integer<long long int>(integerAsBytes[0]) << 24;
    result |= std::to_integer<long long int >(integerAsBytes[1]) << 16;
    result |= std::to_integer<long long int>(integerAsBytes[2]) << 8;
    result |= std::to_integer<long long int>(integerAsBytes[3]);

    return result;
}