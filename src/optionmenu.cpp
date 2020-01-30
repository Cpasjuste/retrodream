//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "optionmenu.h"

using namespace c2d;

OptionMenu::OptionMenu(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    retroDream = rd;

    title = new Text("RETRODREAM OPTIONS", FONT_SIZE);
    title->setOrigin(Origin::BottomLeft);
    title->setPosition(16, -1);
    title->setOutlineThickness(3);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    FloatRect configRect = {16, 16, rect.width - 32, rect.height - 64};
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect, FONT_SIZE + 10);
    configBox->setFillColor(Color::Transparent);

    mainConfig.addOption({"LAUNCH DREAMSHELL", "GO", LaunchDs});
    mainConfig.addOption({"REBOOT", "GO", Reboot});
    mainConfig.addOption({"CREDITS", "GO", Credits});
    configBox->load(&mainConfig);

    FloatRect bounds = configBox->getListBoxLeft()->getGlobalBounds();
    configBox->getListBoxLeft()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxLeft()->setSize(bounds.width + 48, bounds.height);
    configBox->getListBoxLeft()->setTextOutlineColor(Color::Black);
    configBox->getListBoxLeft()->setTextOutlineThickness(2);
    configBox->getListBoxLeft()->setTextColor(COL_BLUE_DARK);

    bounds = configBox->getListBoxRight()->getGlobalBounds();
    configBox->getListBoxRight()->setPosition(bounds.left + 48, bounds.top);
    configBox->getListBoxRight()->setSize(bounds.width - 48, bounds.height);
    configBox->getListBoxRight()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxRight()->setTextOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->setTextOutlineThickness(2);
    configBox->getListBoxRight()->setTextColor(Color::White);
    configBox->getListBoxRight()->getHighlight()->setFillColor(COL_YELLOW);
    configBox->getListBoxRight()->getHighlight()->setOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->getHighlight()->setOutlineThickness(1);

    add(configBox);

    //add(new TweenPosition({rect.left, rect.top + rect.height + 10}, {rect.left, rect.top + 24}, 0.2f));
    add(new TweenPosition({rect.left, rect.top + rect.height + 10}, {rect.left, rect.top + rect.height - 128}, 0.2f));
    setVisibility(Visibility::Hidden);
}

void OptionMenu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {

    }

    RoundedRectangleShape::setVisibility(visibility, tweenPlay);
}

void OptionMenu::save() {
    if (dirty) {
        dirty = false;
    }
}

bool OptionMenu::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        configBox->navigate(ConfigBox::Navigation::Up);
    } else if (keys & Input::Key::Down) {
        configBox->navigate(ConfigBox::Navigation::Down);
    } else if (keys & Input::Key::Right) {
        configBox->navigate(ConfigBox::Navigation::Right);
        dirty = true;
    } else if (keys & Input::Key::Left) {
        configBox->navigate(ConfigBox::Navigation::Left);
        dirty = true;
    } else if (keys & Input::Key::Fire1) {
        auto option = configBox->getSelection();
        if (option != nullptr) {
            if (option->getId() == LaunchDs) {
                RetroUtility::exec(retroDream->getConfig()->get(RetroConfig::DsBinPath));
            } else if (option->getId() == Reboot) {
#ifdef __DREAMCAST__
                arch_reboot();
#endif
            } else if (option->getId() == Credits) {
                // TODO
            }
        }
    } else if (keys & Input::Key::Fire2) {
        save();
        retroDream->getBlur()->setVisibility(Visibility::Hidden, true);
        setVisibility(Visibility::Hidden, true);
    }

    return true;
}
