/*
 * UTAH image format
 * Copyright (c) 2003 Fabrice Bellard
 *
 * This file is NOT part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

//#define DEBUG

#include "libavutil/bprint.h"
#include "libavutil/imgutils.h"
#include "avcodec.h"
#include "bytestream.h"
#include "internal.h"
#include <zlib.h>

typedef struct UTAHContext {
    AVFrame picture;
} UTAHContext;

static int utah_decode_frame(AVCodecContext *avctx,
                        void *data, int *got_frame,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    UTAHContext *s      = avctx->priv_data;
    AVFrame *picture   = data;
    AVFrame *p         = &s->picture;
    //unsigned int fsize, hsize;
    int width, height;
    int i, n, linesize, ret;
    uint8_t *ptr;

    buf += 4; // increase buff past "utah" header

    bytestream_get_le32(&buf); // grab file size

    buf += 4; // move past the buffer spacing

    bytestream_get_le32(&buf); /* header size */
    width  = bytestream_get_le32(&buf); //width of the file
    height = bytestream_get_le32(&buf); //height of the file

    buf +=4; // move to the beginning of picture data

    avctx->width  = width; 
    avctx->height = height > 0 ? height : -height;
    
    avctx->pix_fmt = AV_PIX_FMT_RGB24;

    if (p->data[0])
        avctx->release_buffer(avctx, p);

    p->reference = 0;
    if ((ret = ff_get_buffer(avctx, p)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }
    p->pict_type = AV_PICTURE_TYPE_I;
    p->key_frame = 1;

    ptr = p->data[0];
    linesize = p->linesize[0];
    
    n = avctx->width*3; //width of the line
    
    for (i = 0; i < avctx->height; i++) {
        memcpy(ptr, buf, n); // ptr gets copied into the buffer by n bytes
        buf += n; // increase buffer by n bytes
        ptr += linesize;
    }
    
    *picture = s->picture;
    *got_frame = 1;

    return buf_size;
}

static av_cold int utah_dec_init(AVCodecContext *avctx)
{
    UTAHContext *s = avctx->priv_data;

    avcodec_get_frame_defaults(&s->picture);
    avctx->coded_frame = &s->picture;

    return 0;
}

static av_cold int utah_dec_end(AVCodecContext *avctx)
{
    UTAHContext* c = avctx->priv_data;

    if (c->picture.data[0])
        avctx->release_buffer(avctx, &c->picture);

    return 0;
}

AVCodec ff_utah_decoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UTAHContext),
    .init           = utah_dec_init,
    .close          = utah_dec_end,
    .decode         = utah_decode_frame,
    .capabilities   = CODEC_CAP_DR1 /*| CODEC_CAP_DRAW_HORIZ_BAND*/,
    .long_name      = NULL_IF_CONFIG_SMALL("UTAH image"),
};
