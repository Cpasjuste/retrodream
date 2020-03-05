//
// Created by cpasjuste on 05/03/2020.
//

#ifndef RETRODREAM_PARTITION_H
#define RETRODREAM_PARTITION_H

#include <string>
#include <cstdlib>
#include <cross2d/skeleton/io.h>
#include <kos.h>

class Partition {
public:
    ~Partition();

    bool read();

    bool read(c2d::Io *io, const std::string &path);

    bool write();

    bool write(c2d::Io *io, const std::string &path);

    bool checkMagic();

    int getSize() {
        return size;
    }

    const uint8 *getData() {
        return data;
    }

    int getError() {
        return error;
    }

    std::string getErrorString();

protected:

    int type = 0;
    size_t size = 0;
    int error = FLASHROM_ERR_NONE;
    int sysCfgAddr = 0;
    uint8 *data = nullptr;

};

#endif //RETRODREAM_PARTITION_H
