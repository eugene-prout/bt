#include "commands.hpp"
#include "bencode.hpp"
#include "file_parser.hpp"
#include "peer.hpp"
#include "tcp_client.hpp"
#include "tracker.hpp"
#include "url.hpp"
#include "utils.hpp"
#include <lyra/lyra.hpp>

#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <thread>
#include <utility>

void BT::ParseCommand(std::string &filename) {
  std::ifstream infile{filename};

  std::stringstream buffer;
  buffer << infile.rdbuf();

  FileParser p{};
  Hasher hasher{};

  auto f = p.ParseFile(buffer.str(), hasher);
  f.PrintDetails();
}

void BT::DownloadCommand(std::string &filename) {
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
  while (attempts < 5) {
    response = QueryTrackerForPeers(f);

    if (response.peers.size() > 0) {
      std::cout << "Tracker responsed with " << response.peers.size() << " peers." << std::endl;
      break;
    }

    std::cout << "No peers in tracker response. Attempt " << (attempts + 1) << " failed. Retrying in 10 seconds."
              << std::endl;

    attempts++;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  if (response.peers.size() == 0)
    throw std::runtime_error("No peers found after multiple attempts.");

  TCPConnectionFactory connectionFactory;

  auto peer = response.peers.at(0);

  Peer p{connectionFactory, peer, f.InfoHash};

  auto pieces = f.GetPieces();
  p.Download(pieces);
}