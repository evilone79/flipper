
#include "util.h"
#include <unistd.h>
#include <cstring>

namespace util
{

std::string get_hostname() {
    static constexpr auto MAX_NAME_SIZE = 128;
    char name[MAX_NAME_SIZE] = {};
    if (gethostname(name, MAX_NAME_SIZE)) {
        throw std::runtime_error(std::string{"Failed to get hostname. "} + std::strerror(errno));
    }
    return name;
}

uint32_t flip_bits(uint32_t input) {
    static const auto TEST_MASK = (0x1 << 31);
    static const auto ODD_MASK = 0xAAAAAAAA;
    static const auto EVEN_MASK = 0x55555555;

    return input & TEST_MASK
           ? input ^ ODD_MASK
           : input ^ EVEN_MASK;
}

}