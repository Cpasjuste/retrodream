//
// Created by cpasjuste on 17/01/2020.
//

#include <kos.h>
#include "ds/include/module.h"

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
}
