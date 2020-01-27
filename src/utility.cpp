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
    std::string file = Utility::toLower(fileName);
    return !Utility::startWith(file, "track") &&
           (Utility::endsWith(file, ".iso", false)
            || Utility::endsWith(file, ".cdi", false)
            || Utility::endsWith(file, ".gdi", false));
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
    if (io->exist("/sd/" + path)) {
        return "/sd/" + path;
    } else if (io->exist("/ide/" + path)) {
        return "/ide/" + path;
    } else if (io->exist("/sd/" + path)) {
        return "/sd/" + path;
    } else if (io->exist("/ide/" + path)) {
        return "/ide/" + path;
    } else if (io->exist("/rd/" + path)) {
        return "/rd/" + path;
    }

    return "";
#else
    return io->getHomePath() + path;
#endif
}
