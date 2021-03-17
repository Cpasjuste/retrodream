//
// Created by cpasjuste on 17/03/2021.
//

/****************************
 * DreamShell ##version##   *
 * vmu.c                    *
 * DreamShell VMU utils     *
 * Created by SWAT          *
 ****************************/

#include <cstring>
#include <ds/src/vmu/vmu_font.h>

inline void vmu_set_bit(uint8 bitmap[192], uint8 x, uint8 y)
/* (0, 0) is upper-left corner */
{
    if (x < 48 && y < 32)
        bitmap[6 * (31 - y) + (5 - (x / 8))] |= (1 << (x % 8));
}

static void vmu_draw_char(uint8 bitmap[192], unsigned char c, int x, int y)
/* (x, y) is position for upper-left corner of character,
    (0, 0) is upper-left corner of screen */
{
    int i, j;

    if (x < -4 || x > 47 || y < -9 || y > 31)
        return;

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 5; j++) {
            if (vmufont[(int) c][i] & (0x80 >> j))
                vmu_set_bit(bitmap, x + j, y + i);
        }
    }
}

void vmu_draw_str(uint8 bitmap[192], unsigned char *str, int x, int y)
/* (x, y) is position for upper-left corner of string,
    (0, 0) is upper-left corner of screen */
{
    int i;

    if (y < -9 || y > 31)
        return;

    for (i = 0; str[i] != '\0'; i++) {
        if (x < -4) {
            x += 5;
            continue;
        }
        vmu_draw_char(bitmap, str[i], x, y);
        x += 5;
        if (x > 47)
            break;
    }
}
