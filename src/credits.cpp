//
// Created by cpasjuste on 30/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"


using namespace c2d;

Credits::Credits(RetroDream *rd, const c2d::FloatRect &rect)
        : RectangleShape({rect.width, rect.height}) {

    retroDream = rd;
    RectangleShape::setPosition(rect.left, rect.top);
    setCornersRadius(CORNER_RADIUS);
    setCornerPointCount(CORNER_POINTS);

    title = new Text("CREDITS", FONT_SIZE);
    title->setOrigin(Origin::BottomLeft);
    title->setPosition(16, -1);
    title->setOutlineThickness(3);
    title->setOutlineColor(COL_BLUE_DARK);
    RectangleShape::add(title);

    text = new Text(
            "DREAMSHELL:  A BIG THANKS TO DREAMSHELL DEVELOPERS, WITHOUT THEM RETRODREAM WOULD NOT EXIST.\n\n"
            "DRAGONCITY:  THANKS FOR ALL THE IDEAS AND TESTING \"DRAGONCITY\", I ENJOYED WORKING WITH YOU MATE!  :)\n\n"
            "OPEN SOURCING:  KNOWLEDGE NEED TO BE SHARED, THANKS TO ALL THE PEOPLE AROUND WITH AN OPEN MIND.\n",
            FONT_SIZE);
    text->setPosition(32, 32);
    text->setSizeMax(rect.width - 64, rect.height - 64);
    text->setFillColor(COL_BLUE_DARK);
    text->setOutlineColor(Color::Black);
    text->setOutlineThickness(2);
    text->setOverflow(Text::Overflow::NewLine);
    text->setLineSpacingModifier(6);
    RectangleShape::add(text);

    cpasjuste = new Text("CPASJUSTE IN DA PLACE !", FONT_SIZE);
    cpasjuste->setOrigin(Origin::BottomRight);
    cpasjuste->setPosition(rect.width - 16, rect.height - 16);
    cpasjuste->setFillColor(COL_YELLOW);
    cpasjuste->setOutlineColor(Color::Black);
    cpasjuste->setOutlineThickness(2);
    RectangleShape::add(cpasjuste);

    RectangleShape::add(new TweenPosition({rect.left, -rect.top - 32}, {rect.left, rect.top}, 0.4f));
    RectangleShape::setVisibility(Visibility::Hidden);
}

bool Credits::onInput(c2d::Input::Player *players) {

    unsigned int keys = players[0].keys;

    if (keys != Input::Key::Delay) {
        setVisibility(Visibility::Hidden, true);
        retroDream->getOptionMenu()->setVisibility(Visibility::Visible, true);
        retroDream->getFiler()->setSelectionBack();
    }

    return true;
}
