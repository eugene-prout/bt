#pragma once

#include "torrent_file.hpp"
#include "hasher.hpp"

namespace BT
{
    class FileParser
    {
    public:
        TorrentFile ParseFile(std::string contents, Hasher hasher);
    };
}
