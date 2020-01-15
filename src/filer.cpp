//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "filer.h"


using namespace c2d;

Line::Line(const FloatRect &rect, const std::string &str, Font *font, unsigned int fontSize) : RectangleShape(rect) {

    text = new Text(str, fontSize, font);
    text->setOutlineColor(Color::White);
    text->setOrigin(Origin::Left);
    text->setPosition(2, getSize().y / 2);
    text->setSizeMax(getSize().x - ((float) fontSize + 8), 0);

    setFillColor(Color::Transparent);

    add(text);
}

void Line::setSize(const Vector2f &size) {
    Line::setSize(size.x, size.y);
}

void Line::setSize(float width, float height) {
    RectangleShape::setSize(width, height);
    text->setSizeMax(getSize().x - 8, 0);
}

void Line::setString(const std::string &string) {
    text->setString(Utility::toUpper(string));
}

void Line::setColor(const Color &color) {
    text->setFillColor(color);
}

Text *Line::getText() {
    return text;
}

Filer::Filer(RetroDream *rd, const c2d::FloatRect &rect, const std::string &path) : RectangleShape(rect) {

    retroDream = rd;

    // set default colors
    colorDir = COL_BLUE_DARK;
    colorFile = COL_BLUE_DARK;

    // calculate number of lines shown
    line_height = 30;
    max_lines = (int) (getSize().y / line_height);
    if ((float) max_lines * line_height < getSize().y) {
        line_height = getSize().y / (float) max_lines;
    }

    // add selection rectangle (highlight)
    highlight = new RectangleShape(Vector2f(getSize().x - 2, line_height));
    highlight->setAlpha(100);
    highlight->setOutlineThickness(1);
    add(highlight);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {1, (line_height * (float) i) + 1, getSize().x - 2, line_height - 2};
        auto line = new Line(r, "", retroDream->getRender()->getFont(), (unsigned int) 19);
        line->getText()->setOutlineThickness(1);
        lines.push_back(line);
        add(line);
    }

    getDir(path);
};

void Filer::updateFiles() {

    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {

        if (file_index + i >= files.size()) {
            lines[i]->setVisibility(Visibility::Hidden);
        } else {
            // set file
            Io::File file = files[file_index + i];
            lines[i]->setVisibility(Visibility::Visible);
            lines[i]->setString(file.name);
            lines[i]->getText()->setFillColor(file.type == Io::Type::File ? colorFile : colorDir);
            // set highlight position and color
            if ((int) i == highlight_index) {
                highlight->setPosition(lines[i]->getPosition());
                /*
                highlight->setFillColor({255, 255, 255, 200});
                highlight->setOutlineColor(lines[i]->getText()->getFillColor());
                highlight->setOutlineThickness(2);
                */
                Color color = highlight_use_files_color ?
                              lines[i]->getText()->getFillColor() : highlight->getFillColor();
                color.a = highlight->getAlpha();
                highlight->setFillColor(color);
                color = highlight_use_files_color ?
                        lines[i]->getText()->getFillColor() : highlight->getOutlineColor();
                //color.a = highlight->getAlpha();
                highlight->setOutlineColor(color);
            }
        }
    }

    if (files.empty() || !use_highlight) {
        highlight->setVisibility(Visibility::Hidden, false);
    } else {
        highlight->setVisibility(Visibility::Visible, false);
    }
}

bool Filer::getDir(const std::string &p) {

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }

    files = retroDream->getRender()->getIo()->getDirList(path, true);
    if (files.empty() || files.at(0).name != "..") {
        Io::File file("..", "..", Io::Type::Directory, 0, colorDir);
        files.insert(files.begin(), file);
    }

    setSelection(0);

    return true;
}

std::string Filer::getPath() {
    return path;
}

void Filer::enter(int index) {

    Io::File file = getSelection();
    bool success;

    if (file.name == "..") {
        exit();
        return;
    }

    if (path == "/") {
        success = getDir(path + file.name);
    } else {
        success = getDir(path + "/" + file.name);
    }
    if (success) {
        item_index_prev.push_back(index);
        setSelection(file_index);
    }
}

