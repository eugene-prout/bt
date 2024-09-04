# std::stringstream buffer;
# const std::string protocol = "BitTorrent protocol";
# buffer << (char) protocol.length();
# buffer << protocol;

# std::string reserved;
# for (int i = 0; i < 8; i++)
#     reserved.push_back('\0');
# buffer << reserved;

# buffer << "2aa4f5a7e209e54b32803d43670971c4c8caaa05";
# buffer << "aaaaaaaaaaaaaaaaaaaa";
# auto len = buffer.str().length();
# // assert (buffer.str().length() == protocol.length() + 49);
# return buffer.str();

encode = lambda x: ''.join(chr(int(x[i:i + 2], 16)) for i in range(0, len(x), 2))

msg = encode("13")
msg += "BitTorrent protocol"
for i in range(8):
    msg += "0"
hash = "2aa4f5a7e209e54b32803d43670971c4c8caaa05"
msg += encode(hash)
msg += "aaaaaaaaaaaaaaaaaaaa"


# buffer << 
# buffer << "aaaaaaaaaaaaaaaaaaaa";