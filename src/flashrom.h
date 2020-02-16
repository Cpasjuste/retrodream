//
// Created by cpasjuste on 15/01/2020.
//

#include <string>
#include <arch/types.h>

class FlashRom {

public:

    enum class Country {
        Unknown = 0,
        Europe = 0x32,
        Usa = 0x31,
        Japan = 0x30
    };

    enum class Broadcast {
        Unknown = 0,
        Ntsc = 0x30,
        Pal = 0x31,
        PalM = 0x32,
        PalN = 0x33
    };

    enum class Language {
        Unknown = 0,
        Japan = 0x30,
        English = 0x31,
        German = 0x32,
        French = 0x33,
        Spanish = 0x34,
        Italian = 0x35
    };

    struct FactorySetting {
        Country country = Country::Unknown;
        Broadcast broadcast = Broadcast::Unknown;
        Language language = Language::Unknown;
        std::string error;
    };

    static FactorySetting getFactorySetting();

    static int setFactorySetting(const FactorySetting &setting, std::string err);

    static uint8 *read(unsigned int offset, int size, std::string err);

    static int write(unsigned int offset, int size, uint8 *data, std::string err);

    static int backup(const std::string &path, std::string err);

    static int restore(const std::string &path, std::string err);

private:

    //static int erase(int block, std::string err);
    //static int writeFromFile(const std::string &path, std::string err);
    //static int readToFile(const std::string &path, std::string err);
};
