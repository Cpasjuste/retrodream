//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "regionfreemenu.h"

using namespace c2d;

RegionFreeMenu::RegionFreeMenu(RetroDream *rd, const c2d::FloatRect &rect) : Menu(rd, rect) {

    retroDream = rd;

    title->setString("REGION CHANGER OPTIONS");

    config.addOption({"COUNTRY", {"JAPAN", "USA", "EUROPE"}, 0, Country});
    config.addOption({"BROADCAST", {"NTSC", "PAL", "PAL-M", "PAL-N"}, 0, Broadcast});
    configBox->load(&config);
}

void RegionFreeMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    Menu::setVisibility(visibility, tweenPlay);

    if (visibility == Visibility::Visible) {
#ifdef NDEBUG
        // backup flashrom if needed, this is fast enough to not show any message
        std::string flashBackup = retroDream->getConfig()->getBootDevice() + "RD/system.rom";
        if (!retroDream->getRender()->getIo()->exist(flashBackup)) {
            RomFlash::backup(FLASHROM_PT_SYSTEM, flashBackup);
            // verify backup
            char *magic = retroDream->getRender()->getIo()->read(flashBackup, 5, 10);
            if (magic == nullptr) {
                Menu::setVisibility(Visibility::Hidden, true);
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
                retroDream->showStatus("REGION CHANGER ERROR", "YOUR SYSTEM FLASHROM PARTITION CAN'T BE READ");
                retroDream->getRender()->getIo()->removeFile(flashBackup);
                return;
            }
            magic[9] = '\0';
            if (magic != std::string("Dreamcast")) {
                Menu::setVisibility(Visibility::Hidden, true);
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
                retroDream->showStatus("REGION CHANGER ERROR", "YOUR SYSTEM FLASHROM PARTITION IS CORRUPTED");
                free(magic);
                retroDream->getRender()->getIo()->removeFile(flashBackup);
                return;
            }
            free(magic);
        }
#endif
        retroDream->getFiler()->setSelectionBack();
#ifdef __DREAMCAST__
        RomFlash::getRegionSettings(&settings);
#else
        settings.country = FlashRom::Country::Usa;
        settings.broadcast = FlashRom::Broadcast::Ntsc;
        settings.error = -1;
#endif
        if (settings.error != 0) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("REGION CHANGER ERROR", "AN ERROR OCCURRED WHILE TRYING TO READ YOUR FLASHROM");
            return;
        }
        // country
        if (settings.country == RomFlash::Country::Japan) {
            config.getOption(Country)->setChoicesIndex(0);
        } else if (settings.country == RomFlash::Country::Usa) {
            config.getOption(Country)->setChoicesIndex(1);
        } else if (settings.country == RomFlash::Country::Europe) {
            config.getOption(Country)->setChoicesIndex(2);
        }
        // broadcast
        if (settings.broadcast == RomFlash::Broadcast::Ntsc) {
            config.getOption(Broadcast)->setChoicesIndex(0);
        } else if (settings.broadcast == RomFlash::Broadcast::Pal) {
            config.getOption(Broadcast)->setChoicesIndex(1);
        } else if (settings.broadcast == RomFlash::Broadcast::PalM) {
            config.getOption(Broadcast)->setChoicesIndex(2);
        } else if (settings.broadcast == RomFlash::Broadcast::PalN) {
            config.getOption(Broadcast)->setChoicesIndex(3);
        }
        configBox->load(&config);
    } else {
        if (dirty) {
            dirty = false;
            // country
            int index = config.getOption(Country)->getChoiceIndex();
            if (index == 0) {
                settings.country = RomFlash::Country::Japan;
            } else if (index == 1) {
                settings.country = RomFlash::Country::Usa;
            } else if (index == 2) {
                settings.country = RomFlash::Country::Europe;
            }
            // broadcast
            index = config.getOption(Broadcast)->getChoiceIndex();
            if (index == 0) {
                settings.broadcast = RomFlash::Broadcast::Ntsc;
            } else if (index == 1) {
                settings.broadcast = RomFlash::Broadcast::Pal;
            } else if (index == 2) {
                settings.broadcast = RomFlash::Broadcast::PalM;
            } else if (index == 3) {
                settings.broadcast = RomFlash::Broadcast::PalN;
            }
#ifdef __DREAMCAST__
            // write
            int res = RomFlash::saveRegionSettings(&settings);
            if (res != 0) {
                std::string err = "AN ERROR OCCURRED WITH YOUR FLASHROM";
                if (res == FLASHROM_ERR_NO_PARTITION) {
                    err = "COULD NOT GET PARTITION INFORMATION FROM YOUR FLASHROM";
                } else if (res == FLASHROM_ERR_DELETE_PART) {
                    err = "COULD NOT ERASE PARTITION FROM FLASHROM. MAKE SURE YOU PROVIDE 12V TO R512";
                } else if (res == FLASHROM_ERR_WRITE_PART) {
                    err = "COULD NOT WRITE PARTITION TO FLASHROM. MAKE SURE YOU PROVIDE 12V TO R512";
                }
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
                retroDream->showStatus("REGION CHANGER ERROR", err);
            } else {
                retroDream->showStatus("REGION CHANGER", "FLASHROM SETTINGS UPDATED SUCCESSFULLY", COL_GREEN);
            }
#endif
        }
    }
}

bool RegionFreeMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
        retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        return true;
    }

    return Menu::onInput(players);
}
