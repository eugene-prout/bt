#include <string>
#include <vector>

class TorrentFile
{
public:
    TorrentFile(std::string announceUrl, int length, std::string name, int pieceLengthInBytes, std::vector<std::vector<std::byte>> pieces)
        : AnnounceUrl{announceUrl}, LengthInBytes{length}, Name{name}, PieceLengthInBytes{pieceLengthInBytes},
          Pieces{pieces}
    {
    }
    
    std::string AnnounceUrl;
    int LengthInBytes;
    std::string Name;
    int PieceLengthInBytes;
    std::vector<std::vector<std::byte>> Pieces;
};