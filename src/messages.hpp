#pragma once

#include "piece.hpp"

#include <map>
#include <string>
#include <vector>

namespace BT
{
    enum class MessageType
    {
        KeepAlive = -1,
        Choke = 0,
        Unchoke = 1,
        Interested = 2,
        NotInterested = 3,
        Have = 4,
        Bitfield = 5,
        Request = 6,
        Piece = 7,
        Cancel = 8,
        Port = 9
    };


    class BitTorrentMessage
    {
    public:
        BitTorrentMessage(MessageType type, int length, std::vector<std::byte> content);
        MessageType Type;
        uint32_t Length;
        std::vector<std::byte> Content;

        std::string SerialiseMessage();
    };

    class MessageFactory
    {
    public:
        static BitTorrentMessage GetInterestedMessage();
        static BitTorrentMessage GetRequestMessage(Block& block);
    };

    const std::map<MessageType, std::string> MessageTypeAsString =
    {
        {MessageType::KeepAlive, "KeepAlive"},
        {MessageType::Choke, "Choke"},
        {MessageType::Unchoke, "Unchoke"},
        {MessageType::Interested, "Interested"},
        {MessageType::NotInterested, "NotInterested"},
        {MessageType::Have, "Have"},
        {MessageType::Bitfield, "Bitfield"},
        {MessageType::Request, "Request"},
        {MessageType::Piece, "Piece"},
        {MessageType::Cancel, "Cancel"},
        {MessageType::Port, "Port"}
    };
}