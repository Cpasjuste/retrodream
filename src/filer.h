//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_FILER_H
#define RETRODREAM_FILER_H

#include "cross2d/skeleton/sfml/Text.hpp"
#include "skinrect.h"
#include "retroio.h"

class RetroDream;

class Line : public c2d::RectangleShape {

public:

    Line(const c2d::FloatRect &rect, const std::string &str, c2d::Font *font, unsigned int fontSize);

    void setSize(const c2d::Vector2f &size) override;

    void setSize(float width, float height) override;

    void setString(const std::string &string);

    void setColor(const c2d::Color &color);

    c2d::Text *getText();

private:

    c2d::Text *text = nullptr;
};

class Filer : public SkinRect {

public:

    class RetroFile {
    public:
        RetroFile() = default;

        c2d::Io::File data;
        std::string isoType;
        std::string isoPath;
        std::string preview;
        std::string preview_video;
        std::string preset;
        std::string upperName;
        bool isGame = false;
        bool isVmu = false;
    };

    enum FilerMode {
        Browse = 0,
        Game
    };

    Filer(RetroDream *retroDream, Skin::CustomShape *shape, const std::string &path, int lineSpacing = 2);

    bool getDir(const std::string &path);

    RetroFile getSelection();

    void setSelection(int index);

    void setSelectionFront();

    void setSelectionBack();

    Line *getSelectionLine();

    void up();

    void down();

    void enter(int index);

    void exit();

    void setSize(const c2d::Vector2f &size) override;

    void setSize(float width, float height) override;

    void setAlpha(uint8_t alpha, bool recursive = false) override;

    int getMaxLines();

    int getIndex();

    std::string getPath();

    c2d::RectangleShape *getBlur() {
        return blurLayer;
    }

private:

    void updateLines();

    void onUpdate() override;

    bool onInput(c2d::Input::Player *players) override;

    void flashBios(const RetroFile &file);

    void flashRom(const RetroFile &file);

    void restoreVmu(const RetroFile &file);

    std::string path;
    std::vector<RetroFile> files;
    std::vector<Line *> lines;
    std::vector<int> item_index_prev;
    RetroDream *retroDream = nullptr;
    RetroIo *io = nullptr;
    c2d::RectangleShape *highlight = nullptr;
    c2d::RectangleShape *blurLayer = nullptr;
    c2d::C2DClock previewClock;
    float line_height;
    int max_lines;
    int file_index = 0;
    int highlight_index = 0;
    Skin::CustomColor fileColor;
    Skin::CustomColor dirColor;
    int previewImageDelay = 500;
    int previewVideoDelay = 5000;
};

#endif //RETRODREAM_FILER_H
