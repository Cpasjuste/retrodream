//
// Created by cpasjuste on 15/01/2020.
//
#ifdef __DREAMCAST__

#include <kos.h>
#include <kos/md5.h>
#include "flashrom.h"

int FlashRom::getRegionSettings(RegionSettings *settings) {

    // read factory partition
    settings->partitionSystem = read(&settings->error, FLASHROM_PT_SYSTEM);
    if (settings->error != 0 || settings->partitionSystem == nullptr) {
        return settings->error;
    }
#ifndef NDEBUG
    uint8 md5[16];
    kos_md5(settings->partitionSystem, 0x00002000, md5);
    printf("partitionSystem md5: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x, magic: %.14s\n",
           md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8],
           md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15], settings->partitionSystem);
#endif
    // set country and broadcast
    settings->country = (Country) settings->partitionSystem[2];
    settings->broadcast = (Broadcast) settings->partitionSystem[4];

    return 0;
}

int FlashRom::saveRegionSettings(RegionSettings *settings) {

    if (settings == nullptr || settings->partitionSystem == nullptr) {
        return FLASHROM_ERR_NOMEM;
    }

    // update system partition data
    settings->partitionSystem[2] = (uint8) settings->country;
    settings->partitionSystem[4] = (uint8) settings->broadcast;
    int res = write(FLASHROM_PT_SYSTEM, settings->partitionSystem);
    if (res != 0) {
        return res;
    }

    return 0;
}

int FlashRom::getSystemSettings(SystemSettings *settings) {

    // read block 1 partition
    settings->partitionBlock1 = read(&settings->error, FLASHROM_PT_BLOCK_1);
    if (settings->error != 0 || settings->partitionBlock1 == nullptr) {
        return settings->error;
    }
#ifndef NDEBUG
    uint8 md5[16];
    kos_md5(settings->partitionBlock1, 0x00004000, md5);
    printf("partitionBlock1 md5: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x, magic: %.14s\n",
           md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8],
           md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15], settings->partitionBlock1);
#endif

    // find FLASHROM_B1_SYSCFG block address
    settings->partitionBlock1SysCfg = findBlockAddress(FLASHROM_PT_BLOCK_1, FLASHROM_B1_SYSCFG);
    if (settings->partitionBlock1SysCfg < 0) {
        settings->error = settings->partitionBlock1SysCfg;
        return settings->error;
    }
    // set language offset in FLASHROM_B1_SYSCFG
    settings->partitionBlock1SysCfgLanguage = settings->partitionBlock1SysCfg + 7;
    // set options
    settings->language = (Language) settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage];
    settings->audio = (Audio) settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage + 1];
    settings->autoStart = (AutoStart) settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage + 2];

    return 0;
}

int FlashRom::saveSystemSettings(SystemSettings *settings) {

    if (settings == nullptr || settings->partitionBlock1 == nullptr) {
        return FLASHROM_ERR_NOMEM;
    }

    // update block1 partition data
    settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage] = (uint8) settings->language;
    settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage + 1] = (uint8) settings->audio;
    settings->partitionBlock1[settings->partitionBlock1SysCfgLanguage + 2] = (uint8) settings->autoStart;
    // update block1 SysCfg block crc
    *((uint16 *) (settings->partitionBlock1 + settings->partitionBlock1SysCfg + FLASHROM_OFFSET_CRC))
            = (uint16) flashrom_calc_crc(settings->partitionBlock1 + settings->partitionBlock1SysCfg);
    int res = write(FLASHROM_PT_BLOCK_1, settings->partitionBlock1);
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
        printf("FlashRom::read: FLASHROM_ERR_NO_PARTITION\n");
        return nullptr;
    }

    printf("FlashRom::read(%i): start = 0x%08X, size = 0x%08X\n",
           partition, start, size);

    data = (uint8 *) malloc(size);
    if (data == nullptr) {
        if (error != nullptr) {
            *error = FLASHROM_ERR_NOMEM;
        }
        printf("FlashRom::read: FLASHROM_ERR_NOMEM\n");
        return nullptr;
    }

    if (flashrom_read(start, data, size) < 0) {
        free(data);
        if (error != nullptr) {
            *error = FLASHROM_ERR_READ_PART;
        }
        printf("FlashRom::read: FLASHROM_ERR_READ_PART\n");
        return nullptr;
    }

    return data;
}

int FlashRom::write(int partition, uint8 *data) {

    int start, size;

    if (flashrom_info(partition, &start, &size) != 0) {
        printf("FlashRom::write: FLASHROM_ERR_NO_PARTITION\n");
        return FLASHROM_ERR_NO_PARTITION;
    }

    if (flashrom_delete(start) != 0) {
        printf("FlashRom::write: FLASHROM_ERR_DELETE_PART\n");
        return FLASHROM_ERR_DELETE_PART;
    }

    if (flashrom_write(start, data, size) < 0) {
        printf("FlashRom::write: FLASHROM_ERR_WRITE_PART\n");
        return FLASHROM_ERR_WRITE_PART;
    }

#ifndef NDEBUG
    char wrote_magic[15];
    flashrom_read(start, wrote_magic, 15);
    printf("FlashRom::write(%i): start = 0x%08X, size = 0x%08X, data magic: %.14s, wrote magic: %.14s\n",
           partition, start, size, data, wrote_magic);
#endif

    return 0;
}

