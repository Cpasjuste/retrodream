//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_FILEMENU_H
#define RETRODREAM_FILEMENU_H

class FileMenu : public Menu {

public:

    enum MainOption {
        Browse,
        Copy,
        Paste,
        Delete,
        VmuBackup
    };

    explicit FileMenu(RetroDream *rd, Skin::CustomShape *shape);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;

private:

    Filer::RetroFile file;
};

#endif //RETRODREAM_FILEMENU_H
