#pragma once

#include <string>
#include <vector>

class Hasher {
public:
  static std::string GetSHA1HashAsString(const std::string &source);

  static std::vector<std::byte> GetSHA1HashAsBytes(const std::string &source);
};