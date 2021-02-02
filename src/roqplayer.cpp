//
// Created by cpasjuste on 02/02/2021.
//

#include <cross2d/c2d.h>
#include "roqplayer.h"

using namespace c2d;

RoqPlayerTexture::RoqPlayerTexture() : C2DTexture({256, 128}, Format::RGB565) {
    setVisibility(Visibility::Hidden);
}

int RoqPlayerTexture::play(const std::string &path) {

    stop();

    file = fopen(path.c_str(), "rb");
    if (!file) {
        printf("ROQ_FILE_OPEN_FAILURE: %s\n", path.c_str());
        return ROQ_FILE_OPEN_FAILURE;
    }

    file_ret = fread(read_buffer, CHUNK_HEADER_SIZE, 1, file);
    if (file_ret != 1) {
        fclose(file);
        printf("ROQ_FILE_READ_FAILURE\n");
        return ROQ_FILE_READ_FAILURE;
    }

    chunk_id = LE_16(&read_buffer[0]);
    chunk_size = LE_32(&read_buffer[2]);
    if (chunk_id != RoQ_SIGNATURE || chunk_size != 0xFFFFFFFF) {
        printf("ROQ_FILE_READ_FAILURE: %s\n", path.c_str());
        fclose(file);
        return ROQ_FILE_READ_FAILURE;
    }

    framerate = LE_16(&read_buffer[6]);
    printf("RoQ file plays at %d frames/sec\n", framerate);

    /* Initialize Audio SQRT Look-Up Table */
    for (int i = 0; i < 128; i++) {
        audio.snd_sqr_array[i] = i * i;
        audio.snd_sqr_array[i + 128] = -(i * i);
    }

    setVisibility(Visibility::Visible);
    status = ROQ_PLAYING;

    return status;
}

void RoqPlayerTexture::stop() {

    if (file) {
        fclose(file);
    }
    if (state.frame[0]) {
        free(state.frame[0]);
    }
    if (state.frame[1]) {
        free(state.frame[1]);
    }

    setVisibility(Visibility::Hidden);
    initialized = 0;
    status = ROQ_STOPPED;
}

