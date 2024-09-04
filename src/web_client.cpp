#include "url.hpp"
#include "utils.hpp"
#include "web_client.hpp"

#include <format>
#include <optional>
#include <iostream>
#include <memory>
#include <ranges>
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

BT::HTTPClient::HTTPClient()
{
    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL)
    {
        throw std::runtime_error("Failed to create the SSL_CTX.");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (!SSL_CTX_set_default_verify_paths(ctx))
    {
        throw std::runtime_error("Failed to set the default trusted certificate store.");
    }

    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    {
        throw std::runtime_error("Failed to set the minimum TLS protocol version.");
    }
}

BT::HTTPClient::~HTTPClient()
{
    SSL_CTX_free(ctx);
}

// TODO: add timeouts
BT::HTTPResponse BT::HTTPClient::MakeHTTPRequest(Url requestUrl)
{
    addrinfo hints = {};
    addrinfo *result;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    auto port = std::to_string(requestUrl.port);
    auto status = getaddrinfo(requestUrl.hostname.c_str(), port.c_str(), &hints, &result);

    if (status != 0)
    {
        auto error_message = std::format("getaddrinfo error: {}.", gai_strerror(status));
        throw std::runtime_error(error_message);
    }

    auto sockfd = socket(AF_INET, SOCK_STREAM, 0);

    connect(sockfd, result->ai_addr, result->ai_addrlen);

    freeaddrinfo(result);

    auto request = std::format("GET {} HTTP/1.1\r\nHost: {}\r\n\r\n", requestUrl.path, requestUrl.hostname);

    auto bytes_sent = send(sockfd, request.c_str(), request.length(), 0);

    int bytes_read;
    std::vector<char> buf(5000);
    std::stringstream response;

    while ((bytes_read = recv(sockfd, buf.data(), buf.size(), 0)))
    {
        response << std::string(buf.data());
        buf.clear();
        buf.resize(5000);
    }

    close(sockfd);

    std::cout << std::endl
              << response.str() << std::endl;

    return {200, {}, std::move(response.str())};
}

enum class ParsingState
{
    None,
    ParsingHeaders,
    ParsingBody,
    ParsingBodyChunk
};

