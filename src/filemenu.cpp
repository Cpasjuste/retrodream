//
// Created by cpasjuste on 21/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filemenu.h"

using namespace c2d;

FileMenu::FileMenu(RetroDream *rd, const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    retroDream = rd;

    title = new Text("FILE OPTION", 36);
    title->setOrigin(Origin::Right);
    title->setPosition(rect.width - 16, -22);
    title->setOutlineThickness(4);
    title->setOutlineColor(COL_BLUE_DARK);
    add(title);

    add(new TweenPosition({-(rect.width + 10), rect.top}, {-16, rect.top}, 0.3f));
    setVisibility(Visibility::Hidden);
}
