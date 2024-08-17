#pragma once

#include <string>
#include <vector>

std::string UrlEncodeBytes(const std::vector<std::byte> bytesToConvert);

std::string GetHexFromBytes(auto &&byteList);

std::vector<std::byte> HexStringToBytes(const std::string &hex);

void ltrim(std::string& s);

void rtrim(std::string& s);

void trim(std::string& s);