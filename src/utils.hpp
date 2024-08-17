#pragma once

#include <string>
#include <vector>

std::string UrlEncodeBytes(const std::vector<std::byte> bytesToConvert);

std::string GetHexFromBytes(auto &&byteList);

std::vector<std::byte> HexStringToBytes(const std::string &hex);

void ltrim(std::string& s);

void rtrim(std::string& s);

void trim(std::string& s);

std::vector<std::string> split(std::string_view sv, std::string delimiter);

std::vector<std::string> split_on_first(std::string_view sv, std::string delimiter);