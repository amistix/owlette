package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import android.view.View;

import android.widget.FrameLayout;
import android.content.Context;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }
    
    private GLView glView;
    private native void nativeShutdown();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FrameLayout container = new FrameLayout(this);
        
        new AppStorageManager(this);
        
        glView = new GLView(this);


        KeyboardTriggerer hiddenEditText = new KeyboardTriggerer(this);
        
        container.addView(glView);
        container.addView(hiddenEditText);
        
        setContentView(container);
    }

    @Override
    public void onResume() {
        super.onResume();
        glView.onResume();
    }

    @Override
    public void onPause() {
        glView.onPause();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        if (isFinishing()) {
            nativeShutdown();
        }
        glView.release();
        super.onDestroy();
    }
}