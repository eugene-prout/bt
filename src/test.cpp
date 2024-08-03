#include "file_parser.cpp"
#include "utils.cpp"
#include <fstream>
#include <iomanip>
#include <format>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include <openssl/ssl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

std::string getHexFromBytes(auto &&byteList)
{
    std::stringstream ss{};
    for (auto const b : byteList)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(b);
    }

    return ss.str();
}

void PrintData()
{
    std::ifstream infile{"/home/eugene/programming/bt/data/ubuntu-24.04-desktop-amd64.iso.torrent"};

    std::stringstream buffer;
    buffer << infile.rdbuf();

    FileParser p{};

    Hasher hasher{};
    auto f = p.ParseFile(buffer.str(), hasher);

    std::cout << f->AnnounceUrl << std::endl;
    // std::cout << f->InfoHash << std::endl;
    std::cout << f->LengthInBytes << std::endl;
}

void httpTest()
{

    // SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    // if (ctx == NULL)
    // {
    //     printf("Failed to create the SSL_CTX\n");
    //     exit(-1);
    // }

    // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    // if (!SSL_CTX_set_default_verify_paths(ctx))
    // {
    //     printf("Failed to set the default trusted certificate store\n");
    //     exit(-1);
    // }

    // if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    // {
    //     printf("Failed to set the minimum TLS protocol version\n");
    //     exit(-1);
    // }

    // // Finished setting up SSL context.

    // SSL *ssl = SSL_new(ctx);
    // if (ssl == NULL)
    // {
    //     printf("Failed to create the SSL object\n");
    //     exit(-1);
    // }

    // struct addrinfo hints, *result;

    // memset(&hints, 0, sizeof hints);
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;

    // std::string webAddress{"torrent.ubuntu.com"};
    // int status;
    // if ((status = getaddrinfo(webAddress.c_str(), "443", &hints, &result)) != 0)
    // {
    //     fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    //     exit(1);
    // }
    // int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // // connect(sockfd, result->ai_addr, result->ai_addrlen);

    // std::stringstream url;
    // std::string info_hash{R"(%2a%a4%f5%a7%e2%09%e5%4b%32%80%3d%43%67%09%71%c4%c8%ca%aa%05)"};

    // url << "/announce?info_hash=";
    // url << info_hash;
    // url << "&peer_id=aaaaaaaaaaaaaaaaaaaa&port=6881&uploaded=0&downloaded=0&left=12345&compact=1";

    // std::stringstream request;
    // request << "GET ";
    // request << url.str();
    // request << " HTTP/1.1\r\nHost: test.com\r\n\r\n";

    // std::string line1 = request.str();

    // std::cout << line1 << std::endl;
    // int bytes_sent = SSL_write(conn, line1.c_str(), line1.length());
    // std::cout << std::format("Bytes sent: {}", bytes_sent) << std::endl;

    // int bytes_read;
    // std::vector<char> buf(5000);
    // while (bytes_read = SSL_read(conn, buf.data(), buf.size()))
    // {
    //     std::cout << std::format("Bytes read: {}", bytes_read) << std::endl;
    //     std::cout << std::format("Message read: {}", buf.data()) << std::endl;

    //     buf.clear();
    //     buf.resize(5000);
    // }

    // close(sockfd);
}

