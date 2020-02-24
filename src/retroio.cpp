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
    return "/media/cpasjuste/SSD/dreamcast/";
#endif
}

std::string RetroIo::getDataPath() {
#ifdef __DREAMCAST__
    if (dataPath.empty()) {
        if (exist("/ide/RD")) {
            dataPath = "/ide/RD/";
        } else if (exist("/sd/RD")) {
            dataPath = "/sd/RD/";
        } else if (exist("/cd/RD")) {
            dataPath = "/cd/RD/";
        } else {
            // default value for data directory creation...
            if (exist("/ide")) {
                create("/ide/RD");
                dataPath = "/ide/RD/";
            } else if (exist("/sd")) {
                create("/sd/RD");
                dataPath = "/sd/RD/";
            } else {
                dataPath = "/cd/RD/";
            }
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
