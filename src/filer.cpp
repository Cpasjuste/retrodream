//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filer.h"
#include "utility.h"
#include "isoloader.h"

using namespace c2d;

Line::Line(const FloatRect &rect, const std::string &str, Font *font, unsigned int fontSize) : RectangleShape(rect) {

    text = new Text(str, fontSize, font);
    text->setOutlineColor(Color::Black);
    text->setOutlineThickness(2);
    text->setOrigin(Origin::Left);
    text->setPosition(6, rect.height / 2);
    text->setSizeMax(rect.width - ((float) fontSize) - 4, 0);

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

Filer::Filer(RetroDream *rd, const c2d::FloatRect &rect, const std::string &path, int lineSpacing)
        : RoundedRectangleShape({rect.width, rect.height}, 10, 8) {

    retroDream = rd;
    io = (RetroIo *) retroDream->getRender()->getIo();

    setPosition(rect.left, rect.top);

    // set default colors
    colorDir = COL_BLUE_DARK;
    colorFile = COL_BLUE;

    // calculate number of lines shown
    line_height = FONT_SIZE + lineSpacing;
    max_lines = (int) (rect.height / line_height);
    if ((float) max_lines * line_height < rect.height) {
        line_height = rect.height / (float) max_lines;
    }

    // add selection rectangle (highlight)
    highlight = new RoundedRectangleShape(Vector2f(rect.width - 3, line_height), 10, 8);
    highlight->setFillColor(COL_YELLOW);
    highlight->setOutlineColor(COL_BLUE_DARK);
    highlight->setOutlineThickness(2);
    add(highlight);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {1, (line_height * (float) i) + 1, rect.width - 2, line_height - 2};
        auto line = new Line(r, "", retroDream->getRender()->getFont(), FONT_SIZE);
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
            lines[i]->setString(Utility::toUpper(file.data.name));
            lines[i]->getText()->setFillColor(file.data.type == Io::Type::File ? colorFile : colorDir);
            // set highlight position and color
            if ((int) i == highlight_index) {
                // handle highlight
                highlight->setPosition(lines[i]->getPosition());
                // handle header title
                if (file.isGame) {
                    retroDream->getHeader()->setStringLeft(file.data.name);
                    retroDream->getHeader()->setStringRight(file.isoType);
                    if (file.isoType == "CDI") {
                        retroDream->getHeader()->setStringRightColor(COL_YELLOW);
                    } else if (file.isoType == "GDI") {
                        retroDream->getHeader()->setStringRightColor(COL_GREEN);
                    } else if (file.isoType == "OPT") {
                        retroDream->getHeader()->setStringRightColor(COL_GREEN_DARK);
                    } else if (file.isoType == "ISO") {
                        retroDream->getHeader()->setStringRightColor(COL_ORANGE);
                    }
                    // help buttons
                    retroDream->getHelpBox()->setString(HelpBox::HelpButton::A, "RUN GAME");
                    retroDream->getHelpBox()->setString(HelpBox::HelpButton::Y, "GAME OPTIONS");
                } else {
                    retroDream->getHeader()->setStringLeft(file.data.path);
                    retroDream->getHeader()->setStringRight(file.data.type == Io::Type::File ? "FIL" : "DIR");
                    retroDream->getHeader()->setStringRightColor(COL_BLUE_DARK);
                    // help buttons
                    retroDream->getHelpBox()->setString(HelpBox::HelpButton::A, "ENTER");
                    retroDream->getHelpBox()->setString(HelpBox::HelpButton::Y, "UNUSED");
                }
            }
        }
    }

    if (files.empty()) {
        highlight->setVisibility(Visibility::Hidden, false);
    } else {
        highlight->setVisibility(Visibility::Visible, false);
    }
}

