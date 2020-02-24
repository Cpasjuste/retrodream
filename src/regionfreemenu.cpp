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
        retroDream->getFiler()->setSelectionBack();
#ifdef __DREAMCAST__
        FlashRom::getRegionSettings(&settings);
#else
        settings.country = FlashRom::Country::Usa;
        settings.broadcast = FlashRom::Broadcast::Ntsc;
        settings.error = -1;
#endif
        if (settings.error != 0) {
            retroDream->getMessageBox()->show("REGION FREE ERROR",
                                              "\n\nAN ERROR OCCURRED WHILE TRYING TO READ YOUR FLASHROM."
                                              "\n\nCHANGING FLASHROM SETTINGS IS NOT POSSIBLE...", "OK");
            setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        }
        // country
        if (settings.country == FlashRom::Country::Japan) {
            config.getOption(Country)->setChoicesIndex(0);
        } else if (settings.country == FlashRom::Country::Usa) {
            config.getOption(Country)->setChoicesIndex(1);
        } else if (settings.country == FlashRom::Country::Europe) {
            config.getOption(Country)->setChoicesIndex(2);
        }
        // broadcast
        if (settings.broadcast == FlashRom::Broadcast::Ntsc) {
            config.getOption(Broadcast)->setChoicesIndex(0);
        } else if (settings.broadcast == FlashRom::Broadcast::Pal) {
            config.getOption(Broadcast)->setChoicesIndex(1);
        } else if (settings.broadcast == FlashRom::Broadcast::PalM) {
            config.getOption(Broadcast)->setChoicesIndex(2);
        } else if (settings.broadcast == FlashRom::Broadcast::PalN) {
            config.getOption(Broadcast)->setChoicesIndex(3);
        }
        configBox->load(&config);
    } else {
        if (dirty) {
            // country
            int index = config.getOption(Country)->getChoiceIndex();
            if (index == 0) {
                settings.country = FlashRom::Country::Japan;
            } else if (index == 1) {
                settings.country = FlashRom::Country::Usa;
            } else if (index == 2) {
                settings.country = FlashRom::Country::Europe;
            }
            // broadcast
            index = config.getOption(Broadcast)->getChoiceIndex();
            if (index == 0) {
                settings.broadcast = FlashRom::Broadcast::Ntsc;
            } else if (index == 1) {
                settings.broadcast = FlashRom::Broadcast::Pal;
            } else if (index == 2) {
                settings.broadcast = FlashRom::Broadcast::PalM;
            } else if (index == 3) {
                settings.broadcast = FlashRom::Broadcast::PalN;
            }
#ifdef __DREAMCAST__
            // write
            int res = FlashRom::saveRegionSettings(&settings);
            if (res != 0) {
                retroDream->getMessageBox()->show("REGION FREE ERROR",
                                                  "\n\nAN ERROR OCCURRED WHILE TRYING TO WRITE YOUR FLASHROM."
                                                  "\n\nCHANGING FLASHROM SETTINGS IS NOT POSSIBLE...", "OK");
                setVisibility(Visibility::Hidden, true);
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
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
