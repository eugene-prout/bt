#pragma once

#include <memory>
#include <optional>
#include <vector>

namespace BT {
class Block {
public:
  Block(long long int pieceIndex, long long int byteOffsetWithinPiece, long long int size);

  long long int GetByteOffsetWithinPiece();
  long long int GetPieceIndex();
  long long int GetSize();

private:
  long long int byteOffsetWithinPiece;
  long long int pieceIndex;
  long long int size;
  bool complete;
  std::optional<std::vector<std::byte>> data;
};

class Piece {
public:
  Piece(long long int size, std::vector<std::byte> hashedContents, std::vector<Block> blocks);

  long long int GetSize();
  std::vector<Block> GetBlocks();
  std::unique_ptr<Block> GetNextBlock();
  bool Validate();

private:
  bool complete;
  std::vector<std::byte> hashedContents;
  std::vector<Block> blocks;
  int indexWithinPiece;
  long long int size;
};
} // namespace BT
