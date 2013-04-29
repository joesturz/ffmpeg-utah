#include <iostream>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## LL)
#endif

extern "C"
{

#include "../ffmpeg/libavutil/mathematics.h"
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavformat/avformat.h"
#include "../ffmpeg/libswscale/swscale.h"
#include "../ffmpeg/libavcodec/bytestream.h"
#include "../ffmpeg/libavutil/imgutils.h"
}

#define PI 3.14159
using namespace std;


void SaveFrame(AVFrame *pFrame, AVCodecContext *pCodecCtx, AVCodec *pCodec, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int y,width,height,ret,got_output;
  uint8_t *buf;

  width = pCodecCtx->width;
  height = pCodecCtx->height;

  AVCodecContext *ballCodecCtx;
  AVCodec *ballCodec;
  AVFrame *ballFrame;
  AVPacket packet;

   /* find the mpeg1 video encoder */
  ballCodec = avcodec_find_encoder(AV_CODEC_ID_UTAH);
  if (!ballCodec) 
  {
    fprintf(stderr, "Codec not found\n");
    exit(1);
  }
  ballCodecCtx = avcodec_alloc_context3(ballCodec);
  if (!ballCodecCtx) 
  {
    fprintf(stderr, "Could not allocate video codec context\n");
    exit(1);
  }

  // Set parameters for AVCodecContext
  ballCodecCtx->width = pCodecCtx->width;
  ballCodecCtx->height = pCodecCtx->height;
  ballCodecCtx->pix_fmt = AV_PIX_FMT_RGB24;
  
  /* open it */
  if (avcodec_open2(ballCodecCtx, ballCodec, NULL) < 0) 
  {
    fprintf(stderr, "Could not open codec\n");
    exit(1);
  }

  // Open file
  sprintf(szFilename, "frame%03d.utah", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Allocating space for Frame
  ballFrame = avcodec_alloc_frame();
  if (!ballFrame) 
  {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }

  // Setting parameters for AVFrame
  ballFrame->format = ballCodecCtx->pix_fmt;
  ballFrame->width =  ballCodecCtx->width;
  ballFrame->height = ballCodecCtx->height;

  /* the image can be allocated by any means and av_image_alloc() is
   * just the most convenient way if av_malloc() is to be used */
  ret = av_image_alloc(ballFrame->data, ballFrame->linesize, ballCodecCtx->width
, ballCodecCtx->height, ballCodecCtx->pix_fmt, 32);
  
  if (ret < 0) 
  {
    fprintf(stderr, "Could not allocate raw picture buffer\n");
    exit(1);
  }

  av_init_packet(&packet);
  packet.data = NULL;
  packet.size = 0;
  fflush(stdout);

 // Set parameters for AVCodecContext
  ballCodecCtx->width = pCodecCtx->width;
  ballCodecCtx->height = pCodecCtx->height;
  ballCodecCtx->pix_fmt = AV_PIX_FMT_RGB24;
  
  /* open it */
  if (avcodec_open2(ballCodecCtx, ballCodec, NULL) < 0) 
  {
    fprintf(stderr, "Could not open codec\n");
    exit(1);
  }

  // Open file
  sprintf(szFilename, "frame%03d.utah", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Allocating space for Frame
  ballFrame = avcodec_alloc_frame();
  if (!ballFrame) 
  {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }

  // Setting parameters for AVFrame
  ballFrame->format = ballCodecCtx->pix_fmt;
  ballFrame->width =  ballCodecCtx->width;
  ballFrame->height = ballCodecCtx->height;

  /* the image can be allocated by any means and av_image_alloc() is
   * just the most convenient way if av_malloc() is to be used */
  ret = av_image_alloc(ballFrame->data, ballFrame->linesize, ballCodecCtx->width
, ballCodecCtx->height, ballCodecCtx->pix_fmt, 32);
  
  if (ret < 0) 
  {
    fprintf(stderr, "Could not allocate raw picture buffer\n");
    exit(1);
  }

  av_init_packet(&packet);
  packet.data = NULL;
  packet.size = 0;
  fflush(stdout);

  /* prepare original image */
  // Write pixel data
  for(int y=0; y<height; y++)
    for(int x=0; x<width*3; x++)
      ballFrame->data[0][y * ballFrame->linesize[0] + x] =  pFrame->data[0][y * 
      pFrame->linesize[0] + x];
  
  // Need to find the radius for our ball
  // Want it to be 5% of the smallest dimension
  int radius = ((width < height) ? width : height)*0.1;

  /* The bounce of the ball will depend on the height of the image and will
   * follow the movement of a sine wave
   */
  int startHeight = (height/2) - abs(height*.25*sin(2*PI*iFrame/30));

  // The ball will start bouncing in the middle of our image
  for(int y=startHeight-radius; y<height; y++)
  {
    uint8_t redValue = 0x00;
    for(int x=width/2-radius; x<width; x++)
    {
      int dx = width/2 - x;
      int dy = startHeight - y;
      if((dx*dx + dy*dy) <= radius*radius)
      {
	// This is calculating the shading
	redValue = static_cast<uint8_t>(0xff * (sin((PI*dx/(2*radius+10))+PI/2)
						+ cos((PI*dy/(2*radius+10)))));
	ballFrame->data[0][y * ballFrame->linesize[0] + x*3] = redValue;
	ballFrame->data[0][y * ballFrame->linesize[0] + x*3+1] = 0x10;
	ballFrame->data[0][y * ballFrame->linesize[0] + x*3+2] = 0x19;
      }
    }
  }

  /* encode the image */
  ret = avcodec_encode_video2(ballCodecCtx, &packet, ballFrame, &got_output);
  if (ret < 0) 
  {
    fprintf(stderr, "Error encoding frame\n");
    exit(1);
  }
  
  if (got_output) 
  {
    fwrite(packet.data, 1, packet.size, pFile);
    av_free_packet(&packet);
  }

  // Close file
  fclose(pFile);
  avcodec_close(ballCodecCtx);
  av_free(ballCodecCtx);
  av_freep(&ballFrame->data[0]);
  avcodec_free_frame(&ballFrame);
}

int main(int argc, char* argsv[])
{
  AVFormatContext *pFormatCtx = NULL;
  int i, videoStream;
  AVCodecContext *pCodecCtx = NULL;
  AVCodec *pCodec = NULL;
  AVFrame *pFrame = NULL;
  AVFrame *pFrameRGB = NULL;
  AVFrame *pFrameCopy = NULL;
  AVPacket packet;
  int frameFinished;
  int numBytes;
  uint8_t *buffer = NULL;

  AVDictionary *optionsDict = NULL;
  struct SwsContext *sws_ctx = NULL;
  
  if(argc < 2) {
    printf("Please provide a jpg file\n");
    return -1;
  }
  // Register all formats and codecs
  av_register_all();
  
  // Open video file
  if(avformat_open_input(&pFormatCtx, argsv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, argsv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) 
    {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);

  // Check to make sure image is jpg format
  string name = pCodec->name;
  if(name!="mjpeg") 
  {
    fprintf(stderr, "Please use only jpg files!\n");
    return -1;
  }

  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  pFrame=avcodec_alloc_frame();
  
  // Allocate an AVFrame structure
  pFrameRGB=avcodec_alloc_frame();
  if(pFrameRGB==NULL)
    return -1;

  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
			      pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
  
  sws_ctx =
    sws_getContext
    (
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		 pCodecCtx->width, pCodecCtx->height);

  // Read image frame
  av_read_frame(pFormatCtx, &packet);
  
  // Decode video frame
  avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
			&packet);

  // Did we get a video frame?
  if(frameFinished) 
  {
    // Convert the image from its native format to RGB
    sws_scale
    (
      sws_ctx,
      (uint8_t const * const *)pFrame->data,
      pFrame->linesize,
      0,
      pCodecCtx->height,
      pFrameRGB->data,
      pFrameRGB->linesize
    );
  }

  // Loop 300 times to produce utah images
  for(int i = 1; i < 301; i++)
  {
    SaveFrame(pFrameRGB, pCodecCtx, pCodec, i);
  }

  // Free the packet that was allocated by av_read_frame
  av_free_packet(&packet);

  // Free the RGB image
  av_free(buffer);
  av_free(pFrameRGB);
  
  // Free the YUV frame
  av_free(pFrame);
  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  avformat_close_input(&pFormatCtx);

  return 0;

}


// tutorial01.c
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use the Makefile to build all examples.
//
// Run using
//
// tutorial01 myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
