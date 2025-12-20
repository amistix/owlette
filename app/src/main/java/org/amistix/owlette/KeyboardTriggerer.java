package org.amistix.owlette;

import android.content.Context;

import android.widget.FrameLayout;
import android.widget.EditText;
import android.text.Editable;
import android.text.TextWatcher;

import android.view.inputmethod.InputMethodManager;

public class KeyboardTriggerer extends EditText {

    static {
        System.loadLibrary("native-lib");
    }
        
    private native void nativeOnTextChanged(String text);
    private native void nativeGetKeyboardTriggerer();

    public KeyboardTriggerer(Context context) {
        super(context);
        nativeGetKeyboardTriggerer();

        setLayoutParams(new FrameLayout.LayoutParams(1, 1));
        setBackgroundColor(0x00000000);
        setTextColor(0x00000000);

        addTextChangedListener(new TextWatcher() {
            @Override
            public void afterTextChanged(Editable s) {
                nativeOnTextChanged(s.toString());
            }
            
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
            
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {}
        });
    }

    public void showKeyboard(String initialText) {
        this.post(() -> {
            setText(initialText);
            setSelection(initialText.length());
            requestFocus();
            setFocusableInTouchMode(true);
            
            postDelayed(() -> {
                InputMethodManager imm = (InputMethodManager) 
                    getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.showSoftInput(this, InputMethodManager.SHOW_FORCED);
                }
            }, 100);
        });
    }
    
    public void hideKeyboard() {
        this.post(() -> {
            InputMethodManager imm = (InputMethodManager) 
                 getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.hideSoftInputFromWindow(this.getWindowToken(), 0);
            }
        });
    }
}