//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "systemmenu.h"

using namespace c2d;

SystemMenu::SystemMenu(RetroDream *rd, Skin::CustomShape *shape) : Menu(rd, shape) {

    io = rd->getRender()->getIo();
    backupPath = rd->getIo()->getDataPath() + "block1.rom";

    title->setString("SYSTEM OPTIONS");

    config.addOption({"LANGUAGE",
                      {"JAPANESE", "ENGLISH", "GERMAN", "FRENCH", "SPANISH", "ITALIAN"}, 0, Language});
    config.addOption({"AUDIO", {"STEREO", "MONO"}, 0, Audio});
    config.addOption({"AUTO START", {"ON", "OFF"}, 0, AutoStart});
    config.addOption({"BACKUP", {"GO"}, 0, Backup});
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

        // verify flash values
        if (partition.getLanguage() == Block1Partition::Language::Unknown
            || partition.getAudio() == Block1Partition::Audio::Unknown
            || partition.getAutoStart() == Block1Partition::AutoStart::Unknown) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("SYSTEM CONFIG ERROR", "BLOCK1 PARTITION IS CORRUPTED");
            return;
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
                retroDream->showStatus("SYSTEM CONFIG", "FLASHROM SUCCESSFULLY UPDATED", COL_GREEN);
            }
#endif
        }
    }
}

bool SystemMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire1) {
        auto option = configBox->getSelection();
        if (option != nullptr && option->getId() == Backup) {
            if (!partition.write(io, backupPath)) {
                retroDream->showStatus("SYSTEM CONFIG ERROR", partition.getErrorString());
            } else {
                retroDream->showStatus("SYSTEM CONFIG", "BACKUP SUCCESS: " + backupPath, COL_GREEN);
            }
        }
    } else if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
        retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        return true;
    }

    return Menu::onInput(players);
}
