/* Rough draft of utahenc.c
 *
 */
 
#include "libavutil/imgutils.h"
#include "libavutil/log.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"

#include "avcodec.h"
#include "config.h"


typedef struct UtahEncoderContext {
    AVClass *class;                     ///< for private options
    AVCodecContext *avctx;
    AVFrame picture;

    int width;                          ///< picture width
    int height;                         ///< picture height
    
} UtahEncoderContext;

static av_cold int encode_init(void)
{
    printf("Encode_init() is working!!\n");

    return 0;
}

static av_cold int encode_frame(void)
{
    printf("Encode_frame() is working!!\n");

    return 0;
}

static av_cold int encode_close(void)
{
    printf("Encode_close() is working!!\n");

    return 0;
}

static const AVClass utahenc_class = {
    .class_name = "UTAH encoder",
    .item_name  = av_default_item_name,
    //.option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

AVCodec ff_utah_encoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UtahEncoderContext),
    .init           = encode_init,
    .encode2        = encode_frame,
    .close          = encode_close,
    .pix_fmts       = (const enum AVPixelFormat[]) {
        AV_PIX_FMT_RGB24, AV_PIX_FMT_PAL8, AV_PIX_FMT_GRAY8,
        AV_PIX_FMT_GRAY8A, AV_PIX_FMT_GRAY16LE,
        AV_PIX_FMT_MONOBLACK, AV_PIX_FMT_MONOWHITE,
        AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUV440P, AV_PIX_FMT_YUV444P,
        AV_PIX_FMT_YUV410P, AV_PIX_FMT_YUV411P, AV_PIX_FMT_RGB48LE,
        AV_PIX_FMT_RGBA, AV_PIX_FMT_RGBA64LE,
        AV_PIX_FMT_NONE
    },
    .long_name      = NULL_IF_CONFIG_SMALL("Utah image"),
    .priv_class     = &utahenc_class,
};

