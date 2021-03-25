//
// Created by cpasjuste on 02/02/2021.
//

#ifndef RETRODREAM_ROQLIB_H
#define RETRODREAM_ROQLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define RoQ_INFO           0x1001
#define RoQ_QUAD_CODEBOOK  0x1002
#define RoQ_QUAD_VQ        0x1011
#define RoQ_JPEG           0x1012
#define RoQ_SOUND_MONO     0x1020
#define RoQ_SOUND_STEREO   0x1021
#define RoQ_PACKET         0x1030
#define RoQ_SIGNATURE      0x1084

#define CHUNK_HEADER_SIZE 8

#define LE_16(buf) (*buf | (*(buf+1) << 8))
#define LE_32(buf) (*buf | (*(buf+1) << 8) | (*(buf+2) << 16) | (*(buf+3) << 24))

#define MAX_BUF_SIZE (64 * 1024)

#define ROQ_CODEBOOK_SIZE 256
#define SQR_ARRAY_SIZE 256

#define ROQ_LOAD        0
#define ROQ_PLAYING     1
#define ROQ_STOPPING    2
#define ROQ_STOPPED     3

typedef struct {
    int pcm_samples;
    int channels;
    int position;
    short snd_sqr_array[SQR_ARRAY_SIZE];
    unsigned char *pcm_sample;
} roq_audio;

typedef struct {
    int width;
    int height;
    int mb_width;
    int mb_height;
    int mb_count;
    int alpha;
    int current_frame;
    unsigned char *frame[2];
    int stride;
    int texture_height;
    unsigned short cb2x2_rgb565[ROQ_CODEBOOK_SIZE][4];
    unsigned short cb4x4_rgb565[ROQ_CODEBOOK_SIZE][16];
} roq_state;

int roq_unpack_quad_codebook_rgb565(unsigned char *buf, int size, int arg, roq_state *state);

int roq_unpack_vq_rgb565(unsigned char *buf, int size, unsigned int arg, roq_state *state);

#ifdef __cplusplus
}
#endif

#endif //RETRODREAM_ROQLIB_H
