#pragma once

#include "tcp_client.hpp"
#include "torrent_file.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <print>
#include <ranges>
#include <utility>
#include <vector>

namespace BT {
struct PeerDetails {
  std::vector<std::byte> ipAddress;
  int port;
};

struct TrackerResponse {
  int interval;
  std::vector<PeerDetails> peers;
};

struct HandshakeMessage {
  std::byte ProtocolStringLength = std::byte{19};
  std::string ProtocolString = "BitTorrent protocol";
  std::array<std::byte, 8> ExtensionBytes = {};
  std::vector<std::byte> InfoHash;
  std::string PeerId;
};

std::string SerialiseHandshake(HandshakeMessage message);

HandshakeMessage DerialiseHandshake(std::string message);

TrackerResponse QueryTrackerForPeers(TorrentFile &torrentFile);

bool HandshakeWithPeer(TCPConnection &connection, const std::vector<std::byte> &infoHash);
} // namespace BT
