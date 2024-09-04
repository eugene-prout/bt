#include "bencode.hpp"
#include "utils.hpp"
#include "url.hpp"
#include "torrent_file.hpp"
#include "tracker.hpp"
#include "tcp_client.hpp"
#include "web_client.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <format>
#include <print>
#include <ranges>
#include <sstream>
#include <utility>


BT::TrackerResponse BT::QueryTrackerForPeers(TorrentFile& torrentFile)
{
    std::map<std::string, std::string> query_parameters = {
        {"info_hash", UrlEncodeBytes(torrentFile.InfoHash)},
        {"peer_id", "aaaaaaaaaaaaaaaaaaaa"},
        {"port", "6881"},
        {"uploaded", "0"},
        {"downloaded", "0"},
        {"left", "12345"},
        {"compact", "1"}
    };

    auto path = std::format("{}?{}", torrentFile.AnnounceUrl.path, ConvertParametersToQueryString(query_parameters));

    auto url_struct = Url(torrentFile.AnnounceUrl.protocol, torrentFile.AnnounceUrl.hostname, torrentFile.AnnounceUrl.port, path);

    auto httpClient = HTTPClient();
    auto r = httpClient.MakeHTTPSRequest(url_struct);
    auto data = bencode::decode(r.Body.value());

    auto value = std::get<bencode::dict>(data);
    auto interval = std::get<bencode::integer>(data["interval"]);
    auto peers_present = value.find("peers") != value.end();

    auto peers = std::vector<PeerDetails>();
    if (peers_present)
    {
        auto peers_string = std::get<bencode::string>(data["peers"]);
        auto byteGroups = peers_string | std::views::transform([](char c)
                                                               { return std::byte(c); }) |
                          std::views::chunk(6);

        for (auto address_range : byteGroups)
        {
            auto ipAddress = address_range | std::views::take(4) | std::ranges::to<std::vector<std::byte>>();

            auto port = address_range | std::views::drop(4) | std::ranges::to<std::vector<std::byte>>();
            auto portValue = (std::to_integer<int>(port[0]) << 8) | std::to_integer<int>(port[1]);

            peers.push_back({ipAddress, portValue});
        }
    }

    return TrackerResponse(interval, peers);
}


std::string BT::SerialiseHandshake(HandshakeMessage message)
{
    std::stringstream buffer;

    const std::string protocol = "BitTorrent protocol";
    buffer << static_cast<char>(protocol.length());
    buffer << protocol;
    
    for (int i = 0; i < 8; i++)
        buffer << '\0';

    buffer << EncodeBytesAsCharacters(message.InfoHash);
    buffer << message.PeerId;

    auto len = buffer.str().length();
    assert (buffer.str().length() == protocol.length() + 49);
    return buffer.str();
}


std::vector<std::byte> GetBytesFromCharacters(std::string_view string) {
    std::vector<std::byte> result;
    result.reserve(string.length());
    
    for (const auto& c : string)
        result.push_back(static_cast<std::byte>(c));
    
    return result;
}

BT::HandshakeMessage BT::DerialiseHandshake(std::string message)
{
    HandshakeMessage output = {};
    auto size = static_cast<int>(message[0]);
    
    auto protocol = message.substr(1, 19);
    output.ProtocolString = protocol;


    auto messageBytes = EncodeStringAsBytes(message.substr(20, 8));
    std::array<std::byte, 8> extensionBytes;
    std::copy(messageBytes.begin(), messageBytes.end(), extensionBytes.begin());

    output.ExtensionBytes = extensionBytes;

    auto infoHash = EncodeStringAsBytes(message.substr(28, 20));
    output.InfoHash = infoHash;

    auto peerId = message.substr(48, 20);
    output.PeerId = peerId;

    return output;
}

bool BT::HandshakeWithPeer(TCPConnection& connection, const std::vector<std::byte>& infoHash)
{

    auto outgoing_message = HandshakeMessage();

    outgoing_message.InfoHash = infoHash;
    outgoing_message.PeerId = "aaaaaaaaaaaaaaaaaaaa";

    auto message = SerialiseHandshake(outgoing_message);

    std::cout << "Sending handshake to peer." << std::endl;
    connection.SendMessage(message);

    auto resp = connection.ReceiveMessage(message.size());
    std::cout << "Received handshake response containing " << resp.size() << " bytes." << std::endl;

    auto response = DerialiseHandshake(resp);
    return response.InfoHash == infoHash;
}