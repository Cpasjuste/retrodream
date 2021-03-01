//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "main.h"
#include "colors.h"
#include "filer.h"
#include "utility.h"
#include "isoloader.h"
#include "flash/biosflash.h"
#include "flash/block1partition.h"

using namespace c2d;

Line::Line(const FloatRect &rect, const std::string &str, Font *font, unsigned int fontSize) : RectangleShape(rect) {

    RectangleShape::setFillColor(Color::Transparent);

    text = new Text(str, fontSize, font);
    text->setOrigin(Origin::Left);
    text->setPosition(6, rect.height / 2);
    text->setSizeMax(rect.width - ((float) fontSize) - 4, 0);
    RectangleShape::add(text);
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

Filer::Filer(RetroDream *rd, Skin::CustomShape *shape, const std::string &path, int lineSpacing)
        : SkinRect(shape) {

    retroDream = rd;
    io = (RetroIo *) retroDream->getRender()->getIo();

    // calculate number of lines shown
    line_height = FONT_SIZE + lineSpacing;
    max_lines = (int) (shape->rect.height / line_height);
    if ((float) max_lines * line_height < shape->rect.height) {
        line_height = shape->rect.height / (float) max_lines;
    }

    // add selection rectangle (highlight)
    Skin::CustomShape shape2 = RetroDream::getSkin()->getShape(Skin::Id::FilerHighlightShape);
    highlight = new SkinRect(&shape2);
    SkinRect::add(highlight);

    // custom lines/text colors
    fileColor = RetroDream::getSkin()->getColor(Skin::Id::FilerFileText);
    dirColor = RetroDream::getSkin()->getColor(Skin::Id::FilerDirText);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {0, (line_height * (float) i), shape->rect.width, line_height};
        auto line = new Line(r, "", retroDream->getRender()->getFont(), FONT_SIZE);
        line->setLayer(2);
        lines.push_back(line);
        SkinRect::add(line);
    }

    // "hide main rect" layer
    blurLayer = new RectangleShape({-shape->rect.left - 16, -shape->rect.top - 16,
                                    retroDream->getRender()->getSize().x + 64,
                                    retroDream->getRender()->getSize().y + 64});
    blurLayer->setFillColor(Color::Gray);
    blurLayer->setLayer(3);
    blurLayer->add(new TweenAlpha(0, 230, 0.3));
    blurLayer->setVisibility(Visibility::Hidden);
    SkinRect::add(blurLayer);

    // previews timer delay
    previewImageDelay = RetroDream::getConfig()->getInt(RetroConfig::PreviewImageDelay);
    previewVideoDelay = RetroDream::getConfig()->getInt(RetroConfig::PreviewVideoDelay);

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
            Skin::CustomColor *color = file.data.type == Io::Type::File ? &fileColor : &dirColor;
            lines[i]->getText()->setFillColor(color->color);
            lines[i]->getText()->setOutlineColor(color->outlineColor);
            lines[i]->getText()->setOutlineThickness(color->outlineSize);
            // set highlight position and color
            if ((int) i == highlight_index) {
                // handle highlight
                highlight->setPosition({highlight->getPosition().x, lines[i]->getPosition().y});
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
                    retroDream->getHelpBox()->setString(HelpBox::HelpButton::A, "ENTER DIRECTORY");
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

    //printf("getDir(%s):\n", p.c_str());
    // TODO: fix random crash :/
    //retroDream->showStatus("LOADING DIRECTORY...", p, COL_GREEN);

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }

    std::vector<Io::File> dirList = io->getDirList(path, true, false);
    if (p != "/" && (dirList.empty() || dirList.at(0).name != "..")) {
        Io::File file("..", "..", Io::Type::Directory, 0);
        dirList.insert(dirList.begin(), file);
    }

    for (auto const &fileData : dirList) {
        RetroFile file;
        file.data = fileData;
        file.upperName = Utility::toUpper(file.data.name);
        if (fileData.type == Io::Type::File) {
            if (RetroUtility::isGame(file.data.name)) {
                file.isGame = true;
                if (Utility::endsWith(file.data.path, ".iso")) {
                    file.isoType = "ISO";
                } else if (Utility::endsWith(file.data.path, ".cdi")) {
                    file.isoType = "CDI";
                } else if (Utility::endsWith(file.data.path, ".gdi")) {
                    file.isoType = "GDI";
                }
            }
        } else if (fileData.type == Io::Type::Directory) {
#ifdef __DREAMCAST__
            if (DCIo::existsFile(fileData.path + "/track01.iso")) {
                file.isoType = "OPT";
                file.isGame = true;
            } else if (DCIo::existsFile(fileData.path + "/track01.bin")) {
                file.isoType = "GDI";
                file.isGame = true;
            }
#else
            if (io->exist(fileData.path + "/track01.iso")) {
                file.isoType = "OPT";
                file.isGame = true;
            } else if (io->exist(fileData.path + "/track01.bin")) {
                file.isoType = "GDI";
                file.isGame = true;
            }
#endif
        }

        files.push_back(file);
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

Filer::RetroFile *Filer::getSelectionPtr() {

    if (!files.empty() && files.size() > (size_t) file_index + highlight_index) {
        return &files.at(file_index + highlight_index);
    }

    return nullptr;
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
        Filer::RetroFile *file = getSelectionPtr();
        /*
        // update dir information here as it's too slow from getDirList
        if (file && file->data.type == Io::Type::Directory
            && !retroDream->getPreviewImage()->isLoaded() &&
            previewClock.getElapsedTime().asMilliseconds() > previewImageDelay
            && file->isoType.empty()) {
            if (DCIo::existsFile(file->data.path + "/track01.iso")) {
                file->isoType = "OPT";
                file->isGame = true;
            } else if (DCIo::existsFile(file->data.path + "/track01.bin")) {
                file->isoType = "GDI";
                file->isGame = true;
            } else {
                file->isoType = "DIR";
            }
        }
        */
        if (file && file->isGame) {
            if (previewClock.getElapsedTime().asMilliseconds() > previewVideoDelay) {
                // load preview video
                if (!retroDream->getPreviewVideo()->isLoaded()) {
                    // set preview path
                    std::string gameName = file->data.name;
                    if (file->data.type == Io::Type::Directory) {
                        Io::File gameFile = io->findFile(file->data.path, {".gdi", ".cdi", ".iso"}, "track");
                        gameName = gameFile.name;
                    }
                    if (!gameName.empty()) {
                        std::string mediaPath = retroDream->getIo()->getDataPath() + "medias/";
                        std::string videoPath = RetroDream::getConfig()->get(RetroConfig::PreviewVideoPath);
                        std::string noExt = Utility::removeExt(gameName);
                        retroDream->getPreviewVideo()->load(mediaPath + videoPath + "/" + noExt + ".roq");
                    }
                }
            } else if (previewClock.getElapsedTime().asMilliseconds() > previewImageDelay) {
                // load preview image
                if (!retroDream->getPreviewImage()->isLoaded()) {
                    // set preview path
                    std::string gameName = file->data.name;
                    if (file->data.type == Io::Type::Directory) {
                        Io::File gameFile = io->findFile(file->data.path, {".gdi", ".cdi", ".iso"}, "track");
                        gameName = gameFile.name;
                    }
                    if (!gameName.empty()) {
                        std::string mediaPath = retroDream->getIo()->getDataPath() + "medias/";
                        std::string imagePath = RetroDream::getConfig()->get(RetroConfig::PreviewImagePath);
                        std::string noExt = Utility::removeExt(gameName);
                        retroDream->getPreviewImage()->load(mediaPath + imagePath + "/" + noExt + ".png");
                    }
                }
            }
        }
    }

    RectangleShape::onUpdate();
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

    // stop previews in any case
    retroDream->getPreviewImage()->unload();
    retroDream->getPreviewVideo()->unload();
    previewClock.restart();

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
        RetroFile file = getSelection();
        Io::Type type = file.data.type;
        if (file.isGame) {
            // save last path
            RetroDream::getConfig()->set(RetroConfig::FilerPath, path);
            if (file.data.type == Io::Type::Directory) {
                Io::File gameFile = io->findFile(file.data.path, {".gdi", ".cdi", ".iso"}, "track");
                if (!gameFile.name.empty()) {
                    IsoLoader::run(retroDream, gameFile.path);
                }
            } else {
                IsoLoader::run(retroDream, file.data.path);
            }
        } else if (type == Io::Type::Directory) {
            if (file.data.path == "/cd" && GDPlay::isGame()) {
                int ret = retroDream->getMessageBox()->show("RUN GDROM ?",
                                                            "DO YOU WANT TO RUN THIS GDROM OR BROWSE IT ?",
                                                            "RUN", "BROWSE");
                if (ret == MessageBox::RIGHT) {
                    enter(getIndex());
                } else {
                    GDPlay::run(retroDream);
                }
            } else {
                enter(getIndex());
            }
        } else if (RetroUtility::isElf(file.data.name)) {
            RetroUtility::exec(file.data.path);
        } else if (Utility::endsWith(file.data.name, ".bios", false)) {
            blurLayer->setVisibility(Visibility::Visible, true);
            flashBios(file);
            blurLayer->setVisibility(Visibility::Hidden, true);
        } else if (file.data.name == "system.rom" || file.data.name == "block1.rom") {
            blurLayer->setVisibility(Visibility::Visible, true);
            flashRom(file);
            blurLayer->setVisibility(Visibility::Hidden, true);
        } else if (Utility::endsWith(file.data.name, ".vmu", false)) {
            blurLayer->setVisibility(Visibility::Visible, true);
            restoreVmu(file);
            blurLayer->setVisibility(Visibility::Hidden, true);
        }
    } else if (keys & Input::Key::Fire2) {
        exit();
    }

    return true;
}

