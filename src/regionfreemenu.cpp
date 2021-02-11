//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "regionfreemenu.h"

using namespace c2d;

RegionFreeMenu::RegionFreeMenu(RetroDream *rd, Skin::CustomShape *shape) : Menu(rd, shape) {

    io = rd->getRender()->getIo();
    backupPath = rd->getIo()->getDataPath() + "system.rom";

    title->setString("REGION CHANGER OPTIONS");

    config.addOption({"COUNTRY", {"JAPAN", "USA", "EUROPE"}, 0, Country});
    config.addOption({"BROADCAST", {"NTSC", "PAL", "PAL-M", "PAL-N"}, 0, Broadcast});
    config.addOption({"BACKUP", {"GO"}, 0, Backup});
}

void RegionFreeMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    Menu::setVisibility(visibility, tweenPlay);

    if (visibility == Visibility::Visible) {

        retroDream->getFiler()->setSelectionBack();

        // read flashrom partition
        if (!partition.read()) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("REGION CHANGER ERROR", partition.getErrorString());
            return;
        }

        // verify flash values
        if (partition.getCountry() == SystemPartition::Country::Unknown
            || partition.getBroadcast() == SystemPartition::Broadcast::Unknown) {
            Menu::setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
            retroDream->showStatus("REGION CHANGER ERROR", "SYSTEM PARTITION IS CORRUPTED");
            return;
        }

        // country
        if (partition.getCountry() == SystemPartition::Country::Japan) {
            config.getOption(Country)->setChoicesIndex(0);
        } else if (partition.getCountry() == SystemPartition::Country::Usa) {
            config.getOption(Country)->setChoicesIndex(1);
        } else if (partition.getCountry() == SystemPartition::Country::Europe) {
            config.getOption(Country)->setChoicesIndex(2);
        }

        // broadcast
        if (partition.getBroadcast() == SystemPartition::Broadcast::Ntsc) {
            config.getOption(Broadcast)->setChoicesIndex(0);
        } else if (partition.getBroadcast() == SystemPartition::Broadcast::Pal) {
            config.getOption(Broadcast)->setChoicesIndex(1);
        } else if (partition.getBroadcast() == SystemPartition::Broadcast::PalM) {
            config.getOption(Broadcast)->setChoicesIndex(2);
        } else if (partition.getBroadcast() == SystemPartition::Broadcast::PalN) {
            config.getOption(Broadcast)->setChoicesIndex(3);
        }

        configBox->load(&config);
    } else {
        if (dirty) {

            dirty = false;

            // country
            int index = config.getOption(Country)->getChoiceIndex();
            if (index == 0) {
                partition.setCountry(SystemPartition::Country::Japan);
            } else if (index == 1) {
                partition.setCountry(SystemPartition::Country::Usa);
            } else if (index == 2) {
                partition.setCountry(SystemPartition::Country::Europe);
            }

            // broadcast
            index = config.getOption(Broadcast)->getChoiceIndex();
            if (index == 0) {
                partition.setBroadcast(SystemPartition::Broadcast::Ntsc);
            } else if (index == 1) {
                partition.setBroadcast(SystemPartition::Broadcast::Pal);
            } else if (index == 2) {
                partition.setBroadcast(SystemPartition::Broadcast::PalM);
            } else if (index == 3) {
                partition.setBroadcast(SystemPartition::Broadcast::PalN);
            }
#ifdef __DREAMCAST__
            if (!partition.write()) {
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
                retroDream->showStatus("REGION CHANGER ERROR", partition.getErrorString());
            } else {
                retroDream->showStatus("REGION CHANGER", "FLASHROM SUCCESSFULLY UPDATED", COL_GREEN);
            }
#endif
        }
    }
}

bool RegionFreeMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire1) {
        auto option = configBox->getSelection();
        if (option != nullptr && option->getId() == Backup) {
            if (!partition.write(io, backupPath)) {
                retroDream->showStatus("REGION CHANGER ERROR", partition.getErrorString());
            } else {
                retroDream->showStatus("REGION CHANGER", "BACKUP SUCCESS: " + backupPath, COL_GREEN);
            }
        }
    } else if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
        retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        return true;
    }

    return Menu::onInput(players);
}
