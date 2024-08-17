#pragma once

#include "url.hpp"

#include <string>
#include <vector>


class TorrentFile
{
public:
    TorrentFile(Url announceUrl, int length, std::string name, int pieceLengthInBytes, 
                std::vector<std::vector<std::byte>> pieces, std::vector<std::byte> infoHash);

    Url AnnounceUrl;
    int LengthInBytes;
    std::string Name;
    int PieceLengthInBytes;
    std::vector<std::vector<std::byte>> Pieces;
    std::vector<std::byte> InfoHash;
};