int FlashRom::backup(int partition, const std::string &path) {

    file_t fd;
    uint8 *data = nullptr;
    int start = 0, size = 0x20000;

    if (partition != FLASHROM_PT_ALL) {
        if (flashrom_info(partition, &start, &size) < 0) {
            printf("FlashRom::backup: FLASHROM_ERR_NO_PARTITION\n");
            return FLASHROM_ERR_NO_PARTITION;
        }
    }

    fd = fs_open(path.c_str(), O_WRONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::backup: FLASHROM_ERR_OPEN_FILE\n");
        return FLASHROM_ERR_OPEN_FILE;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        fs_close(fd);
        printf("FlashRom::backup: FLASHROM_ERR_NOMEM\n");
        return FLASHROM_ERR_NOMEM;
    }

    if (flashrom_read(start, data, size) < 0) {
        fs_close(fd);
        free(data);
        printf("FlashRom::backup: FLASHROM_ERR_READ_PART\n");
        return FLASHROM_ERR_READ_PART;
    }

    if (fs_write(fd, data, size) < 0) {
        fs_close(fd);
        free(data);
        printf("FlashRom::backup: FLASHROM_ERR_WRITE_FILE\n");
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
            printf("FlashRom::restore: FLASHROM_ERR_NO_PARTITION\n");
            return FLASHROM_ERR_NO_PARTITION;
        }
    }

    fd = fs_open(path.c_str(), O_RDONLY);
    if (fd == FILEHND_INVALID) {
        printf("FlashRom::restore: FLASHROM_ERR_OPEN_FILE\n");
        return FLASHROM_ERR_OPEN_FILE;
    }

    data = (uint8 *) memalign(32, size);
    if (data == nullptr) {
        fs_close(fd);
        printf("FlashRom::restore: FLASHROM_ERR_NOMEM\n");
        return FLASHROM_ERR_NOMEM;
    }

    if (fs_read(fd, data, size) < 0) {
        fs_close(fd);
        free(data);
        printf("FlashRom::restore: FLASHROM_ERR_READ_FILE\n");
        return FLASHROM_ERR_READ_FILE;
    }

    if (flashrom_write(start, data, size) < 0) {
        fs_close(fd);
        free(data);
        printf("FlashRom::restore: FLASHROM_ERR_WRITE_PART\n");
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
    if (flashrom_info(partid, &start, &size) != 0) {
        printf("FlashRom::findBlockAddress: FLASHROM_ERR_NO_PARTITION\n");
        return FLASHROM_ERR_NO_PARTITION;
    }

    /* Verify the partition */
    if (flashrom_read(start, magic, 18) < 0) {
        printf("FlashRom::findBlockAddress: can't read part %d magic\n", partid);
        return FLASHROM_ERR_READ_PART;
    }

    if (strncmp(magic, "KATANA_FLASH____", 16) != 0 || *((uint16 *) (magic + 16)) != partid) {
        bmcnt = *((uint16 *) (magic + 16));
        magic[16] = 0;
        printf("FlashRom::findBlockAddress: invalid magic '%s' or id %d in part %d\n", magic, bmcnt, partid);
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
        printf("FlashRom::findBlockAddress: bogus part %p/%d\n", (void *) start, size);
        return FLASHROM_ERR_BOGUS_PART;
    }

    if ((bitmap = (uint8 *) malloc(bmcnt)) == nullptr) {
        printf("FlashRom::findBlockAddress: FLASHROM_ERR_NOMEM\n");
        return FLASHROM_ERR_NOMEM;
    }

    if (flashrom_read(start + size - bmcnt, bitmap, bmcnt) < 0) {
        printf("FlashRom::findBlockAddress: can't read part %d bitmap\n", partid);
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
    if (i == 0) {
        printf("FlashRom::findBlockAddress: FLASHROM_ERR_EMPTY_PART\n");
        return FLASHROM_ERR_EMPTY_PART;
    }

    i--;    /* 'i' was the first unused block, so back up one */

    for (; i > 0; i--) {
        /* Read the block; +1 because bitmap block zero is actually
           _user_ block zero, which is physical block 1. */
        if (flashrom_read(start + (i + 1) * 64, buffer, 64) < 0) {
            printf("FlashRom::findBlockAddress: can't read part %d phys block %d\n", partid, i + 1);
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
        printf("FlashRom::findBlockAddress: block found, crc: %04x\n", bmcnt);
        return (i + 1) * 64;
    }

    /* Didn't find anything */
    printf("FlashRom::findBlockAddress: FLASHROM_ERR_NOT_FOUND\n");
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
