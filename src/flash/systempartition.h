//
// Created by cpasjuste on 05/03/2020.
//

#ifndef RETRODREAM_SYSTEMPARTITION_H
#define RETRODREAM_SYSTEMPARTITION_H

#include "partition.h"

class SystemPartition : public Partition {

public:

    enum class Country {
        Japan = 0x30,
        Usa = 0x31,
        Europe = 0x32,
        Unknown = 0xFF
    };

    enum class Broadcast {
        Ntsc = 0x30,
        Pal = 0x31,
        PalM = 0x32,
        PalN = 0x33,
        Unknown = 0xFF
    };

    SystemPartition();

    Country getCountry();

    bool setCountry(const Country &country);

    Broadcast getBroadcast();

    bool setBroadcast(const Broadcast &broadcast);
};

#endif //RETRODREAM_SYSTEMPARTITION_H
