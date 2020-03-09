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

    uint8 country = data[2];
    if (country < 0x30 || country > 0x32) {
        return Country::Unknown;
    }

    return (Country) country;
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

    uint8 broadcast = data[4];
    if (broadcast < 0x30 || broadcast > 0x33) {
        return Broadcast::Unknown;
    }

    return (Broadcast) broadcast;
}

bool SystemPartition::setBroadcast(const SystemPartition::Broadcast &broadcast) {

    if (data == nullptr) {
        return false;
    }

    data[4] = (uint8) broadcast;

    return true;
}
