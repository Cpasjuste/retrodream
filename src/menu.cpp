//
// Created by cpasjuste on 17/02/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "menu.h"

using namespace c2d;

Menu::Menu(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    retroDream = rd;

    title = new Text("MENU", FONT_SIZE);
    title->setOrigin(Origin::BottomLeft);
    title->setPosition(16, 0);
    title->setOutlineThickness(3);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    FloatRect configRect = {16, 16, rect.width - 28, rect.height - 64};
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect, FONT_SIZE + 10);
    configBox->getListBoxLeft()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxLeft()->setTextOutlineColor(Color::Black);
    configBox->getListBoxLeft()->setTextOutlineThickness(2);
    configBox->getListBoxLeft()->setTextColor(COL_BLUE_DARK);

    configBox->getListBoxRight()->setFillColor(COL_BLUE_GRAY);
    configBox->getListBoxRight()->setTextOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->setTextOutlineThickness(2);
    configBox->getListBoxRight()->setTextColor(Color::White);
    configBox->getListBoxRight()->getHighlight()->setFillColor(COL_YELLOW);
    configBox->getListBoxRight()->getHighlight()->setOutlineColor(COL_BLUE_DARK);
    configBox->getListBoxRight()->getHighlight()->setOutlineThickness(1);

    add(configBox);

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.2f));
    setVisibility(Visibility::Hidden);
}

void Menu::setVisibility(c2d::Visibility visibility, bool tweenPlay) {

    if (visibility == Visibility::Visible) {
        configBox->reset();
        retroDream->getFiler()->getBlur()->setVisibility(Visibility::Visible, true);
        retroDream->getFiler()->setSelectionFront();
    } else {
        retroDream->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
        retroDream->getFiler()->setSelectionBack();
    }

    RoundedRectangleShape::setVisibility(visibility, tweenPlay);
}

bool Menu::onInput(c2d::Input::Player *players) {

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
    } else if (keys & Input::Key::Fire2) {
        setVisibility(Visibility::Hidden, true);
    }

    return true;
}
