//
// Created by cpasjuste on 15/01/2020.
//

#include <kos.h>
#include <dc/flashrom.h>
#include <utility>
#include "flashrom.h"

FlashRom::Settings FlashRom::getSettings() {

    uint8 *data = nullptr;
    Settings setting{};

    // read country on factory partition
    data = read(FLASHROM_PT_SYSTEM, 2, 1, setting.error);
    if (data == nullptr) {
        return setting;
    }
    setting.country = (Country) data[0];
    free(data);

    // read country and broadcast on factory partition
    data = read(FLASHROM_PT_SYSTEM, 4, 1, setting.error);
    if (data == nullptr) {
        return setting;
    }
    setting.broadcast = (Broadcast) data[0];
    free(data);

    // read language on block 1 partition
    int sysCfgBlock = findBlockAddress(FLASHROM_PT_BLOCK_1, FLASHROM_B1_SYSCFG);
    if (sysCfgBlock < 0) {
        return setting;
    }
    data = read(FLASHROM_PT_BLOCK_1, sysCfgBlock + 7, 1, setting.error);
    if (data == nullptr) {
        return setting;
    }
    setting.language = (Language) data[0];
    free(data);

    return setting;
}

int FlashRom::setSettings(const FlashRom::Settings &setting, std::string err) {

    // TODO
    /*
    uint8 data[3] = {(uint8) setting.country, (uint8) setting.language, (uint8) setting.broadcast};
    if (write(FLASHROM_PT_SYSTEM, 2, 3, data, std::move(err)) != 0) {
        return -1;
    }
    */

    return 0;
}

