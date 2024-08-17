#include "torrent_file.hpp"

TorrentFile::TorrentFile(Url announceUrl, int length, std::string name, int pieceLengthInBytes,
                         std::vector<std::vector<std::byte>> pieces, std::vector<std::byte> infoHash)
    : AnnounceUrl{announceUrl}, LengthInBytes{length}, Name{name}, PieceLengthInBytes{pieceLengthInBytes},
      Pieces{pieces}, InfoHash{infoHash}
{
}