#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ranges>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>


class Hasher
{
public:
    static std::string GetSHA1HashAsString(const std::string& source)
    {
        CryptoPP::SHA1 sha1; 
        std::string hash;
        
        CryptoPP::StringSource{
            source,
            true,
            new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash)))};
            
        return hash;
    }

    static std::vector<std::byte> GetSHA1HashAsBytes(const std::string& source)
    {
        CryptoPP::SHA1 sha1; 
        
        std::vector<CryptoPP::byte> hash{};
        
        CryptoPP::StringSource test{
            source,
            true,
            new CryptoPP::HashFilter(sha1, new CryptoPP::VectorSink(hash))};
            
        return hash | std::views::transform([](CryptoPP::byte c) { return std::byte(c); }) | std::ranges::to<std::vector<std::byte>>();
    }
};