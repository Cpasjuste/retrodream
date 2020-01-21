//
// Created by cpasjuste on 21/01/2020.
//

#ifndef RETRODREAM_RETROIO_H
#define RETRODREAM_RETROIO_H

class RetroIo : public c2d::C2DIo {

public:

    std::string getHomePath() override;

    std::string getDataPath() override;

    void setDataPath(const std::string &path);

    std::string getConfigPath();

private:

    std::string dataPath;
};

#endif //RETRODREAM_RETROIO_H
