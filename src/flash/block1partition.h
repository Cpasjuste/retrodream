//
// Created by cpasjuste on 05/03/2020.
//

#ifndef RETRODREAM_BLOCK1PARTITION_H
#define RETRODREAM_BLOCK1PARTITION_H

#include "partition.h"

class Block1Partition : public Partition {

public:

    enum class Language {
        Japan = FLASHROM_LANG_JAPANESE,
        English = FLASHROM_LANG_ENGLISH,
        German = FLASHROM_LANG_GERMAN,
        French = FLASHROM_LANG_FRENCH,
        Spanish = FLASHROM_LANG_SPANISH,
        Italian = FLASHROM_LANG_ITALIAN,
        Unknown = 6
    };

    enum class Audio {
        Stereo = 0,
        Mono = 1,
        Unknown = 2
    };

    enum class AutoStart {
        On = 0,
        Off = 1,
        Unknown = 2
    };

    Block1Partition();

    Language getLanguage();

    bool setLanguage(const Language &language);

    Audio getAudio();

    bool setAudio(const Audio &audio);

    AutoStart getAutoStart();

    bool setAutoStart(const AutoStart &autoStart);
};

#endif //RETRODREAM_BLOCK1PARTITION_H