uint8 *FlashRom::read(int partition, unsigned int offset, int size, std::string err) {

    int pt_start, pt_size;
    uint8 *data = nullptr;

    if (flashrom_info(partition, &pt_start, &pt_size) != 0) {
        printf("FlashRom::read: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return nullptr;
    }

    if (size <= 0) {
        size = pt_size;
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

int FlashRom::write(int partition, unsigned int offset, int size, uint8 *data, std::string err) {

    int pt_start, pt_size;

    if (flashrom_info(partition, &pt_start, &pt_size) < 0) {
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

int FlashRom::backup(int partition, const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    int size, start;

    if (flashrom_info(partition, &start, &size) < 0) {
        printf("FlashRom::backup: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::backup: COULD NOT OPEN DESTINATION FILE\n");
        err = "COULD NOT OPEN DESTINATION FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::backup: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (flashrom_read(start, data, size) < 0) {
        printf("FlashRom::backup: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (fs_write(fd, data, size) < 0) {
        printf("FlashRom::backup: COULD NOT WRITE TO DESTINATION FILE\n");
        err = "COULD NOT WRITE TO DESTINATION FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::restore(int partition, const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    int size, start;

    if (flashrom_info(partition, &start, &size) < 0) {
        printf("FlashRom::restore: COULD NOT GET FLASHROM INFORMATION\n");
        err = "COULD NOT GET FLASHROM INFORMATION";
        return -1;
    }

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::restore: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::restore: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (fs_read(fd, data, size) < 0) {
        printf("FlashRom::restore: COULD NOT READ SOURCE FILE\n");
        err = "COULD NOT READ SOURCE FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (flashrom_write(start, data, size) < 0) {
        printf("FlashRom::restore: COULD NOT WRITE TO FLASHROM\n");
        err = "COULD NOT WRITE TO FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::backupAll(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    size_t size = 0x20000;

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::backupAll: COULD NOT OPEN DESTINATION FILE\n");
        err = "COULD NOT OPEN DESTINATION FILE";
        return -1;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::backupAll: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (flashrom_read(0, data, size) < 0) {
        printf("FlashRom::backupAll: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (fs_write(fd, data, size) < 0) {
        printf("FlashRom::backupAll: COULD NOT WRITE TO DESTINATION FILE\n");
        err = "COULD NOT WRITE TO DESTINATION FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::restoreAll(const std::string &path, std::string err) {

    file_t fd;
    uint8 *data = nullptr;
    size_t size;

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::restoreAll: COULD NOT OPEN SOURCE FILE\n");
        err = "COULD NOT OPEN SOURCE FILE";
        return -1;
    }

    size = fs_total(fd);
    if (size > 0x20000) {
        size = 0x20000;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        printf("FlashRom::restoreAll: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        fs_close(fd);
        return -1;
    }

    if (fs_read(fd, data, size) < 0) {
        printf("FlashRom::restoreAll: COULD NOT READ SOURCE FILE\n");
        err = "COULD NOT READ SOURCE FILE";
        fs_close(fd);
        free(data);
        return -1;
    }

    if (flashrom_write(0, data, size) < 0) {
        printf("FlashRom::restoreAll: COULD NOT WRITE TO FLASHROM\n");
        err = "COULD NOT WRITE TO FLASHROM";
        fs_close(fd);
        free(data);
        return -1;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::findBlockAddress(int partid, int blockid) {

    int start, size;
    int bmcnt;
    char magic[18];
    uint8 *bitmap;
    uint8 buffer[64];
    int i;

    /* First, figure out where the partition is located. */
    if (flashrom_info(partid, &start, &size) != 0)
        return FLASHROM_ERR_NO_PARTITION;

    /* Verify the partition */
    if (flashrom_read(start, magic, 18) < 0) {
        dbglog(DBG_ERROR, "flashrom_get_block: can't read part %d magic\n", partid);
        return FLASHROM_ERR_READ_PART;
    }

    if (strncmp(magic, "KATANA_FLASH____", 16) != 0 || *((uint16 *) (magic + 16)) != partid) {
        bmcnt = *((uint16 *) (magic + 16));
        magic[16] = 0;
        dbglog(DBG_ERROR, "flashrom_get_block: invalid magic '%s' or id %d in part %d\n", magic, bmcnt, partid);
        return FLASHROM_ERR_BAD_MAGIC;
    }

    /* We need one bit per 64 bytes of partition size. Figure out how
       many blocks we have in this partition (number of bits needed). */
    bmcnt = size / 64;

    /* Round it to an even 64-bytes (64*8 bits). */
    bmcnt = (bmcnt + (64 * 8) - 1) & ~(64 * 8 - 1);

    /* Divide that by 8 to get the number of bytes from the end of the
       partition that the bitmap will be located. */
    bmcnt = bmcnt / 8;

    /* This is messy but simple and safe... */
    if (bmcnt > 65536) {
        dbglog(DBG_ERROR, "flashrom_get_block: bogus part %p/%d\n", (void *) start, size);
        return FLASHROM_ERR_BOGUS_PART;
    }

    if ((bitmap = (uint8 *) malloc(bmcnt)) == nullptr)
        return FLASHROM_ERR_NOMEM;

    if (flashrom_read(start + size - bmcnt, bitmap, bmcnt) < 0) {
        dbglog(DBG_ERROR, "flashrom_get_block: can't read part %d bitmap\n", partid);
        free(bitmap);
        return FLASHROM_ERR_READ_BITMAP;
    }

    /* Go through all the allocated blocks, and look for the latest one
       that has a matching logical block ID. We'll start at the end since
       that's easiest to deal with. Block 0 is the magic block, so we
       won't check that. */
    for (i = 0; i < bmcnt * 8; i++) {
        /* Little shortcut */
        if (bitmap[i / 8] == 0)
            i += 8;

        if ((bitmap[i / 8] & (0x80 >> (i % 8))) != 0)
            break;
    }

    /* Done with the bitmap, free it. */
    free(bitmap);

    /* All blocks unused -> file not found. Note that this is probably
       a very unusual condition. */
    if (i == 0)
        return FLASHROM_ERR_EMPTY_PART;

    i--;    /* 'i' was the first unused block, so back up one */

    for (; i > 0; i--) {
        /* Read the block; +1 because bitmap block zero is actually
           _user_ block zero, which is physical block 1. */
        if (flashrom_read(start + (i + 1) * 64, buffer, 64) < 0) {
            dbglog(DBG_ERROR, "flashrom_get_block: can't read part %d phys block %d\n", partid, i + 1);
            return FLASHROM_ERR_READ_BLOCK;
        }

        /* Does the block ID match? */
        if (*((uint16 *) buffer) != blockid)
            continue;

        /* Check the checksum to make sure it's valid */
        bmcnt = flashrom_calc_crc(buffer);

        if (bmcnt != *((uint16 *) (buffer + FLASHROM_OFFSET_CRC))) {
            dbglog(DBG_WARNING,
                   "flashrom_get_block: part %d phys block %d has invalid checksum %04x (should be %04x)\n",
                   partid, i + 1, *((uint16 *) (buffer + FLASHROM_OFFSET_CRC)), bmcnt);
            continue;
        }

        /* Ok, looks like we got it! */
        return (i + 1) * 64;
    }

    /* Didn't find anything */
    return FLASHROM_ERR_NOT_FOUND;
}

/* Internal function calculates the checksum of a flashrom block. Thanks
   to Marcus Comstedt for this code. */
int FlashRom::flashrom_calc_crc(const uint8 *buffer) {
    int i, c, n = 0xffff;

    for (i = 0; i < FLASHROM_OFFSET_CRC; i++) {
        n ^= buffer[i] << 8;

        for (c = 0; c < 8; c++) {
            if (n & 0x8000)
                n = (n << 1) ^ 4129;
            else
                n = (n << 1);
        }
    }

    return (~n) & 0xffff;
}

#if 0
uint8 *FlashRom::read(unsigned int offset, int size, std::string err) {

    uint8 *data = nullptr;

    data = (uint8 *) malloc(size);
    if (data == nullptr) {
        printf("FlashRom::read: COULD NOT ALLOCATE MEMORY\n");
        err = "COULD NOT ALLOCATE MEMORY";
        return nullptr;
    }

    if (flashrom_read((int) offset, data, size) < 0) {
        printf("FlashRom::read: COULD NOT READ FROM FLASHROM\n");
        err = "COULD NOT READ FROM FLASHROM";
        return nullptr;
    }

    return data;
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
