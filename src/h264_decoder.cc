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

/**
 * @file
 * video decoding with libavcodec API example
 *
 * @example decode_video.c
 */

#include "H264Decoder.h"

#define INBUF_SIZE 40960

std::string path = "./output/test";

int i = 0;

inline void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename, "wb");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

// static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
//                      char *filename)
// {
//     FILE *f;
//     int i;

//     f = fopen(filename, "wb");
//     fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
//     for (i = 0; i < ysize; i++)
//         fwrite(buf + i * wrap, 1, xsize, f);
//     fclose(f);
// }

int SaveFrameToPPM(const char *pPicFile, AVFrame *pFrame, int nWidth, int nHeight)
{
    FILE *fp = fopen(pPicFile, "wb");
    if (NULL == fp)
    {
        printf("file open error %s\n", pPicFile);
        return -1;
    }
    // write header
    fprintf(fp, "P6\n%d %d\n255\n", nWidth, nHeight);

    // write pixel data
    for (int y = 0; y < nHeight; y++)
    {
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, nWidth * 3, fp);
    }
    fclose(fp);

    return 0;
}

H264Decoder::H264Decoder()
{
}

H264Decoder::~H264Decoder()
{
    av_parser_close(_parser);
    avcodec_free_context(&_codec_context);
    av_frame_free(&_frame);
    av_packet_free(&_pkt);
}

void H264Decoder::initcodec()
{
    _codec_context = NULL;
    // pkt init;
    _pkt = av_packet_alloc();
    if (!_pkt)
        exit(1);

    /* find the MPEG-1 video decoder */
    // codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    // _codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    _codec = avcodec_find_decoder(AV_CODEC_ID_H265);
    if (!_codec)
    {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    _parser = av_parser_init(_codec->id);
    if (!_parser)
    {
        fprintf(stderr, "parser not found\n");
        exit(1);
    }

    _codec_context = avcodec_alloc_context3(_codec);
    if (!_codec_context)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    // _codec_context->width = 1920;
    // _codec_context->height = 1080;
    // _codec_context->pix_fmt = AV_PIX_FMT_YUVJ420P;

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(_codec_context, _codec, NULL) < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    _frame = av_frame_alloc();
    if (!_frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    swsContext = sws_getContext(1920, 1080, AV_PIX_FMT_YUVJ420P, 1920, 1080, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    rgbFrame = av_frame_alloc();
    // avpicture_alloc((AVPicture *)rgbFrame, AV_PIX_FMT_RGB24, 1920, 1080);

    av_image_alloc(rgbFrame->data, rgbFrame->linesize, 1920, 1080, AV_PIX_FMT_RGB24, 1);
}

void H264Decoder::decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
{
    char buf[1024000];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0)
        {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        sws_scale(this->swsContext, this->_frame->data, this->_frame->linesize,
                  0, this->_codec_context->height,
                  this->rgbFrame->data, this->rgbFrame->linesize);

        std::string savepath;
        savepath = path + std::to_string(i) + ".ppm";
        i++;

        SaveFrameToPPM(savepath.c_str(), rgbFrame, 1920, 1080);

        printf("saving frame %5d\n", i);
        fflush(stdout);
    }
}

int main(int argc, char **argv)
{

    H264Decoder decoder = H264Decoder();
    decoder.initcodec();

    FILE *f;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t data_size;
    int ret;
    int eof;

    const char *filename, *outfilename;

    filename = argv[1];
    outfilename = argv[2];

    f = fopen(filename, "rb");
    if (!f)
    {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    do
    {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (ferror(f))
            break;
        eof = !data_size;

        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0 || eof)
        {
            ret = av_parser_parse2(decoder._parser, decoder._codec_context, &decoder._pkt->data, &decoder._pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0)
            {
                fprintf(stderr, "Error while parsing\n");
                exit(1);
            }
            data += ret;
            data_size -= ret;

            if (decoder._pkt->size)
                decoder.decode(decoder._codec_context, decoder._frame, decoder._pkt);
            else if (eof)
                break;
        }
    } while (!eof);

    /* flush the decoder */
    decoder.decode(decoder._codec_context, decoder._frame, NULL);

    fclose(f);
    return 0;
}