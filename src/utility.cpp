//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "cross2d/skeleton/stb_image.h"
#include "cross2d/skeleton/stb_image_write.h"
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
#else
    printf(" RetroUtility::exec(%s): not implemented on linux\n", path.c_str());
#endif
}

bool RetroUtility::isGame(const std::string &fileName) {
    return !Utility::startWith(fileName, "track") &&
           (Utility::endsWith(fileName, ".iso")
            || Utility::endsWith(fileName, ".cdi")
            || Utility::endsWith(fileName, ".gdi"));
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
    if (io->exist("/ide/" + path)) {
        return "/ide/" + path;
    } else if (io->exist("/sd/" + path)) {
        return "/sd/" + path;
    }

    return "";
#else
    return io->getHomePath() + path;
#endif
}

bool RetroUtility::screenshot(RetroDream *retroDream, const std::string &path) {
#ifdef __DREAMCAST__
    int i, res;
    char shotPath[MAX_PATH];
    Io *io = retroDream->getRender()->getIo();

    snprintf(shotPath, MAX_PATH, "%s001", path.c_str());
    for (i = 2; i < 999; i++) {
        if (!io->exist(std::string(shotPath) + ".png")) {
            break;
        }
        snprintf(shotPath, MAX_PATH, "%s%03d", path.c_str(), i);
    }

    res = vid_screen_shot((std::string(shotPath) + ".ppm").c_str());
    if (res != 0) {
        return false;
    }

    // convert to png, on requests.... (crap)
    int w, h, n;
    unsigned char *pixels = stbi_load((std::string(shotPath) + ".ppm").c_str(), &w, &h, &n, 3);
    if (pixels == nullptr) {
        io->removeFile((std::string(shotPath) + ".ppm"));
        return false;
    }

    // convert!
    res = stbi_write_png((std::string(shotPath) + ".png").c_str(), w, h, n, pixels, w * n);
    // free resources, delete ppm
    free(pixels);
    io->removeFile((std::string(shotPath) + ".ppm"));

    if (res == 1) {
        retroDream->showStatus("SCREENSHOT SAVED...", (std::string(shotPath) + ".png"), COL_GREEN);
    } else {
        retroDream->showStatus("SCREENSHOT NOT SAVED...", (std::string(shotPath) + ".png"), COL_RED);
    }

    return res == 1;
#else
    printf("RetroUtility::screenshot: not supported on linux\n");
    return false;
#endif
}
