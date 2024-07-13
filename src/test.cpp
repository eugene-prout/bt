#include "file_parser.cpp"

#include <fstream>
#include <iomanip>
#include <openssl/sha.h>

std::string getHexFromBytes(auto&& byteList)
{
    std::stringstream ss{};
    for (auto const b : byteList) 
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(b);
    }

    return ss.str();
}

int main()
{
    std::ifstream infile{"ubuntu-24.04-desktop-amd64.iso.torrent"};

    std::stringstream buffer;
    buffer << infile.rdbuf();

    FileParser p{};

    auto f = p.ParseFile(buffer.str());
    
    std::cout << f->Name << std::endl;
    return 0;
}