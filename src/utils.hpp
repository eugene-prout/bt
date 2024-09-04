#pragma once

#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace BT
{
    std::string UrlEncodeBytes(const std::vector<std::byte>& bytesToConvert);

    std::string UrlEncodeByte(std::byte byte);

    std::string UrlEncodeBytes(const std::array<std::byte, 8>& bytesToConvert);

    std::vector<std::byte> EncodeStringAsBytes(std::string_view s);

    std::vector<std::string> ToHex(std::vector<std::byte> const& v);

    std::string EncodeBytesAsCharacters(const std::span<const std::byte> bytes);

    std::string DecodeBytesToHex(auto &&byteList);

    std::vector<std::byte> ConvertHexStringToBytes(const std::string &hex);

    void ltrim(std::string& s);

    void rtrim(std::string& s);

    void trim(std::string& s);

    std::vector<std::string> split(std::string_view sv, std::string delimiter);

    std::vector<std::string> split_on_first(std::string_view sv, std::string delimiter);

    std::string ConvertParametersToQueryString(const std::map<std::string, std::string> &query_parameters);

    std::string IPAddressToString(const std::vector<std::byte>& ipAddressOctets);

    std::array<std::byte, 4> EncodeIntegerForMessage(unsigned int integer);

    long long int DecodeIntegerFromMessage(std::span<std::byte> integerAsBytes);
}