#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

audio load_audio()

video load_video(char *pth) {
    video vid = (video)malloc(sizeof(*vid));

    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
    AVCodecParserContext *parser = av_parser_init(codec->id);

    AVCodecContext *ctx = avcodec_alloc_context3(codec);

    AVFrame *frame = av_frame_alloc();
    AVPacket *pkt = av_packet_alloc();

    avcodec_open2(ctx, codec, NULL);

    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) return NULL;

    while (ret >= 0) {
        ret = avcodec_receive_frame(ctx, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return NULL;

        
    }
}

#ifdef __cplusplus
}
#endif