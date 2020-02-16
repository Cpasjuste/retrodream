//
// Created by cpasjuste on 15/01/2020.
//

#include <kos.h>
#include <cerrno>
#include <utility>
#include "flashrom.h"
#include "ds/firmware/isoldr/loader/kos/dc/flashrom.h"

FlashRom::FactorySetting FlashRom::getFactorySetting() {

    FactorySetting setting{};

    uint8 *data = read(2, 3, setting.error);
    if (data == nullptr) {
        return setting;
    }

    setting.country = (Country) data[0];
    setting.language = (Language) data[1];
    setting.broadcast = (Broadcast) data[2];

    free(data);

    return setting;
}

int FlashRom::setFactorySetting(const FlashRom::FactorySetting &setting, std::string err) {

    uint8 data[3] = {(uint8) setting.country, (uint8) setting.language, (uint8) setting.broadcast};
    if (write(2, 3, data, std::move(err)) != 0) {
        return -1;
    }

    return 0;
}

uint8 *FlashRom::read(unsigned int offset, int size, std::string err) {

    int pt_start, pt_size;
    uint8 *data = nullptr;

    if (flashrom_info(FLASHROM_PT_SYSTEM, &pt_start, &pt_size) != 0) {
        printf("FlashRom::read: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return nullptr;
    }

    if ((int) offset > pt_size || (int) offset + size > pt_size) {
        printf("FlashRom::read: COULD NOT READ OUTSIDE PARTITION\n");
        err = "COULD NOT READ OUTSIDE PT PARTITION";
        return nullptr;
    }

    data = (uint8 *) malloc(size);
    if (data == nullptr) {
        printf("FlashRom::read: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        return nullptr;
    }

    if (flashrom_read(pt_start + (int) offset, data, size) < 0) {
        printf("FlashRom::read: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        return nullptr;
    }

    return data;
}

int FlashRom::write(unsigned int offset, int size, uint8 *data, std::string err) {

    int pt_start, pt_size;

    if (flashrom_info(FLASHROM_PT_SYSTEM, &pt_start, &pt_size) < 0) {
        printf("FlashRom::write: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    if ((int) offset > pt_size || (int) offset + size > pt_size) {
        printf("FlashRom::write: COULD NOT READ OUTSIDE PARTITION\n");
        err = "COULD NOT WRITE OUTSIDE PT PARTITION";
        return -1;
    }

    if (flashrom_write(pt_start + (int) offset, data, size) < 0) {
        printf("FlashRom::write: COULD NOT WRITE TO FLASHROM\n");
        err = "COULD NOT WRITE TO FLASHROM";
        return -1;
    }

    return 0;
}

int FlashRom::backup(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    int size, start;

    if (flashrom_info(FLASHROM_PT_SYSTEM, &start, &size) < 0) {
        printf("FlashRom::readFactoryToFile: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::readFactoryToFile: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::readFactoryToFile: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (flashrom_read(start, data, size) < 0) {
        printf("FlashRom::readFactoryToFile: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (fs_write(fd, data, size) < 0) {
        printf("FlashRom::readFactoryToFile: COULD NOT WRITE TO FILE\n");
        err = "COULD NOT WRITE TO FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::restore(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    int size, start;

    if (flashrom_info(FLASHROM_PT_SYSTEM, &start, &size) < 0) {
        printf("FlashRom::writeFactoryFromFile: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::writeFactoryFromFile: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::writeFactoryFromFile: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (fs_read(fd, data, size) < 0) {
        printf("FlashRom::writeFactoryFromFile: COULD NOT READ SOURCE FILE\n");
        err = "COULD NOT READ SOURCE FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (flashrom_write(start, data, size) < 0) {
        printf("FlashRom::writeFactoryFromFile: COULD NOT WRITE TO FLASHROM\n");
        err = "COULD NOT WRITE TO FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

#if 0
int FlashRom::writeFromFile(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    size_t size;

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::writeFile: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    size = fs_total(fd);
    if (size > 0x20000) {
        size = 0x20000;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::writeFile: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (fs_read(fd, data, size) < 0) {
        printf("FlashRom::writeFile: COULD NOT READ SOURCE FILE\n");
        err = "COULD NOT READ SOURCE FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (flashrom_write(0, data, size) < 0) {
        printf("FlashRom::writeFile: COULD NOT WRITE TO FLASHROM\n");
        err = "COULD NOT WRITE TO FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::readToFile(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    size_t size = 0x20000;

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::readFile: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::readFile: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (flashrom_read(0, data, size) < 0) {
        printf("FlashRom::readFile: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (fs_write(fd, data, size) < 0) {
        printf("FlashRom::readFile: COULD NOT WRITE TO FILE\n");
        err = "COULD NOT WRITE TO FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::erase(int block, std::string err) {

    int size, start;

    if (flashrom_info(block, &start, &size) < 0) {
        printf("FlashRom::erase: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    if (flashrom_delete(start) < 0) {
        printf("FlashRom::erase: COULD NOT ERASE FLASHROM\n");
        err = "COULD NOT ERASE FLASHROM";
        return -1;
    }

    return 0;
}
#endif
