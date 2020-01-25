//
// Created by cpasjuste on 15/01/2020.
//

#ifndef RETRODREAM_FILER_H
#define RETRODREAM_FILER_H

#include "cross2d/skeleton/sfml/RectangleShape.hpp"
#include "cross2d/skeleton/sfml/Text.hpp"
#include "cross2d/skeleton/io.h"

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

class Filer : public c2d::RoundedRectangleShape {

public:

    class RetroFile {
    public:
        RetroFile() = default;

        c2d::Io::File data;
        std::string isoPath;
        std::string preview;
        std::string preset;
        bool isGame = false;
    };

    enum FilerMode {
        Browse = 0,
        Game
    };

    Filer(RetroDream *retroDream, const c2d::FloatRect &rect, const std::string &path);

    bool getDir(const std::string &path);

    RetroFile getSelection();

    void setSelection(int index);

    void up();

    void down();

    void enter(int index);

    void exit();

    void setSize(const c2d::Vector2f &size) override;

    void setSize(float width, float height) override;

    void setColor(const c2d::Color &dirColor, const c2d::Color &fileColor);

    void setAlpha(uint8_t alpha, bool recursive = false) override;

    int getMaxLines();

    int getIndex();

private:

    void updateLines();

    void onUpdate() override;

    std::string path;
    std::vector<RetroFile> files;
    std::vector<Line *> lines;
    std::vector<int> item_index_prev;
    RetroDream *retroDream = nullptr;
    c2d::Io *io = nullptr;
    RoundedRectangleShape *highlight = nullptr;
    c2d::C2DClock previewClock;
    int previewLoadDelay = 300;
    float line_height;
    int max_lines;
    int file_index = 0;
    int highlight_index = 0;
    bool use_highlight = true;
    bool highlight_use_files_color = false;
    c2d::Color colorDir;
    c2d::Color colorFile;
};

#endif //RETRODREAM_FILER_H
