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

    uint32 addr = ISOLDR_DEFAULT_ADDR_LOW;
    isoldr_info_t *isoldr = isoldr_get_info(path, 0);
    if (isoldr == NULL) {
        printf("NOK: isoldr == nullptr\n");
        return -1;
    }

    strncpy(isoldr->fs_dev, "auto", 4);
    isoldr_exec(isoldr, addr);
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