void RoqPlayerTexture::onUpdate() {

    C2DTexture::onUpdate();

    if (!isVisible() || status != ROQ_PLAYING) {
        return;
    }

    file_ret = fread(read_buffer, CHUNK_HEADER_SIZE, 1, file);
    if (file_ret != 1) {
        /* if the read failed but the file is not EOF, there is a deeper
         * problem; don't entertain the idea of looping */
        if (!feof(file)) {
            return;
        } else if (loop) {
            fseek(file, 8, SEEK_SET);
            return;
        } else {
            stop();
            return;
        }
    }

    chunk_id = LE_16(&read_buffer[0]);
    chunk_size = LE_32(&read_buffer[2]);
    chunk_arg = LE_16(&read_buffer[6]);

    if (chunk_size > MAX_BUF_SIZE) {
        stop();
        status = ROQ_CHUNK_TOO_LARGE;
        return;
    }

    file_ret = fread(read_buffer, chunk_size, 1, file);
    if (file_ret != 1) {
        stop();
        status = ROQ_FILE_READ_FAILURE;
        return;
    }

    state.colorspace = colorspace;

    switch (chunk_id) {
        case RoQ_INFO:
            if (initialized)
                return;

            state.alpha = chunk_arg;
            state.width = LE_16(&read_buffer[0]);
            state.height = LE_16(&read_buffer[2]);
            /* width and height each need to be divisible by 16 */
            if ((state.width & 0xF) || (state.height & 0xF)) {
                status = ROQ_INVALID_PIC_SIZE;
                break;
            }
            state.mb_width = state.width / 16;
            state.mb_height = state.height / 16;
            state.mb_count = state.mb_width * state.mb_height;
            if (state.width < 8 || state.width > 1024)
                status = ROQ_INVALID_DIMENSION;
            else {
                state.stride = 8;
                while (state.stride < state.width)
                    state.stride <<= 1;
            }
            if (state.height < 8 || state.height > 1024)
                status = ROQ_INVALID_DIMENSION;
            else {
                state.texture_height = 8;
                while (state.texture_height < state.height)
                    state.texture_height <<= 1;
            }
            printf("  RoQ_INFO: dimensions = %dx%d, %dx%d; %d mbs, texture = %dx%d\n",
                   state.width, state.height, state.mb_width, state.mb_height,
                   state.mb_count, state.stride, state.texture_height);
            if (colorspace == ROQ_RGB565) {
                state.frame[0] = (unsigned char *) malloc(state.texture_height * state.stride * sizeof(unsigned short));
                state.frame[1] = (unsigned char *) malloc(state.texture_height * state.stride * sizeof(unsigned short));
            } else {
                state.frame[0] = (unsigned char *) malloc(state.texture_height * state.stride * sizeof(unsigned int));
                state.frame[1] = (unsigned char *) malloc(state.texture_height * state.stride * sizeof(unsigned int));
            }
            state.current_frame = 0;
            if (!state.frame[0] || !state.frame[1]) {
                free(state.frame[0]);
                free(state.frame[1]);
                status = ROQ_NO_MEMORY;
                break;
            }
            memset(state.frame[0], 0, state.texture_height * state.stride * sizeof(unsigned short));
            memset(state.frame[1], 0, state.texture_height * state.stride * sizeof(unsigned short));

            /* set this flag so that this code is not executed again when
             * looping */
            initialized = 1;
            break;

        case RoQ_QUAD_CODEBOOK:
            if (colorspace == ROQ_RGB565)
                status = roq_unpack_quad_codebook_rgb565(read_buffer, chunk_size,
                                                         chunk_arg, &state);
            else if (colorspace == ROQ_RGBA)
                status = roq_unpack_quad_codebook_rgba(read_buffer, chunk_size,
                                                       chunk_arg, &state);
            break;

        case RoQ_QUAD_VQ:
            if (colorspace == ROQ_RGB565)
                status = roq_unpack_vq_rgb565(read_buffer, chunk_size,
                                              chunk_arg, &state);
            else if (colorspace == ROQ_RGBA)
                status = roq_unpack_vq_rgba(read_buffer, chunk_size,
                                            chunk_arg, &state);

            // TODO
            /*
            if (cbs->render_cb)
                status = cbs->render_cb(state.frame[state.current_frame & 1],
                                        state.width, state.height, state.stride, state.texture_height,
                                        colorspace);
            */

            void *buf;
            lock(nullptr, &buf, nullptr);
            memcpy(buf, state.frame[state.current_frame & 1], state.stride * state.texture_height * 2);
            unlock();
            //printf("width: %i, height: %i, stride: %i, texture_height: %i, colorspace: %i\n",
              //     state.width, state.height, state.stride, state.texture_height, colorspace);
            state.current_frame++;
            break;

        case RoQ_JPEG:
            break;

        case RoQ_SOUND_MONO:
            audio.channels = 1;
            audio.pcm_samples = chunk_size * 2;
            snd_left = chunk_arg;
            for (unsigned int i = 0; i < chunk_size; i++) {
                snd_left += audio.snd_sqr_array[read_buffer[i]];
                audio.pcm_sample[i * 2] = snd_left & 0xff;
                audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
            }
            /* TODO
            if (cbs->audio_cb)
                status = cbs->audio_cb(roq_audio.pcm_sample, roq_audio.pcm_samples,
                                       roq_audio.channels);
            */
            break;

        case RoQ_SOUND_STEREO:
            audio.channels = 2;
            audio.pcm_samples = chunk_size * 2;
            snd_left = (chunk_arg & 0xFF00);
            snd_right = (chunk_arg & 0xFF) << 8;
            for (unsigned int i = 0; i < chunk_size; i += 2) {
                snd_left += audio.snd_sqr_array[read_buffer[i]];
                snd_right += audio.snd_sqr_array[read_buffer[i + 1]];
                audio.pcm_sample[i * 2] = snd_left & 0xff;
                audio.pcm_sample[i * 2 + 1] = (snd_left & 0xff00) >> 8;
                audio.pcm_sample[i * 2 + 2] = snd_right & 0xff;
                audio.pcm_sample[i * 2 + 3] = (snd_right & 0xff00) >> 8;
            }
            /* TODO
            if (cbs->audio_cb)
                status = cbs->audio_cb(roq_audio.pcm_sample, roq_audio.pcm_samples,
                                       roq_audio.channels);
            */
            break;

        case RoQ_PACKET:
            /* still unimplemented */
            break;

        default:
            break;
    }
}
