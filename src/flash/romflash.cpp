//
// Created by cpasjuste on 15/01/2020.
//
#ifdef __DREAMCAST__

#include <kos.h>
#include <kos/md5.h>

#endif

#include <cstring>
#include "romflash.h"
#include "partition.h"

uint8 *RomFlash::read(int *error, int partition) {

    int start, size;
    uint8 *data = nullptr;

    if (flashrom_info(partition, &start, &size) != 0) {
        if (error != nullptr) {
            *error = FLASHROM_ERR_NO_PARTITION;
        }
        printf("FlashRom::read: FLASHROM_ERR_NO_PARTITION\n");
        return nullptr;
    }

#ifndef NDEBUG
    printf("FlashRom::read(%i): start = 0x%08X, size = 0x%08X\n", partition, start, size);
#endif

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

int RomFlash::write(int partition, uint8 *data) {

    int start, size;

    if (flashrom_info(partition, &start, &size) != 0) {
        printf("FlashRom::write: FLASHROM_ERR_NO_PARTITION\n");
        return FLASHROM_ERR_NO_PARTITION;
    }

#ifndef NDEBUG
    printf("FlashRom::write(%i): start = 0x%08X, size = 0x%08X\n", partition, start, size);
#endif

    if (flashrom_delete(start) != 0) {
        printf("FlashRom::write: FLASHROM_ERR_DELETE_PART\n");
        return FLASHROM_ERR_DELETE_PART;
    }

    if (flashrom_write(start, data, size) < 0) {
        printf("FlashRom::write: FLASHROM_ERR_WRITE_PART\n");
        return FLASHROM_ERR_WRITE_PART;
    }

    return 0;
}

int RomFlash::findBlockAddress(int partid, int blockid) {

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
        bmcnt = crc(buffer);

        if (bmcnt != *((uint16 *) (buffer + FLASHROM_OFFSET_CRC))) {
            printf("flashrom_get_block: part %d phys block %d has invalid checksum %04x (should be %04x)\n",
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
int RomFlash::crc(const uint8 *data) {
    int i, c, n = 0xffff;

    for (i = 0; i < FLASHROM_OFFSET_CRC; i++) {
        n ^= data[i] << 8;

        for (c = 0; c < 8; c++) {
            if (n & 0x8000)
                n = (n << 1) ^ 4129;
            else
                n = (n << 1);
        }
    }

    return (~n) & 0xffff;
}

