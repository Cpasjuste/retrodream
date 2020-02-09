//
// Created by cpasjuste on 09/02/2020.
//

#ifndef RETRODREAM_BIOSFLASH_H
#define RETRODREAM_BIOSFLASH_H

#include <string>
#include <functional>

class BiosFlash {

public:

    static bool flash(const std::string &biosFile,
                      const std::function<void(const std::string, float)> &callback = nullptr);
};


#endif //RETRODREAM_BIOSFLASH_H
