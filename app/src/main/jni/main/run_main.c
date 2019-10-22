//
// Created by ecar on 2019.10.21.
//
#include <libavutil/imgutils.h>
#include "common.h"
#include <jni.h>
#include <SDL_syswm.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "log.h"
#include "../ffmpeg/include/libavformat/avformat.h"
#include "../ffmpeg/include/libavutil/pixfmt.h"
#include "../ffmpeg/include/libswscale/swscale.h"
#include "../ffmpeg/include/libavcodec/avcodec.h"
#include "../ffmpeg/include/libavutil/frame.h"

#include "icon.xpm"
#include "tiny_xpm.h"

void Java_com_ai_aiplayer_FFJniCaller_decode
        (JNIEnv *env, jclass jcls, jstring input_jstr, jstring output_jstr) {

    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    const char *output_cstr = (*env)->GetStringUTFChars(env, output_jstr, NULL);

    LOGD("%s %s %s", __FUNCTION__, input_cstr, output_cstr);
    //1.注册组件
//#ifdef SCRCPY_LAVF_REQUIRES_REGISTER_ALL
//    av_register_all();
//#endif
    if (avformat_network_init()) {
        return;
    }
    //封装格式文件
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    int err_code;
    //2.打开输入视频文件
    if ((err_code = avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL)) != 0) {
        char buf[1024];
        av_strerror(err_code, buf, 1024);
        LOGE("%s %d %s", "打开输入视频文件失败", err_code, buf);
        return;
    }


    LOGI("%s  ", " ooo 01 ");
    //3.获取文件视频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;
    }


    //4.获取解码器
    AVStream *st = NULL;
    AVCodecContext *pCodecCtx = NULL;
    int ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGE("%s", "av_find_best_stream failed");
        return;
    }

    st = pFormatCtx->streams[ret];
    AVCodec *pCodec = avcodec_find_decoder(st->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "找不到解码器");
        return;
    }
    {
        //4.1.获取解码器上下文
        LOGI("%s  ", " ooo 1 ");
        pCodecCtx = avcodec_alloc_context3(pCodec);
        if (pCodecCtx == NULL) {
            LOGE("%s", "Failed to alloc codec context");
            return;
        }

        LOGI("%s  ", " ooo 2 ");
        ret = avcodec_parameters_to_context(pCodecCtx, st->codecpar);
        if (ret < 0) {
            LOGE("%s", "Failed to avcodec_parameters_to_context");
            return;
        }
    }


    //5.打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    };
    //输出视频信息
    LOGE("视频的文件格式：%s", pFormatCtx->iformat->name);
    LOGE("视频时长：%lld", (pFormatCtx->duration) / 1000000);
    LOGE("视频的宽高：%d,%d", pCodecCtx->width, pCodecCtx->height);
    LOGE("解码器的名称：%s", pCodec->name);

    //编码数据
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    //像素数据(解码数据)
    AVFrame *frame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();


    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    //初始化缓冲区
    avpicture_fill((AVPicture *) yuvFrame, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width,
                   pCodecCtx->height);

    //输出文件
    FILE *fp_yuv = fopen(output_cstr, "wb");

    //用于像素格式转换或者缩放
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                                pCodecCtx->pix_fmt, pCodecCtx->width,
                                                pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR,
                                                NULL, NULL, NULL);

    int len, got_frame, framecount = 0;

    //6.一帧一帧读取压缩的视频数据AVPacket
    while (av_read_frame(pFormatCtx, packet) >= 0) {

        //AVPacket->AVFrame

        int ret;
        if ((ret = avcodec_send_packet(pCodecCtx, packet)) < 0) {
            LOGE("Could not send video packet: %d", ret);
            return;
        }
        ret = avcodec_receive_frame(pCodecCtx,
                                    frame);
        if (!ret) {
            // a frame was received
            got_frame = 1;
        } else if (ret != AVERROR(EAGAIN)) {
            LOGE("Could not receive video frame: %d", ret);
            return;
        }

//        len = avcodec_decode_video2(pCodecCtx, frame, &got_frame, packet);


        //Zero if no frame cloud be decompressed
        //非零，正在解码
        if (got_frame) {
            //AVFrame转为像素格式YUV420，宽高
            //2 6输入、输出数据
            //3 7输入、输出画面一行的数据的大小 AVFrame 转换是一行一行转换的
            //4 输入数据第一列要转码的位置 从0开始
            //5 输入画面的高度
            sws_scale(sws_ctx, (const uint8_t *const *) frame->data, frame->linesize, 0,
                      frame->height, yuvFrame->data, yuvFrame->linesize);

            //输出到YUV文件
            //AVFrame像素帧写入文件
            //data解码后的图像像素数据（音频采样数据）
            //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感
            //U V 个数是Y的1/4
            int y_size = pCodecCtx->width * pCodecCtx->height;

            fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
            fwrite(yuvFrame->data[1], 1, y_size / 4, fp_yuv);
            fwrite(yuvFrame->data[2], 1, y_size / 4, fp_yuv);

            LOGI("解码%d帧", framecount++);
        }
        av_packet_unref(packet);
    }
    LOGI("解码 结束");
    fclose(fp_yuv);
    av_frame_free(&frame);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);


    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_cstr);
}


