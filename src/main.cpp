#include "commands.hpp"
#include "lyra/lyra.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <format>
#include <print>
#include <ranges>
#include <utility>

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