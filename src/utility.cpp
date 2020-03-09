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
    if (f == FILEHND_INVALID) {
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
    } else if (io->exist("/cd/" + path)) {
        return "/cd/" + path;
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
        io->removeFile(std::string(shotPath) + ".ppm");
        return false;
    }

    // convert!
    res = stbi_write_png((std::string(shotPath) + ".png").c_str(), w, h, n, pixels, w * n);
    // free resources, delete ppm
    free(pixels);
    io->removeFile(std::string(shotPath) + ".ppm");

    if (res == 1) {
        retroDream->showStatus("SCREENSHOT SAVED...", std::string(shotPath) + ".png", COL_GREEN);
    } else {
        retroDream->showStatus("SCREENSHOT NOT SAVED...", std::string(shotPath) + ".png");
    }

    return res == 1;
#else
    printf("RetroUtility::screenshot: not supported on linux\n");
    return false;
#endif
}

bool RetroUtility::vmuBackup(RetroDream *retroDream, const std::string &vmuPath,
                             const std::function<void(const std::string, float)> &callback) {

    char dstPath[MAX_PATH];
    uint8 *data = nullptr;
    Io *io = retroDream->getRender()->getIo();

    callback("DETECTING VMU DEVICE...", 0);
    auto dev = (maple_device_t *) getVmuDevice(vmuPath);
    if (dev == nullptr) {
        callback("VMU DETECTION FAILED", -1);
        return false;
    }

    std::string rdPath = retroDream->getConfig()->getBootDevice() + "RD/";
    snprintf(dstPath, MAX_PATH, "%s001.vmu", rdPath.c_str());
    for (int i = 2; i < 999; i++) {
        if (!io->exist(dstPath)) {
            break;
        }
        snprintf(dstPath, MAX_PATH, "%s%03d.vmu", rdPath.c_str(), i);
    }

    file_t fd = fs_open(dstPath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == FILEHND_INVALID) {
        callback("COULD NOT OPEN VMU BACKUP FILE: " + std::string(dstPath), -1);
        return false;
    }

    callback("SAVING VMU...", 0);
    data = (uint8 *) calloc(1, 512);
    for (int i = 0; i < 256; i++) {
        if (vmu_block_read(dev, i, data) < 0) {
            fs_close(fd);
            free(data);
            callback("COULD NOT READ VMU DEVICE", -1);
            return false;
        }
        callback("SAVING VMU...", (float) i / 256);
        fs_write(fd, data, 512);
    }

    fs_close(fd);
    free(data);

    callback(dstPath, 2);

    return true;
}

bool RetroUtility::vmuRestore(RetroDream *retroDream, const std::string &path,
                              const std::function<void(const std::string, float)> &callback) {

    return true;
}

void *RetroUtility::getVmuDevice(const std::string &path) {

    maple_device_t *device = nullptr;

    if (path == "/vmu/a1") {
        device = maple_enum_dev(0, 1);
    } else if (path == "/vmu/a2") {
        device = maple_enum_dev(0, 2);
    } else if (path == "/vmu/b1") {
        device = maple_enum_dev(1, 1);
    } else if (path == "/vmu/b2") {
        device = maple_enum_dev(1, 2);
    } else if (path == "/vmu/c1") {
        device = maple_enum_dev(2, 1);
    } else if (path == "/vmu/c2") {
        device = maple_enum_dev(2, 2);
    } else if (path == "/vmu/d1") {
        device = maple_enum_dev(3, 1);
    } else if (path == "/vmu/d2") {
        device = maple_enum_dev(3, 2);
    } else {
        return nullptr;
    }

    return device;
}
