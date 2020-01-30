//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_OPTIONMENU_H
#define RETRODREAM_OPTIONMENU_H

class OptionMenu : public c2d::RoundedRectangleShape {

public:

    enum MainOption {
        LaunchDs = 0,
        Reboot,
        Credits
    };

    explicit OptionMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void save();

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;

private:

    RetroDream *retroDream = nullptr;
    c2d::Text *title = nullptr;
    c2d::ConfigBox *configBox = nullptr;
    c2d::config::Group mainConfig;
    bool dirty = false;
};

#endif //RETRODREAM_OPTIONMENU_H
