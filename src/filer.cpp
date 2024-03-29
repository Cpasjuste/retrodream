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

Filer::DirCache::DirCache(c2d::Io *io, const std::string &path, std::vector<c2d::Io::File> *files) {

    char *buffer;
    m_io = io;
    m_path = path + "/.cache";

    m_io->read(m_path, &buffer);
    if (!buffer) {
        return;
    }

    std::vector<std::string> items = Utility::split(buffer, "/");
    if (items.empty()) {
        free(buffer);
        return;
    }

    for (const auto &item : items) {
        std::vector<std::string> split = Utility::split(item, "\\");
        if (split.size() == 3) {
            // don't add dir cache if source doesn't exist anymore (cleanup cache)
            const std::string pathName = split[0];
            auto it = std::find_if(files->begin(), files->end(), [pathName](const Io::File &f) {
                return f.type == Io::Type::Directory && f.name == pathName;
            });
            if (it != files->end()) {
                // add
                m_dirList.push_back({split[0], split[1], split[2]});
            }
        }
    }

    free(buffer);
}

void Filer::DirCache::save() {

    std::string buffer = {};

    if (m_dirList.empty()) {
        return;
    }

    for (auto &item : m_dirList) {
        buffer += item.pathName + "\\";
        buffer += item.gameName + "\\";
        buffer += item.gameType + "/";
    }

    m_io->write(m_path, buffer.c_str(), buffer.size());
}

Filer::DirData *Filer::DirCache::getData(const std::string &pathName) {

    //printf("DirCache::getData(%s)\n", pathName.c_str());
    for (auto &ptr : m_dirList) {
        if (ptr.pathName == pathName) {
            return &ptr;
        }
    }

    return nullptr;
}

void Filer::DirCache::addData(const std::string &pathName, const std::string &gameName, const std::string &gameType) {
    m_dirList.push_back({pathName, gameName, gameType});
}

Filer::Filer(RetroDream *rd, Skin::CustomShape *shape, const std::string &path)
        : SkinRect(shape) {

    retroDream = rd;
    io = (RetroIo *) retroDream->getRender()->getIo();

    // get lines text values
    skinFileText = RetroDream::getSkin()->getText(Skin::Id::FilerFileText);
    skinDirText = RetroDream::getSkin()->getText(Skin::Id::FilerDirText);
    int lineSpacing = RetroDream::getSkin()->getGroup(Skin::Id::FilerShape)->getOption(
            Skin::Id::FilerTextSpacing)->getInteger();
    int fontSize = skinFileText.size > skinDirText.size ? skinFileText.size : skinDirText.size;

    // calculate number of lines shown
    line_height = (float) (fontSize + lineSpacing);
    max_lines = (int) (shape->rect.height / line_height);
    if ((float) max_lines * line_height < shape->rect.height) {
        line_height = shape->rect.height / (float) max_lines;
    }

    // add selection rectangle (highlight)
    Skin::CustomShape shape2 = RetroDream::getSkin()->getShape(Skin::Id::FilerHighlightShape);
    highlight = new SkinRect(&shape2);
    SkinRect::add(highlight);

    // add lines
    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {
        FloatRect r = {0, (line_height * (float) i), shape->rect.width, line_height};
        auto line = new Line(r, "", retroDream->getRender()->getFont(), fontSize);
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
}

bool Filer::getDir(const std::string &p) {

    if (p.empty()) {
        return false;
    }

    //printf("getDir(%s):\n", p.c_str());

    files.clear();
    path = p;
    if (path.size() > 1 && Utility::endsWith(path, "/")) {
        path = Utility::removeLastSlash(path);
    }

    // get directory listing
    std::vector<Io::File> dirList = io->getDirList(path, true, false);
    if (p != "/" && (dirList.empty() || dirList.at(0).name != "..")) {
        Io::File file("..", "..", Io::Type::Directory, 0);
        dirList.insert(dirList.begin(), file);
    }

    // get cached information about directories (for faster directory listing with lot of games)
    DirCache dirCache = DirCache(retroDream->getIo(), path, &dirList);

    std::string mediaPath = retroDream->getIo()->getDataPath() + "medias/";
    std::string imagePath = RetroDream::getConfig()->get(RetroConfig::PreviewImagePath);
    std::string videoPath = RetroDream::getConfig()->get(RetroConfig::PreviewVideoPath);

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
                // set medias path
                std::string noExt = Utility::removeExt(file.data.name);
                file.preview = mediaPath + imagePath + "/";
                file.preview += noExt + ".png";
                file.preview_video = mediaPath + videoPath + "/";
                file.preview_video += noExt + ".roq";
            }
        } else if (fileData.type == Io::Type::Directory) {
            std::string fileNameNoExt;
            DirData *dirData = dirCache.getData(fileData.name);
            if (dirData) {
                // dir cache found...
                //printf("getDirList: cached: %s\n", fileData.name.c_str());
                file.isGame = !dirData->gameType.empty();
                file.isoPath = file.isGame ? fileData.path + "/" + dirData->gameName : "";
                file.isoType = dirData->gameType;
                fileNameNoExt = Utility::removeExt(dirData->gameName);
            } else {
                // dir cache not found...
                //printf("getDirList: not cached: %s\n", fileData.name.c_str());
                Io::File gameFile = io->findFile(fileData.path, {".gdi", ".cdi", ".iso"}, "track");
                // directory contains a game
                if (!gameFile.name.empty()) {
                    file.isGame = true;
                    // if sub dir contains iso or cdi but more than one file, don't assume it's a game
                    if (Utility::endsWith(gameFile.name, ".cdi")) {
                        if (io->hasMoreThanOneFile(fileData.path)) {
                            file.isGame = false;
                        } else {
                            file.isoType = "CDI";
                        }
                    } else if (Utility::endsWith(gameFile.name, ".iso")) {
                        if (io->hasMoreThanOneFile(fileData.path)) {
                            file.isGame = false;
                        } else {
                            file.isoType = "ISO";
                        }
                    } else if (Utility::endsWith(gameFile.name, ".gdi")) {
                        file.isoType = "GDI";
                        if (io->exist(fileData.path + "/track01.iso")) {
                            file.isoType = "OPT";
                        }
                    }
                    if (file.isGame) {
                        file.isoPath = gameFile.path;
                        fileNameNoExt = Utility::removeExt(gameFile.name);
                    }
                }

                dirCache.addData(file.data.name, gameFile.name, file.isoType);
            }

            if (file.isGame) {
                // set medias path
                file.preview = mediaPath + imagePath + "/";
                file.preview += fileNameNoExt + ".png";
                file.preview_video = mediaPath + videoPath + "/";
                file.preview_video += fileNameNoExt + ".roq";
            }
        }

        files.push_back(file);
    }

    dirCache.save();
    setSelection(0);
    previewClock.restart();

    return true;
}

