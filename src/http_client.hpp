#pragma once

#include "url.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <openssl/ssl.h>

namespace BT {
struct HTTPResponse {
  int StatusCode;
  std::map<std::string, std::string> Headers;
  std::optional<std::string> Body;
};

class HTTPClient {
public:
  HTTPClient();
  ~HTTPClient();
  // TODO: add timeouts
  HTTPResponse MakeHTTPGetRequest(Url requestUrl);
  HTTPResponse MakeHTTPSGetRequest(Url requestUrl);

private:
  SSL_CTX *ctx;
};
} // namespace BT
