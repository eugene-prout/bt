#include <string>
#include <vector>

class TorrentFile
{
public:
    TorrentFile(std::string announceUrl, int length, std::string name, int pieceLengthInBytes, std::vector<std::vector<std::byte>> pieces, std::vector<std::byte> infoHash)
        : AnnounceUrl{announceUrl}, LengthInBytes{length}, Name{name}, PieceLengthInBytes{pieceLengthInBytes},
          Pieces{pieces}, InfoHash{infoHash}
    {
    }
    
    std::string AnnounceUrl;
    int LengthInBytes;
    std::string Name;
    int PieceLengthInBytes;
    std::vector<std::vector<std::byte>> Pieces;
    std::vector<std::byte> InfoHash;
};