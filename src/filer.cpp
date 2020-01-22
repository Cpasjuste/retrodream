//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filer.h"
#include "utility.h"

using namespace c2d;

Line::Line(const FloatRect &rect, const std::string &str, Font *font, unsigned int fontSize) : RectangleShape(rect) {

    text = new Text(str, fontSize, font);
    text->setOutlineColor(Color::Black);
    text->setOutlineThickness(2);
    text->setOrigin(Origin::Left);
    text->setPosition(6, getSize().y / 2);
    text->setSizeMax(getSize().x - ((float) fontSize), 0);

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

Filer::Filer(RetroDream *rd, const c2d::FloatRect &rect, const std::string &path)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    retroDream = rd;
    io = retroDream->getRender()->getIo();
    setPosition(rect.left, rect.top);

    // set default colors
    colorDir = COL_BLUE_DARK;
    colorFile = COL_BLUE;

    // calculate number of lines shown
    line_height = getSize().y / 13; // 13 lines
    max_lines = (int) (getSize().y / line_height);
    if ((float) max_lines * line_height < getSize().y) {
        line_height = getSize().y / (float) max_lines;
    }

    // add selection rectangle (highlight)
    highlight = new RoundedRectangleShape(Vector2f(getSize().x - 2, line_height), 10, 8);
    highlight->setFillColor(COL_YELLOW);
    highlight->setOutlineColor(COL_BLUE_DARK);
    highlight->setOutlineThickness(2);
    add(highlight);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {1, (line_height * (float) i) + 1, getSize().x - 2, line_height - 2};
        auto line = new Line(r, "", retroDream->getRender()->getFont(),
                             (unsigned int) (line_height - (line_height / 10)));
        lines.push_back(line);
        add(line);
    }

    getDir(path);
};

