package com.ai.aiplayer;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class SimpleRenderActivity extends AppCompatActivity implements SurfaceHolder.Callback {


    private FFJniCaller player;
    private SurfaceHolder surfaceHolder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.simple_render_player);

        SurfaceView video_view = findViewById(R.id.video_view);
        player = new FFJniCaller();
        surfaceHolder = video_view.getHolder();
        surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        new Thread(new Runnable() {

            @Override
            public void run() {
                String input = new File(Environment.getExternalStorageDirectory(),
                        "sample_h264.mp4").getAbsolutePath();
                ll("input : " + input);
                player.render(input, surfaceHolder.getSurface());
            }
        }).start();
    }

    private void ll(String s) {
        StackTraceElement[] stackTrace = new Exception().getStackTrace();
        Log.d("11\t", stackTrace[1].getMethodName() + " at " + stackTrace[1].getLineNumber() + ": " + s);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        surfaceHolder.getSurface().release();
    }
}
