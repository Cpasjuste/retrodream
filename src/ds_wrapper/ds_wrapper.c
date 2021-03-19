//
// Created by cpasjuste on 17/01/2020.
//

#include <stdio.h>
#include <kos/dbgio.h>
#include "ds/include/console.h"
#include "ds/include/SDL/SDL_console.h"

void pvr_set_zclip(float zc) {}

int ds_printf(const char *fmt, ...) {

    char buff[512];
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buff, sizeof(buff), fmt, args);
    va_end(args);

    printf("%s", buff);

    return i;
}

void dbgio_set_dev_fb() {
    dbgio_dev_select("fb");
}

ConsoleInformation *GetConsole() {
    return NULL;
}

int CallCmdFile(const char *fn, int argc, char *argv[]) {
    return CMD_ERROR;
}

void LockVideo() {}

void UnlockVideo() {}

void ShutdownVideo() {}

void ShutdownVideoThread() {}

void *IMG_Load(const char *file) {
    return NULL;
}

void SDL_FreeSurface(SDL_Surface *surface) {}

Cmd_t *AddCmd(const char *cmd, const char *helpmsg, CmdHandler *handler) {
    return NULL;
}

void RemoveCmd(Cmd_t *cmd) {}

Cmd_t *GetCmdByName(const char *name) {
    return NULL;
}

#ifdef __EMBEDDED_MODULE_DEBUG__
export_sym_t ds_isofs_symtab[] = {};
export_sym_t ds_isoldr_symtab[] = {};
#endif
