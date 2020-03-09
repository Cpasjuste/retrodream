//
// Created by cpasjuste on 09/02/2020.
//

#include "cross2d/c2d.h"
#include "biosflash.h"

#ifdef __DREAMCAST__

#include <cstdio>

extern "C" {
#include "ds/include/drivers/bflash.h"
};

using namespace c2d;

static const uint16 CHUNK_SIZE = 32 * 1024;

bool BiosFlash::flash(const std::string &biosFile,
                      const std::function<void(const std::string, float)> &callback) {

    file_t pFile = 0;
    uint8 *data = nullptr;
    bflash_dev_t *dev = nullptr;
    bflash_manufacturer_t *mrf = nullptr;

    callback("DETECTING FLASH CHIP...", 0);
    if (bflash_detect(&mrf, &dev) < 0) {
        callback("FLASH DETECTION FAILED !", -1);
        return false;
    }
    std::string chip = "FLASH CHIP: " + std::string(mrf->name) + ", ID: 0x" + Utility::toString(mrf->id);
    callback(chip, 0);
    sleep(3);

    if ((dev->flags & F_FLASH_PROGRAM) == 0) {
        callback("FLASH CHIP IS NOT WRITE CAPABLE !", -1);
        return false;
    }

    callback("READING BIOS TO MEMORY... ", 0);
    pFile = fs_open(biosFile.c_str(), O_RDONLY);
    if (pFile == FILEHND_INVALID) {
        callback("COULD NOT OPEN BIOS FILE: " + biosFile, -1);
        return false;
    }

    size_t fileSize = fs_total(pFile);
    if (fileSize > dev->size * 1024) {
        std::string msg = "BIOS FILE IS LARGER THAN FLASH CHIP ("
                          + Utility::toString((int) (fileSize / 1024)) + " vs " + Utility::toString(dev->size) + ")";
        callback(msg, -1);
        fs_close(pFile);
        return false;
    }

    data = (uint8 *) memalign(32, fileSize);
    if (data == nullptr) {
        callback("COULD NOT ALLOCATE MEMORY !", -1);
        fs_close(pFile);
        return false;
    }

    size_t readLen = fs_read(pFile, data, fileSize);
    if (fileSize != readLen) {
        callback("COULD NOT READ BIOS FILE TO MEMORY !", -1);
        free(data);
        fs_close(pFile);
        return false;
    }
    fs_close(pFile);

    callback("ERASING FLASH CHIP...", 0);
    sleep(1);
    if (((dev->flags & F_FLASH_ERASE_SECTOR) != 0) || ((dev->flags & F_FLASH_ERASE_ALL) != 0)) {
        for (int i = 0; i < dev->sec_count; i++) {
            callback("ERASING FLASH CHIP...", (float) i / dev->sec_count);
            if (bflash_erase_sector(dev, dev->sectors[i]) < 0) {
                callback("COULD NOT ERASE FLASH CHIP !", -1);
                free(data);
                return false;
            }
        }
    }

    callback("WRITING FLASH CHIP...", 0);
    sleep(1);
    size_t offset = 0;
    size_t chunkCount = fileSize / CHUNK_SIZE;
    for (size_t i = 0; i <= chunkCount; ++i) {
        callback("WRITING FLASH CHIP...", (float) i / chunkCount);
        size_t dataPos = i * CHUNK_SIZE + offset;
        size_t dataLen = (dataPos + CHUNK_SIZE > fileSize) ? fileSize - dataPos : CHUNK_SIZE;
        int result = bflash_write_data(dev, dataPos, data + dataPos, dataLen);
        if (result < 0) {
            callback("COULD NOT WRITE FLASH CHIP !", -1);
            free(data);
            return false;
        }
    }

    callback("BIOS SUCCESSFULLY FLASHED TO CHIP!\nENJOY YOUR NEW BIOS", 2);
    free(data);

    return true;
}

#else

bool BiosFlash::flash(const std::string &biosFile,
                      const std::function<void(const std::string, float)> &callback) {
    printf("BiosFlash::flash: not implemented on linux\n");
    callback("NOTHING TO FLASH IN LINUX...", -1);
    return false;
}

#endif
