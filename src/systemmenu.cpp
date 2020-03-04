//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "systemmenu.h"

using namespace c2d;

SystemMenu::SystemMenu(RetroDream *rd, const c2d::FloatRect &rect) : Menu(rd, rect) {

    retroDream = rd;

    title->setString("SYSTEM OPTIONS");

    config.addOption({"LANGUAGE", {"JAPANESE", "ENGLISH", "GERMAN", "FRENCH", "SPANISH", "ITALIAN"}, 0, Language});
    config.addOption({"AUDIO", {"STEREO", "MONO"}, 0, Audio});
    config.addOption({"AUTO START", {"ON", "OFF"}, 0, AutoStart});
    configBox->load(&config);
}

void SystemMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    Menu::setVisibility(visibility, tweenPlay);

    if (visibility == Visibility::Visible) {
        // backup flashrom if needed, this is fast enough to not show any message
        std::string flashBackup = retroDream->getConfig()->getBootDevice() + "RD/settings.rom"; // block1 partition
        if (!retroDream->getRender()->getIo()->exist(flashBackup)) {
            RomFlash::backup(FLASHROM_PT_BLOCK_1, flashBackup);
        }

        retroDream->getFiler()->setSelectionBack();
#ifdef __DREAMCAST__
        RomFlash::getSystemSettings(&settings);
#else
        settings.error = -1;
#endif
        if (settings.error != 0) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("SYSTEM CONFIG ERROR", "AN ERROR OCCURRED WHILE TRYING TO READ YOUR FLASHROM");
        }
        // system options
        config.getOption(Language)->setChoicesIndex((int) settings.language);
        config.getOption(Audio)->setChoicesIndex((int) settings.audio);
        config.getOption(AutoStart)->setChoicesIndex((int) settings.autoStart);

        configBox->load(&config);
    } else {
        if (dirty) {
            dirty = false;
            // system options
            settings.language = (RomFlash::Language) config.getOption(Language)->getChoiceIndex();
            settings.audio = (RomFlash::Audio) config.getOption(Audio)->getChoiceIndex();
            settings.autoStart = (RomFlash::AutoStart) config.getOption(AutoStart)->getChoiceIndex();
#ifdef __DREAMCAST__
            // write
            int res = RomFlash::saveSystemSettings(&settings);
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
                retroDream->showStatus("SYSTEM CONFIG", "FLASHROM SETTINGS UPDATED SUCCESSFULLY", COL_GREEN);
            }
#endif
        }
    }
}

bool SystemMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
        retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        return true;
    }

    return Menu::onInput(players);
}
