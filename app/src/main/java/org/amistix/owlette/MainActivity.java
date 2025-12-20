package org.amistix.owlette;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.content.Context;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    private GLView glView;
    private EditText hiddenEditText;
    
    private native void nativeOnTextChanged(String text);
    private native void nativeGetMainActivity();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        nativeGetMainActivity();
        FrameLayout container = new FrameLayout(this);
        
        glView = new GLView(this);
        FontRenderer.initFont(64.0f);
        
        // Create hidden EditText
        hiddenEditText = new EditText(this);
        hiddenEditText.setLayoutParams(new FrameLayout.LayoutParams(1, 1));  // 1x1 pixel, not 0x0
        hiddenEditText.setBackgroundColor(0x00000000);  // Transparent
        hiddenEditText.setTextColor(0x00000000);  // Transparent text
        
        hiddenEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {
                nativeOnTextChanged(s.toString());
            }
            
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {}
        });
        
        container.addView(glView);
        container.addView(hiddenEditText);
        
        setContentView(container);
    }

    public void showKeyboard(String initialText) {
        runOnUiThread(() -> {
            hiddenEditText.setText(initialText);
            hiddenEditText.setSelection(initialText.length());
            hiddenEditText.requestFocus();
            hiddenEditText.setFocusableInTouchMode(true);
            
            // Post delayed to ensure the view is ready
            hiddenEditText.postDelayed(() -> {
                InputMethodManager imm = (InputMethodManager) 
                    getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.showSoftInput(hiddenEditText, InputMethodManager.SHOW_FORCED);
                }
            }, 100);
        });
    }
    
    public void hideKeyboard() {
        runOnUiThread(() -> {
            InputMethodManager imm = (InputMethodManager) 
                getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.hideSoftInputFromWindow(hiddenEditText.getWindowToken(), 0);
            }
        });
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