bool Filer::getDir(const std::string &p) {

    printf("getDir(%s):\n", p.c_str());
    retroDream->showStatus("LOADING DIRECTORY...", p);

    if (p.empty()) {
        return false;
    }

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }
    retroDream->getConfig()->set(RetroConfig::FilerPath, path);

    std::vector<Io::File> dirList = io->getDirList(path, true, false);
    if (p != "/" && (dirList.empty() || dirList.at(0).name != "..")) {
        Io::File file("..", "..", Io::Type::Directory, 0, colorDir);
        dirList.insert(dirList.begin(), file);
    }

    std::string dsPath = retroDream->getConfig()->get(RetroConfig::OptionId::DsPath);

    for (auto const &fileData : dirList) {
        RetroFile file;
        file.data = fileData;
        if (fileData.type == Io::Type::File) {
            if (RetroUtility::isGame(file.data.name)) {
                file.isGame = true;
                file.isoPath = file.data.path;
                if (Utility::endsWith(file.isoPath, ".iso", false)) {
                    file.isoType = "ISO";
                } else if (Utility::endsWith(file.isoPath, ".cdi", false)) {
                    file.isoType = "CDI";
                } else if (Utility::endsWith(file.isoPath, ".gdi", false)) {
                    file.isoType = "GDI";
                }
                // DreamShell compatibility
                file.preview = dsPath + "apps/iso_loader/covers/" + Utility::removeExt(file.data.name) + ".jpg";
            }
        } else if (fileData.type == Io::Type::Directory) {
            Io::File gameFile = io->findFile(fileData.path, {".gdi", ".cdi", ".iso"}, "track");
            // directory contains a game
            if (!gameFile.name.empty()) {
                // if sub dir contains iso or cdi but more than one file, don't assume it's a game
                if (Utility::endsWith(gameFile.name, ".cdi")) {
                    file.isoType = "CDI";
                } else if (Utility::endsWith(gameFile.name, ".iso")) {
                    file.isoType = "ISO";
                }
                if (!file.isoType.empty()) {
                    if (io->hasMoreThanOneFile(fileData.path)) {
                        files.emplace_back(file);
                        continue;
                    }
                }
                file.isGame = true;
                file.isoPath = gameFile.path;
                if (file.isoType.empty()) {
                    if (io->exist(fileData.path + "/track01.iso")) {
                        file.isoType = "OPT";
                    } else {
                        file.isoType = "GDI";
                    }
                }
                // DreamShell compatibility
                file.preview = dsPath + "apps/iso_loader/covers/" + Utility::removeExt(gameFile.name) + ".jpg";
            }
        }

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

    if (new_index == 0 && files.size() > 1) {
        new_index = 1;
    }

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
        return files.at(file_index + highlight_index);
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

    if (!isVisible()
        || retroDream->getOptionMenu()->isVisible()
        || retroDream->getFileMenu()->isVisible()) {
        return;
    }

    unsigned int keys = retroDream->getRender()->getInput()->getKeys();

    if (keys > 0 && keys != Input::Delay
        && keys != Input::Key::Start && keys != Input::Key::Fire1
        && keys != Input::Key::Fire3 && keys != Input::Key::Fire4) {
        retroDream->getPreview()->unload();
        previewClock.restart();
    } else if (keys == 0) {
        if (getSelection().isGame && !getSelection().preview.empty() && !retroDream->getPreview()->isLoaded()
            && previewClock.getElapsedTime().asMilliseconds() > previewLoadDelay) {
            retroDream->getPreview()->load(getSelection().preview);
            retroDream->showStatus("LOADING PREVIEW...", getSelection().preview);
        }
    }

    RoundedRectangleShape::onUpdate();
}

bool Filer::onInput(c2d::Input::Player *players) {

    if (retroDream->getOptionMenu()->isVisible()
        || retroDream->getFileMenu()->isVisible()) {
        return false;
    }

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        up();
    } else if (keys & Input::Key::Down) {
        down();
    } else if (keys & Input::Key::Right) {
        setSelection(getIndex() + getMaxLines());
    } else if (keys & Input::Key::Left) {
        setSelection(getIndex() - getMaxLines());
    } else if (keys & Input::Key::Fire1) {
        Io::Type type = getSelection().data.type;
        if (getSelection().isGame) {
            IsoLoader::run(retroDream, getSelection().isoPath);
        } else if (type == Io::Type::File && RetroUtility::isElf(getSelection().data.name)) {
            RetroUtility::exec(getSelection().data.path);
        } else if (type == Io::Type::Directory) {
            enter(getIndex());
        }
    } else if (keys & Input::Key::Fire2) {
        exit();
    }

    return true;
}
