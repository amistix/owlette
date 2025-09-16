package org.amistix.owlette;

import android.app.Activity;
import android.os.Bundle;

import org.amistix.owlette.databinding.ActivityChannelBinding;

public class MainActivity extends Activity {

    private ActivityChannelBinding binding;
    private static MainActivity instance;

    static {
        System.loadLibrary("native-lib");
    }

    public native String stringFromJNI();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        instance = this;

        binding = ActivityChannelBinding.inflate(getLayoutInflater());

        setContentView(binding.getRoot());
        System.out.println(stringFromJNI());
    }
}
