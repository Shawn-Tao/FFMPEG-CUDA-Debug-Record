#ifndef _H264DECODER_H_
#define _H264DECODER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
// #include <libavformat/avio.h>
// #include <libavformat/avformat.h>
}


class H264Decoder
{

// Variables:
public:
    const char *_filename, *_outfilename;
    const AVCodec *_codec;
    AVCodecParserContext *_parser;
    AVCodecContext *_codec_context;
    AVPacket *_pkt;
    AVFrame *_frame, *rgbFrame;
    SwsContext *swsContext;

// Functions:
public:
    H264Decoder();
    ~H264Decoder();
    void initcodec();
    // void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename);
    void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);
};

#endif