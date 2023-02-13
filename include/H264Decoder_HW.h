#ifndef _H264DECODER_HW_H_
#define _H264DECODER_HW_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libavutil/error.h>
#include <libavutil/buffer.h>
#include <libavutil/hwcontext.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
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
    AVFrame *_frame, *rgbFrame, *memframe;
    SwsContext *swsContext;

// Functions:
public:
    H264Decoder();
    ~H264Decoder();
    void initcodec();
    // void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename);
    void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVFrame *memframe, AVPacket *pkt);
};

#endif