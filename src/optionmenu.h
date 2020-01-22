//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_OPTIONMENU_H
#define RETRODREAM_OPTIONMENU_H

class OptionMenu : public c2d::RoundedRectangleShape {

public:
    explicit OptionMenu(RetroDream *rd, const c2d::FloatRect &rect);

private:
    RetroDream *retroDream = nullptr;
    c2d::Text *title = nullptr;
};

#endif //RETRODREAM_OPTIONMENU_H
