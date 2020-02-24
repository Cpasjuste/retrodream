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
        retroDream->getFiler()->setSelectionBack();
#ifdef __DREAMCAST__
        FlashRom::getSystemSettings(&settings);
#else
        settings.error = -1;
#endif
        if (settings.error != 0) {
            retroDream->getMessageBox()->show("SYSTEM CONFIG ERROR",
                                              "\n\nAN ERROR OCCURRED WHILE TRYING TO READ YOUR FLASHROM."
                                              "\n\nCHANGING FLASHROM SETTINGS IS NOT POSSIBLE...", "OK");
            setVisibility(Visibility::Hidden, true);
            retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        }
        // system options
        config.getOption(Language)->setChoicesIndex((int) settings.language);
        config.getOption(Audio)->setChoicesIndex((int) settings.audio);
        config.getOption(AutoStart)->setChoicesIndex((int) settings.autoStart);

        configBox->load(&config);
    } else {
        if (dirty) {
            // system options
            settings.language = (FlashRom::Language) config.getOption(Language)->getChoiceIndex();
            settings.audio = (FlashRom::Audio) config.getOption(Audio)->getChoiceIndex();
            settings.autoStart = (FlashRom::AutoStart) config.getOption(AutoStart)->getChoiceIndex();
#ifdef __DREAMCAST__
            // write
            int res = FlashRom::saveSystemSettings(&settings);
            if (res != 0) {
                retroDream->getMessageBox()->show("SYSTEM CONFIG ERROR",
                                                  "\n\nAN ERROR OCCURRED WHILE TRYING TO WRITE YOUR FLASHROM."
                                                  "\n\nCHANGING FLASHROM SETTINGS IS NOT POSSIBLE...", "OK");
                setVisibility(Visibility::Hidden, true);
                retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
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