void Filer::updateLines() {

    for (unsigned int i = 0; i < (unsigned int) max_lines; i++) {

        if (file_index + i >= files.size()) {
            lines[i]->setVisibility(Visibility::Hidden);
        } else {
            // set file
            Filer::RetroFile file = files[file_index + i];
            lines[i]->setVisibility(Visibility::Visible);
            lines[i]->setString(file.upperName);
            lines[i]->getText()->setFillColor(file.data.type == Io::Type::File ?
                                              skinFileText.color : skinDirText.color);
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

    if (new_index >= (int) files.size()) {
        return;
    }

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

int Filer::getMaxLines() const {
    return max_lines;
}

int Filer::getIndex() const {
    return file_index + highlight_index;
}

std::string Filer::getPath() const {
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

    unsigned int keys = retroDream->getRender()->getInput()->getButtons();
    if (keys == 0) {
        Filer::RetroFile *file = getSelectionPtr();
        if (file && file->isGame) {
            if (previewClock.getElapsedTime().asMilliseconds() > previewImageDelay) {
                // load preview image
                if (!retroDream->getPreviewImage()->isLoaded()) {
                    retroDream->getPreviewImage()->load(file->preview);
                }
            }
            if (previewClock.getElapsedTime().asMilliseconds() > previewVideoDelay) {
                // load preview video
                if (!retroDream->getPreviewVideo()->isLoaded()) {
                    retroDream->getPreviewVideo()->load(file->preview_video);
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

    unsigned int keys = players[0].buttons;
    if (keys & Input::Button::Up) {
        up();
    } else if (keys & Input::Button::Down) {
        down();
    } else if (keys & Input::Button::Right) {
        setSelection(getIndex() + getMaxLines());
    } else if (keys & Input::Button::Left) {
        setSelection(getIndex() - getMaxLines());
    } else if (keys & Input::Button::A) {
        RetroFile file = getSelection();
        Io::Type type = file.data.type;
        if (file.isGame) {
            // save last path
            RetroDream::getConfig()->set(RetroConfig::FilerPath, path, false);
            RetroDream::getConfig()->setInt(RetroConfig::FilerItem, getIndex(), true);
            IsoLoader::run(retroDream, file.isoPath);
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
    } else if (keys & Input::Button::B) {
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
