#include "torrent_file.hpp"
#include "piece.hpp"
#include "utils.hpp"

#include <cmath>
#include <print>
#include <ranges>

BT::TorrentFile::TorrentFile(Url announceUrl, long long int length, std::string name, long long int pieceLengthInBytes,
                         std::vector<std::vector<std::byte>> pieceHashes, std::vector<std::byte> infoHash)
    : AnnounceUrl{announceUrl}, LengthInBytes{length}, Name{name}, PieceLengthInBytes{pieceLengthInBytes},
      PieceHashes{pieceHashes}, InfoHash{infoHash}
{
}

void BT::TorrentFile::PrintDetails() const
{
    std::print("Torrent File Details:\n");
    std::print("Announce URL: {}\n", ToString(AnnounceUrl));
    std::print("Length (in bytes): {}\n", LengthInBytes);
    std::print("Name: {}\n", Name);
    std::print("Piece Length (in bytes): {}\n", PieceLengthInBytes);
    std::print("Info Hash: {}\n", ToHex(InfoHash) | std::views::join | std::ranges::to<std::string>());
    std::print("Pieces hashes: {} of size {}\n", PieceHashes.size(), PieceHashes.at(0).size());
    std::print("\n");
}

constexpr long long int BLOCK_SIZE = 16384;

std::vector<BT::Piece> BT::TorrentFile::GetPieces()
{
    auto numberOfPieces = PieceHashes.size();
    auto calculatedNumberOfPieces = std::ceil((float)LengthInBytes / (float)PieceLengthInBytes);
    if (numberOfPieces != calculatedNumberOfPieces)
        throw std::runtime_error(
            std::format("Inconsistent number of pieces. Have {} piece hashes. Length / PieceLength = {} / {} = {}",
            numberOfPieces, LengthInBytes, PieceLengthInBytes, calculatedNumberOfPieces));
    

    auto [numberOfFullPieces, sizeOfFinalPiece] = std::div(LengthInBytes, PieceLengthInBytes);
    std::vector<Piece> pieces;
    pieces.reserve(numberOfFullPieces + 1);

    for (int i = 0; i < numberOfFullPieces; i++)
    {
        auto [numberOfFullBlocks, sizeOfFinalBlock] = std::div(PieceLengthInBytes, BLOCK_SIZE);
        
        std::vector<Block> blocks;
        blocks.reserve(numberOfFullBlocks + 1);

        for (int j = 0; j < numberOfFullBlocks; j++)
        {
            Block b1{i, j * BLOCK_SIZE, BLOCK_SIZE};
            blocks.push_back(b1);
        }

        if (sizeOfFinalBlock > 0)
        {
            Block b{i, numberOfFullBlocks * BLOCK_SIZE, sizeOfFinalBlock};
            blocks.push_back(b);
        }

        pieces.emplace_back(PieceLengthInBytes, PieceHashes.at(i), blocks);
    }

    if (sizeOfFinalPiece > 0)
    {
        auto [numberOfFullBlocks, sizeOfFinalBlock] = std::div(sizeOfFinalPiece, BLOCK_SIZE);
        
        std::vector<Block> blocks;
        blocks.reserve(numberOfFullBlocks + 1);

        for (int j = 0; j < numberOfFullBlocks; j++)
        {
            blocks.emplace_back(numberOfFullPieces + 1, j * BLOCK_SIZE, BLOCK_SIZE);
        }

        if (sizeOfFinalBlock > 0)
        {
            blocks.emplace_back(numberOfFullPieces + 1, numberOfFullBlocks * BLOCK_SIZE, sizeOfFinalBlock);
        }

        pieces.emplace_back(PieceLengthInBytes, PieceHashes.at(numberOfFullPieces), blocks);
    }    

    return pieces;
}