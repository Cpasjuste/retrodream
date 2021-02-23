//
// Created by cpasjuste on 23/02/2021.
//

#ifdef __DREAMCAST__

#include <cstdio>
#include <ds.h>

extern "C" void gdplay_run_game(void *param);

#endif

#include "cross2d/c2d.h"

using namespace c2d;

#include "main.h"

static bool loaded = false;
static bool game = false;

bool GDPlay::check(RetroDream *rd) {
#ifdef __DREAMCAST__
    int status, cd_status, type;

    if ((cd_status = cdrom_get_status(&status, &type)) != ERR_OK) {
        if (cd_status == ERR_DISC_CHG) {
            loaded = false;
            game = false;
            cdrom_reinit();
        }
        return false;
    }

    if (status == CD_STATUS_OPEN || status == CD_STATUS_NO_DISC) {
        loaded = false;
        game = false;
        return false;
    }

    if (!loaded) {
        switch (type) {
            case CD_CDDA:
                rd->showStatus("GDROM", "CDDA MEDIA LOADED...");
                break;
            case CD_GDROM:
            case CD_CDROM_XA:
                rd->showStatus("GDROM", "GDROM MEDIA LOADED...");
                game = true;
                break;
            case CD_CDROM:
                rd->showStatus("GDROM", "CDROM MEDIA LOADED...");
                break;
            case CD_CDI:
                rd->showStatus("GDROM", "CDI MEDIA LOADED...");
                break;
            default:
                rd->showStatus("GDROM", "??? MEDIA LOADED...");
                break;
        }
        cdrom_spin_down();
        loaded = true;
    }

    return isGame();
#else
    return false;
#endif
}

bool GDPlay::isLoaded() {
    return loaded;
}

bool GDPlay::isGame() {
    return loaded && game;
}

bool GDPlay::run(RetroDream *rd) {
#ifdef __DREAMCAST__
    // https://github.com/DC-SWAT/DreamShell/blob/master/applications/gdplay/modules/module.c
    std::string path = rd->getIo()->getDsPath() + "firmware/rungd.bin";
    FILE *f = fopen(path.c_str(), "rb");
    if (!f) {
        printf("GDPlay::run: could not load rungd.bin (%s)\n", path.c_str());
        return false;
    }

    void *bios_patch = memalign(32, 65280);
    fread(bios_patch, 65280, 1, f);
    fclose(f);

    gdplay_run_game(bios_patch);
    return true;
#else
    return false;
#endif
}
