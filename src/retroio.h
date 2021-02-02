//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_RETROIO_H
#define RETRODREAM_RETROIO_H

class RetroIo : public c2d::C2DIo {

public:

    std::string getHomePath() override;

    std::string getDataPath() override;

    std::string getDsPath();

    std::string getDsBinPath();

    std::string getConfigPath();

    std::string getScreenshotPath();

    bool hasMoreThanOneFile(const std::string &path);

private:

    std::string rdPath;
    std::string dsPath;
    std::string dsBinPath;
};

#endif //RETRODREAM_RETROIO_H
