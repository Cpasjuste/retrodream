//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_ROMFLASH_H
#define RETRODREAM_ROMFLASH_H

#include <string>

#ifdef __DREAMCAST__

#include <arch/types.h>
#include <dc/flashrom.h>
#include "partition.h"

#else
#define FLASHROM_LANG_JAPANESE  0
#define FLASHROM_LANG_ENGLISH   1
#define FLASHROM_LANG_GERMAN    2
#define FLASHROM_LANG_FRENCH    3
#define FLASHROM_LANG_SPANISH   4
#define FLASHROM_LANG_ITALIAN   5
typedef unsigned short uint8;
#endif

#define FLASHROM_PT_ALL 10

#define FLASHROM_ERR_DELETE_PART    -20
#define FLASHROM_ERR_WRITE_PART     -21
#define FLASHROM_ERR_READ_FILE      -22
#define FLASHROM_ERR_WRITE_FILE     -23
#define FLASHROM_ERR_OPEN_FILE      -24

class RomFlash {

public:

    static uint8 *read(int *error, int partition);

    static int write(int partition, uint8 *data);

    static int backup(int partition, const std::string &path);

    static int restore(int partition, const std::string &path);

    static int findBlockAddress(int partid, int blockid);

    static int flashrom_calc_crc(const uint8 *data);
};

#endif //RETRODREAM_ROMFLASH_H
