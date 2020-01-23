//
// Created by cpasjuste on 17/01/2020.
//

#include <cstdlib>
#include "cross2d/c2d.h"
#include "isoloader.h"
#include "utility.h"

using namespace c2d;

#ifdef __DREAMCAST__

#include <kos.h>

extern "C" {
#include "ds/include/isofs/isofs.h"
#include "ds/include/isoldr.h"
#include "ds/include/module.h"
}
#endif

int IsoLoader::run(Io *io, const std::string &path) {

#ifdef __DREAMCAST__
#ifdef __EMBEDDED_MODULE_DEBUG__
    fs_iso_init();

    isoldr_info_t *isoLdr = isoldr_get_info(path.c_str(), 0);
    if (isoLdr == nullptr) {
        printf("NOK: isoldr_get_info == null\n");
        return -1;
    }

    // TODO: load preset
    IsoLoader::Config cfg =
            IsoLoader::load(io,
                            "/home/cpasjuste/Téléchargements/DS/apps/iso_loader/presets/ide_0b06bed5916d5580723206ecb26da7f3.cfg");

    isoLdr->use_dma = cfg.dma;
    isoLdr->emu_cdda = cfg.cdda;
    isoLdr->emu_async = cfg.async;
    isoLdr->fast_boot = cfg.fastboot;
    isoLdr->boot_mode = cfg.mode;
    isoLdr->exec.type = cfg.type;

    // find loader path
    if (cfg.device == std::string("auto")) {
        setenv("PATH",
               RetroUtility::findPath(io, "firmware/isoldr/ide.bin").c_str(), 1);
    } else {
        setenv("PATH",
               RetroUtility::findPath(io, "firmware/isoldr/" + cfg.device + ".bin").c_str(), 1);
    }

    isoldr_exec(isoLdr, strtoul(cfg.memory.c_str(), nullptr, 16));

    // NOK
    free(isoLdr);
    fs_iso_shutdown();

    return 0;
#else
    // TODO: fix exports
    //std::string path = render->getIo()->getRomFsPath() + "modules/minilzo.klf";
    //load_module(path.c_str());
#endif
#else
    printf("run_iso(%s): not supported on linux\n", path.c_str());
    return -1;
#endif

}

IsoLoader::Config IsoLoader::load(Io *io, const std::string &path) {

    char *buf = nullptr;
    IsoLoader::Config config{};

    buf = io->read(path);
    if (buf == nullptr) {
        printf("IsoLoader::parseConf: preset not found: %s\n", path.c_str());
        return config;
    }

    std::string strBuf = std::string(buf);
    std::vector<std::string> lines = c2d::Utility::split(strBuf, "\n");
    for (auto const &line : lines) {
        std::vector<std::string> option = c2d::Utility::split(line, "=");
        if (option.size() != 2) {
            continue;
        }
        std::string value = Utility::trim(option[1]);
        if (Utility::startWith(line, "title")) {
            config.title = value;
        } else if (Utility::startWith(line, "device")) {
            config.device = value;
        } else if (Utility::startWith(line, "dma")) {
            config.dma = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "async")) {
            config.async = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "cdda")) {
            config.cdda = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "fastboot")) {
            config.fastboot = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "type")) {
            config.type = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "mode")) {
            config.mode = Utility::parseInt(value, 0);
        } else if (Utility::startWith(line, "memory")) {
            config.memory = value;
        }
    }

    printf("== IsoLoader::load ==\ntitle = %s\ndevice = %s\ndma = %d\nasync = %d\n"
           "cdda = %d\nfastboot = %d\ntype = %d\nmode = %d\nmemory = %s\n== IsoLoader::load ==\n",
           config.title.c_str(), config.device.c_str(), config.dma, config.async,
           config.cdda, config.fastboot, config.type, config.mode, config.memory.c_str());

    return config;
}

#if !defined (__EMBEDDED_MODULE_DEBUG__)
int IsoLoader::loadModule(const std::string &module) {
#ifdef __DREAMCAST__
    klibrary_t *mdl = OpenModule(module.c_str());
    if (mdl != nullptr) {
        printf("DS_OK: Opened module \"%s\"\n", mdl->lib_get_name());
        return 0;
    } else {
        printf("DS_NOK: Could not open module...\n");
        return -1;
    }
#else
    printf("load_module(%s): not supported on linux\n", module.c_str());
    return -1;
#endif
}
#endif
