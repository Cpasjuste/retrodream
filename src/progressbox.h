//
// Created by cpasjuste on 04/02/18.
//

#ifndef PROGRESSBOX_H
#define PROGRESSBOX_H

class ProgressBox : public c2d::RectangleShape {

public:

    ProgressBox(RetroDream *rd, const c2d::Color &fill, const c2d::Color &out, const c2d::Color &textFill);

    void setTitle(const std::string &text);

    void setMessage(const std::string &text);

    void setProgress(const std::string &text, float progress);

    c2d::Text *getTitle();

private:

    c2d::Text *title;
    c2d::Text *message;
    c2d::Text *progress_message;
    c2d::RectangleShape *progress_bg;
    c2d::RectangleShape *progress_fg;
};

#endif //PROGRESSBOX_H
