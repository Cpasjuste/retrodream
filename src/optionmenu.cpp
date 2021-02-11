//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "optionmenu.h"

using namespace c2d;

OptionMenu::OptionMenu(RetroDream *rd, Skin::CustomShape *shape) : Menu(rd, shape) {

    retroDream = rd;

    title->setString("RETRODREAM OPTIONS");

    configBox->getListBoxLeft()->setSize(
            configBox->getListBoxLeft()->getSize().x + 32,
            configBox->getListBoxLeft()->getSize().y);
    configBox->getListBoxRight()->move(32, 0);

    config.addOption({"LAUNCH DS", "GO", LaunchDs});
    config.addOption({"SYSTEM OPTIONS", "GO", SystemConfig});
    config.addOption({"REGION CHANGER", "GO", RegionChanger});
    config.addOption({"REBOOT", "GO", Reboot});
    config.addOption({"CREDITS", "GO", Credits});
    configBox->load(&config);
}

void OptionMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    Menu::setVisibility(visibility, tweenPlay);

    if (visibility == Visibility::Visible) {
        retroDream->getFiler()->setSelectionBack();
    }
}

bool OptionMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Fire1) {
        auto option = configBox->getSelection();
        if (option != nullptr) {
            if (option->getId() == LaunchDs) {
                RetroDream::getConfig()->set(RetroConfig::FilerPath, retroDream->getFiler()->getPath());
                RetroUtility::exec(retroDream->getIo()->getDsBinPath());
            } else if (option->getId() == SystemConfig) {
                setVisibility(Visibility::Hidden, true);
                retroDream->getSystemMenu()->setVisibility(Visibility::Visible, true);
                retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible, true);
            } else if (option->getId() == RegionChanger) {
                setVisibility(Visibility::Hidden, true);
                retroDream->getRegionFreeMenu()->setVisibility(Visibility::Visible, true);
                retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible, true);
            } else if (option->getId() == Reboot) {
#ifdef __DREAMCAST__
                retroDream->getConfig()->set(RetroConfig::FilerPath, retroDream->getFiler()->getPath());
                arch_reboot();
#endif
            } else if (option->getId() == Credits) {
                setVisibility(Visibility::Hidden, true);
                retroDream->getCredits()->setVisibility(Visibility::Visible, true);
                retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible, true);
            }
        }
    }

    return Menu::onInput(players);
}
