/*
 * Dreamroq by Mike Melanson
 * Audio support by Josh Pearson
 * 
 * This is the main playback engine.
 */

#include <string.h>
#include "roqlib.h"

int roq_unpack_quad_codebook_rgb565(unsigned char *buf, int size,
                                    int arg, roq_state *state) {
    int y[4];
    int yp, u, v;
    int r, g, b;
    int count2x2;
    int count4x4;
    int i, j;
    unsigned short *v2x2;
    unsigned short *v4x4;

    count2x2 = (arg >> 8) & 0xFF;
    count4x4 = arg & 0xFF;

    if (!count2x2)
        count2x2 = ROQ_CODEBOOK_SIZE;
    /* 0x00 means 256 4x4 vectors iff there is enough space in the chunk
     * after accounting for the 2x2 vectors */
    if (!count4x4 && count2x2 * 6 < size)
        count4x4 = ROQ_CODEBOOK_SIZE;

    /* size sanity check, taking alpha into account */
    if (state->alpha && (count2x2 * 10 + count4x4 * 4) != size) {
        return ROQ_STOPPED;
    }
    if (!state->alpha && (count2x2 * 6 + count4x4 * 4) != size) {
        return ROQ_STOPPED;
    }

    /* unpack the 2x2 vectors */
    for (i = 0; i < count2x2; i++) {
        /* unpack the YUV components from the bytestream */
        for (j = 0; j < 4; j++) {
            y[j] = *buf++;
            if (state->alpha)
                buf++;
        }
        u = *buf++;
        v = *buf++;

        /* convert to RGB565 */
        for (j = 0; j < 4; j++) {
            yp = (y[j] - 16) * 1.164;
            r = (yp + 1.596 * (v - 128)) / 8;
            g = (yp - 0.813 * (v - 128) - 0.391 * (u - 128)) / 4;
            b = (yp + 2.018 * (u - 128)) / 8;

            if (r < 0) r = 0;
            if (r > 31) r = 31;
            if (g < 0) g = 0;
            if (g > 63) g = 63;
            if (b < 0) b = 0;
            if (b > 31) b = 31;

            state->cb2x2_rgb565[i][j] = (
                    (r << 11) |
                    (g << 5) |
                    (b << 0));
        }
    }

    /* unpack the 4x4 vectors */
    for (i = 0; i < count4x4; i++) {
        for (j = 0; j < 4; j++) {
            v2x2 = state->cb2x2_rgb565[*buf++];
            v4x4 = state->cb4x4_rgb565[i] + (j / 2) * 8 + (j % 2) * 2;
            v4x4[0] = v2x2[0];
            v4x4[1] = v2x2[1];
            v4x4[4] = v2x2[2];
            v4x4[5] = v2x2[3];
        }
    }

    return ROQ_PLAYING;
}

#define GET_BYTE(x) \
    if (index >= size) { \
        status = ROQ_STOPPED; \
        x = 0; \
    } else { \
        x = buf[index++]; \
    }

#define GET_MODE() \
    if (!mode_count) { \
        GET_BYTE(mode_lo); \
        GET_BYTE(mode_hi); \
        mode_set = (mode_hi << 8) | mode_lo; \
        mode_count = 16; \
    } \
    mode_count -= 2; \
    mode = (mode_set >> mode_count) & 0x03;