void Filer::flashBios(const RetroFile &file) {

    int ret = retroDream->getMessageBox()->show("BIOS FLASH",
                                                "YOU ARE ABOUT TO WRITE TO YOUR ROM CHIP."
                                                "BE SURE YOU KNOW WHAT YOU'RE DOING!",
                                                "CANCEL", "CONFIRM");
    if (ret == MessageBox::RIGHT) {
        retroDream->getProgressBox()->setTitle("BIOS FLASHING IN PROGRESS");
        retroDream->getProgressBox()->setMessage(
                "\nFLASHING BIOS TO FLASH ROM, DO NOT POWER OFF THE DREAMCAST OR DO ANYTHING STUPID...");
        retroDream->getProgressBox()->setProgress("LOADING STUFF.... \n", 0.0f);
        retroDream->getProgressBox()->setVisibility(Visibility::Visible);
        RetroDream *rd = retroDream;
        BiosFlash::flash(file.data.path, [rd](const std::string &msg, float progress) {
            if (progress < 0) {
                rd->getProgressBox()->setVisibility(Visibility::Hidden);
                rd->getMessageBox()->show("BIOS FLASH - ERROR", "\n" + msg, "OK");
                rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
            } else if (progress > 1) {
                rd->getProgressBox()->setVisibility(Visibility::Hidden);
                rd->getMessageBox()->getTitleText()->setFillColor(COL_GREEN);
                rd->getMessageBox()->show("BIOS FLASH - SUCCESS", "\n" + msg, "OK");
                rd->getMessageBox()->getTitleText()->setFillColor(COL_RED);
                rd->getFiler()->getBlur()->setVisibility(Visibility::Hidden, true);
            } else {
                rd->getProgressBox()->setProgress(msg, progress);
                rd->getRender()->flip(true, false);
            }
        });
    }
}

