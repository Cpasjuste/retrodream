//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_FILEMENU_H
#define RETRODREAM_FILEMENU_H

class FileMenu : public c2d::RoundedRectangleShape {

public:

    enum MainOption {
        Copy = 0,
        Paste,
        Delete,
    };

    explicit FileMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setVisibility(c2d::Visibility visibility, bool tweenPlay = false) override;

    bool onInput(c2d::Input::Player *players) override;

private:

    RetroDream *retroDream = nullptr;
    c2d::Text *title = nullptr;
    c2d::ConfigBox *configBox = nullptr;
    c2d::config::Group mainConfig;
    Filer::RetroFile file;
    bool dirty = false;
};

#endif //RETRODREAM_FILEMENU_H
