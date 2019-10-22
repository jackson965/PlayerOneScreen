package com.ai.aiplayer;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.werb.permissionschecker.PermissionChecker;

import java.io.File;

import static android.os.Environment.getExternalStorageDirectory;

public class MainActivity extends AppCompatActivity implements ActivityCompat.OnRequestPermissionsResultCallback {
    static final String[] PERMISSIONS = new String[]{
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };
    private PermissionChecker permissionChecker;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        View btn_video_render = findViewById(R.id.btn_video_render);
        ll("find ........ byid " + btn_video_render);
        btn_video_render.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ll("llllllllllllll");
                Intent ii = new Intent();
                ii.setClass(MainActivity.this, SimpleRenderActivity.class);
                startActivity(ii);
            }
        });

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });


        permissionChecker = new PermissionChecker(this);
        if (permissionChecker.isLackPermissions(PERMISSIONS)) {
            permissionChecker.requestPermissions();
        } else {
            youCanDo();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case PermissionChecker.PERMISSION_REQUEST_CODE:
                if (permissionChecker.hasAllPermissionsGranted(grantResults)) {
                    youCanDo();
                } else {
                    permissionChecker.showDialog();
                }
                break;
        }
    }


    void youCanDo() {
        ll(" ");
//        doDecode();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    /**
     * 视频解码
     */
    public void doDecode() {
        final String input = new File(getExternalStorageDirectory(), "sample_h264.mp4").getAbsolutePath();
        final String output = new File(getExternalStorageDirectory(), "sample_h264_yuv_out.yuv").getAbsolutePath();
        ll("decode ");

        new Thread(new Runnable() {
            @Override
            public void run() {
                FFJniCaller.decode(input, output);
            }
        }).start();

        Toast.makeText(this, "正在解码...", Toast.LENGTH_SHORT).show();

    }


    private void ll(String s) {
        StackTraceElement[] stackTrace = new Exception().getStackTrace();
        Log.d("00\t", stackTrace[1].getMethodName() + " at " + stackTrace[1].getLineNumber() + ": " + s);
    }

}
