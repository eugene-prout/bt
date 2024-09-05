#pragma once

#include <cstddef>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace BT {
std::string UrlEncodeBytes(const std::span<const std::byte> bytesToConvert);

std::string UrlEncodeByte(const std::byte byte);

std::vector<std::byte> EncodeStringAsBytes(const std::string_view s);

std::vector<std::string> ToHex(const std::span<const std::byte> v);

std::string EncodeBytesAsCharacters(const std::span<const std::byte> bytes);

void TrimInPlace(std::string &s);

std::string Trim(const std::string &s);

std::vector<std::string> Split(const std::string_view sv, const std::string &delimiter);

std::pair<std::string, std::string> SplitOnFirst(const std::string_view sv, const std::string &delimiter);

std::string IPAddressToString(const std::span<const std::byte> ipAddressOctets);

std::array<std::byte, 4> EncodeIntegerForMessage(const unsigned int integer);

long long int DecodeIntegerFromMessage(const std::span<const std::byte> integerAsBytes);
} // namespace BT