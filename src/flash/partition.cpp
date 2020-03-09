//
// Created by cpasjuste on 05/03/2020.
//

#include "partition.h"
#include "romflash.h"

bool Partition::read() {

    error = FLASHROM_ERR_NONE;

    if (data != nullptr) {
        free(data);
    }

    data = RomFlash::read(&error, type);
    if (data == nullptr) {
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (error != FLASHROM_ERR_NONE) {
        free(data);
        data = nullptr;
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (!checkMagic()) {
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (type == FLASHROM_PT_BLOCK_1) {
        // find FLASHROM_B1_SYSCFG block address
        sysCfgAddr = RomFlash::findBlockAddress(FLASHROM_PT_BLOCK_1, FLASHROM_B1_SYSCFG);
        if (sysCfgAddr < 0) {
            error = sysCfgAddr;
            free(data);
            data = nullptr;
            printf("Partition::read: %s\n", getErrorString().c_str());
            return false;
        }
    }

#if 0
    uint8 md5[16];
    kos_md5(data, 0x00002000, md5);
    printf("partition md5: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x, magic: %.14s\n",
           md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8],
           md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15], data);
#endif

    return true;
}

bool Partition::read(c2d::Io *io, const std::string &path) {

    error = FLASHROM_ERR_NONE;

    if (data != nullptr) {
        free(data);
    }

    c2d::Io::File file = io->getFile(path);
    if (file.type != c2d::Io::Type::File) {
        error = FLASHROM_ERR_READ_FILE;
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (file.size != size) {
        error = FLASHROM_ERR_BOGUS_PART;
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    data = (uint8 *) io->read(path);
    if (data == nullptr) {
        error = FLASHROM_ERR_NOMEM;
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (!checkMagic()) {
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (type == FLASHROM_PT_BLOCK_1) {
        // find FLASHROM_B1_SYSCFG block address
        sysCfgAddr = RomFlash::findBlockAddress(FLASHROM_PT_BLOCK_1, FLASHROM_B1_SYSCFG);
        if (sysCfgAddr < 0) {
            error = sysCfgAddr;
            free(data);
            data = nullptr;
            printf("Partition::read: %s\n", getErrorString().c_str());
            return false;
        }
    }

    return true;
}

bool Partition::write() {

    error = FLASHROM_ERR_NONE;

    if (data == nullptr) {
        error = FLASHROM_ERR_NOMEM;
        printf("Partition::read: %s\n", getErrorString().c_str());
        return false;
    }

    if (type == FLASHROM_PT_BLOCK_1) {
        // update block1 SysCfg block crc
        *((uint16 *) (data + sysCfgAddr + FLASHROM_OFFSET_CRC))
                = (uint16) RomFlash::flashrom_calc_crc(data + sysCfgAddr);
    }

    error = RomFlash::write(type, data);
    if (error != FLASHROM_ERR_NONE) {
        printf("Partition::write: %s\n", getErrorString().c_str());
    }

    return error == FLASHROM_ERR_NONE;
}

bool Partition::write(c2d::Io *io, const std::string &path) {

    error = FLASHROM_ERR_NONE;

    if (data == nullptr) {
        error = FLASHROM_ERR_NOMEM;
        printf("Partition::write: %s\n", getErrorString().c_str());
        return false;
    }

    if (type == FLASHROM_PT_BLOCK_1) {
        // update block1 SysCfg block crc
        *((uint16 *) (data + sysCfgAddr + FLASHROM_OFFSET_CRC))
                = (uint16) RomFlash::flashrom_calc_crc(data + sysCfgAddr);
    }

    return io->write(path, (const char *) data, size);
}

bool Partition::checkMagic() {

    if (type == FLASHROM_PT_SYSTEM) {
        std::string magic = std::string((char *) data + 5, 9);
        if (magic != "Dreamcast") {
            error = FLASHROM_ERR_BAD_MAGIC;
            free(data);
            data = nullptr;
            return false;
        }
    } else {
        std::string magic = std::string((char *) data, 16);
        if (magic != "KATANA_FLASH____") {
            error = FLASHROM_ERR_BAD_MAGIC;
            free(data);
            data = nullptr;
            return false;
        }
    }

    return true;
}

Partition::~Partition() {

    if (data != nullptr) {
        free(data);
        data = nullptr;
    }
}

std::string Partition::getErrorString() {

    if (error == FLASHROM_ERR_NOT_FOUND) {
        return "PARTITION BLOCK NOT FOUND";
    } else if (error == FLASHROM_ERR_NO_PARTITION) {
        return "PARTITION NOT FOUND";
    } else if (error == FLASHROM_ERR_READ_PART) {
        return "COULD NOT READ PARTITION";
    } else if (error == FLASHROM_ERR_BAD_MAGIC) {
        return "BAD MAGIC, PARTITION IS CORRUPTED";
    } else if (error == FLASHROM_ERR_BOGUS_PART) {
        return "WRONG PARTITION SIZE";
    } else if (error == FLASHROM_ERR_NOMEM) {
        return "MEMORY ERROR";
    } else if (error == FLASHROM_ERR_READ_BITMAP) {
        return "BITMAP NOT FOUND";
    } else if (error == FLASHROM_ERR_EMPTY_PART) {
        return "EMPTY PARTITION";
    } else if (error == FLASHROM_ERR_READ_BLOCK) {
        return "CAN'T READ BLOCK PARTITION";
    } else if (error == FLASHROM_ERR_DELETE_PART) {
        return "CAN'T DELETE PARTITION";
    } else if (error == FLASHROM_ERR_WRITE_PART) {
        return "CAN'T WRITE PARTITION";
    } else if (error == FLASHROM_ERR_READ_FILE) {
        return "CAN'T READ FILE";
    } else if (error == FLASHROM_ERR_WRITE_FILE) {
        return "CAN'T WRITE FILE";
    } else if (error == FLASHROM_ERR_OPEN_FILE) {
        return "CAN'T OPEN FILE";
    }

    return "NO ERROR";
}
