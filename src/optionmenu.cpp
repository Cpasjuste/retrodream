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
    title->setOrigin(Origin::Right);
    title->setPosition(rect.width - 16, -22);
    title->setOutlineThickness(4);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    add(new TweenPosition({rect.left, rect.top + rect.height + 10}, {rect.left, rect.top + 24}, 0.3f));
    setVisibility(Visibility::Hidden);
}
