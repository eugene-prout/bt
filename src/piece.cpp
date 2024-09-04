#include "piece.hpp"

#include <optional>
#include <vector>

BT::Block::Block(long long int pieceIndex, long long int byteOffsetWithinPiece, long long int size)
    : pieceIndex(pieceIndex), byteOffsetWithinPiece(byteOffsetWithinPiece), size(size)
{
}

long long int BT::Block::GetByteOffsetWithinPiece()
{
    return byteOffsetWithinPiece;
}
long long int BT::Block::GetPieceIndex()
{
    return pieceIndex;
}
long long int BT::Block::GetSize()
{
    return size;
}

BT::Piece::Piece(long long int size, std::vector<std::byte> hashedContents, std::vector<Block> blocks)
    : size(size), hashedContents(hashedContents), blocks(blocks)
{
}

std::vector<BT::Block> BT::Piece::GetBlocks()
{
    return blocks;
}

long long int BT::Piece::GetSize()
{
    return size;
}