void Filer::flashRom(const RetroFile &file) {

    int ret = retroDream->getMessageBox()->show("ROM FLASH",
                                                "YOU ARE ABOUT TO WRITE TO YOUR FLASH ROM. "
                                                "BE SURE YOU KNOW WHAT YOU'RE DOING!",
                                                "CANCEL", "CONFIRM");
    if (ret == MessageBox::RIGHT) {

        // set partition
        Partition *partition;
        if (file.data.name == "system.rom") {
            partition = new SystemPartition();
        } else {
            partition = new Block1Partition();
        }

        if (!partition->read(io, file.data.path)) {
            retroDream->showStatus("FLASH ROM READ ERROR", partition->getErrorString());
            delete (partition);
            return;
        }

        if (!partition->write()) {
            retroDream->showStatus("FLASH ROM WRITE ERROR", partition->getErrorString());
            delete (partition);
            return;
        }

        delete (partition);
        retroDream->showStatus("FLASH ROM", "FLASHROM BACKUP SUCCESSFULLY RESTORED", COL_GREEN);
    }
}

void Filer::restoreVmu(const Filer::RetroFile &file) {

    int ret = retroDream->getMessageBox()->show("VMU RESTORE",
                                                "YOU ARE ABOUT TO RESTORE TO YOUR SLOT ONE VMU.",
                                                "CANCEL", "CONFIRM");
    if (ret == MessageBox::RIGHT) {

        retroDream->getProgressBox()->setTitle("VMU RESTORE IN PROGRESS");
        retroDream->getProgressBox()->setMessage("\nDOING RAW VMU RESTORE...");
        retroDream->getProgressBox()->setProgress("PLEASE WAIT.... \n", 0.0f);
        retroDream->getProgressBox()->setVisibility(Visibility::Visible);

        RetroDream *rd = retroDream;
        RetroUtility::vmuRestore(
                file.data.path, [rd](const std::string &msg, float progress) {
                    if (progress < 0) {
                        rd->getProgressBox()->setVisibility(Visibility::Hidden);
                        rd->showStatus("VMU RESTORE ERROR", msg);
                    } else if (progress > 1) {
                        rd->getProgressBox()->setVisibility(Visibility::Hidden);
                        rd->showStatus("VMU RESTORE SUCCESS", "VMU RESTORED FROM " + msg, COL_GREEN);
                    } else {
                        rd->getProgressBox()->setProgress(msg, progress);
                        rd->getRender()->flip(true, false);
                    }
                });
    }
}
