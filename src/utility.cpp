//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "utility.h"

using namespace c2d;

void RetroUtility::exec(const std::string &path) {
#ifdef __DREAMCAST__
    file_t f;
    void *elf;

    f = fs_open(path.c_str(), O_RDONLY);
    if (f < 0) {
        return;
    }

    elf = fs_mmap(f);
    if (elf == nullptr) {
        return;
    }

    arch_exec(elf, fs_total(f));
#endif
}

bool RetroUtility::isGame(const std::string &fileName) {
    return !Utility::startWith(fileName, "track") &&
           (Utility::endsWith(fileName, ".iso", false)
            || Utility::endsWith(fileName, ".cdi", false)
            || Utility::endsWith(fileName, ".gdi", false));
}

bool RetroUtility::isElf(const std::string &fileName) {
    return Utility::endsWith(fileName, ".elf", false)
           || Utility::endsWith(fileName, ".bin", false);
}

float RetroUtility::percentSize(float size, float percent) {
    return (percent * size) / 100;
}

std::string RetroUtility::findPath(c2d::Io *io, const std::string &path) {

#ifdef __DREAMCAST__
    if (io->exist("/sd/RD/" + path)) {
        return "/sd/RD/" + path;
    } else if (io->exist("/ide/RD/" + path)) {
        return "/ide/RD/" + path;
    } else if (io->exist("/sd/DS/" + path)) {
        return "/sd/DS/" + path;
    } else if (io->exist("/ide/DS/" + path)) {
        return "/ide/DS/" + path;
    } else if (io->exist("/rd/" + path)) {
        return "/rd/" + path;
    }

    return "";
#else
    return io->getDataPath() + "/RD/" + path;
#endif
}
