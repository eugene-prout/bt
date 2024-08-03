#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

std::string UrlEncodeBytes(const std::vector<std::byte> bytesToConvert)
{
    std::stringstream encodedBytes;
    encodedBytes << std::hex; 

    std::byte oneCharacterLimit{2^4-1};
    for (auto& b : bytesToConvert)
    {
        encodedBytes << "%" << std::setw(2) << std::setfill('0') << std::to_integer<int>(b);
    }

    return encodedBytes.str();
}