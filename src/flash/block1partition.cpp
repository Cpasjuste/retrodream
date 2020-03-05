//
// Created by cpasjuste on 05/03/2020.
//

#include "block1partition.h"

Block1Partition::Block1Partition() {
    type = FLASHROM_PT_BLOCK_1;
    size = 0x00004000;
}

Block1Partition::Language Block1Partition::getLanguage() {

    if (data == nullptr) {
        return Language::Unknown;
    }

    return (Language) data[sysCfgAddr + 7];
}

bool Block1Partition::setLanguage(const Block1Partition::Language &language) {

    if (data == nullptr) {
        return false;
    }

    data[sysCfgAddr + 7] = (uint8) language;

    return true;
}

Block1Partition::Audio Block1Partition::getAudio() {

    if (data == nullptr) {
        return Audio::Unknown;
    }

    return (Audio) data[sysCfgAddr + 8];
}

bool Block1Partition::setAudio(const Block1Partition::Audio &audio) {

    if (data == nullptr) {
        return false;
    }

    data[sysCfgAddr + 8] = (uint8) audio;

    return true;
}

Block1Partition::AutoStart Block1Partition::getAutoStart() {

    if (data == nullptr) {
        return AutoStart::Unknown;
    }

    return (AutoStart) data[sysCfgAddr + 9];
}

bool Block1Partition::setAutoStart(const Block1Partition::AutoStart &autoStart) {

    if (data == nullptr) {
        return false;
    }

    data[sysCfgAddr + 9] = (uint8) autoStart;

    return true;
}