void Filer::exit() {

    std::string p = path;

    if (p == "/" || p.find('/') == std::string::npos) {
        return;
    }

    while (p.back() != '/') {
        p.erase(p.size() - 1);
    }

    if (p.size() > 1 && Utility::endsWith(p, "/")) {
        p.erase(p.size() - 1);
    }

    if (getDir(p)) {
        if (!item_index_prev.empty()) {
            int last = (int) item_index_prev.size() - 1;
            if (item_index_prev[last] < (int) files.size()) {
                file_index = item_index_prev[last];
            }
            item_index_prev.erase(item_index_prev.end() - 1);
        }
        setSelection(file_index);
    }
}

void Filer::clearHistory() {
    item_index_prev.clear();
}

void Filer::up() {

    if (highlight_index <= max_lines / 2 && file_index > 0) {
        file_index--;
    } else {
        highlight_index--;
        if (highlight_index < 0) {
            highlight_index = max_lines / 2;
            if (highlight_index >= (int) files.size()) {
                highlight_index = (int) files.size() - 1;
                file_index = 0;
            } else {
                file_index = ((int) files.size() - 1) - highlight_index;
            }
        }
    }

    updateFiles();
}

void Filer::down() {

    if (highlight_index >= max_lines / 2) {
        file_index++;
        if (file_index + highlight_index >= (int) files.size()) {
            file_index = 0;
            highlight_index = 0;
        }
    } else {
        highlight_index++;
        if (highlight_index >= (int) files.size()) {
            highlight_index = 0;
        }
    }

    updateFiles();
}

void Filer::setSelection(int new_index) {

    if (new_index < max_lines / 2) {
        file_index = 0;
        highlight_index = 0;
    } else if (new_index > (int) files.size() - max_lines / 2) {
        highlight_index = max_lines / 2;
        file_index = (int) files.size() - 1 - highlight_index;
        if (highlight_index >= (int) files.size()) {
            highlight_index = (int) files.size() - 1;
            file_index = 0;
        }
    } else {
        highlight_index = max_lines / 2;
        file_index = new_index - highlight_index;
    }

    updateFiles();
}

void Filer::setSize(const Vector2f &size) {
    Filer::setSize(size.x, size.y);
}

void Filer::setSize(float width, float height) {
    RectangleShape::setSize(width, height);
    highlight->setSize(width, highlight->getSize().y);
    for (auto &line : lines) {
        line->setSize(width, line->getSize().y);
    }
}

std::vector<Io::File> Filer::getFiles() {
    return files;
}

Io::File Filer::getSelection() {
    if (!files.empty() && files.size() > (size_t) file_index + highlight_index) {
        return files[file_index + highlight_index];
    }
    return Io::File();
}

void Filer::setTextColor(const Color &color) {
    for (auto &line : lines) {
        line->getText()->setFillColor(color);
    }
}

void Filer::setTextOutlineColor(const Color &color) {
    for (auto &line : lines) {
        line->getText()->setOutlineColor(color);
    }
}

void Filer::setTextOutlineThickness(float thickness) {
    for (auto &line : lines) {
        line->getText()->setOutlineThickness(thickness);
    }
}

void Filer::setAlpha(uint8_t alpha, bool recursive) {
    for (auto &line : lines) {
        line->getText()->setAlpha(alpha);
    }
    Shape::setAlpha(alpha, recursive);
}

void Filer::setHighlightEnabled(bool enable) {
    use_highlight = enable;
    highlight->setVisibility(enable ? Visibility::Visible
                                    : Visibility::Hidden);
}

void Filer::setHighlightUseFileColor(bool enable) {
    highlight_use_files_color = enable;
    setSelection(file_index);
}

int Filer::getMaxLines() {
    return max_lines;
}

int Filer::getIndex() {
    return file_index + highlight_index;
}

RectangleShape *Filer::getHighlight() {
    return highlight;
}
