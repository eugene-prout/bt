#include "bencode.hpp"
#include "commands.hpp"
#include "file_parser.hpp"
#include <lyra/lyra.hpp>
#include "utils.hpp"
#include "url.hpp"
#include "peer.hpp"
#include "tracker.hpp"
#include "tcp_client.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <format>
#include <print>
#include <ranges>
#include <utility>
#include <thread>
#include <chrono>

void BT::ParseCommand(std::string& filename)
{
    std::ifstream infile{filename};

    std::stringstream buffer;
    buffer << infile.rdbuf();

    FileParser p{};
    Hasher hasher{};

    auto f = p.ParseFile(buffer.str(), hasher);
    f.PrintDetails();
}

void BT::DownloadCommand(std::string& filename)
{
    std::ifstream infile{filename};

    std::stringstream buffer;
    buffer << infile.rdbuf();
    

    FileParser parser{};

    Hasher hasher{};

    auto f = parser.ParseFile(buffer.str(), hasher);
    std::cout << "Parsed torrent file for '" << f.Name << "'." << std::endl;

    std::cout << "Querying tracker at '" << ToString(f.AnnounceUrl) << "'." << std::endl;
    int attempts = 0;
    TrackerResponse response;
    while (attempts < 5)
    {
        response = QueryTrackerForPeers(f);

        if (response.peers.size() > 0)
        {
            std::cout << "Tracker responsed with " << response.peers.size() << " peers." << std::endl;
            break;
        }

        std::cout << "No peers in tracker response. Attempt " << (attempts + 1) << " failed. Retrying in 10 seconds." << std::endl;

        attempts++;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    if (response.peers.size() == 0)
        throw std::runtime_error("No peers found after multiple attempts.");

    // auto peer = PeerDetails { 
    //     {std::byte{185}, std::byte{125}, std::byte{190}, std::byte{59}},
    //     6893
    // };

    TCPConnectionFactory connectionFactory;

    // auto ip = IPAddressToString(response.peers.at(0).ipAddress);
    auto peer = response.peers.at(0); 

    Peer p{connectionFactory, peer, f.InfoHash};

    auto pieces = f.GetPieces();
    p.Download(pieces);
    // auto connection = connectionFactory.OpenConnection(ip, response.peers.at(0).port);

    // HandshakeWithPeer(connection, f->InfoHash);

    // std::cout << "Blocking for bitfield." << std::endl;
    // auto bitfield = connection.ReceiveMessage();
    // std::cout << "Received bitfield message of " << bitfield.size() << " bytes." << std::endl;


}