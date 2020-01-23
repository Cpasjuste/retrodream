//
// Created by cpasjuste on 17/01/2020.
//

#ifndef RETRODREAM_ISOLOADER_H
#define RETRODREAM_ISOLOADER_H

#include <string>

class IsoLoader {

public:

    struct Config {
        int dma = 0;
        int cdda = 0;
        int async = 0;
        int mode = 0;
        int type = 0;
        int fastboot = 0;
        std::string memory = "0x8c004000";
        std::string title;
        std::string device = "auto";
        std::string pa1;
        std::string pv1;
        std::string pa2;
        std::string pv2;
        std::string path;
    };

    static int run(c2d::Io *io, const std::string &path);

    static Config loadConfig(c2d::Io *io, const std::string &isoPath);

    static void saveConfig(c2d::Io *io, const Config &config);

#if defined (__EMBEDDED_MODULE_DEBUG__)

    static int loadModule(const std::string &module);

#endif

private:
    static void getConfigInfo(Config *config, const std::string &isoPath);

};

#endif //RETRODREAM_ISOLOADER_H
