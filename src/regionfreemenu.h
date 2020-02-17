//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_REGIONFREEMENU_H
#define RETRODREAM_REGIONFREEMENU_H

class RegionFreeMenu : public Menu {

public:

    enum MainOption {
        Country = 0,
        Broadcast,
        Language
    };

    explicit RegionFreeMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;

private:
    FlashRom::Settings settings;
};

#endif //RETRODREAM_REGIONFREEMENU_H
