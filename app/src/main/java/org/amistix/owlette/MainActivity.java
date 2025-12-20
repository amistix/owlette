package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import android.view.View;

import android.widget.FrameLayout;
import android.content.Context;

public class MainActivity extends AppCompatActivity {

    private GLView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FrameLayout container = new FrameLayout(this);
        
        glView = new GLView(this);
        FontRenderer.initFont(64.0f);

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
        glView.release();
        super.onDestroy();
    }
}