// TODO: think about how this handles the SSL/TLS records chunking and buffering.
// TODO: protect against this https://owasp.org/www-community/attacks/HTTP_Response_Splitting
BT::HTTPResponse BT::HTTPClient::MakeHTTPSRequest(Url requestUrl)
{
    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL)
    {
        throw std::runtime_error("Failed to create the SSL object.");
    }

    BIO_ADDRINFO *res;
    auto port = std::to_string(requestUrl.port);

    if (!BIO_lookup_ex(requestUrl.hostname.c_str(), port.c_str(), BIO_LOOKUP_CLIENT, AF_INET, SOCK_STREAM, 0, &res))
    {
        throw std::runtime_error("Failed to BIO lookup ex.");
    }

    const BIO_ADDRINFO *ai = NULL;
    auto sock = -1;
    for (ai = res; ai != NULL; ai = BIO_ADDRINFO_next(ai))
    {
        /*
         * Create a TCP socket. We could equally use non-OpenSSL calls such
         * as "socket" here for this and the subsequent connect and close
         * functions. But for portability reasons and also so that we get
         * errors on the OpenSSL stack in the event of a failure we use
         * OpenSSL's versions of these functions.
         */
        sock = BIO_socket(BIO_ADDRINFO_family(ai), SOCK_STREAM, 0, 0);
        if (sock != -1)
        {
            // Connect the socket to the server's address.
            if (BIO_connect(sock, BIO_ADDRINFO_address(ai), BIO_SOCK_NODELAY))
            {
                break;
            }
            else
            {
                BIO_closesocket(sock);
                sock = -1;
            }
        }
    }

    // Free the address information resources we allocated earlier.
    BIO_ADDRINFO_free(res);

    // Create a BIO to wrap the socket
    BIO *bio = BIO_new(BIO_s_socket());
    if (bio == NULL)
    {
        BIO_closesocket(sock);
        throw new std::runtime_error("Failed to create BIO.");
    }

    /*
     * Associate the newly created BIO with the underlying socket. By
     * passing BIO_CLOSE here the socket will be automatically closed when
     * the BIO is freed. Alternatively you can use BIO_NOCLOSE, in which
     * case you must close the socket explicitly when it is no longer
     * needed.
     */
    BIO_set_fd(bio, sock, BIO_CLOSE);

    // associate the bio with ssl so it is freed for us.
    SSL_set_bio(ssl, bio, bio);

    /*
     * Tell the server during the handshake which hostname we are attempting
     * to connect to in case the server supports multiple hosts.
     */
    if (!SSL_set_tlsext_host_name(ssl, requestUrl.hostname.c_str()))
    {
        throw new std::runtime_error("Failed to set the SNI hostname.");
    }

    /*
     * Ensure we check during certificate verification that the server has
     * supplied a certificate for the hostname that we were expecting.
     * Virtually all clients should do this unless you really know what you
     * are doing.
     */
    if (!SSL_set1_host(ssl, requestUrl.hostname.c_str()))
    {
        throw std::runtime_error("Failed to set the certificate verification hostname.");
    }

    // Do the handshake with the server
    if (SSL_connect(ssl) < 1)
    {
        std::string error_message = "Failed to connect to the server.";

        // If the failure is due to a verification error we can get more information about
        // it from SSL_get_verify_result().
        if (SSL_get_verify_result(ssl) != X509_V_OK)
            error_message += std::format("Verify error: {}.", X509_verify_cert_error_string(SSL_get_verify_result(ssl)));

        throw std::runtime_error(error_message);
    }

    auto request_text = std::format("GET {} HTTP/1.1\r\nHost: {}\r\n\r\n", requestUrl.path, requestUrl.hostname);

    // Write an HTTP GET request to the peer
    if (!SSL_write(ssl, request_text.c_str(), request_text.size()))
    {
        throw std::runtime_error("Failed to write HTTP request.");
    }

    size_t readbytes;
    std::vector<char> buf(500);
    std::stringstream string_response;
    HTTPResponse response;

    /*
     * Get up to sizeof(buf) bytes of the response. We keep reading until the
     * server closes the connection.
     */
    while (SSL_read_ex(ssl, buf.data(), buf.size(), &readbytes))
    {
        /*
         * OpenSSL does not guarantee that the returned data is a string or
         * that it is NUL terminated so we use fwrite() to write the exact
         * number of bytes that we read. The data could be non-printable or
         * have NUL characters in the middle of it. For this simple example
         * we're going to print it to stdout anyway.
         */
        // fwrite(buf, 1, readbytes, stdout);
        auto chunk = std::string(buf.begin(), buf.begin() + readbytes);
        string_response << chunk;

        // TODO: implement understanding that the response is chunked + how to process it.
        // Don't handle chunked extensions.
        buf.clear();
        buf.resize(500);
    }

    // std::cout << std::endl << string_response.str() << std::endl;

    auto response_lines = split(string_response.str(), "\r\n");

    auto chunkedStatusLine = split(response_lines.at(0), " ");
    response.StatusCode = std::stoi(chunkedStatusLine.at(1));

    // We expect the response headers first.
    auto state = ParsingState::ParsingHeaders;

    bool response_chunked;
    std::map<std::string, std::string> headers = {};

    int current_chunk_size = -1;
    std::stringstream response_body;

    for (auto line : response_lines | std::views::drop(1))
    {
        if (state == ParsingState::ParsingHeaders)
        {
            if (line == "")
            {
                // End of headers.
                if (headers.contains("Transfer-Encoding"))
                {
                    if (headers.at("Transfer-Encoding") == "chunked")
                    {
                        response_chunked = true;
                        state = ParsingState::ParsingBodyChunk;
                    }
                    else
                    {
                        throw std::runtime_error("Unsupported Transfer-Encoding scheme.");
                    }
                }
                else
                {
                    state = ParsingState::ParsingBody;
                }
                response.Headers = headers;
                
            }
            else
            {
               auto split_line = split_on_first(line, ":");
                std::for_each(split_line.begin(), split_line.end(), [](std::string &s)
                            { trim(s); });
                headers[split_line.at(0)] = split_line.at(1);
            }
        }
        else if (state == ParsingState::ParsingBodyChunk)
        {
            if (current_chunk_size == -1)
            {
                current_chunk_size = std::stoi(line, nullptr, 16);
            }
            else if (current_chunk_size == 0 && line == "")
            {
                // End of chunks
                state = ParsingState::None;
            }
            else
            {
                response_body << line;
                current_chunk_size = -1;
            }
        }
        else if (state == ParsingState::ParsingBody)
        {
            response_body << line;
        }
    }

    response.Body = response_body.str();
    /*
     * Check whether we finished the while loop above normally or as the
     * result of an error. The 0 argument to SSL_get_error() is the return
     * code we received from the SSL_read_ex() call. It must be 0 in order
     * to get here. Normal completion is indicated by SSL_ERROR_ZERO_RETURN.
     */
    if (SSL_get_error(ssl, 0) != SSL_ERROR_ZERO_RETURN)
    {
        // Some error occurred other than a graceful close down by the peer
        throw std::runtime_error("Failed reading remaining data.");
    }

    /*
     * The peer already shutdown gracefully (we know this because of the
     * SSL_ERROR_ZERO_RETURN above). We should do the same back.
     */
    if (SSL_shutdown(ssl) < 1)
    {
        /*
         * ret < 0 indicates an error. ret == 0 would be unexpected here
         * because that means "we've sent a close_notify and we're waiting
         * for one back". But we already know we got one from the peer
         * because of the SSL_ERROR_ZERO_RETURN above.
         */
        throw std::runtime_error("Error shutting down.");
    }

    /*
     * Free the resources we allocated. We do not free the BIO object here
     * because ownership of it was immediately transferred to the SSL object
     * via SSL_set_bio(). The BIO will be freed when we free the SSL object.
     */
    SSL_free(ssl);

    return response;
}