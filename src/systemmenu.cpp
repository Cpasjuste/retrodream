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

    config.addOption({"LANGUAGE",
                      {"JAPANESE", "ENGLISH", "GERMAN", "FRENCH", "SPANISH", "ITALIAN", "UNKNOWN"}, 0, Language});
    config.addOption({"AUDIO", {"STEREO", "MONO", "UNKNOWN"}, 0, Audio});
    config.addOption({"AUTO START", {"ON", "OFF", "UNKNOWN"}, 0, AutoStart});
    configBox->load(&config);
}

void SystemMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    Menu::setVisibility(visibility, tweenPlay);

    if (visibility == Visibility::Visible) {

        retroDream->getFiler()->setSelectionBack();

        // read flashrom partition
        if (!partition.read()) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("SYSTEM CONFIG ERROR", partition.getErrorString());
            return;
        }

        // backup flashrom if needed, this is fast enough to not show any message
        std::string flashBackup = retroDream->getConfig()->getBootDevice() + "RD/block1.rom";
        if (!retroDream->getRender()->getIo()->exist(flashBackup)) {
            partition.write(retroDream->getRender()->getIo(), flashBackup);
        }

        // system options
        config.getOption(Language)->setChoicesIndex((int) partition.getLanguage());
        config.getOption(Audio)->setChoicesIndex((int) partition.getAudio());
        config.getOption(AutoStart)->setChoicesIndex((int) partition.getAutoStart());

        configBox->load(&config);

    } else {
        if (dirty) {

            dirty = false;

            // system options
            partition.setLanguage((Block1Partition::Language) config.getOption(Language)->getChoiceIndex());
            partition.setAudio((Block1Partition::Audio) config.getOption(Audio)->getChoiceIndex());
            partition.setAutoStart((Block1Partition::AutoStart) config.getOption(AutoStart)->getChoiceIndex());
#ifdef __DREAMCAST__
            if (!partition.write()) {
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
                retroDream->showStatus("SYSTEM CONFIG ERROR", partition.getErrorString());
            } else {
                retroDream->showStatus("SYSTEM CONFIG", "FLASHROM UPDATED SUCCESSFULLY", COL_GREEN);
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
