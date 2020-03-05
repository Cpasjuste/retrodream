//
// Created by cpasjuste on 05/03/2020.
//

#include "systempartition.h"

SystemPartition::SystemPartition() {
    type = FLASHROM_PT_SYSTEM;
    size = 0x00002000;
}

SystemPartition::Country SystemPartition::getCountry() {

    if (data == nullptr) {
        return Country::Unknown;
    }

    return (Country) data[2];
}

bool SystemPartition::setCountry(const SystemPartition::Country &country) {

    if (data == nullptr) {
        return false;
    }

    data[2] = (uint8) country;

    return true;
}

SystemPartition::Broadcast SystemPartition::getBroadcast() {

    if (data == nullptr) {
        return Broadcast::Unknown;
    }

    return (Broadcast) data[4];
}

bool SystemPartition::setBroadcast(const SystemPartition::Broadcast &broadcast) {

    if (data == nullptr) {
        return false;
    }

    data[4] = (uint8) broadcast;

    return true;
}
