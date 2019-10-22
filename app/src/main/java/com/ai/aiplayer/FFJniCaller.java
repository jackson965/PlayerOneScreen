package com.ai.aiplayer;

import android.view.Surface;

public class FFJniCaller {
    public native static void decode(String input, String output);
    //video play
    public native void render(String input, Surface surface);

    //audio decode
    public native void audioDecode(String input, String output);

    //audio player
    public native void audioPlayer(String input);

    //av player
    public native void play(String input, Surface surface);

    //video convert
    public native void ffmpegCmdUtil(int argc, String[] argv);

    //add water mark
    public native void addWatermark(int argc, String[] argv);

    static {
        String[] libs = new String[]{
                "avcodec",
                "avdevice",
                "avfilter",
                "avformat",
                "avutil",
                "swresample",
                "swscale",
                "runmain",
        };

        for (String s : libs) {
            try {
                System.loadLibrary(s);
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }
}
