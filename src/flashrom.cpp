//
// Created by cpasjuste on 15/01/2020.
//
#ifdef __DREAMCAST__

#include <kos.h>
#include "flashrom.h"

FlashRom::Settings FlashRom::getSettings() {

    Settings settings{};

    // read factory partition
    settings.partitionSystem = read(&settings.error, FLASHROM_PT_SYSTEM);
    if (settings.error != 0 || settings.partitionSystem == nullptr) {
        return settings;
    }
    // set country and broadcast
    settings.country = (Country) settings.partitionSystem[2];
    settings.broadcast = (Broadcast) settings.partitionSystem[4];

    // read block 1 partition
    settings.partitionBlock1 = read(&settings.error, FLASHROM_PT_BLOCK_1);
    if (settings.error != 0 || settings.partitionBlock1 == nullptr) {
        return settings;
    }
    // find FLASHROM_B1_SYSCFG block address
    settings.partitionBlock1LanguageOffset = findBlockAddress(FLASHROM_PT_BLOCK_1, FLASHROM_B1_SYSCFG);
    if (settings.partitionBlock1LanguageOffset < 0) {
        settings.error = settings.partitionBlock1LanguageOffset;
        return settings;
    }
    // set language offset in FLASHROM_B1_SYSCFG
    settings.partitionBlock1LanguageOffset += 7;
    // set language
    settings.language = (Language) settings.partitionBlock1[settings.partitionBlock1LanguageOffset];

    return settings;
}

int FlashRom::saveSettings(const FlashRom::Settings &settings) {

    if (settings.partitionSystem == nullptr || settings.partitionBlock1 == nullptr) {
        return FLASHROM_ERR_NOMEM;
    }

    settings.partitionSystem[2] = (uint8) settings.country;
    settings.partitionSystem[4] = (uint8) settings.broadcast;
    settings.partitionBlock1[settings.partitionBlock1LanguageOffset] = (uint8) settings.language;

    int res = write(FLASHROM_PT_SYSTEM, settings.partitionSystem);
    if (res != 0) {
        return res;
    }

    res = write(FLASHROM_PT_BLOCK_1, settings.partitionBlock1);
    if (res != 0) {
        return res;
    }

    return 0;
}

uint8 *FlashRom::read(int *error, int partition) {

    int start, size;
    uint8 *data = nullptr;

    if (flashrom_info(partition, &start, &size) != 0) {
        if (error != nullptr) {
            *error = FLASHROM_ERR_NO_PARTITION;
        }
        return nullptr;
    }

    data = (uint8 *) malloc(size);
    if (data == nullptr) {
        if (error != nullptr) {
            *error = FLASHROM_ERR_NOMEM;
        }
        return nullptr;
    }

    if (flashrom_read(start, data, size) < 0) {
        free(data);
        if (error != nullptr) {
            *error = FLASHROM_ERR_READ_PART;
        }
        return nullptr;
    }

    return data;
}

int FlashRom::write(int partition, uint8 *data) {

    int start, size;

    if (flashrom_info(partition, &start, &size) != 0) {
        return FLASHROM_ERR_NO_PARTITION;
    }

    if (flashrom_delete(start) != 0) {
        return FLASHROM_ERR_DELETE_PART;
    }

    if (flashrom_write(start, data, size) < 0) {
        return FLASHROM_ERR_WRITE_PART;
    }

    return 0;
}

int FlashRom::backup(int partition, const std::string &path) {

    file_t fd;
    uint8 *data = nullptr;
    int start = 0, size = 0x20000;

    if (partition != FLASHROM_PT_ALL) {
        if (flashrom_info(partition, &start, &size) < 0) {
            return FLASHROM_ERR_NO_PARTITION;
        }
    }

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        return FLASHROM_ERR_OPEN_FILE;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        fs_close(fd);
        return FLASHROM_ERR_NOMEM;
    }

    if (flashrom_read(start, data, size) < 0) {
        fs_close(fd);
        free(data);
        return FLASHROM_ERR_READ_PART;
    }

    if (fs_write(fd, data, size) < 0) {
        fs_close(fd);
        free(data);
        return FLASHROM_ERR_WRITE_FILE;
    }

    fs_close(fd);
    free(data);

    return 0;
}

int FlashRom::restore(int partition, const std::string &path) {

    file_t fd;
    uint8 *data = nullptr;
    int start = 0, size = 0x20000;

    if (partition != FLASHROM_PT_ALL) {
        if (flashrom_info(partition, &start, &size) < 0) {
            return FLASHROM_ERR_NO_PARTITION;
        }
    }

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        return FLASHROM_ERR_OPEN_FILE;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        fs_close(fd);
        return FLASHROM_ERR_NOMEM;
    }

    if (fs_read(fd, data, size) < 0) {
        fs_close(fd);
        free(data);
        return FLASHROM_ERR_READ_FILE;
    }

    if (flashrom_write(start, data, size) < 0) {
        fs_close(fd);
        free(data);
        return FLASHROM_ERR_WRITE_PART;
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

#endif
