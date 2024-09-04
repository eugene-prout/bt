#pragma once

#include "piece.hpp"
#include "tracker.hpp"
#include "messages.hpp"
#include "tcp_client.hpp"

namespace BT
{
    class Peer
    {
    public:
        Peer(TCPConnectionFactory& connectionFactory, PeerDetails& details, const std::vector<std::byte>& infoHash);
        void Download(std::vector<Piece>& pieces);
    private:
        TCPConnection connection;
    };

    void SendMessage(TCPConnection &connection, BitTorrentMessage bitTorrentMessage);
    BitTorrentMessage ReadMessage(TCPConnection &connection);
}
