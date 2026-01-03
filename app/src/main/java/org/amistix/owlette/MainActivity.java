package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.view.ViewTreeObserver;
import android.os.Bundle;

import android.view.View;
import android.graphics.Rect;

import android.widget.FrameLayout;
import android.content.Context;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }
    
    private GLView glView;

    private int lastVisibleHeight = 0;

    private native void nativeShutdown();
    private native void nativeOnKeyboardVisibilityChanged(int visibleHeight, int keyboardHeight);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FrameLayout container = new FrameLayout(this);
        
        new AppStorageManager(this);
        
        glView = new GLView(this);


        KeyboardTriggerer hiddenEditText = new KeyboardTriggerer(this);
        
        container.addView(glView);
        container.addView(hiddenEditText);

        setupKeyboardListener();
        
        setContentView(container);
    }

    private void setupKeyboardListener() {
        final View rootView = findViewById(android.R.id.content);
        
        rootView.getViewTreeObserver().addOnGlobalLayoutListener(
            new ViewTreeObserver.OnGlobalLayoutListener() {
                @Override
                public void onGlobalLayout() {
                    Rect rect = new Rect();
                    rootView.getWindowVisibleDisplayFrame(rect);
                    
                    int screenHeight = rootView.getRootView().getHeight();
                    int visibleHeight = rect.bottom - rect.top;
                    int keyboardHeight = screenHeight - visibleHeight;

                    boolean isKeyboardVisible = keyboardHeight > 100;
                    
                    // Only notify if height changed significantly (>100px)
                    if (Math.abs(lastVisibleHeight - visibleHeight) > 100) {
                        lastVisibleHeight = visibleHeight;
        
                        nativeOnKeyboardVisibilityChanged(visibleHeight, keyboardHeight);
                    }
                }
            }
        );
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