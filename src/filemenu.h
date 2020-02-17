//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_FILEMENU_H
#define RETRODREAM_FILEMENU_H

class FileMenu : public Menu {

public:

    enum MainOption {
        Copy = 0,
        Paste,
        Delete,
    };

    explicit FileMenu(RetroDream *rd, const c2d::FloatRect &rect);

    bool onInput(c2d::Input::Player *players) override;

private:

    Filer::RetroFile file;
};

#endif //RETRODREAM_FILEMENU_H
