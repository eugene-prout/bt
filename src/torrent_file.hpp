#pragma once

#include "piece.hpp"
#include "url.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace BT {
class TorrentFile {
public:
  TorrentFile(Url announceUrl, long long int length, std::string name, long long int pieceLengthInBytes,
              std::vector<std::vector<std::byte>> pieceHashes, std::vector<std::byte> infoHash);

  Url AnnounceUrl;
  long long int LengthInBytes;
  std::string Name;
  long long int PieceLengthInBytes;
  std::vector<std::vector<std::byte>> PieceHashes;
  std::vector<std::byte> InfoHash;

  void PrintDetails() const;
  std::vector<Piece> GetPieces();
};
} // namespace BT
