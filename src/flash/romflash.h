//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_ROMFLASH_H
#define RETRODREAM_ROMFLASH_H

class RomFlash {

public:

    static uint8 *read(int *error, int partition);

    static int write(int partition, uint8 *data);

    static int findBlockAddress(int partid, int blockid);

    static int crc(const uint8 *data);
};

#endif //RETRODREAM_ROMFLASH_H
