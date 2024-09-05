#pragma once

#include "messages.hpp"
#include "piece.hpp"
#include "tcp_client.hpp"
#include "tracker.hpp"

#include <cstddef>
#include <vector>

namespace BT {
class Peer {
public:
  Peer(TCPConnectionFactory &connectionFactory, PeerDetails &details, const std::vector<std::byte> &infoHash);
  void Download(std::vector<Piece> &pieces);

private:
  TCPConnection connection;
};

void SendMessage(TCPConnection &connection, BitTorrentMessage bitTorrentMessage);
BitTorrentMessage ReadMessage(TCPConnection &connection);
} // namespace BT
