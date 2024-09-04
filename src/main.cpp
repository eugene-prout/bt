#include "bencode.hpp"
#include "commands.hpp"
#include "file_parser.hpp"
#include "lyra/lyra.hpp"
#include "utils.hpp"
#include "url.hpp"
#include "tracker.hpp"
#include "tcp_client.hpp"
#include "web_client.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <format>
#include <print>
#include <ranges>
#include <utility>
#include <thread>
#include <chrono>

bool StructUrlEqual(BT::Url struct1, BT::Url struct2)
{
    return struct1.protocol == struct2.protocol && struct1.hostname == struct2.hostname && struct1.port == struct2.port && struct1.path == struct2.path;
}

void AssertEqual(BT::Url url1, BT::Url url2)
{
    if (!StructUrlEqual(url1, url2))
    {
        auto message = std::format("{} does not equal {}", BT::ToString(url1), ToString(url2));
        throw std::runtime_error(message);
    }
}

void TestUrlParsing()
{
    std::vector<std::pair<std::string, BT::Url>> testCases =
        {
            {"http://torrent.ubuntu.com", BT::Url{BT::TrackerProtocol::HTTP, "torrent.ubuntu.com", 80, "/"}},
            {"http://torrent.ubuntu.com/test", BT::Url{BT::TrackerProtocol::HTTP, "torrent.ubuntu.com", 80, "/test"}},
            {"http://localhost:3000", BT::Url{BT::TrackerProtocol::HTTP, "localhost", 3000, "/"}},
            {"http://localhost:3000/test", BT::Url{BT::TrackerProtocol::HTTP, "localhost", 3000, "/test"}}};

    for (const auto &[urlString, expectedResult] : testCases)
    {
        auto result = BT::ParseUrl(urlString);
        AssertEqual(result, expectedResult);
    }

    std::cout << "All good!" << std::endl;
}

struct parse_command 
{
    std::string file_name; 
    bool verbose = false;
    bool show_help = false;

    parse_command(lyra::cli & cli) 
    {
        cli.add_argument(
            lyra::command("parse",
                [this](const lyra::group & g) { this->do_command(g); }) 
                .help("Parses the provided .torrent file.")
                .add_argument(lyra::help(show_help))
                .add_argument(
                    lyra::arg(file_name, "filename")
                        .required()
                        .help("The file path for the .torrent file.")));
    }

    void do_command(const lyra::group & g)
    {
        if (show_help)
            std::cout << g; 
        else
        {
            BT::ParseCommand(file_name);
        }
    }
};

struct download_command 
{
    std::string file_name; 
    bool show_help = false;

    download_command(lyra::cli & cli) 
    {
        cli.add_argument(
            lyra::command("dl",
                [this](const lyra::group & g) { this->do_command(g); }) 
                .help("Downloads the provided .torrent file.")
                .add_argument(lyra::help(show_help))
                .add_argument(
                    lyra::arg(file_name, "filename")
                        .required()
                        .help("The file path for the .torrent file.")));
    }

    void do_command(const lyra::group & g)
    {
        if (show_help)
            std::cout << g; 
        else
        {
            BT::DownloadCommand(file_name);
        }
    }
};


int main(int argc, const char** argv)
{
    bool show_help = false; 

    auto cli = lyra::cli();
    cli.add_argument(lyra::help(show_help));

    parse_command r{cli};
    download_command d{cli};

    auto result = cli.parse({ argc, argv });

    if (!result)
    {
        std::cerr << "Error in command line: " << result.message() << std::endl;
        return 1;
    }

    if (show_help) 
    {
        std::cout << cli << "\n";
        return 0;
    }


    return 0;
}