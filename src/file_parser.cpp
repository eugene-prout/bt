#include "torrent_file.cpp"
#include "bencode.hpp"

#include <memory>
#include <algorithm>
#include <ranges>

class FileParser
{
public:
    std::unique_ptr<TorrentFile> ParseFile(std::string contents)
    {
        auto data = bencode::decode(contents);
        auto announceUrl = std::get<bencode::string>(data["announce"]);

        auto infoDict = std::get<bencode::dict>(data["info"]);
        auto length = std::get<bencode::integer>(infoDict["length"]);
        auto name = std::get<bencode::string>(infoDict["name"]);
        auto pieceLength = std::get<bencode::integer>(infoDict["piece length"]);

        auto pieceList = std::get<bencode::string>(infoDict["pieces"]);
        auto pieces = pieceList | std::views::transform([](char c)
                                                        { return std::byte(c); }) |
                      std::views::chunk(20) | std::ranges::to<std::vector<std::vector<std::byte>>>();

        return std::make_unique<TorrentFile>(announceUrl, length, name, pieceLength, pieces);
    }
};