#pragma once

#include "url.hpp"

#include <format>
#include <optional>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>


#include <openssl/ssl.h>

struct HTTPResponse
{
    int StatusCode;
    std::map<std::string, std::string> Headers;
    std::optional<std::string> Body;
};

class HTTPClient
{
public:
    HTTPClient();
    ~HTTPClient();
    // TODO: add timeouts
    HTTPResponse MakeHTTPRequest(Url requestUrl);
    void MakeHTTPSRequest(Url requestUrl);
private:
    SSL_CTX* ctx;
};