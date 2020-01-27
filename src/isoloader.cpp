//
// Created by cpasjuste on 17/01/2020.
//

#include <cstdlib>
#include "cross2d/c2d.h"
#include "main.h"
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

int IsoLoader::run(RetroDream *retroDream, const std::string &path) {

#ifdef __DREAMCAST__
#ifdef __EMBEDDED_MODULE_DEBUG__

    isoldr_info_t *isoLdr = isoldr_get_info(path.c_str(), 0);
    if (isoLdr == nullptr) {
        printf("IsoLoader::run: isoldr_get_info == null\n");
        return -1;
    }

    IsoLoader::Config cfg = IsoLoader::loadConfig(retroDream, path);

    isoLdr->use_dma = cfg.dma;
    isoLdr->emu_cdda = cfg.cdda;
    isoLdr->emu_async = cfg.async;
    isoLdr->fast_boot = cfg.fastboot;
    isoLdr->boot_mode = cfg.mode;
    if (cfg.type != BIN_TYPE_AUTO) {
        isoLdr->exec.type = cfg.type;
    }

    // find loader path
    std::string loaderPath;
    if (cfg.device == std::string("auto")) {
        std::string p = RetroUtility::findPath(retroDream->getRender()->getIo(),
                                               "DS/firmware/isoldr/ide.bin");
        if (!p.empty()) {
            printf("IsoLoader::run: loaderPath: %s\n", loaderPath.c_str());
            loaderPath = Utility::remove(p, "/firmware/isoldr/ide.bin");
            strncpy(isoLdr->fs_dev, "ide", 7);
        }
    } else {
        std::string p = RetroUtility::findPath(retroDream->getRender()->getIo(),
                                               "DS/firmware/isoldr/" + cfg.device + ".bin");
        if (!p.empty()) {
            printf("IsoLoader::run: loaderPath: %s\n", loaderPath.c_str());
            loaderPath = Utility::remove(p, "/firmware/isoldr/" + cfg.device + ".bin");
            strncpy(isoLdr->fs_dev, cfg.device.c_str(), 7);
        }
    }
    setenv("PATH", loaderPath.c_str(), 1);

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

IsoLoader::Config IsoLoader::loadConfig(RetroDream *retroDream, const std::string &isoPath) {

    IsoLoader::Config config{};

    getConfigInfo(retroDream, &config, isoPath);

    char *buf = retroDream->getRender()->getIo()->read(config.path);
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
           "cdda = %d\nfastboot = %d\ntype = %d\nmode = %d\nmemory = %s\n\n",
           config.title.c_str(), config.device.c_str(), config.dma, config.async,
           config.cdda, config.fastboot, config.type, config.mode, config.memory.c_str());

    free(buf);

    return config;
}

void IsoLoader::saveConfig(RetroDream *retroDream, const Config &config) {

    char str[512];
    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str),
             "title = %s\ndevice = %s\ndma = %d\nasync = %d\n"
             "cdda = %d\nfastboot = %d\ntype = %d\nmode = %d\nmemory = %s\n",
             config.title.c_str(), config.device.c_str(), config.dma, config.async,
             config.cdda, config.fastboot, config.type, config.mode, config.memory.c_str());
    retroDream->getRender()->getIo()->write(config.path, str);
}

void IsoLoader::getConfigInfo(RetroDream *retroDreamn, Config *config, const std::string &isoPath) {

    if (config == nullptr || isoPath.empty()) {
        return;
    }

#ifdef __DREAMCAST__
    printf("IsoLoader::getConfigInfo: %s\n", isoPath.c_str());

    uint8 md5[16];
    uint8 boot_sector[2048];

    printf("IsoLoader::getConfigInfo: fs_iso_mount\n");
    if (fs_iso_mount("/iso", isoPath.c_str()) != 0) {
        printf("IsoLoader::getConfigInfo: could not mound iso: %s\n", isoPath.c_str());
        return;
    }

    printf("IsoLoader::getConfigInfo: fs_iso_first_file\n");
    file_t fd;
    fd = fs_iso_first_file("/iso");

    if (fd != FILEHND_INVALID) {
        printf("IsoLoader::getConfigInfo: fs_ioctl\n");
        if (fs_ioctl(fd, (int) boot_sector, ISOFS_IOCTL_GET_BOOT_SECTOR_DATA) > -1) {
            kos_md5(boot_sector, sizeof(boot_sector), md5);
        }
        config->title = Utility::trim(((ipbin_meta_t *) boot_sector)->title);
        fs_close(fd);
    }

    printf("IsoLoader::getConfigInfo: fs_iso_unmount\n");
    fs_iso_unmount("/iso");

    std::string device;
    size_t pos = isoPath.find('/', 1);
    if (pos != std::string::npos) {
        device = isoPath.substr(0, pos);
    }

    // TODO: do this better
    char _md5[64];
    snprintf(_md5, 64, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8],
             md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);

    config->path = retroDreamn->getConfig()->get(RetroConfig::DsPath)
                   + "apps/iso_loader/presets" + device + "_" + _md5 + ".cfg";

    printf("IsoLoader::getConfigInfo: config->path: %s\n", config->path.c_str());
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
