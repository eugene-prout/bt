#include "file_parser.hpp"
#include "bencode.hpp"
#include "hasher.hpp"
#include "torrent_file.hpp"
#include "url.hpp"

#include <ranges>

BT::TorrentFile BT::FileParser::ParseFile(std::string contents, Hasher hasher) {
  auto data = bencode::decode(contents);

  auto announceUrl = std::get<bencode::string>(data["announce"]);
  auto parsedUrl = ParseUrl(std::string_view{announceUrl});

  auto infoDict = std::get<bencode::dict>(data["info"]);
  auto length = std::get<bencode::integer>(infoDict["length"]);
  auto name = std::get<bencode::string>(infoDict["name"]);
  auto pieceLength = std::get<bencode::integer>(infoDict["piece length"]);

  auto pieceList = std::get<bencode::string>(infoDict["pieces"]);
  auto pieces = pieceList | std::views::transform([](char c) { return std::byte(c); }) | std::views::chunk(20) |
                std::ranges::to<std::vector<std::vector<std::byte>>>();

  auto hashedInfoDict = hasher.GetSHA1HashAsBytes(bencode::encode(infoDict));

  return TorrentFile{parsedUrl, length, name, pieceLength, pieces, hashedInfoDict};
}