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
#include "avcodec.h"
#include "internal.h"
#include "bytestream.h"
#include "dsputil.h"

#include "libavutil/imgutils.h"
#include "libavutil/avassert.h"
#include <zlib.h>

typedef struct UTAHContext {
    AVFrame picture;
} UTAHContext;


static int utah_encode_frame(AVCodecContext *avctx, AVPacket *pkt,
                        const AVFrame *pict, int *got_packet)
{
    UTAHContext *s = avctx->priv_data;
    AVFrame * const p = &s->picture;
    int n_bytes_image, n_bytes, hsize, ret, width, height;
    uint8_t *ptr, *buf;
    *p = *pict;

    p->pict_type= AV_PICTURE_TYPE_I;
    p->key_frame= 1;
    width = avctx->width*3;
    height = avctx->height;
    n_bytes_image = avctx->height * avctx->width*3; 

#define SIZE_UTAHFILEHEADER 28
    hsize = SIZE_UTAHFILEHEADER;
    n_bytes = n_bytes_image + hsize;
    if ((ret = ff_alloc_packet2(avctx, pkt, n_bytes)) < 0)
        return ret;
    buf = pkt->data;

    bytestream_put_byte(&buf, 'U');                   // Header info
    bytestream_put_byte(&buf, 'T'); 
    bytestream_put_byte(&buf, 'A');
    bytestream_put_byte(&buf, 'H');
    bytestream_put_le32(&buf, n_bytes);               // Total bytes
    bytestream_put_le32(&buf, 0);                     // Buffer space
    bytestream_put_le32(&buf, hsize);                 // Header size
    bytestream_put_le32(&buf, avctx->width);          // Width
    bytestream_put_le32(&buf, avctx->height);         // Height
    bytestream_put_le32(&buf, n_bytes_image);         // Image size

    // Loop through each byte in the picture and save to the buffer
    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            ptr = p->data[0] + row*p->linesize[0] + col;
            bytestream_put_byte(&buf, *ptr);
        }
    }

    pkt->flags |= AV_PKT_FLAG_KEY;
    *got_packet = 1;
    return 0;
}

static av_cold int utah_enc_init(AVCodecContext *avctx)
{
    UTAHContext *s = avctx->priv_data;

    avcodec_get_frame_defaults(&s->picture);
    avctx->coded_frame = &s->picture;
    av_log(avctx, AV_LOG_INFO, "Entering utah_encode_frame()\n");
    return 0;
}

AVCodec ff_utah_encoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UTAHContext),
    .init           = utah_enc_init,
    .encode2        = utah_encode_frame,
    .capabilities   = CODEC_CAP_INTRA_ONLY,
    .pix_fmts       = (const enum AVPixelFormat[]){
        AV_PIX_FMT_RGB24, AV_PIX_FMT_NONE
    },
    .long_name      = NULL_IF_CONFIG_SMALL("UTAH image"),
};
