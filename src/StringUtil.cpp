#include "StringUtil.h"

std::vector<std::string> splitText(const std::string &s, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}
