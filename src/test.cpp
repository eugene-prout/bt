#include "file_parser.hpp"
#include "utils.hpp"

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
    return struct1.protocol == struct2.protocol
            && struct1.hostname == struct2.hostname
            && struct1.port == struct2.port
            && struct1.path == struct2.path;
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
        {"http://localhost:3000/test", Url{TrackerProtocol::HTTP, "localhost", 3000, "/test"}}
    };

    for (const auto& [urlString, expectedResult] : testCases)
    {
        auto result = ParseUrl(urlString);
        AssertEqual(result, expectedResult);
    }

    std::cout << "All good!" << std::endl;
}
    
std::vector<std::string> SplitOnDelimiter(std::string_view inputString, char delimiter)
{
    return inputString
            | std::views::split(delimiter)
            | std::ranges::to<std::vector<std::string>>();
}



int main()
{
    // Hasher hasher{};
    // std::string hash{"2AA4F5A7E209E54B32803D43670971C4C8CAAA05"};
    // std::cout << hash << std::endl;
    // std::cout << UrlEncodeBytes(hexStringToBytes(hash)) << std::endl;
    // std::cout << "TEST" << std::endl;
    // PrintData();
    // TestUrlParsing();
    // httpsTest();
    auto url = "https://www.google.com";
    auto httpClient = HTTPClient();
    httpClient.MakeHTTPSRequest(ParseUrl(url));

    // auto str = std::string("HTTP/1.0 200 OK\r\nServer: BaseHTTP/0.6 Python/3.12.3\r\nDate: Tue, 06 Aug 2024 10:48:50 GMT\r\nContent-type: text/html\r\n\r\nHello, world!");
    // std::erase(str, '\r');
    // auto response_lines = SplitOnDelimiter(str, '\n');

    // std::string status_line;

    // auto chunks = SplitOnDelimiter(response_lines.at(0), ' ');
    // auto responseCode = std::stoi(chunks.at(1));
    
    // auto headers_lines = response_lines
    //                         | std::views::drop(1)
    //                         | std::views::take_while([](std::string line) { return line != ""; });

    // std::map<std::string, std::string> headers = {};
    // for (auto& line : headers_lines)
    // {
    //     // This splits on all colons. If there are multiple colons in the string it will be truncated.
    //     auto split_line = SplitOnDelimiter(line, ':');
    //     if (split_line.size() != 2)
    //     {
    //         // Log error
    //     }

    //     std::for_each(split_line.begin(), split_line.end(), [](std::string& s) { trim(s);});
        
    //     headers[split_line.at(0)] = split_line.at(1);
    // }

}