void Filer::updateLines() {

    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {

        if (file_index + i >= files.size()) {
            lines[i]->setVisibility(Visibility::Hidden);
        } else {
            // set file
            Filer::RetroFile file = files[file_index + i];
            lines[i]->setVisibility(Visibility::Visible);
            lines[i]->setString(file.data.name);
            lines[i]->getText()->setFillColor(file.data.type == Io::Type::File ? colorFile : colorDir);
            // set highlight position and color
            if ((int) i == highlight_index) {
                // handle highlight
                highlight->setPosition(lines[i]->getPosition());
                Color color = highlight_use_files_color ?
                              lines[i]->getText()->getFillColor() : highlight->getFillColor();
                color.a = highlight->getAlpha();
                highlight->setFillColor(color);
                color = highlight_use_files_color ?
                        lines[i]->getText()->getFillColor() : highlight->getOutlineColor();
                highlight->setOutlineColor(color);
                // handle header title
                retroDream->getHeader()->setString(lines[i]->getText()->getString());
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

    printf("getDir(%s):\n", p.c_str());

    if (p.empty()) {
        return false;
    }

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }

    retroDream->getConfig()->setLastPath(path);

    std::vector<Io::File> dirList = io->getDirList(path, true);
    if (p != "/" && (dirList.empty() || dirList.at(0).name != "..")) {
        Io::File file("..", "..", Io::Type::Directory, 0, colorDir);
        dirList.insert(dirList.begin(), file);
    }

    for (auto const &fileData : dirList) {
        RetroFile file;
        file.data = fileData;

        if (fileData.type == Io::Type::File) {
            if (RetroUtility::isGame(file.data.name)) {
                printf("\tI\t%s\n", file.data.name.c_str());
                file.isGame = true;
                file.isoPath = file.data.path;
            } else {
                printf("\tF\t%s\n", file.data.name.c_str());
            }
        } else if (fileData.type == Io::Type::Directory) {
            // lxdream crash
            if (file.data.name == "pc") {
                continue;
            }
            // search sub directory for a game (.iso, .cdi, .gdi)
            std::vector<Io::File> subFiles = io->getDirList(fileData.path);
            auto gameFile = std::find_if(subFiles.begin(), subFiles.end(), [](const Io::File &f) {
                return f.type == Io::Type::File && RetroUtility::isGame(f.name);
            });
            // directory contains a game
            if (gameFile != subFiles.end()) {
                printf("\tI\t%s\n", file.data.name.c_str());
                file.isGame = true;
                file.isoPath = gameFile->path;
                // search for a preview image in the game sub directory
                auto previewFile = std::find_if(subFiles.begin(), subFiles.end(), [](const Io::File &f) {
                    return Utility::endsWith(f.name, ".jpg", false)
                           || Utility::endsWith(f.name, ".png", false);
                });
                if (previewFile != subFiles.end()) {
                    file.preview = previewFile->path;
                } else {
                    // DreamShell compatibility
#ifdef __DREAMCAST__
                    std::string dev = "/ide/";
#else
                    std::string dev = "/media/cpasjuste/SSD/dreamcast/";
#endif
                    if (io->exist(dev + "DS/apps/iso_loader/covers/" + file.data.name + ".jpg")) {
                        file.preview = dev + "DS/apps/iso_loader/covers/" + file.data.name + ".jpg";
                    } else {
#ifdef __DREAMCAST__
                        if (io->exist("/sd/DS/apps/iso_loader/covers/" + file.data.name + ".jpg")) {
                            file.preview = "/sd/DS/apps/iso_loader/covers/" + file.data.name + ".jpg";
                        }
#endif
                    }
                }
            } else {
                printf("\tD\t%s\n", file.data.name.c_str());
            }
        }

        // TODO: build preset path (isoloader.c)
        files.emplace_back(file);
    }

    setSelection(0);

    return true;
}

void Filer::enter(int index) {

    RetroFile file = getSelection();
    bool success;

    if (file.data.name == "..") {
        exit();
        return;
    }

    if (path == "/") {
        success = getDir(path + file.data.name);
    } else {
        success = getDir(path + "/" + file.data.name);
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

    updateLines();
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

    updateLines();
}

void Filer::setSelection(int new_index) {

    if (new_index < max_lines / 2) {
        file_index = 0;
        highlight_index = new_index < 0 ? 0 : new_index;
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

    updateLines();
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

Filer::RetroFile Filer::getSelection() {
    if (!files.empty() && files.size() > (size_t) file_index + highlight_index) {
        return files[file_index + highlight_index];
    }
    return Filer::RetroFile();
}

void Filer::setColor(const Color &dirColor, const Color &fileColor) {
    colorDir = dirColor;
    colorFile = fileColor;
    updateLines();
}

void Filer::setAlpha(uint8_t alpha, bool recursive) {
    for (auto &line : lines) {
        line->getText()->setAlpha(alpha);
        line->getText()->setOutlineThickness(alpha < 255 ? 1 : 2);
    }
    highlight->setAlpha(alpha < 150 ? alpha : 150);
    if (alpha > 150) {
        Color c = highlight->getOutlineColor();
        c.a = 255;
        highlight->setOutlineColor(c);
    }
    Shape::setAlpha(alpha);
}

int Filer::getMaxLines() {
    return max_lines;
}

int Filer::getIndex() {
    return file_index + highlight_index;
}

void Filer::onUpdate() {

    if (!isVisible()) {
        return;
    }

    unsigned int keys = retroDream->getRender()->getInput()->getKeys();

    if (keys > 0 && keys != Input::Delay &&
        ((keys == Input::Key::Up) || (keys == Input::Key::Down)
         || (keys == Input::Key::Left) || (keys == Input::Key::Right) || (keys == Input::Key::Fire2))) {
        previewClock.restart();
        retroDream->getPreview()->unload();
    } else if (keys == 0) {
        if (getSelection().isGame && !getSelection().preview.empty() && !retroDream->getPreview()->isLoaded()
            && previewClock.getElapsedTime().asMilliseconds() > previewLoadDelay) {
            retroDream->getPreview()->load(getSelection().preview);
        }
    }

    C2DObject::onUpdate();
}

