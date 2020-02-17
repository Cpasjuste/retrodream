//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_FLASHROM_H
#define RETRODREAM_FLASHROM_H

#include <string>

#ifdef __DREAMCAST__
#include <arch/types.h>
#include <dc/flashrom.h>
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

class FlashRom {

public:

    enum class Country {
        Unknown = 0x99,
        Japan = 0x30,
        Usa = 0x31,
        Europe = 0x32,
    };

    enum class Broadcast {
        Unknown = 0x99,
        Ntsc = 0x30,
        Pal = 0x31,
        PalM = 0x32,
        PalN = 0x33
    };

    enum class Language {
        Unknown = 0x99,
        Japan = FLASHROM_LANG_JAPANESE,
        English = FLASHROM_LANG_ENGLISH,
        German = FLASHROM_LANG_GERMAN,
        French = FLASHROM_LANG_FRENCH,
        Spanish = FLASHROM_LANG_SPANISH,
        Italian = FLASHROM_LANG_ITALIAN
    };

    class Settings {
    public:
        ~Settings() {
            if (partitionSystem != nullptr) {
                free(partitionSystem);
            }
            if (partitionBlock1 != nullptr) {
                free(partitionBlock1);
            }
        }

        Country country = Country::Unknown;
        Broadcast broadcast = Broadcast::Unknown;
        Language language = Language::Unknown;
        uint8 *partitionSystem = nullptr;
        uint8 *partitionBlock1 = nullptr;
        int partitionBlock1LanguageOffset = 0;
        int error = 0;
    };

    static Settings getSettings();

    static int setSettings(const Settings &setting, std::string err);

    static uint8 *read(int *error, int partition);

    static int write(int partition, uint8 *data);

    static int backup(int partition, const std::string &path);

    static int restore(int partition, const std::string &path);

private:

    static int findBlockAddress(int partid, int blockid);

    static int flashrom_calc_crc(const uint8 *buffer);
};

#endif // RETRODREAM_FLASHROM_H
