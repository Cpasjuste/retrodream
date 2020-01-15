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

class Filer : public c2d::RectangleShape {

public:

    Filer(RetroDream *retroDream, const c2d::FloatRect &rect, const std::string &path);

    bool getDir(const std::string &path);

    std::string getPath();

    c2d::Io::File getSelection();

    void setSelection(int index);

    void up();

    void down();

    void enter(int index);

    void exit();

    void setSize(const c2d::Vector2f &size) override;

    void setSize(float width, float height) override;

    void setTextColor(const c2d::Color &color);

    void setTextOutlineColor(const c2d::Color &color);

    void setTextOutlineThickness(float thickness);

    void setHighlightEnabled(bool enable);

    void setHighlightUseFileColor(bool enable);

    RectangleShape *getHighlight();

    std::vector<c2d::Io::File> getFiles();

    int getMaxLines();

    int getIndex();

    c2d::Color colorDir;
    c2d::Color colorFile;

private:

    void updateFiles();

    void clearHistory();

    std::string path;
    std::vector<c2d::Io::File> files;
    std::vector<Line *> lines;
    std::vector<int> item_index_prev;
    RetroDream *retroDream = nullptr;
    RectangleShape *highlight = nullptr;
    float line_height;
    int max_lines;
    int file_index = 0;
    int highlight_index = 0;
    bool use_highlight = true;
    bool highlight_use_files_color = true;
};

#endif //RETRODREAM_FILER_H
