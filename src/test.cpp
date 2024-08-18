#include "bencode.hpp"
#include "file_parser.hpp"
#include "utils.hpp"
#include "url.hpp"
#include "web_client.cpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <format>
#include <print>
#include <ranges>
#include <utility>

void PrintData()
{
    std::ifstream infile{"/home/eugene/programming/bt/data/ubuntu-24.04-desktop-amd64.iso.torrent"};

    std::stringstream buffer;
    buffer << infile.rdbuf();

    FileParser p{};

    Hasher hasher{};
    auto f = p.ParseFile(buffer.str(), hasher);

    std::cout << TrackerProtocols.at(f->AnnounceUrl.protocol) << std::endl;
    std::cout << f->AnnounceUrl.hostname << std::endl;
    std::cout << f->AnnounceUrl.path << std::endl;
    // std::cout << f->InfoHash << std::endl;
    // std::cout << f->LengthInBytes << std::endl;
}

bool StructUrlEqual(Url struct1, Url struct2)
{
    return struct1.protocol == struct2.protocol && struct1.hostname == struct2.hostname && struct1.port == struct2.port && struct1.path == struct2.path;
}

void AssertEqual(Url url1, Url url2)
{
    if (!StructUrlEqual(url1, url2))
    {
        auto message = std::format("{} does not equal {}", ToString(url1), ToString(url2));
        throw std::runtime_error(message);
    }
}

void TestUrlParsing()
{
    std::vector<std::pair<std::string, Url>> testCases =
        {
            {"http://torrent.ubuntu.com", Url{TrackerProtocol::HTTP, "torrent.ubuntu.com", 80, "/"}},
            {"http://torrent.ubuntu.com/test", Url{TrackerProtocol::HTTP, "torrent.ubuntu.com", 80, "/test"}},
            {"http://localhost:3000", Url{TrackerProtocol::HTTP, "localhost", 3000, "/"}},
            {"http://localhost:3000/test", Url{TrackerProtocol::HTTP, "localhost", 3000, "/test"}}};

    for (const auto &[urlString, expectedResult] : testCases)
    {
        auto result = ParseUrl(urlString);
        AssertEqual(result, expectedResult);
    }

    std::cout << "All good!" << std::endl;
}

struct PeerConnection
{
    std::vector<std::byte> ipAddress;
    int port;
};


struct TrackerResponse
{
    int interval;
    std::vector<PeerConnection> peers;
};

std::string ipAddressToString(std::vector<std::byte> ipAddressOctets)
{
    return ipAddressOctets
            | std::views::transform([](std::byte b) { return std::to_string(std::to_integer<int>(b)); })
            | std::views::join_with('.')
            | std::ranges::to<std::string>();
}

int main()
{
    std::ifstream infile{"/home/eugene/programming/bt/data/ubuntu-24.04-desktop-amd64.iso.torrent"};

    std::stringstream buffer;
    buffer << infile.rdbuf();

    FileParser p{};

    Hasher hasher{};

    auto f = p.ParseFile(buffer.str(), hasher);

    std::map<std::string, std::string> query_parameters = {
        {"info_hash", UrlEncodeBytes(f->InfoHash)},
        {"peer_id", "aaaaaaaaaaaaaaaaaaaa"},
        {"port", "6881"},
        {"uploaded", "0"},
        {"downloaded", "0"},
        {"left", "12345"},
        {"compact", "1"}
    };
    
    auto path = std::format("{}?{}", f->AnnounceUrl.path, ConvertParametersToQueryString(query_parameters));

    auto url_struct = Url(f->AnnounceUrl.protocol, f->AnnounceUrl.hostname, f->AnnounceUrl.port, path);

    auto httpClient = HTTPClient();
    auto r = httpClient.MakeHTTPSRequest(url_struct);
    auto data = bencode::decode(r.Body.value());

    auto value = std::get<bencode::dict>(data);
    auto interval = std::get<bencode::integer>(data["interval"]);
    auto peers_present = value.find("peers") != value.end(); 
    if (peers_present)
    {
        auto peers = std::get<bencode::string>(data["peers"]);
        auto byteGroups = peers | std::views::transform([](char c) { return std::byte(c); })
                                | std::views::chunk(6);

        for (auto address_range : byteGroups)
        {
            auto ipAddress = address_range | std::views::take(4) | std::ranges::to<std::vector<std::byte>>();;
            std::cout << ipAddressToString(ipAddress) << std::endl;

            auto port = address_range | std::views::drop(4) | std::ranges::to<std::vector<std::byte>>();
            auto portValue = (port[0] << 8) | port[1];
            std::cout << std::to_string(std::to_integer<int>(portValue)) << std::endl;
        }
    }
    else
    {
        std::cout << "No peers present." << std::endl;
    }
}