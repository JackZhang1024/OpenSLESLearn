package com.luckyboy.opensleslearn;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private String pcmPath = "";

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btnCopyPCM = findViewById(R.id.btn_copy);
        btnCopyPCM.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                pcmPath = AssetManagerUtil.Companion.getInstance().loadAssetsToFiles(
                        MainActivity.this,
                        "16k.pcm",
                        "16k.pcm",
                        "audio");

            }
        });
        Button btnPlayPCM = findViewById(R.id.btn_play);
        btnPlayPCM.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (pcmPath !=null&& !TextUtils.isEmpty(pcmPath)){
                    playPCM(pcmPath);
                }
            }
        });
    }

    public native void playPCM(String musicPath);

}
