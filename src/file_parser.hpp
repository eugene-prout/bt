#pragma once

#include "torrent_file.hpp"
#include "hasher.hpp"

#include <memory>
#include <algorithm>

class FileParser
{
public:
    std::unique_ptr<TorrentFile> ParseFile(std::string contents, Hasher hasher);
};