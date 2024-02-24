#ifndef CMPT785_G5_SECURE_FILESYSTEM_STRINGMANIP_H
#define CMPT785_G5_SECURE_FILESYSTEM_STRINGMANIP_H

#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> splittext(const std::string &s, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

#endif //CMPT785_G5_SECURE_FILESYSTEM_STRINGMANIP_H
