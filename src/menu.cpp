//
// Created by cpasjuste on 17/02/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "menu.h"

using namespace c2d;

Menu::Menu(RetroDream *rd, Skin::CustomShape *shape) : SkinRect(shape, false) {

    retroDream = rd;

    title = new Text("MENU", FONT_SIZE * 1.2f);
    title->setFillColor(COL_YELLOW);
    title->setOrigin(Origin::BottomLeft);
    title->setPosition(16, 0);
    title->setOutlineThickness(3);
    title->setOutlineColor(COL_BLUE_DARK);
    Menu::add(title);

    FloatRect configRect = {16, 16, shape->rect.width - 28, shape->rect.height - 64};
    configBox = new ConfigBox(retroDream->getRender()->getFont(), FONT_SIZE, configRect, FONT_SIZE + 10);
    // left box
    Skin::CustomText ltext = RetroDream::getSkin()->getText(Skin::Id::MenuLeftText);
    configBox->getListBoxLeft()->setFillColor(shape->color);
    configBox->getListBoxLeft()->setTextColor(ltext.color, ltext.color);
    // right box
    Skin::CustomText rtext = RetroDream::getSkin()->getText(Skin::Id::MenuRightText);
    configBox->getListBoxRight()->setFillColor(shape->color);
    configBox->getListBoxRight()->setTextColor(rtext.color, rtext.color);
    // right highlight
    Skin::CustomShape hshape = RetroDream::getSkin()->getShape(Skin::Id::MenuHighlightShape);
    configBox->getListBoxRight()->getHighlight()->setFillColor(hshape.color);
    configBox->getListBoxRight()->getHighlight()->setOutlineColor(hshape.outlineColor);
    configBox->getListBoxRight()->getHighlight()->setOutlineThickness(hshape.outlineSize);
    configBox->getListBoxRight()->getHighlight()->setCornersRadius((float) hshape.corners_radius);
    configBox->getListBoxRight()->getHighlight()->setCornerPointCount(CORNER_POINTS);
    //
    Menu::add(configBox);

    Menu::add(new TweenPosition(
            {shape->rect.left + shape->rect.width + 10, shape->rect.top},
            {shape->rect.left, shape->rect.top}, 0.2f));
    Menu::setVisibility(Visibility::Hidden);
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

    RectangleShape::setVisibility(visibility, tweenPlay);
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
