//
// Created by cpasjuste on 16/01/2020.
//

#include "cross2d/c2d.h"
#include "preview.h"

using namespace c2d;

Preview::Preview(const c2d::FloatRect &rect)
        : RoundedRectangleShape({rect.width, rect.height}, 8, 4) {

    add(new TweenPosition({rect.left + rect.width + 10, rect.top}, {rect.left, rect.top}, 0.1f));
    setVisibility(Visibility::Hidden);
}

bool Preview::load(const std::string &path) {

    unload();

    // set loaded to true so we don't loop on non-existing preview
    loaded = true;

    // roq video
    if (Utility::endsWith(path, ".roq")) {
        file = fopen(path.c_str(), "rb");
        if (!file) {
            printf("ROQ_FILE_OPEN_FAILURE: %s\n", path.c_str());
            status = ROQ_FILE_OPEN_FAILURE;
            return false;
        }

        file_buffer = (unsigned char *) malloc(MAX_BUF_SIZE);
        file_ret = fread(file_buffer, CHUNK_HEADER_SIZE, 1, file);
        if (file_ret != 1) {
            fclose(file);
            printf("ROQ_FILE_READ_FAILURE\n");
            status = ROQ_FILE_READ_FAILURE;
            return false;
        }

        chunk_id = LE_16(&file_buffer[0]);
        chunk_size = LE_32(&file_buffer[2]);
        if (chunk_id != RoQ_SIGNATURE || chunk_size != 0xFFFFFFFF) {
            printf("ROQ_FILE_READ_FAILURE: %s\n", path.c_str());
            fclose(file);
            status = ROQ_FILE_READ_FAILURE;
            return false;
        }

        framerate = LE_16(&file_buffer[6]);
        printf("RoQ file plays at %d frames/sec\n", framerate);

        /* Initialize Audio SQRT Look-Up Table */
        for (int i = 0; i < 128; i++) {
            audio.snd_sqr_array[i] = i * i;
            audio.snd_sqr_array[i + 128] = -(i * i);
        }
        status = ROQ_PLAYING;
        setVisibility(Visibility::Visible, true);
        return true;
    }

    // preview image
    texture = new C2DTexture(path);
    if (!texture->available) {
        delete (texture);
        texture = nullptr;
        return false;
    }

    texture->setOrigin(Origin::Left);
    texture->setPosition(Vector2f(8, getSize().y / 2));
    texture_scaling = std::min(
            getSize().x / ((float) texture->getTextureRect().width + 16),
            getSize().y / ((float) texture->getTextureRect().height + 16));
    texture->setScale(texture_scaling, texture_scaling);
    add(texture);
    setVisibility(Visibility::Visible, true);

    return true;
}

void Preview::unload() {

    // roq video player
    if (file) {
        fclose(file);
        file = nullptr;
    }
    if (state.frame[0]) {
        free(state.frame[0]);
        state.frame[0] = nullptr;
    }
    if (state.frame[1]) {
        free(state.frame[1]);
        state.frame[1] = nullptr;
    }
    if (file_buffer != nullptr) {
        free(file_buffer);
        file_buffer = nullptr;
    }
    initialized = 0;
    status = ROQ_STOPPED;
    // roq video player

    if (texture != nullptr) {
        delete (texture);
        texture = nullptr;
    }

    if (isVisible()) {
        setVisibility(Visibility::Hidden, true);
    }

    loaded = false;
}

bool Preview::isLoaded() {
    return loaded;
}

void Preview::onUpdate() {

    RoundedRectangleShape::onUpdate();

    if (!isVisible() || status != ROQ_PLAYING) {
        return;
    }

    file_ret = fread(file_buffer, CHUNK_HEADER_SIZE, 1, file);
    if (file_ret != 1) {
        /* if the read failed but the file is not EOF, there is a deeper
         * problem; don't entertain the idea of looping */
        if (!feof(file)) {
            return;
        } else if (loop) {
            fseek(file, 8, SEEK_SET);
            return;
        } else {
            unload();
            return;
        }
    }

    chunk_id = LE_16(&file_buffer[0]);
    chunk_size = LE_32(&file_buffer[2]);
    chunk_arg = LE_16(&file_buffer[6]);

    if (chunk_size > MAX_BUF_SIZE) {
        unload();
        status = ROQ_CHUNK_TOO_LARGE;
        return;
    }

    file_ret = fread(file_buffer, chunk_size, 1, file);
    if (file_ret != 1) {
        unload();
        status = ROQ_FILE_READ_FAILURE;
        return;
    }

    state.colorspace = colorspace;

    switch (chunk_id) {
        case RoQ_INFO:
            if (initialized)
                return;

            state.alpha = chunk_arg;
            state.width = LE_16(&file_buffer[0]);
            state.height = LE_16(&file_buffer[2]);
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

            // create texture
            texture = new C2DTexture({state.width, state.width}, Texture::Format::RGB565);
            texture->setOrigin(Origin::Left);
            //texture->setSize(state.width, state.height);
            //texture->setTextureRect(IntRect{0, 0, state.width, state.height});
            texture->setPosition(Vector2f(8, getSize().y / 2));
            texture_scaling = std::min(
                    getSize().x / ((float) texture->getTextureRect().width + 32),
                    getSize().y / ((float) texture->getTextureRect().height + 32));
            texture->setScale(texture_scaling, texture_scaling);
            add(texture);

            /* set this flag so that this code is not executed again when
             * looping */
            initialized = 1;
            break;

        case RoQ_QUAD_CODEBOOK:
            if (colorspace == ROQ_RGB565)
                status = roq_unpack_quad_codebook_rgb565(file_buffer, chunk_size,
                                                         chunk_arg, &state);
            else if (colorspace == ROQ_RGBA)
                status = roq_unpack_quad_codebook_rgba(file_buffer, chunk_size,
                                                       chunk_arg, &state);
            break;

        case RoQ_QUAD_VQ:
            if (colorspace == ROQ_RGB565)
                status = roq_unpack_vq_rgb565(file_buffer, chunk_size,
                                              chunk_arg, &state);
            else if (colorspace == ROQ_RGBA)
                status = roq_unpack_vq_rgba(file_buffer, chunk_size,
                                            chunk_arg, &state);

            void *buf;
            texture->lock(nullptr, &buf, nullptr);
            memcpy(buf, state.frame[state.current_frame & 1], state.stride * state.height * 2);
            texture->unlock();
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
                snd_left += audio.snd_sqr_array[file_buffer[i]];
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
                snd_left += audio.snd_sqr_array[file_buffer[i]];
                snd_right += audio.snd_sqr_array[file_buffer[i + 1]];
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
