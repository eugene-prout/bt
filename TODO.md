# TODO

[ ] Take advantage of SHA-1 always returning 20 bytes. stackalloc an array? !!
[ ] Timeouts
[ ] Reuse TCP client in HTTP client
[ ] Consolidate utils.cpp
[ ] Use std::span for read-only memory
[ ] Use std::array for fixed-size containers (IP etc)


# Pieces

pieces consist of blocks

assumption: a piece will always fit into memory

block is 2^14 (16384) bytes

cut a piece into blocks, request a block

request: <len=0013><id=6><index><begin><length>

The request message is fixed length, and is used to request a block. The payload contains the following information:

    index: integer specifying the zero-based piece index
    begin: integer specifying the zero-based byte offset within the piece
    length: integer specifying the requested length.

track seen pieces, map: 

piece: <len=0009+X><id=7><index><begin><block>

The piece message is variable length, where X is the length of the block. The payload contains the following information:

    index: integer specifying the zero-based piece index
    begin: integer specifying the zero-based byte offset within the piece
    block: block of data, which is a subset of the piece specified by index.