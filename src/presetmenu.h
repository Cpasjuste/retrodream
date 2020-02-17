//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_PRESETMENU_H
#define RETRODREAM_PRESETMENU_H

#include "isoloader.h"

class PresetMenu : public Menu {

public:
    enum GameOption {
        Mode = 0,
        Memory,
        Dma,
        Async,
        Cdda,
        Device,
        Type
    };

    explicit PresetMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

private:

    void save();

    static std::vector<std::string> getAddresses();

    IsoLoader::Config isoLoaderConfig;
    bool dirty = false;
};

#endif //RETRODREAM_PRESETMENU_H
