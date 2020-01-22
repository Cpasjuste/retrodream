//
// Created by cpasjuste on 17/01/2020.
//

#include <stdio.h>

#ifdef __DREAMCAST__

#include <kos.h>
#include <ds/include/isofs/isofs.h>
#include <ds/include/isoldr.h>
#include "ds/include/module.h"

#endif

int run_iso(const char *path) {

#ifdef __DREAMCAST__
    //std::string path = render->getIo()->getRomFsPath() + "modules/minilzo.klf";
    //load_module(path.c_str());
#ifdef __EMBEDDED_MODULE_DEBUG__
    fs_iso_init();

    isoldr_info_t *isoldr = isoldr_get_info(path, 0);
    if (isoldr == NULL) {
        printf("NOK: isoldr == nullptr\n");
        return -1;
    }

    // from dreamshell
    /*
    int use_dma = 1, emu_async = 16, emu_cdda = 0;
    int fastboot = 0;
    int boot_mode = BOOT_MODE_DIRECT;
    int bin_type = BIN_TYPE_AUTO;
    char title[32] = "";
    char device[8] = "auto";
    char memory[12] = "0x8c004000";
    */

    isoldr->use_dma = 1;
    isoldr->emu_async = 16;
    isoldr->emu_cdda = 0;
    isoldr->fast_boot = 0;
    isoldr->boot_mode = 0;
    strncpy(isoldr->fs_type, "0", 8);
    strncpy(isoldr->fs_dev, "auto", 4);
    isoldr_exec(isoldr, 0x8c004000);

    // NOK
    free(isoldr);
    return 0;

#endif
#else
    printf("run_iso(%s): not supported on linux\n", path);
    return -1;
#endif

}

int load_module(const char *path) {
#ifdef __DREAMCAST__
    klibrary_t *mdl = OpenModule(path);
    if (mdl != NULL) {
        printf("DS_OK: Opened module \"%s\"\n", mdl->lib_get_name());
        return 0;
    } else {
        printf("DS_NOK: Could not open module...\n");
        return -1;
    }
#endif
    printf("load_module(%s): not supported on linux\n", path);
    return -1;
}

#ifdef __EMBEDDED_MODULE_DEBUG__
//fs_iso_shutdown();
#endif
