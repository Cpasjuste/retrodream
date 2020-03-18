//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_SYSTEMMENU_H
#define RETRODREAM_SYSTEMMENU_H

#include "flash/block1partition.h"

class SystemMenu : public Menu {

public:

    enum MainOption {
        Language,
        Audio,
        AutoStart
    };

    explicit SystemMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;

private:

    Block1Partition partition;
    c2d::Io *io = nullptr;
    std::string backupPath;
};

#endif //RETRODREAM_SYSTEMMENU_H
