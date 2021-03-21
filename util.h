#pragma once

#include <iostream>

namespace util
{

std::string get_hostname();
uint32_t flip_bits(uint32_t input);

template <typename ...Ts>
void log(Ts&& ...val)
{
    (std::cout << ... << std::forward<Ts>(val)) << "\n";
}

}

