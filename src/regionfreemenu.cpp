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

    config.addOption({"COUNTRY", {"JAPAN", "USA", "EUROPE", "UNKNOWN"}, 0, Country});
    config.addOption({"BROADCAST", {"NTSC", "PAL", "PAL-M", "PAL-N", "UNKNOWN"}, 0, Broadcast});
    configBox->load(&config);
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

        // backup flashrom if needed, this is fast enough to not show any message
        std::string flashBackup = retroDream->getConfig()->getBootDevice() + "RD/system.rom";
        if (!retroDream->getRender()->getIo()->exist(flashBackup)) {
            partition.write(retroDream->getRender()->getIo(), flashBackup);
        }

        // country
        if (partition.getCountry() == SystemPartition::Country::Japan) {
            config.getOption(Country)->setChoicesIndex(0);
        } else if (partition.getCountry() == SystemPartition::Country::Usa) {
            config.getOption(Country)->setChoicesIndex(1);
        } else if (partition.getCountry() == SystemPartition::Country::Europe) {
            config.getOption(Country)->setChoicesIndex(2);
        } else {
            config.getOption(Country)->setChoicesIndex(3);
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
        } else {
            config.getOption(Broadcast)->setChoicesIndex(4);
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
                retroDream->showStatus("REGION CHANGER", "FLASHROM UPDATED SUCCESSFULLY", COL_GREEN);
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
