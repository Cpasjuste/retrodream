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

    uint8 language = data[sysCfgAddr + 7];
    if (language < 0 || language > 5) {
        return Language::Unknown;
    }

    return (Language) language;
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

    uint8 audio = data[sysCfgAddr + 8];
    if (audio < 0 || audio > 1) {
        return Audio::Unknown;
    }

    return (Audio) audio;
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

    uint8 autoStart = data[sysCfgAddr + 9];
    if (autoStart < 0 || autoStart > 1) {
        return AutoStart::Unknown;
    }

    return (AutoStart) autoStart;
}

bool Block1Partition::setAutoStart(const Block1Partition::AutoStart &autoStart) {

    if (data == nullptr) {
        return false;
    }

    data[sysCfgAddr + 9] = (uint8) autoStart;

    return true;
}
