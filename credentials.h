#pragma once

#include <string>

struct credentials
{
    static credentials load_from_file(const std::string &file_name);

    std::string username;
    std::string password;
};

