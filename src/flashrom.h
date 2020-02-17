//
// Created by cpasjuste on 15/01/2020.
//

#include <string>
#include <arch/types.h>

class FlashRom {

public:

    enum class Country {
        Unknown = 0x99,
        Europe = 0x32,
        Usa = 0x31,
        Japan = 0x30
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

    struct Settings {
        Country country = Country::Unknown;
        Broadcast broadcast = Broadcast::Unknown;
        Language language = Language::Unknown;
        std::string error;
    };

    static Settings getSettings();

    static int setSettings(const Settings &setting, std::string err);

    static uint8 *read(int partition, unsigned int offset, int size, std::string err);

    static int write(int partition, unsigned int offset, int size, uint8 *data, std::string err);

    static int backup(int partition, const std::string &path, std::string err);

    static int restore(int partition, const std::string &path, std::string err);

    static int backupAll(const std::string &path, std::string err);

    static int restoreAll(const std::string &path, std::string err);

private:

    static int findBlockAddress(int partid, int blockid);

    static int flashrom_calc_crc(const uint8 *buffer);
};
