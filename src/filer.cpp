//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filer.h"
#include "utility.h"
#include "isoloader.h"
#include "biosflash.h"

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
    text->setString(string);
}

void Line::setColor(const Color &color) {
    text->setFillColor(color);
}

Text *Line::getText() {
    return text;
}

Filer::Filer(RetroDream *rd, const c2d::FloatRect &rect, const std::string &path, int lineSpacing)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

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
    highlight = new RoundedRectangleShape(Vector2f(rect.width - 2, line_height), 8, 4);
    highlight->setFillColor(COL_YELLOW);
    highlight->setOutlineColor(COL_BLUE_DARK);
    highlight->setOutlineThickness(2);
    add(highlight);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {1, (line_height * (float) i) + 1, rect.width - 2, line_height - 2};
        auto line = new Line(r, "", retroDream->getRender()->getFont(), FONT_SIZE);
        line->setLayer(2);
        lines.push_back(line);
        add(line);
    }

    // "hide main rect" layer
    blurLayer = new RectangleShape({-rect.left - 16, -rect.top - 16,
                                    retroDream->getRender()->getSize().x + 64,
                                    retroDream->getRender()->getSize().y + 64});
    blurLayer->setFillColor(Color::Gray);
    blurLayer->setLayer(3);
    blurLayer->add(new TweenAlpha(0, 230, 0.3));
    blurLayer->setVisibility(Visibility::Hidden);
    add(blurLayer);

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
            lines[i]->setString(file.upperName);
            lines[i]->getText()->setFillColor(file.data.type == Io::Type::File ? colorFile : colorDir);
            // set highlight position and color
            if ((int) i == highlight_index) {
                // handle highlight
                highlight->setPosition(lines[i]->getPosition());
                // handle header title
                if (file.isGame) {
                    retroDream->getHeader()->setStringLeft(file.upperName);
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
                    retroDream->getHeader()->setStringLeft(Utility::toUpper(file.data.path));
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

    if (p.empty()) {
        return false;
    }

    printf("getDir(%s):\n", p.c_str());
    // TODO: fix random crash :/
    //retroDream->showStatus("LOADING DIRECTORY...", p, COL_GREEN);

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }

    std::vector<Io::File> dirList = io->getDirList(path, true, false);
    if (p != "/" && (dirList.empty() || dirList.at(0).name != "..")) {
        Io::File file("..", "..", Io::Type::Directory, 0, colorDir);
        dirList.insert(dirList.begin(), file);
    }

    std::string dsPath = retroDream->getConfig()->get(RetroConfig::OptionId::DsPath);

    for (auto const &fileData : dirList) {
        RetroFile file;
        file.data = fileData;
        file.upperName = Utility::toUpper(file.data.name);
        if (fileData.type == Io::Type::File) {
            if (RetroUtility::isGame(file.data.name)) {
                file.isGame = true;
                file.isoPath = file.data.path;
                if (Utility::endsWith(file.isoPath, ".iso")) {
                    file.isoType = "ISO";
                } else if (Utility::endsWith(file.isoPath, ".cdi")) {
                    file.isoType = "CDI";
                } else if (Utility::endsWith(file.isoPath, ".gdi")) {
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
                    // if we found an iso/cdi and more than one file in the directory, don't add it as a game
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
    previewClock.restart();

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

    if (new_index <= max_lines / 2) {
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

Filer::RetroFile Filer::getSelection() {

    if (!files.empty() && files.size() > (size_t) file_index + highlight_index) {
        return files.at(file_index + highlight_index);
    }

    return Filer::RetroFile();
}

Line *Filer::getSelectionLine() {

    if ((size_t) highlight_index < files.size()) {
        return lines.at(highlight_index);
    }

    return nullptr;
}

void Filer::setSelectionFront() {
    Line *line = getSelectionLine();
    if (line != nullptr) {
        highlight->setLayer(4);
        line->setLayer(5);
    }
}

void Filer::setSelectionBack() {
    Line *line = getSelectionLine();
    if (line != nullptr) {
        // TODO: fix layers ?
        highlight->setLayer(1);
        line->setLayer(2);
    }
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

std::string Filer::getPath() {
    return path;
}

void Filer::onUpdate() {

    if (!isVisible()
        || retroDream->getOptionMenu()->isVisible()
        || retroDream->getPresetMenu()->isVisible()
        || retroDream->getFileMenu()->isVisible()
        || retroDream->getSystemMenu()->isVisible()
        || retroDream->getRegionFreeMenu()->isVisible()
        || retroDream->getCredits()->isVisible()
        || retroDream->getProgressBox()->isVisible()) {
        return;
    }

    unsigned int keys = retroDream->getRender()->getInput()->getKeys();

    if (keys == 0) {
        if (getSelection().isGame && !getSelection().preview.empty() && !retroDream->getPreview()->isLoaded()
            && previewClock.getElapsedTime().asMilliseconds() > previewLoadDelay) {
            bool loaded = retroDream->getPreview()->load(getSelection().preview);
            if (!loaded) {
                retroDream->showStatus("PREVIEW NOT FOUND", getSelection().preview, COL_RED);
            }
        }
    }

    RoundedRectangleShape::onUpdate();
}

bool Filer::onInput(c2d::Input::Player *players) {

    if (retroDream->getOptionMenu()->isVisible()
        || retroDream->getPresetMenu()->isVisible()
        || retroDream->getFileMenu()->isVisible()
        || retroDream->getSystemMenu()->isVisible()
        || retroDream->getRegionFreeMenu()->isVisible()
        || retroDream->getCredits()->isVisible()
        || retroDream->getProgressBox()->isVisible()) {
        return false;
    }

    unsigned int keys = players[0].keys;

    if (keys & Input::Key::Up) {
        retroDream->getPreview()->unload();
        previewClock.restart();
        up();
    } else if (keys & Input::Key::Down) {
        retroDream->getPreview()->unload();
        previewClock.restart();
        down();
    } else if (keys & Input::Key::Right) {
        retroDream->getPreview()->unload();
        previewClock.restart();
        setSelection(getIndex() + getMaxLines());
    } else if (keys & Input::Key::Left) {
        retroDream->getPreview()->unload();
        previewClock.restart();
        setSelection(getIndex() - getMaxLines());
    } else if (keys & Input::Key::Fire1) {
        RetroFile file = getSelection();
        Io::Type type = file.data.type;
        if (file.isGame) {
            // save last path
            retroDream->getConfig()->set(RetroConfig::FilerPath, path);
            IsoLoader::run(retroDream, file.isoPath);
        } else if (type == Io::Type::Directory) {
            retroDream->getPreview()->unload();
            previewClock.restart();
            enter(getIndex());
        } else if (RetroUtility::isElf(file.data.name)) {
            RetroUtility::exec(file.data.path);
        } else if (Utility::endsWith(file.data.name, ".bios", false)) {
            blurLayer->setVisibility(Visibility::Visible, true);
            int ret = retroDream->getMessageBox()->show("BIOS FLASH",
                                                        "YOU ARE ABOUT TO WRITE '" + file.upperName
                                                        + "' TO YOUR ROM CHIP.\n\n"
                                                          "BE SURE YOU KNOW WHAT YOU'RE DOING BEFORE SELECTING THE 'CONFIRM' BOX",
                                                        "CANCEL", "CONFIRM");
            if (ret == MessageBox::RIGHT) {
                retroDream->getProgressBox()->setTitle("BIOS FLASHING IN PROGRESS");
                retroDream->getProgressBox()->setMessage(
                        "\n\nFLASHING BIOS TO FLASH ROM, DO NOT POWER OFF THE DREAMCAST OR DO ANYTHING STUPID...");
                retroDream->getProgressBox()->setProgress("LOADING STUFF.... \n", 0.0f);
                retroDream->getProgressBox()->setVisibility(Visibility::Visible);
                RetroDream *rd = retroDream;
                BiosFlash::flash(file.data.path, [rd](const std::string &msg, float progress) {
                    if (progress < 0) {
                        rd->getProgressBox()->setVisibility(Visibility::Hidden);
                        rd->getMessageBox()->show("BIOS FLASH - ERROR", "\n\n\n" + msg, "OK");
                        rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                    } else if (progress > 1) {
                        rd->getProgressBox()->setVisibility(Visibility::Hidden);
                        rd->getMessageBox()->getTitleText()->setFillColor(COL_GREEN);
                        rd->getMessageBox()->show("BIOS FLASH - SUCCESS", "\n\n\n" + msg, "OK");
                        rd->getMessageBox()->getTitleText()->setFillColor(COL_RED);
                        rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
                    } else {
                        rd->getProgressBox()->setProgress(msg, progress);
                        rd->getRender()->flip(true, false);
                    }
                });
            } else {
                retroDream->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
            }
        }
    } else if (keys & Input::Key::Fire2) {
        retroDream->getPreview()->unload();
        previewClock.restart();
        exit();
    }

    return true;
}
