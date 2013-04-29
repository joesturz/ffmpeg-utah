/* Rough draft form of our utahdec.c
 *
 */

#include "avcodec.h"
#include "bytestream.h"
#include "config.h"
#include "faxcompr.h"
#include "mathops.h"
#include "libavutil/attributes.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/imgutils.h"
#include "libavutil/avstring.h" 
 
typedef struct UtahContext {
    AVCodecContext *avctx;
    AVFrame picture;
    GetByteContext gb;

    int width, height;
} UtahContext; 
 
static int decode_frame()
{
	printf("Decode_frame() is working!!\n");
	
	return 0;
} 
 
static av_cold int utah_init()
{
    printf("Utah_init() is working!!\n");

    return 0;
} 

static av_cold int utah_end()
{
    printf("Utah_end() is working!!\n");

    return 0;
} 
 
AVCodec ff_utah_decoder = {
    .name           = "utah",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_UTAH,
    .priv_data_size = sizeof(UtahContext),
    .init           = utah_init,
    .close          = utah_end,
    .decode         = decode_frame,
    .capabilities   = CODEC_CAP_DR1,
    .long_name      = NULL_IF_CONFIG_SMALL("UTAH image"),
};
