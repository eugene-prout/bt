# BT: A C++ BitTorrent client

A BitTorrent client implemented in C++23.

Currently the client only supports downloading pieces to an in-memory buffer.

# Features

- Parsing a `.torrent` file.
- Connecting to trackers.
- CLI with documented commands.
- TCP client written from scratch.
- HTTP client written from scratch.
- BitTorrent protocol (de)serialisation written from scratch.

# Command line interface

Run `./bt --help` to view the available commands and arguments. 

```
USAGE:
  bt [-?|-h|--help] [ parse { [-?|-h|--help] <filename> } ] [ dl { [-?|-h|--help] <filename> } ]

Display usage information.

OPTIONS, ARGUMENTS:
  -?, -h, --help          
                          
  parse                   Parses the provided .torrent file.
                          
  -?, -h, --help          
  <filename>              The file path for the .torrent file.
                          
  dl                      Downloads the provided .torrent file.
                          
  -?, -h, --help          
  <filename>              The file path for the .torrent file.
```