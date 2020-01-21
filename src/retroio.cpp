//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "retroio.h"

using namespace c2d;

std::string RetroIo::getHomePath() {
#ifdef __DREAMCAST__
    return "/";
#else
    return "/media/cpasjuste/SSD/dreamcast/";
#endif
}

std::string RetroIo::getDataPath() {
#ifdef __DREAMCAST__
    if (dataPath.empty()) {
        if (exist("/sd/RD")) {
            dataPath = "/sd/RD/";
        } else if (exist("/ide/RD")) {
            dataPath = "/ide/RD/";
        }
    }

    return dataPath;
#else
    if (dataPath.empty()) {
        dataPath = C2DIo::getHomePath();
    }

    return dataPath;
#endif
}

std::string RetroIo::getConfigPath() {
    return getDataPath() + "retrodream.cfg";
}

void RetroIo::setDataPath(const std::string &path) {
    if (exist(path)) {
        dataPath = path;
    } else {
        dataPath.clear();
        getDataPath();
    }
}
