
#include "credentials.h"
#include <fstream>
#include <filesystem>
#include <cstring>

using namespace std::filesystem;

credentials credentials::load_from_file(const std::string &file_name)
{
    if (!exists(file_name))
        throw std::runtime_error("credentials file not found at " + file_name);
    std::ifstream ifs(file_name);

    if (!ifs)
        throw std::runtime_error(std::string("failed to load credentials. ") + std::strerror(errno));

    credentials creds;
    std::getline(ifs, creds.username);
    std::getline(ifs, creds.password);

    if (creds.username.empty())
        throw std::logic_error("username cannot be empty");

    return creds;

}