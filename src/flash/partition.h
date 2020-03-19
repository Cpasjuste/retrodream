//
// Created by cpasjuste on 05/03/2020.
//

#ifndef RETRODREAM_PARTITION_H
#define RETRODREAM_PARTITION_H

#include <string>
#include <cstdlib>
#include <cross2d/skeleton/io.h>

#ifdef __DREAMCAST__
#include <kos.h>
#else
typedef unsigned char uint8;
typedef unsigned short int uint16;
#define FLASHROM_OFFSET_CRC 0
#define FLASHROM_PT_SYSTEM 0
#define FLASHROM_PT_BLOCK_1 0
#define FLASHROM_B1_SYSCFG 0
#define FLASHROM_LANG_JAPANESE 0
#define FLASHROM_LANG_ENGLISH 0
#define FLASHROM_LANG_GERMAN 0
#define FLASHROM_LANG_FRENCH 0
#define FLASHROM_LANG_SPANISH 0
#define FLASHROM_LANG_ITALIAN 0
#define FLASHROM_ERR_NONE 0
#define FLASHROM_ERR_BOGUS_PART -1
#define FLASHROM_ERR_NOMEM -1
#define FLASHROM_ERR_BAD_MAGIC -1
#define FLASHROM_ERR_NOT_FOUND -1
#define FLASHROM_ERR_NO_PARTITION -1
#define FLASHROM_ERR_READ_PART -1
#define FLASHROM_ERR_READ_BITMAP -1
#define FLASHROM_ERR_EMPTY_PART -1
#define FLASHROM_ERR_READ_BLOCK -1
#define flashrom_info(x, y, z) printf("flashrom_info: not implemented on linux\n")
#define flashrom_read(x, y, z) printf("flashrom_read: not implemented on linux\n")
#define flashrom_delete(x) printf("flashrom_delete: not implemented on linux\n")
#define flashrom_write(x, y, z) printf("flashrom_write: not implemented on linux\n")

#endif

#define FLASHROM_ERR_DELETE_PART    -20
#define FLASHROM_ERR_WRITE_PART     -21
#define FLASHROM_ERR_READ_FILE      -22
#define FLASHROM_ERR_WRITE_FILE     -23
#define FLASHROM_ERR_OPEN_FILE      -24

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
    bool fromFile = false;
    uint8 *data = nullptr;

};

#endif //RETRODREAM_PARTITION_H
