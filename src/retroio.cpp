//
// Created by cpasjuste on 21/01/2020.
//

#ifdef __DREAMCAST__

#include <kos.h>

#else

#include <dirent.h>

#endif

#include "cross2d/c2d.h"
#include "retroio.h"

using namespace c2d;

std::string RetroIo::getHomePath() {
#ifdef __DREAMCAST__
    return "/";
#else
    return C2DIo::getDataPath();
#endif
}

std::string RetroIo::getDataPath() {
#ifdef __DREAMCAST__
    if (rdPath.empty()) {
        if (exist("/cd/RD")) {
            rdPath = "/cd/RD/";
            dsPath = "/cd/DS/";
            dsBinPath = "/cd/DS/DS.BIN";
        } else if (exist("/sd/RD")) {
            rdPath = "/sd/RD/";
            dsPath = "/sd/DS/";
            dsBinPath = "/sd/DS/DS.BIN";
        } else if (exist("/ide/RD")) {
            rdPath = "/ide/RD/";
            dsPath = "/ide/DS/";
            dsBinPath = "/ide/DS/DS.BIN";
        } else {
            // default value for data directory creation...
            if (exist("/sd")) {
                create("/sd/RD");
                rdPath = "/sd/RD/";
                dsPath = "/sd/DS/";
                dsBinPath = "/sd/DS/DS.BIN";
            } else if (exist("/ide")) {
                create("/ide/RD");
                rdPath = "/ide/RD/";
                dsPath = "/ide/DS/";
                dsBinPath = "/ide/DS/DS.BIN";
            }
        }
        // screenshots save directory
        if (!exist(rdPath + "screenshots")) {
            create(rdPath + "screenshots");
        }
    }

    return rdPath;
#else
    rdPath = C2DIo::getDataPath() + "RD/";
    dsPath = C2DIo::getDataPath() + "DS/";
    dsBinPath = C2DIo::getDataPath() + "DS/DS.BIN";
    return rdPath;
#endif
}

std::string RetroIo::getDsPath() {
    if (rdPath.empty()) {
        getDataPath();
    }
    return dsPath;
}

std::string RetroIo::getDsBinPath() {
    if (rdPath.empty()) {
        getDataPath();
    }
    return dsBinPath;
}

std::string RetroIo::getConfigPath() {
    return getDataPath() + "retrodream.cfg";
}

std::string RetroIo::getSkinPath() {
    return getDataPath() + "skin.cfg";
}

std::string RetroIo::getScreenshotPath() {
    return getDataPath() + "screenshots/";
}

#ifdef __DREAMCAST__

bool RetroIo::hasMoreThanOneFile(const std::string &path) {

    dirent_t *ent;
    file_t fd;
    int count = 0;

    if (path.empty()) {
        return false;
    }

    if ((fd = fs_open(path.c_str(), O_RDONLY | O_DIR)) != FILEHND_INVALID) {
        while ((ent = fs_readdir(fd)) != nullptr) {
            if (ent->name[0] == '.') {
                continue;
            }
            count++;
            if (count > 1) {
                break;
            }
        }
        fs_close(fd);
    }

    return count > 1;
}

#else

bool RetroIo::hasMoreThanOneFile(const std::string &path) {

    struct dirent *ent;
    DIR *dir;
    int count = 0;

    if (path.empty()) {
        return false;
    }

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (ent->d_name[0] == '.') {
                continue;
            }
            count++;
            if (count > 1) {
                break;
            }
        }
        closedir(dir);
    }

    return count > 1;
}

#endif
