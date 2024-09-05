#include "peer.hpp"
#include "messages.hpp"
#include "tracker.hpp"
#include "tcp_client.hpp"
#include "utils.hpp"

#include <chrono>
#include <unordered_set>
#include <format>
#include <queue>
#include <thread>
#include <vector>

void BT::SendMessage(TCPConnection &connection, BitTorrentMessage bitTorrentMessage)
{
    std::cout << std::format("Sending '{}' message.", MessageTypeAsString.at(bitTorrentMessage.Type)) << std::endl;
    auto message = bitTorrentMessage.SerialiseMessage();
    connection.SendMessage(message);
}

BT::BitTorrentMessage BT::ReadMessage(TCPConnection &connection)
{
    auto messageSize = connection.ReceiveMessage(4);
    std::vector<int> digits(4);
    std::transform(messageSize.begin(), messageSize.end(), digits.begin(), [](char c)
                   { return static_cast<int>(c); });
    auto length = (digits[0] << 24) | (digits[1] << 16) | (digits[2] << 8) | digits[3];

    if (length == 0)
        return BitTorrentMessage(MessageType::KeepAlive, 0, {});

    auto messageIdRaw = connection.ReceiveMessage(1);
    int parsedId = static_cast<int>(messageIdRaw[0]);
    if (parsedId < 0 || parsedId > 9)
        throw std::runtime_error(std::format("Unexpected message id: {}.", parsedId));

    auto amountLeftToRead = length - 1;
    // Maybe use a std::optional for the body?
    std::vector<std::byte> body(amountLeftToRead);
    if (amountLeftToRead != 0)
    {
        auto bodyText = connection.ReceiveMessage(amountLeftToRead);
        std::transform(bodyText.begin(), bodyText.end(), body.begin(), [](unsigned char c)
                       { return std::byte{c}; });
    }

    return BitTorrentMessage{static_cast<MessageType>(parsedId), length, body};
}

BT::Peer::Peer(TCPConnectionFactory &connectionFactory, PeerDetails &details, const std::vector<std::byte> &infoHash)
    : connection(connectionFactory.OpenConnection(IPAddressToString(details.ipAddress), details.port))
{
    if (!HandshakeWithPeer(connection, infoHash))
        throw std::runtime_error("Failed to handshake with peer. Peer responded with unexpected message.");
}

constexpr long long int BLOCK_SIZE = 16384;

void BT::Peer::Download(std::vector<Piece> &pieces)
{
    MessageFactory msf{};

    std::cout << "Opening connection." << std::endl;
    std::vector<long long> seenPieces;
    std::unordered_set<long long> downloadedPieces;
    std::optional<long long> pieceInProgress;
    std::vector<std::byte> downloadedPieceBuffer;
    std::queue<Block> blocksToDownload;

    while (true)
    {
        auto message = ReadMessage(connection);
        std::cout << std::format("Recieved {} message from peer.", MessageTypeAsString.at(message.Type)) << std::endl;

        if (message.Type == MessageType::Unchoke)
        {
            SendMessage(connection, msf.GetInterestedMessage());
            std::cout << "Sent interested message." << std::endl;
        }
        else if (message.Type == MessageType::KeepAlive)
        {
            auto timeout = 10;
            std::cout << std::format("Sleeping for {} seconds.", timeout) << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
        }
        else if (message.Type == MessageType::Piece)
        {
            if (!pieceInProgress.has_value())
                throw std::runtime_error("Unexpected piece message. No piece download in progress.");

            // verify that piece index is correct
            std::span<std::byte> data{message.Content};


            auto indexBytes = data.subspan(0, 4);
            auto parsedIndex = DecodeIntegerFromMessage(indexBytes);
            if (parsedIndex != pieceInProgress)
                throw std::runtime_error("Piece message's index does not match pieceInProgress.");

            auto beginBytes = data.subspan(4, 4);
            auto parsedBegin = DecodeIntegerFromMessage(beginBytes);

            // TODO: assert that downloadedPieceBuffer.size() == message.Length - 9
            std::copy(message.Content.begin() + 8, message.Content.end(), downloadedPieceBuffer.begin() + parsedBegin);

            if (blocksToDownload.empty())
            {
                std::cout << "Downloaded all blocks." << std::endl;

                // compute hash of block buffer
            }
            else
            {
                std::cout << std::format("Requesting next block. {} remaining.", blocksToDownload.size()) << std::endl;
                auto nextBlock = blocksToDownload.front();
                blocksToDownload.pop();
                SendMessage(connection, msf.GetRequestMessage(nextBlock));
            }
        }
        else if (message.Type == MessageType::Have)
        {
            auto havePiece = DecodeIntegerFromMessage(message.Content);
            if (downloadedPieces.contains(havePiece))
            {
                std::cout << std::format("Have message contains already downloaded piece {}, skipping download.", havePiece) << std::endl;
                continue;
            }
            else if (pieceInProgress.has_value())
            {
                seenPieces.push_back(havePiece);
                std::cout << std::format(
                    "Have message contains an undownloaded piece {}, but a piece download is currently in progress. "
                    "Adding to seen but not starting download.",
                    havePiece) << std::endl;
            }
            else
            {
                std::cout << std::format(
                    "Have message contains an undownloaded piece {}, and no piece download is currently in progress. "
                    "Sending request message.",
                    havePiece) << std::endl;
                
                auto piece = pieces.at(havePiece);

                auto blocks = piece.GetBlocks();

                pieceInProgress = havePiece;

                while (!blocksToDownload.empty())
                    blocksToDownload.pop();

                for (auto it = blocks.rbegin(); it != blocks.rend(); ++it)
                {
                    blocksToDownload.push(*it);
                }
                
                downloadedPieceBuffer.clear();
                downloadedPieceBuffer.reserve(piece.GetSize());

                auto firstBlock = blocksToDownload.front();
                blocksToDownload.pop();

                SendMessage(connection, msf.GetRequestMessage(firstBlock));
            }
        }
    }
}