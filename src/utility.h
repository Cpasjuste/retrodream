//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_UTILITY_H
#define RETRODREAM_UTILITY_H

#include <string>

class RetroUtility {

public:

    static bool isGame(const std::string &fileName);

    static bool isElf(const std::string &fileName);

    static void exec(const std::string &path);
};

#endif //RETRODREAM_UTILITY_H
