//
// Created by ecar on 2019.10.22.
//

#include <jni.h>
#include <android/native_window_jni.h>
#include <libavutil/imgutils.h>
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_events.h"
#include "log.h"
#include "../ffmpeg/include/libavcodec/avcodec.h"
#include "../ffmpeg/include/libavformat/avformat.h"
#include "../ffmpeg/include/libavutil/pixfmt.h"
#include "../ffmpeg/include/libswscale/swscale.h"

int main(int argc, char *argv[]) {
    char *file_path = argv[1];
    LOGI("file_path:%s", file_path);

    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    AVPacket *packet;
    uint8_t *out_buffer;

    SDL_Texture *bmp = NULL;
    SDL_Window *screen = NULL;
    SDL_Rect rect;
    SDL_Event event;

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

//    av_register_all();
    pFormatCtx = avformat_alloc_context();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        LOGE("Could not initialize SDL - %s. \n", SDL_GetError());
        exit(1);
    }

    if (avformat_open_input(&pFormatCtx, file_path, NULL, NULL) != 0) {
        LOGE("can't open the file. \n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Could't find stream infomation.\n");
        return -1;
    }

    videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    LOGI("videoStream:%d", videoStream);
    if (videoStream == -1) {
        LOGE("Didn't find a video stream.\n");
        return -1;
    }

    AVStream *st = pFormatCtx->streams[videoStream];
    pCodec = avcodec_find_decoder(st->codecpar->codec_id);

    if (pCodec == NULL) {
        LOGE("Codec not found.\n");
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        LOGE("alloc codec context failed ");
        return -1;
    }

    if (avcodec_parameters_to_context(pCodecCtx, st->codecpar) < 0) {
        LOGE(" avcodec_parameters_to_context .. Failed ");
        return -1;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    LOGI("init sdl 1.\n");
    //---------------------------init sdl---------------------------//
    screen = SDL_CreateWindow("My Player Window", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, pCodecCtx->width, pCodecCtx->height,
                              SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

    LOGI("init sdl 2.\n");
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0 );

    LOGI("init sdl 3 \n");
    bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
                            SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);


    LOGI("init sdl 4.\n");
    //-------------------------------------------------------------//

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
                                  pCodecCtx->height, 1);
    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

//    avpicture_fill((AVPicture *) pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P,
//                   pCodecCtx->width, pCodecCtx->height);
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1);

    rect.x = 0;
    rect.y = 0;
    rect.w = pCodecCtx->width;
    rect.h = pCodecCtx->height;

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket));
    av_new_packet(packet, y_size);

    av_dump_format(pFormatCtx, 0, file_path, 0);

    int cnt = 0;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoStream) {
//            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
//                                        packet);

//            if (ret < 0) {
//                LOGE("decode error.\n");
//                return -1;
//            }
            if (avcodec_send_packet(pCodecCtx, packet) < 0) {
                LOGE("avcodec_send_packet failed");
                return -1;
            }
            if ((got_picture = avcodec_receive_frame(pCodecCtx, pFrame)) < 0) {
                LOGE("avcodec_receive_frame failed %d ", got_picture);
                return -1;
            }


            if (got_picture == 0) {
                LOGI("解码:%d", ++cnt);
                sws_scale(img_convert_ctx,
                          (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
                          pFrameYUV->linesize);
                ////iPitch 计算yuv一行数据占的字节数
                //SDL_UpdateTexture(bmp, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
                SDL_UpdateYUVTexture(bmp, &rect,
                                     pFrameYUV->data[0], pFrameYUV->linesize[0],
                                     pFrameYUV->data[1], pFrameYUV->linesize[1],
                                     pFrameYUV->data[2], pFrameYUV->linesize[2]);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, bmp, &rect, &rect);
                SDL_RenderPresent(renderer);
            }
            SDL_Delay(50);
        }
        av_packet_unref(packet);

        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                LOGI("解码:quit");
//                SDL_Quit();
//                exit(0);
            default:
                break;
        }
    }
    LOGI("解码:End");
    SDL_DestroyTexture(bmp);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);

    av_free(out_buffer);
    av_free(pFrameYUV);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}