void httpsTest()
{
    // Create SSL context.

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL)
    {
        printf("Failed to create the SSL_CTX\n");
        exit(-1);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if (!SSL_CTX_set_default_verify_paths(ctx))
    {
        printf("Failed to set the default trusted certificate store\n");
        exit(-1);
    }

    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION))
    {
        printf("Failed to set the minimum TLS protocol version\n");
        exit(-1);
    }

    // Finished setting up SSL context.

    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL)
    {
        printf("Failed to create the SSL object\n");
        exit(-1);
    }

    int sock = -1;
    BIO_ADDRINFO *res;
    const BIO_ADDRINFO *ai = NULL;

    if (!BIO_lookup_ex("torrent.ubuntu.com", "443", BIO_LOOKUP_CLIENT, AF_INET, SOCK_STREAM, 0, &res))
    {
        printf("Failed to BIO lookup ex\n");
        exit(-1);
    }

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
        if (sock == -1)
            continue;

        /* Connect the socket to the server's address */
        if (!BIO_connect(sock, BIO_ADDRINFO_address(ai), BIO_SOCK_NODELAY))
        {
            BIO_closesocket(sock);
            sock = -1;
            continue;
        }

        /* We have a connected socket so break out of the loop */
        break;
    }

    BIO_ADDRINFO_free(res);

    BIO *bio;

    /* Create a BIO to wrap the socket */
    bio = BIO_new(BIO_s_socket());
    if (bio == NULL)
    {
        BIO_closesocket(sock);
        printf("Failed to create BIO\n");
        exit(-1);
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
    if (!SSL_set_tlsext_host_name(ssl, "torrent.ubuntu.com"))
    {
        printf("Failed to set the SNI hostname\n");
    }

    /*
     * Ensure we check during certificate verification that the server has
     * supplied a certificate for the hostname that we were expecting.
     * Virtually all clients should do this unless you really know what you
     * are doing.
     */
    if (!SSL_set1_host(ssl, "torrent.ubuntu.com"))
    {
        printf("Failed to set the certificate verification hostname");
        exit(-1);
    }

    /* Do the handshake with the server */
    if (SSL_connect(ssl) < 1)
    {
        printf("Failed to connect to the server\n");
        /*
         * If the failure is due to a verification error we can get more
         * information about it from SSL_get_verify_result().
         */
        if (SSL_get_verify_result(ssl) != X509_V_OK)
            printf("Verify error: %s\n", X509_verify_cert_error_string(SSL_get_verify_result(ssl)));
        exit(-1);
    }

    size_t written;
    // const char *request_start = "GET / HTTP/1.0\r\nConnection: close\r\nHost: ";
    const char *request_start = "GET /announce?info_hash=%2a%a4%f5%a7%e2%09%e5%4b%32%80%3d%43%67%09%71%c4%c8%ca%aa%05&peer_id=aaaaaaaaaaaaaaaaaaaa&port=6881&uploaded=0&downloaded=0&left=12345&compact=1 HTTP/1.0\r\nConnection: close\r\nHost: ";
    const char *request_end = "\r\n\r\n";

    /* Write an HTTP GET request to the peer */
    if (!SSL_write_ex(ssl, request_start, strlen(request_start), &written))
    {
        printf("Failed to write start of HTTP request\n");
        exit(-1);
    }
    std::string hostname{"test.com"};
    if (!SSL_write_ex(ssl, hostname.c_str(), hostname.size(), &written))
    {
        printf("Failed to write hostname in HTTP request\n");
        exit(-1);
    }
    if (!SSL_write_ex(ssl, request_end, strlen(request_end), &written))
    {
        printf("Failed to write end of HTTP request\n");
        exit(-1);
    }

    size_t readbytes;
    char buf[160];

    /*
     * Get up to sizeof(buf) bytes of the response. We keep reading until the
     * server closes the connection.
     */
    while (SSL_read_ex(ssl, buf, sizeof(buf), &readbytes))
    {
        /*
         * OpenSSL does not guarantee that the returned data is a string or
         * that it is NUL terminated so we use fwrite() to write the exact
         * number of bytes that we read. The data could be non-printable or
         * have NUL characters in the middle of it. For this simple example
         * we're going to print it to stdout anyway.
         */
        fwrite(buf, 1, readbytes, stdout);
    }
    /* In case the response didn't finish with a newline we add one now */
    printf("\n");

    /*
     * Check whether we finished the while loop above normally or as the
     * result of an error. The 0 argument to SSL_get_error() is the return
     * code we received from the SSL_read_ex() call. It must be 0 in order
     * to get here. Normal completion is indicated by SSL_ERROR_ZERO_RETURN.
     */
    if (SSL_get_error(ssl, 0) != SSL_ERROR_ZERO_RETURN)
    {
        /*
         * Some error occurred other than a graceful close down by the
         * peer
         */
        printf("Failed reading remaining data\n");
        exit(-1);
    }

    /*
     * The peer already shutdown gracefully (we know this because of the
     * SSL_ERROR_ZERO_RETURN above). We should do the same back.
     */
    int ret = SSL_shutdown(ssl);
    if (ret < 1)
    {
        /*
         * ret < 0 indicates an error. ret == 0 would be unexpected here
         * because that means "we've sent a close_notify and we're waiting
         * for one back". But we already know we got one from the peer
         * because of the SSL_ERROR_ZERO_RETURN above.
         */
        printf("Error shutting down\n");
        exit(-1);
    }

    /*
     * Free the resources we allocated. We do not free the BIO object here
     * because ownership of it was immediately transferred to the SSL object
     * via SSL_set_bio(). The BIO will be freed when we free the SSL object.
     */
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    
    // std::stringstream url;
    // std::string info_hash{R"(%2a%a4%f5%a7%e2%09%e5%4b%32%80%3d%43%67%09%71%c4%c8%ca%aa%05)"};

    // url << "/announce?info_hash=";
    // url << info_hash;
    // url << "&peer_id=aaaaaaaaaaaaaaaaaaaa&port=6881&uploaded=0&downloaded=0&left=12345&compact=1";

    // std::stringstream request;
    // request << "GET ";
    // request << url.str();
    // request << " HTTP/1.1\r\nHost: test.com\r\n\r\n";

    // std::string line1 = request.str();

    // std::cout << line1 << std::endl;
    // int bytes_sent = SSL_write(conn, line1.c_str(), line1.length());
    // std::cout << std::format("Bytes sent: {}", bytes_sent) << std::endl;

    // int bytes_read;
    // std::vector<char> buf(5000);
    // while (bytes_read = SSL_read(conn, buf.data(), buf.size()))
    // {
    //     std::cout << std::format("Bytes read: {}", bytes_read) << std::endl;
    //     std::cout << std::format("Message read: {}", buf.data()) << std::endl;

    //     buf.clear();
    //     buf.resize(5000);
    // }

    // close(sockfd);
}

std::vector<std::byte> hexStringToBytes(const std::string &hex)
{
    std::vector<std::byte> bytes;
    bytes.reserve(hex.length() / 2); // Reserve space for efficiency

    for (size_t i = 0; i < hex.length(); i += 2)
    {
        std::byte byte{static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16))};
        bytes.push_back(byte);
    }
    return bytes;
}

int main()
{
    // Hasher hasher{};
    // std::string hash{"2AA4F5A7E209E54B32803D43670971C4C8CAAA05"};
    // std::cout << hash << std::endl;
    // std::cout << UrlEncodeBytes(hexStringToBytes(hash)) << std::endl;
    std::cout << "TEST" << std::endl;
    httpsTest();
}
