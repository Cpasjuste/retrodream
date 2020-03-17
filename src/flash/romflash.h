//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_ROMFLASH_H
#define RETRODREAM_ROMFLASH_H

#include "partition.h"

#ifdef __DREAMCAST__
#include <arch/types.h>
#include <dc/flashrom.h>
#endif

class RomFlash {

public:

    static uint8 *read(int *error, int partition);

    static int write(int partition, uint8 *data);

    static int findBlockAddress(int partid, int blockid);

    static int flashrom_calc_crc(const uint8 *data);
};

#endif //RETRODREAM_ROMFLASH_H
