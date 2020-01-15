//
// Created by cpasjuste on 15/01/2020.
//

#include "cross2d/c2d.h"
#include "utility.h"

void RetroUtility::exec(const std::string &path) {

#ifdef __DREAMCAST__
    file_t f;
    void *elf;

    f = fs_open(path.c_str(), O_RDONLY);
    if (f < 0) {
        return;
    }

    elf = fs_mmap(f);
    if (elf == nullptr) {
        return;
    }

    arch_exec(elf, fs_total(f));
#endif
}