int roq_unpack_vq_rgb565(unsigned char *buf, int size, unsigned int arg,
                         roq_state *state) {
    int status = ROQ_PLAYING;
    int mb_x, mb_y;
    int block;     /* 8x8 blocks */
    int subblock;  /* 4x4 blocks */
    int stride = state->stride;
    int i;

    /* frame and pixel management */
    unsigned short *this_frame;
    unsigned short *last_frame;

    int line_offset;
    int mb_offset;
    int block_offset;
    int subblock_offset;

    unsigned short *this_ptr;
    unsigned int *this_ptr32;
    unsigned short *last_ptr;
    unsigned short *vector16;
    unsigned int *vector32;
    int stride32 = stride / 2;

    /* bytestream management */
    int index = 0;
    int mode_set = 0;
    int mode, mode_lo, mode_hi;
    int mode_count = 0;

    /* vectors */
    int mx, my;
    int motion_x, motion_y;
    unsigned char data_byte;

    mx = (signed char) (arg >> 8);
    my = (signed char) arg;

    if (state->current_frame & 1) {
        this_frame = (unsigned short *) state->frame[1];
        last_frame = (unsigned short *) state->frame[0];
    } else {
        this_frame = (unsigned short *) state->frame[0];
        last_frame = (unsigned short *) state->frame[1];
    }
    /* special case for frame 1, which needs to begin with frame 0's data */
    if (state->current_frame == 1) {
        memcpy(state->frame[1], state->frame[0],
               state->texture_height * state->stride * sizeof(unsigned short));
    }

    for (mb_y = 0; mb_y < state->mb_height && status == ROQ_PLAYING; mb_y++) {
        line_offset = mb_y * 16 * stride;
        for (mb_x = 0; mb_x < state->mb_width && status == ROQ_PLAYING; mb_x++) {
            mb_offset = line_offset + mb_x * 16;
            for (block = 0; block < 4 && status == ROQ_PLAYING; block++) {
                block_offset = mb_offset + (block / 2 * 8 * stride) + (block % 2 * 8);
                /* each 8x8 block gets a mode */
                GET_MODE();
                switch (mode) {
                    case 0:  /* MOT: skip */
                        break;

                    case 1:  /* FCC: motion compensation */
                        /* this needs to be done 16 bits at a time due to
                         * data alignment issues on the SH-4 */
                        GET_BYTE(data_byte);
                        motion_x = 8 - (data_byte >> 4) - mx;
                        motion_y = 8 - (data_byte & 0xF) - my;
                        last_ptr = last_frame + block_offset +
                                   (motion_y * stride) + motion_x;
                        this_ptr = this_frame + block_offset;
                        for (i = 0; i < 8; i++) {
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;
                            *this_ptr++ = *last_ptr++;

                            last_ptr += stride - 8;
                            this_ptr += stride - 8;
                        }
                        break;

                    case 2:  /* SLD: upsample 4x4 vector */
                        GET_BYTE(data_byte);
                        vector16 = state->cb4x4_rgb565[data_byte];
                        for (i = 0; i < 4 * 4; i++) {
                            this_ptr = this_frame + block_offset +
                                       (i / 4 * 2 * stride) + (i % 4 * 2);
                            this_ptr[0] = *vector16;
                            this_ptr[1] = *vector16;
                            this_ptr[stride + 0] = *vector16;
                            this_ptr[stride + 1] = *vector16;
                            vector16++;
                        }
                        break;

                    case 3:  /* CCC: subdivide into 4 subblocks */
                        for (subblock = 0; subblock < 4; subblock++) {
                            subblock_offset = block_offset + (subblock / 2 * 4 * stride) + (subblock % 2 * 4);

                            GET_MODE();
                            switch (mode) {
                                case 0:  /* MOT: skip */
                                    break;

                                case 1:  /* FCC: motion compensation */
                                    GET_BYTE(data_byte);
                                    motion_x = 8 - (data_byte >> 4) - mx;
                                    motion_y = 8 - (data_byte & 0xF) - my;
                                    last_ptr = last_frame + subblock_offset +
                                               (motion_y * stride) + motion_x;
                                    this_ptr = this_frame + subblock_offset;
                                    for (i = 0; i < 4; i++) {
                                        *this_ptr++ = *last_ptr++;
                                        *this_ptr++ = *last_ptr++;
                                        *this_ptr++ = *last_ptr++;
                                        *this_ptr++ = *last_ptr++;

                                        last_ptr += stride - 4;
                                        this_ptr += stride - 4;
                                    }
                                    break;

                                case 2:  /* SLD: use 4x4 vector from codebook */
                                    GET_BYTE(data_byte);
                                    vector32 = (unsigned int *) state->cb4x4_rgb565[data_byte];
                                    this_ptr32 = (unsigned int *) this_frame;
                                    this_ptr32 += subblock_offset / 2;
                                    for (i = 0; i < 4; i++) {
                                        *this_ptr32++ = *vector32++;
                                        *this_ptr32++ = *vector32++;

                                        this_ptr32 += stride32 - 2;
                                    }
                                    break;

                                case 3:  /* CCC: subdivide into 4 subblocks */
                                    GET_BYTE(data_byte);
                                    vector16 = state->cb2x2_rgb565[data_byte];
                                    this_ptr = this_frame + subblock_offset;
                                    this_ptr[0] = vector16[0];
                                    this_ptr[1] = vector16[1];
                                    this_ptr[stride + 0] = vector16[2];
                                    this_ptr[stride + 1] = vector16[3];

                                    GET_BYTE(data_byte);
                                    vector16 = state->cb2x2_rgb565[data_byte];
                                    this_ptr[2] = vector16[0];
                                    this_ptr[3] = vector16[1];
                                    this_ptr[stride + 2] = vector16[2];
                                    this_ptr[stride + 3] = vector16[3];

                                    this_ptr += stride * 2;

                                    GET_BYTE(data_byte);
                                    vector16 = state->cb2x2_rgb565[data_byte];
                                    this_ptr[0] = vector16[0];
                                    this_ptr[1] = vector16[1];
                                    this_ptr[stride + 0] = vector16[2];
                                    this_ptr[stride + 1] = vector16[3];

                                    GET_BYTE(data_byte);
                                    vector16 = state->cb2x2_rgb565[data_byte];
                                    this_ptr[2] = vector16[0];
                                    this_ptr[3] = vector16[1];
                                    this_ptr[stride + 2] = vector16[2];
                                    this_ptr[stride + 3] = vector16[3];

                                    break;
                            }
                        }
                        break;
                }
            }
        }
    }

    /* sanity check to see if the stream was fully consumed */
    if (status == ROQ_PLAYING && index < size - 2) {
        status = ROQ_STOPPED;
    }

    return status;
}
