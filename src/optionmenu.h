//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_OPTIONMENU_H
#define RETRODREAM_OPTIONMENU_H

class OptionMenu : public Menu {

public:

    enum MainOption {
        LaunchDs = 0,
        SystemConfig,
        RegionChanger,
        Reboot,
        Credits
    };

    explicit OptionMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;
};

#endif //RETRODREAM_OPTIONMENU_H