JNIEXPORT void JNICALL Java_com_ai_aiplayer_FFJniCaller_render
        (JNIEnv *env, jobject jobj, jstring input_jstr, jobject surface) {
    const char *filename = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    LOGD("Java_com_ai_aiplayer_FFJniCaller_render play");

//    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        LOGE("Couldn't open file: %s", filename);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't file stream info");
        return;
    }

    int video_stream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream < 0) {
        LOGE("Didn't find best stream ");
        return;
    }
    AVStream *st = pFormatCtx->streams[video_stream];

    AVCodec *pCodec = avcodec_find_decoder(st->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("Couldn't find decoder ");
        return;
    }

    AVCodecContext *pCoderCtx = avcodec_alloc_context3(pCodec);
    if (pCoderCtx == NULL) {
        LOGE("Couldn't alloc codec context");
        return;
    }
    LOGI("Frame X0 %d %d ", pCoderCtx->width, pCoderCtx->height);
    if (avcodec_parameters_to_context(pCoderCtx, st->codecpar) < 0) {
        LOGE("Couldn't get context from params");
        return;
    }
    LOGI("Frame X1 %d %d ", pCoderCtx->width, pCoderCtx->height);

    if (avcodec_open2(pCoderCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec");
        return;
    }
    LOGI("Frame X2 %d %d ", pCoderCtx->width, pCoderCtx->height);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    int vWidth = pCoderCtx->width;
    int vHeight = pCoderCtx->height;

    ANativeWindow_setBuffersGeometry(nativeWindow, vWidth, vHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

//    if(avcodec_open2(pCoderCtx, pCodec, NULL) < 0){
//        LOGE("Couldn't open codec");
//        return;
//    }

    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameRGBA = av_frame_alloc();

    if (pFrame == NULL || pFrameRGBA == NULL) {
        LOGE("Couldn't alloc video frame ");
        return;
    }

    int numbytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCoderCtx->width,
                                            pCoderCtx->height, 1);

    uint8_t *buffer = av_malloc(numbytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCoderCtx->width, pCoderCtx->height, 1);

    struct SwsContext *swsContext = sws_getContext(pCoderCtx->width,
                                                   pCoderCtx->height,
                                                   pCoderCtx->pix_fmt,
                                                   pCoderCtx->width,
                                                   pCoderCtx->height,
                                                   AV_PIX_FMT_RGBA,
                                                   SWS_BILINEAR,
                                                   NULL, NULL, NULL);

    int frameFinished;
    struct AVPacket packet;
    int cnt ;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == video_stream) {
//            Use avcodec_send_packet() and avcodec_receive_frame().
            if (avcodec_send_packet(pCoderCtx, &packet) < 0) {
                LOGE("avcodec_send_packet failed");
                return;
            }
            if ((frameFinished = avcodec_receive_frame(pCoderCtx, pFrame)) < 0) {
                LOGE("avcodec_receive_frame failed %d ", frameFinished);
                return;
            }

            if (frameFinished == 0) {
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                sws_scale(swsContext, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCoderCtx->height, pFrameRGBA->data,
                          pFrameRGBA->linesize);

                uint8_t *dst = windowBuffer.bits;
                int dtsStride = windowBuffer.stride * 4;
                uint8_t *src = pFrameRGBA->data[0];
                int srcStride = pFrameRGBA->linesize[0];

                int h;
                for (h = 0; h < vHeight; ++h) {
                    memcpy(dst + h * dtsStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(10 * 1000);
                LOGI("视频显示 %d ", ++cnt);
            }
        }
        av_packet_unref(&packet);

    }
    LOGI("视频结束 ");

    av_free(buffer);
    av_free(pFrameRGBA);

    av_free(pFrame);
    avcodec_close(pCoderCtx);
    avformat_close_input(&pFormatCtx);

    (*env)->ReleaseStringUTFChars(env, input_jstr, filename);
    return;
}