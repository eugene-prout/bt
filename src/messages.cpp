#include "messages.hpp"
#include "piece.hpp"
#include "utils.hpp"

#include <string>
#include <utility>
#include <vector>

BT::BitTorrentMessage::BitTorrentMessage(MessageType type, int length, std::vector<std::byte> content)
    : Type(type), Length(length), Content(std::move(content)) {}

std::string BT::BitTorrentMessage::SerialiseMessage() {
  std::string serialisedMessage;

  auto lengthBytes = EncodeIntegerForMessage(Length);

  for (auto &b : lengthBytes)
    serialisedMessage += static_cast<char>(b);

  serialisedMessage += static_cast<char>(std::to_underlying(Type));

  for (auto &b : Content)
    serialisedMessage += static_cast<char>(std::to_integer<int>(b));

  return serialisedMessage;
}

BT::BitTorrentMessage BT::MessageFactory::GetInterestedMessage() { return {MessageType::Interested, 1, {}}; }

BT::BitTorrentMessage BT::MessageFactory::GetRequestMessage(Block &block) {
  std::vector<std::byte> body;
  body.reserve(12);

  auto pieceIndexEncoded = EncodeIntegerForMessage(block.GetPieceIndex());
  body.insert(body.end(), pieceIndexEncoded.begin(), pieceIndexEncoded.end());

  auto offsetEncoded = EncodeIntegerForMessage(block.GetByteOffsetWithinPiece());
  body.insert(body.end(), offsetEncoded.begin(), offsetEncoded.end());

  auto blockLengthEncoded = EncodeIntegerForMessage(block.GetSize());
  body.insert(body.end(), blockLengthEncoded.begin(), blockLengthEncoded.end());

  return {MessageType::Request, 13, body};
}