//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_FILEMENU_H
#define RETRODREAM_FILEMENU_H

class FileMenu : public c2d::RoundedRectangleShape {

public:
    explicit FileMenu(RetroDream *rd, const c2d::FloatRect &rect);

    void setTitle(const std::string &text);

private:
    RetroDream *retroDream = nullptr;
    c2d::Text *title = nullptr;
};

#endif //RETRODREAM_FILEMENU_H
