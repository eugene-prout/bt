#pragma once

#include "hasher.hpp"
#include "torrent_file.hpp"

namespace BT {
class FileParser {
public:
  TorrentFile ParseFile(std::string contents, Hasher hasher);
};
} // namespace BT
