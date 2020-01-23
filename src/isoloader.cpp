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
#include <kos/md5.h>

extern "C" {
#include "ds/include/isofs/isofs.h"
#include "ds/include/isoldr.h"
#include "ds/include/module.h"
}
#endif

int IsoLoader::run(Io *io, const std::string &path) {

#ifdef __DREAMCAST__
#ifdef __EMBEDDED_MODULE_DEBUG__

    isoldr_info_t *isoLdr = isoldr_get_info(path.c_str(), 0);
    if (isoLdr == nullptr) {
        printf("IsoLoader::run: isoldr_get_info == null\n");
        return -1;
    }

    IsoLoader::Config cfg = IsoLoader::loadConfig(io, path);

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

IsoLoader::Config IsoLoader::loadConfig(Io *io, const std::string &isoPath) {

    IsoLoader::Config config{};

    getConfigInfo(&config, isoPath);

    char *buf = io->read(config.path);
    if (buf == nullptr) {
        printf("IsoLoader::loadConfig: preset not found: %s\n", config.path.c_str());
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

    printf("== IsoLoader::loadConfig ==\ntitle = %s\ndevice = %s\ndma = %d\nasync = %d\n"
           "cdda = %d\nfastboot = %d\ntype = %d\nmode = %d\nmemory = %s\n== IsoLoader::loadConfig ==\n",
           config.title.c_str(), config.device.c_str(), config.dma, config.async,
           config.cdda, config.fastboot, config.type, config.mode, config.memory.c_str());

    free(buf);

    return config;
}

void IsoLoader::saveConfig(c2d::Io *io, const Config &config) {

    char str[1024];

    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str),
             "title = %s\ndevice = %s\ndma = %d\nasync = %d\n"
             "cdda = %d\nfastboot = %d\ntype = %d\nmode = %d\nmemory = %s\n",
             config.title.c_str(), config.device.c_str(), config.dma, config.async,
             config.cdda, config.fastboot, config.type, config.mode, config.memory.c_str());
    io->write(config.path, str);
}

void IsoLoader::getConfigInfo(Config *config, const std::string &isoPath) {

    if (config == nullptr || isoPath.empty()) {
        return;
    }

#ifdef __DREAMCAST__
    uint8 md5[16];
    uint8 boot_sector[2048];

    if (fs_iso_mount("/iso", isoPath.c_str()) != 0) {
        printf("IsoLoader::getBootSectorMd5: could not mound iso: %s\n", isoPath.c_str());
        return;
    }

    file_t fd;
    fd = fs_iso_first_file("/iso");

    if (fd != FILEHND_INVALID) {
        if (fs_ioctl(fd, (int) boot_sector, ISOFS_IOCTL_GET_BOOT_SECTOR_DATA) > -1) {
            kos_md5(boot_sector, sizeof(boot_sector), md5);
        }
        config->title = Utility::trim(((ipbin_meta_t *) boot_sector)->title);
        fs_close(fd);
    }

    fs_iso_unmount("/iso");

    std::string dev;
    size_t pos = isoPath.find('/', 1);
    if (pos != std::string::npos) {
        dev = isoPath.substr(0, pos);
    }
    config->path = dev + "DS/apps/iso_loader/presets/" + dev + "_" + std::string(md5, md5 + 16) + ".cfg";
#else
    config->title = "test";
    config->path = c2d_renderer->getIo()->getDataPath() + "RD/test.cfg";
#endif
}

#if defined (__EMBEDDED_MODULE_DEBUG__)

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
